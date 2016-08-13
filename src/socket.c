/*
 * The Driver Station Library (LibDS)
 * Copyright (C) 2015-2016 Alex Spataru <alex_spataru@outlook>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "DS_Array.h"
#include "DS_Utils.h"
#include "DS_Socket.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#if defined _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <errno.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
#endif

#if defined _WIN32
    static WSADATA socket_data;
    #define GET_ERR WSAGetLastError()
    #define SO_REUSEPORT SO_REUSEADDR
#else
    #define GET_ERR errno
#endif

static int running = 0;
static DS_Array sockets;

/**
 * Represents the private socekt information used by the sockets module
 */
struct __socket_info {
    sds sender_buffer;
    sds data_recv;
    int socket_in;
    int socket_out;
    int socket_tmp;
    int initialized;
    int server_initialized;
    int client_initialized;
    struct addrinfo in_addr;
    struct addrinfo out_addr;
};

/**
 * Closes the given \a socket using OS-specific functions
 *
 * \param descriptor the socket descriptor to close
 */
static void close_socket (int descriptor)
{
    if (descriptor > 0) {
#ifdef WIN32
        closesocket (descriptor);
#else
        close (descriptor);
#endif
    }
}

/**
 * Alerts the user about an error during a socket operation
 *
 * \param ptr a pointer to a \c DS_Socket structure
 * \param message a descriptive error message (e.g. cannot bind socket)
 * \param error the error code
 */
static void error (DS_Socket* ptr, sds message, int error)
{
    if (ptr) {
        fprintf (stderr,
                 "Socket %p:\n"
                 "\t Address: %s\n"
                 "\t Message: %s\n"
                 "\t Error Code: %d\n"
                 "\t Error Desc: %s\n",
                 ptr, ptr->address, message, error, strerror (error));
    }
}

/**
 * Obtains information for the given socket structure and port
 *
 * \param ptr pointer to a \c DS_Socket structure
 * \param local if set to \c 1, the node-name will be empty (localhost)
 */
static struct addrinfo* get_address_info (DS_Socket* ptr, int server)
{
    /* Check for NULL pointer */
    if (!ptr)
        return NULL;

    /* Initialize variables */
    struct addrinfo hints, *res;
    memset (&hints, 0, sizeof (hints));

    /* Set hints */
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = (ptr->type == DS_SOCKET_TCP) ? SOCK_STREAM : SOCK_DGRAM;

    /* Get the port string */
    int port = (server == 1) ? ptr->input_port : ptr->output_port;
    sds port_str = sdscatfmt (sdsempty(), "%i", port);

    /* Get local address */
    if (server)
        getaddrinfo (NULL, port_str, &hints, &res);

    /* Get remote address */
    else {
        int err = getaddrinfo (ptr->address, port_str, &hints, &res);

        /* Cannot get remote address, use local host */
        if (err)
            getaddrinfo (NULL, port_str, &hints, &res);
    }

    /* De-allocate port string */
    sdsfree (port_str);

    /* Return obtained address info */
    return res;
}

/**
 * Creates a new socket and configures it to use the following flags:
 *    - \c SO_REUSEPORT
 *    - \c SO_BROADCAST (if the \a ptr requires it)
 *
 * \param ptr a pointer to a \c DS_Socket struct
 * \param addr the address information to use to create the socket
 */
static int create_socket (DS_Socket* ptr, struct addrinfo* addr)
{
    /* Check for NULL pointers */
    if (!ptr)
        return -1;

    /* Create the socket */
    int sockfd = socket (addr->ai_family,
                         addr->ai_socktype,
                         addr->ai_protocol);

    /* Windows uses char instead of int in setsockopt() */
#ifdef WIN32
    char name = 1;
#else
    int name = 1;
#endif

    /* Reuse the address and port */
    int reuse_port = setsockopt (sockfd,
                                 SOL_SOCKET,
                                 SO_REUSEPORT,
                                 &name, sizeof (name));

    /* Setting the SO_REUSEPORT failed */
    if (reuse_port != 0) {
        close_socket (sockfd);
        error (ptr, "cannot set SO_REUSEPORT", GET_ERR);
        return -1;
    }

    /* Socket is UDP and broadcast flag is set */
    if ((ptr->broadcast == 1) && (ptr->type == DS_SOCKET_UDP)) {
        int broadcast = setsockopt (sockfd,
                                    SOL_SOCKET,
                                    SO_BROADCAST,
                                    &name, sizeof (name));

        /* Cannot set the broadcast option */
        if (broadcast != 0) {
            close_socket (sockfd);
            error (ptr, "cannot set SO_BROADCAST", GET_ERR);
            return -1;
        }
    }

    /* Return the socket's file descriptor */
    return sockfd;
}

