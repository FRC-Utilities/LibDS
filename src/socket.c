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

#include "DS_Timer.h"
#include "DS_Array.h"
#include "DS_Utils.h"
#include "DS_Socket.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#if defined _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <netdb.h>
    #include <errno.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
#endif

#if defined _WIN32
    static WSADATA wsa_data;
    #define GET_ERR WSAGetLastError()
    #define SO_REUSEPORT SO_REUSEADDR
#else
    #define GET_ERR errno
#endif

#define SOCK_ERROR -1

/**
 * Holds the sockets in a dynamic array (for automatic closing)
 */
static DS_Array sockets;

/**
 * Closes the given \a socket using OS-specific functions
 *
 * \param descriptor the socket descriptor to close
 */
static void close_socket (int descriptor)
{
    if (descriptor > 0 && descriptor != SOCK_ERROR) {
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
 * Enables or disables socket blocking
 *
 * \param sockfd the socket file descriptor
 * \param blocking whenever the file should be blocking or not
 *
 * \returns 1 on success, 0 on failure
 */
static int set_socket_blocking (int sockfd, int blocking)
{
    if (sockfd < 0)
        return 0;

#ifdef WIN32
    unsigned long mode = (blocking ? 0 : 1);
    return (ioctlsocket (fd, FIONBIO, &mode) == 0);
#else
    int flags = fcntl (sockfd, F_GETFL, 0);

    if (flags < 0)
        return 0;

    flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
    return (fcntl (sockfd, F_SETFL, flags) == 0);
#endif
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
    hints.ai_family = AF_INET;
    hints.ai_socktype = (ptr->type == DS_SOCKET_TCP) ? SOCK_STREAM : SOCK_DGRAM;

    /* Get the port string */
    sds port_str = sdscatfmt (sdsempty(), "%i",
                              server ? ptr->input_port : ptr->output_port);

    /* Get address information */
    getaddrinfo (server ? "localhost" : ptr->address,
                 port_str,
                 &hints,
                 &res);

    /* De-allocate port string */
    DS_FREESTR (port_str);

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
        return SOCK_ERROR;

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

    /* Set timeout value */
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;

    /* Set a socket timeout flag */
    int timeout_err = setsockopt (sockfd,
                                  SOL_SOCKET,
                                  SO_SNDTIMEO,
                                  &tv, sizeof (tv));

    /* Setting the SO_SNDTIMEO flag failed */
    if (timeout_err != 0) {
        close_socket (sockfd);
        error (ptr, "cannot set SO_SNDTIMEO", GET_ERR);
        return SOCK_ERROR;
    }

    /* Reuse the address and port */
    int reuse_port = setsockopt (sockfd,
                                 SOL_SOCKET,
                                 SO_REUSEPORT,
                                 &name, sizeof (name));

    /* Setting the SO_REUSEPORT flag failed */
    if (reuse_port != 0) {
        close_socket (sockfd);
        error (ptr, "cannot set SO_REUSEPORT", GET_ERR);
        return SOCK_ERROR;
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
            return SOCK_ERROR;
        }
    }

    /* Return the socket's file descriptor */
    return sockfd;
}

/**
 * Executes the server socket loop
 *
 * \param ptr pointer to an initialized \c DS_Socket structure
 */
static void* run_server (void* ptr)
{
    /* Pointer is NULL */
    if (!ptr)
        return NULL;

    /* Cast the pointer to a socket */
    DS_Socket* sock = (DS_Socket*) ptr;

    /* Make the socket non-blocking */
    set_socket_blocking (sock->info.socket_in, 0);

    /* Run the main server loop */
    while (sock->info.initialized && sock->info.server_initialized) {
        char* buffer = NULL;
        int bytes = recvfrom (sock->info.socket_in,
                              buffer, 1024, MSG_DONTWAIT,
                              sock->info.in_addr->ai_addr,
                              &sock->info.in_addr->ai_addrlen);

        if (bytes > 0)
            sock->info.buffer = sdscat (sock->info.buffer, buffer);

        DS_FREE (buffer);
        DS_Sleep (10);
    }

    return NULL;
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
    struct addrinfo* addr = get_address_info (ptr, server);

    /* Configure the server socket */
    if (server) {
        ptr->info.in_addr = addr;
        ptr->info.socket_in = create_socket (ptr, addr);

        /* Check if socket is valid */
        if (ptr->info.socket_in < 0) {
            error (ptr, "cannot create server socket", GET_ERR);
            return 0;
        }

        /* Bind the socket */
        int bind_err = bind (ptr->info.socket_in,
                             ptr->info.in_addr->ai_addr,
                             ptr->info.in_addr->ai_addrlen);

        /* Check if there was an error while binding the socket */
        if (bind_err) {
            error (ptr, "cannot bind server socket", GET_ERR);
            return 0;
        }

        /* Start server loop */
        pthread_t thread;
        pthread_create (&thread, NULL, &run_server, (void*) ptr);
    }

    /* Configure the client socket */
    else {
        ptr->info.out_addr = addr;
        ptr->info.socket_out = create_socket (ptr, addr);

        /* Check if socket is valid */
        if (ptr->info.socket_out < 0) {
            error (ptr, "cannot create client socket", GET_ERR);
            return 0;
        }

        if (ptr->type == DS_SOCKET_TCP) {
            /* Connect the socket (if we use TCP) */
            int connect_err = connect (ptr->info.socket_out,
                                       ptr->info.out_addr->ai_addr,
                                       ptr->info.out_addr->ai_addrlen);

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
    DS_Socket* sock = (DS_Socket*) ptr;
    if (sock) {
        /* Do not allow the module to use this socket yet */
        sock->info.initialized = 0;
        sock->info.client_initialized = 0;
        sock->info.server_initialized = 0;

        /* Clear the address if required */
        if (sock->broadcast || DS_StringIsEmpty (sock->address))
            sock->address = sdsempty();

        /* Initialize and configure the client and server sockets */
        int client = configure_socket (sock, 0);
        int server = configure_socket (sock, 1);

        /* Apply the obtained configuration */
        sock->info.client_initialized = client;
        sock->info.server_initialized = server;
        sock->info.initialized = client || server;

        /* Register the new socket with the module */
        DS_ArrayInsert (&sockets, (void*) ptr);
    }

    return NULL;
}

/**
 * Returns an empty socket for safe initialization
 */
DS_Socket DS_SocketEmpty()
{
    DS_Socket socket;
    DS_SocketInfo info;

    info.initialized = 0;
    info.buffer = sdsempty();
    info.server_initialized = 0;
    info.client_initialized = 0;
    info.socket_in = SOCK_ERROR;
    info.socket_out = SOCK_ERROR;
    info.socket_tmp = SOCK_ERROR;

    socket.info = info;
    socket.disabled = 0;
    socket.broadcast = 0;
    socket.input_port = 0;
    socket.output_port = 0;
    socket.address = sdsempty();
    socket.type = DS_SOCKET_TCP;

    return socket;
}

/**
 * Initializes the sockets module and its event system
 */
void Sockets_Init()
{
#if defined _WIN32
    if (WSAStartup (MAKEWORD (2, 0), &wsa_data) != 0) {
        fprintf (stderr, "Cannot initialize WinSock!\n");
        exit (EXIT_FAILURE);
    }
#endif

    DS_ArrayInit (&sockets, sizeof (DS_Socket) * 5);
}

/**
 * Stops the event loops and closes all socket structures
 */
void Sockets_Close()
{
    /* Close WinSock */
#if defined _WIN32
    WSACleanup();
#endif

    /* Close all sockets */
    int i = 0;
    for (i = 0; i < (int) sockets.used; ++i)
        DS_SocketClose ((DS_Socket*) sockets.data [i]);

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

    /* Socket is disabled */
    if (ptr->disabled)
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

    /* Close socket descriptors */
    close_socket (ptr->info.socket_in);
    close_socket (ptr->info.socket_out);

    /* Close TCP server descriptor */
    if (ptr->type == DS_SOCKET_TCP)
        close_socket (ptr->info.socket_tmp);

    /* Reset the info structure */
    ptr->info.initialized = 0;
    ptr->info.client_initialized = 0;
    ptr->info.server_initialized = 0;

    /* Clear socket buffer */
    DS_FREESTR (ptr->info.buffer);
}

/**
 * Sends the given \a data using the given socket
 *
 * \param data the data buffer to send
 * \param ptr pointer to the socket to use to send the given \a data
 *
 * \returns number of bytes written on success, -1 on failure
 */
int DS_SocketSend (DS_Socket* ptr, sds data)
{
    /* Invalid pointer and/or empty data buffer */
    if (!ptr || DS_StringIsEmpty (data))
        return SOCK_ERROR;

    /* Socket is disabled or uninitialized */
    if (ptr->info.client_initialized != 1 || ptr->disabled == 1)
        return SOCK_ERROR;

    /* Send data using TCP */
    if (ptr->type == DS_SOCKET_TCP)
        return send (ptr->info.socket_out, data, sdslen (data), 0);

    /* Send data using UDP */
    else if (ptr->type == DS_SOCKET_UDP) {
        return sendto (ptr->info.socket_out, data, sdslen (data), 0,
                       ptr->info.out_addr->ai_addr,
                       ptr->info.out_addr->ai_addrlen);
    }

    return SOCK_ERROR;
}

/**
 * Writes any data received from the socket to the given buffer
 *
 * \param ptr the socket to read data from
 * \param data the buffer in which to write received data
 *
 * \returns number of received bytes on success, SOCK_ERROR on failure
 */
int DS_SocketRead (DS_Socket* ptr, sds data)
{
    /* Invalid pointer */
    if (!ptr)
        return SOCK_ERROR;

    /* Socket is disabled or uninitialized */
    if (ptr->info.server_initialized != 1 || ptr->disabled == 1)
        return SOCK_ERROR;

    /* Copy buffer to data pointer */
    if (ptr->info.buffer != NULL) {
        data = sdscpy (sdsempty(), ptr->info.buffer);
        DS_FREESTR (ptr->info.buffer);
    }

    /* Return length of received data */
    return (int) sdslen (data);
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

    /* Close socket */
    DS_SocketClose (ptr);

    /* Re-assign address */
    DS_FREESTR (ptr->address);
    ptr->address = sdscpy (sdsempty(), address);

    /* Open socket */
    DS_SocketOpen (ptr);
}