/**
 * Creates and configures a new socket descriptor for the given structure
 *
 * \param ptr pointer to \c DS_Socket structure
 * \param server if set to \c 1, then this function shall configure the input
 *        socket. Otherwise, this function shall configure the output socket.
 */
static int configure_socket (DS_Socket* ptr, int server)
{
    /* Check for NULL pointers */
    if (!ptr)
        return 0;

    /* Get the address information */
    struct addrinfo addr = *get_address_info (ptr, server);

    /* Configure the server socket */
    if (server) {
        ptr->info->in_addr = addr;
        ptr->info->socket_in = create_socket (ptr, &addr);

        /* Check if socket is valid */
        if (ptr->info->socket_in < 0) {
            error (ptr, "cannot create server socket", GET_ERR);
            return 0;
        }

        /* Bind the socket */
        int bind_err = bind (ptr->info->socket_in,
                             ptr->info->in_addr.ai_addr,
                             ptr->info->in_addr.ai_addrlen);

        /* Check if there was an error while binding the socket */
        if (bind_err) {
            error (ptr, "cannot bind server socket", GET_ERR);
            return 0;
        }

        /* Allow the socket to accept connections */
        int listen_err = listen (ptr->info->socket_in, 5);

        /* Check if there was an error while configuring the socket */
        if (listen_err) {
            error (ptr, "cannot configure connection-mode socket", GET_ERR);
            return 0;
        }
    }

    /* Configure the client socket */
    else {
        ptr->info->out_addr = addr;
        ptr->info->socket_out = create_socket (ptr, &addr);

        /* Check if socket is valid */
        if (ptr->info->socket_out < 0) {
            error (ptr, "cannot create client socket", GET_ERR);
            return 0;
        }

        /* Connect the socket (if we use TCP) */
        if (ptr->type == DS_SOCKET_TCP) {
            int connect_err = connect (ptr->info->socket_out,
                                       ptr->info->out_addr.ai_addr,
                                       ptr->info->out_addr.ai_addrlen);

            /* Check if there was an error while connecting the socket */
            if (connect_err) {
                error (ptr, "cannot connect client socket", GET_ERR);
                return 0;
            }
        }
    }

    /* If we reach this, then socket initialization was successfull */
    return 1;
}

/**
 * Initializes the given socket structure
 *
 * \param ptr pointer to a \c DS_Socket structure
 */
static void* initialize (void* ptr)
{
    /* Cast the pointer to a socket structure */
    DS_Socket* sock = (DS_Socket*) ptr;
    if (sock) {
        /* Close socket (just to be sure) */
        DS_SocketClose (sock);

        /* Allocate memory for the socket information */
        sock->info = calloc (0, sizeof (struct __socket_info));

        /* Do not allow the module to use this socket yet */
        sock->info->initialized = 0;
        sock->info->client_initialized = 0;
        sock->info->server_initialized = 0;

        /* Empty the sender and receiver buffers */
        sock->info->data_recv = sdsempty();
        sock->info->sender_buffer = sdsempty();

        /* Initialize and configure the client and server sockets */
        int client = configure_socket (sock, 0);
        int server = configure_socket (sock, 1);

        /* Apply the obtained configuration */
        sock->info->client_initialized = client;
        sock->info->server_initialized = server;
        sock->info->initialized = client || server;

        /* Register the new socket with the module */
        DS_ArrayInsert (&sockets, (void*) sock);
    }

    return NULL;
}

/**
 * Sends any client data from the given socket
 */
static void send_data (DS_Socket* ptr)
{
    /* Invalid pointer or data buffer */
    if (!ptr)
        return;

    /* Buffer is empty */
    if (DS_StringIsEmpty (ptr->info->sender_buffer))
        return;

    /* Socket is disabled or uninitialized */
    if ((ptr->disabled == 1) || (ptr->info->client_initialized != 1))
        return;

    /* Send data using TCP */
    if (ptr->type == DS_SOCKET_TCP)
        send (ptr->info->socket_out,
              ptr->info->sender_buffer,
              sdslen (ptr->info->sender_buffer), 0);

    /* Send data using UDP */
    else if (ptr->type == DS_SOCKET_UDP) {
        sendto (ptr->info->socket_out,
                ptr->info->sender_buffer,
                sdslen (ptr->info->sender_buffer), 0,
                ptr->info->out_addr.ai_addr,
                ptr->info->out_addr.ai_addrlen);
    }

    /* Clear sender buffer */
    sdsfree (ptr->info->sender_buffer);
}

/**
 * Runs all client operations (sending packets)
 */
static void* client_loop()
{
    while (running) {
        int i = 0;
        for (i = 0; i < (int) sockets.used; ++i)
            send_data ((DS_Socket*) sockets.data [i]);
    }

    return NULL;
}

/**
 * Runs all server operations (receiving packets, accepting connections, etc)
 */
static void* server_loop()
{
    while (running) {

    }

    return NULL;
}

/**
 * Initializes the sockets module and its event system
 */
void Sockets_Init()
{
    /* Allow the event loops to run */
    running = 1;

    /* Initialize the sockets array list */
    DS_ArrayInit (&sockets, sizeof (DS_Socket) * 10);

    /* Start client event loop */
    pthread_t client_thread;
    int client_err = pthread_create (&client_thread, NULL, &client_loop, NULL);

    /* Start server event loop */
    pthread_t server_thread;
    int server_err = pthread_create (&server_thread, NULL, &server_loop, NULL);

    /* Quit if client or server threads fail to start */
    if (client_err || server_err) {
        fprintf (stderr, "Cannot initialize sockets module\n");
        exit (EXIT_FAILURE);
    }
}

/**
 * Stops the event loops and closes all socket structures
 */
void Sockets_Close()
{
    running = 0;

    /* Close all sockets */
    int i = 0;
    for (i = 0; i < (int) sockets.used; ++i) {
        DS_Socket* sock = (DS_Socket*) sockets.data [i];

        if (sock)
            DS_SocketClose (sock);
    }

    /* Free socket array */
    DS_ArrayFree (&sockets);
}

/**
 * Initializes and configures the given socket
 *
 * \note The socket will be initialzed in another thread to avoid blocking
 *       the main thread of the application
 */
void DS_SocketOpen (DS_Socket* ptr)
{
    /* Pointer is NULL */
    if (!ptr)
        return;

    /* Socket is already initialized */
    if (ptr->info)
        if (ptr->info->initialized)
            return;

    /* Initialize socket in another thread */
    pthread_t thread;
    pthread_create (&thread, NULL, &initialize, (void*) ptr);
}

/**
 * Closes the socket file descriptors of the given socket structure
 * and resets the structure's information.
 *
 * \param ptr pointer to the \c DS_Socket to close
 */
void DS_SocketClose (DS_Socket* ptr)
{
    /* Check for NULL pointer */
    if (!ptr)
        return;

    /* Check that socket information is present */
    if (ptr->info) {
        /* Close socket descriptors */
        close_socket (ptr->info->socket_in);
        close_socket (ptr->info->socket_out);
        close_socket (ptr->info->socket_tmp);

        /* De-allocate socket information */
        free (ptr->info);
    }
}

/**
 * Sends the given \a data using the given socket
 *
 * \param data the data buffer to send
 * \param ptr pointer to the socket to use to send the given \a data
 *
 * \returns 1 on success, 0 on failure
 */
int DS_SocketSend (DS_Socket* ptr, sds data)
{
    if (!ptr)
        return 0;

    if (ptr->info) {
        if (!ptr->info->client_initialized || ptr->disabled)
            return 0;

        sdsfree (ptr->info->sender_buffer);
        ptr->info->sender_buffer = sdscpy (sdsempty(), data);

        return 1;
    }

    return 0;
}

/**
 * Writes any data received from the socket to the given buffer
 *
 * \param ptr the socket to read data from
 * \param data the buffer in which to write received data
 *
 * \returns 1 if received data is not empty, otherwise,
 *          this function shall return 0
 */
int DS_SocketRead (DS_Socket* ptr, sds data)
{
    if (!ptr)
        return 0;

    if (ptr->info) {
        if (!ptr->info->server_initialized || ptr->disabled)
            return 0;

        data = sdscpy (sdsempty(), ptr->info->data_recv);
        sdsfree (ptr->info->data_recv);

        return !DS_StringIsEmpty (data);
    }

    return 0;
}

/**
 * Changes the \a address of the given socket structre
 *
 * \param ptr pointer to a \c DS_Socket structure
 * \param address the new address to apply to the socket
 */
void DS_SocketChangeAddress (DS_Socket* ptr, sds address)
{
    /* Check if pointer is NULL */
    if (!ptr)
        return;

    /* Close the socket */
    DS_SocketClose (ptr);

    /* Re-assign the address */
    sdsfree (ptr->address);
    ptr->address = sdscpy (sdsempty(), address);

    /* Re-open the socket */
    DS_SocketOpen (ptr);
}
