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

#include "DS_Utils.h"
#include "DS_Socket.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#ifdef WIN32
    static WSADATA socket_data;
    #define GET_ERR WSAGetLastError()
    #define SO_REUSEPORT SO_REUSEADDR
#else
    #define GET_ERR errno
#endif

/**
 * Closes the given \a socket using OS-specific functions
 *
 * \param descriptor the socket descriptor to close
 */
static void close_socket (int descriptor)
{
#ifdef WIN32
    closesocket (descriptor);
#else
    close (descriptor);
#endif
}

/**
 * Returns the socket type that should be used for the given socket
 *
 * \param ptr a pointer to a \c DS_Socket structure
 */
static int get_type (DS_Socket* ptr)
{
    if (ptr) {
        if (ptr->type == DS_SOCKET_TCP)
            return SOCK_STREAM;
    }

    return SOCK_DGRAM;
}

/**
 * Standard socket error report function
 *
 * \param ptr pointer to a \c DS_Socket structure
 * \param msg the message to show
 * \param error the error code
 */
static void error (DS_Socket* ptr, const sds msg, int error)
{
    if (!ptr)
        return;

    fprintf (stderr, "Socket %p (%s): %s %d\n", ptr, ptr->address, msg, error);
}

/**
 * Standard socket error report function
 *
 * \param ptr pointer to a \c DS_Socket structure
 * \param msg the message to show
 * \param error the error string
 */
static void error_str (DS_Socket* ptr, const sds msg, const char* error)
{
    if (!ptr)
        return;

    fprintf (stderr, "Socket %p (%s): %s %s\n", ptr, ptr->address, msg, error);
}

/**
 * Returns the default socket domain/family used by the
 * native implementation.
 *
 * We prefer to use IPv6, since its backwards compatible with IPv4
 * and simplifies the code by eliminating the need of using different
 * structures and functions to support IPv4
 */
static int get_domain()
{
    return AF_INET;
}

/**
 * Sets the \c SO_REUSEPORT and \c SO_BROADCAST flags to the socket
 *
 * \param ptr a pointer to a \c DS_Socket structure
 * \param socket the socked file descriptor
 *
 * \note \c SO_BROADCAST is only set if the \c DS_Socket requires it
 *
 * \returns \c 1 on success, \c 0 on failure
 */
static int set_socket_flags (DS_Socket* ptr, int sock)
{
    /* Check for NULL pointers */
    if (!ptr)
        return 0;

    /* Windows uses char instead of int in setsockopt() */
#ifdef WIN32
    char name = 1;
#else
    int name = 1;
#endif

    /* Reuse the address and port */
    int reuse_port = setsockopt (sock,
                                 SOL_SOCKET,
                                 SO_REUSEPORT,
                                 &name, sizeof (name));

    /* Setting the SO_REUSEPORT failed */
    if (reuse_port != 0) {
        error (ptr, "cannot set SO_REUSEPORT", GET_ERR);
        return 0;
    }

    /* Socket is UDP and broadcast flag is set */
    if ((ptr->broadcast == 1) && (ptr->type == DS_SOCKET_UDP)) {
        int broadcast = setsockopt (sock,
                                    SOL_SOCKET,
                                    SO_BROADCAST,
                                    &name, sizeof (name));

        /* Cannot set the broadcast option */
        if (broadcast != 0) {
            error (ptr, "cannot set SO_BROADCAST", GET_ERR);
            return 0;
        }
    }

    /* Success! */
    return 1;
}

/**
 * Returns information from the local address
 *
 * \param ptr a pointer to \c DS_Socket structure, used to get the
 *        server port number
 */
static struct addrinfo* get_local_address (DS_Socket* ptr)
{
    /* Check for NULL pointers */
    if (!ptr)
        return NULL;

    /* Initialize the hints */
    struct addrinfo hints, *addr;
    memset (&hints, 0, sizeof (hints));

    /* Set hints */
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = get_domain();
    hints.ai_socktype = get_type (ptr);

    /* Get port string */
    char* port = malloc (sizeof (char) * 5);
    sprintf (port, "%d", ptr->input_port);

    /* Get the address info */
    int err = getaddrinfo (NULL, port, &hints, &addr);

    /* Something went wrong */
    if (err) {
        free (port);
        error_str (ptr, "local address error", gai_strerror (err));
        return NULL;
    }

    /* De-allocate memory */
    free (port);

    /* Return the address */
    return addr;
}

/**
 * Allows the given socket to acccept any incoming connections
 */
static void accept_connection (DS_Socket* ptr)
{
    if (!ptr)
        return;

    if ((ptr->initialized == 1) && (ptr->accepted != 1)) {
        ptr->accepted = 0;

        if (ptr->type == DS_SOCKET_TCP) {
            struct sockaddr addr;
            socklen_t addr_size = sizeof (addr);
            int res = accept (ptr->socket_in, &addr, &addr_size);

            if (res > 0) {
                ptr->accepted = 1;
                ptr->socket_tmp = res;
            }
        }

        else {
            ptr->accepted = 1;
            ptr->socket_tmp = ptr->socket_in;
        }
    }
}

/**
 * Returns the remote address information for the given \c DS_Socket
 *
 * \todo For the moment, this function will bind to the local address
 *       for testing purposes. Change this when the sockets module is
 *       fully functional
 * \param ptr a pointer to \c DS_Socket structure, used to get the
 *        client port number
 */
static struct addrinfo* get_remote_address (DS_Socket* ptr)
{
    /* Check for NULL pointers */
    if (!ptr)
        return NULL;

    /* Initialize the hints */
    struct addrinfo hints, *addr;
    memset (&hints, 0, sizeof (hints));

    /* Set hints */
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = get_domain();
    hints.ai_socktype = get_type (ptr);

    /* Get port string */
    char* port = malloc (sizeof (char) * 5);
    sprintf (port, "%d", ptr->output_port);

    /* Get the address info */
    int err = getaddrinfo (ptr->address, port, &hints, &addr);

    /* Something went wrong */
    if (err) {
        free (port);
        error_str (ptr, "remote address error", gai_strerror (err));
        return NULL;
    }

    /* De-allocate memory */
    free (port);

    /* Return the address */
    return addr;
}

/**
 * Creates a new socket with the given address info
 *
 * \param ptr the pointer to a \c DS_Socket structure
 * \param addr the address info to use to create the socket
 */
static int get_socket (DS_Socket* ptr, struct addrinfo* addr)
{
    /* Check for NULL pointers */
    if (!ptr || !addr)
        return -1;

    /* Open socket */
    int sockfd = socket (addr->ai_family,
                         addr->ai_socktype,
                         addr->ai_protocol);

    /* Check that the socket is valid */
    if (sockfd < 0) {
        close_socket (sockfd);
        error (ptr, "cannot create socket", sockfd);
        return -1;
    }

    /* Set socket options */
    if (set_socket_flags (ptr, sockfd) == 0) {
        close_socket (sockfd);
        error (ptr, "cannot create socket", sockfd);
        return -1;
    }

    return sockfd;
}

/**
 * Initializes the sockets for the given structure
 *
 * \param ptr a pointer to a \c DS_Socket structure
 * \param is_input used to know if we should use the input or
 *        output port specified by the \c DS_Socket
 */
static int open_socket (DS_Socket* ptr, int is_input)
{
    /* Check if pointer is valid */
    if (!ptr)
        return 0;

    /* Socket is already initialized or disabled, abort */
    if (ptr->initialized == 1 || ptr->disabled == 1)
        return 0;

    /* Ensure that address is valid */
    if (DS_StringIsEmpty (ptr->address))
        ptr->address = sdsnew ("0.0.0.0");

    /* Configure the server/input socket */
    if (is_input == 1) {
        ptr->in_addr = *get_local_address (ptr);
        ptr->socket_in = get_socket (ptr, &ptr->in_addr);

        /* Socket is invalid */
        if (ptr->socket_in < 0)
            return 0;

        /* Bind the socket */
        int bind_out = bind (ptr->socket_in,
                             ptr->in_addr.ai_addr,
                             ptr->in_addr.ai_addrlen);

        /* Check if binding was successfull */
        if (bind_out != 0) {
            error (ptr, "bind error", GET_ERR);
            close_socket (ptr->socket_in);
            return 0;
        }

        /* Allow 5 connections on the incoming queue */
        if (ptr->type == DS_SOCKET_TCP) {
            if (listen (ptr->socket_in, 5) != 0) {
                error (ptr, "listen error", GET_ERR);
                close_socket (ptr->socket_in);
                return 0;
            }
        }
    }

    /* Configure the client/output socket */
    else {
        ptr->out_addr = *get_remote_address (ptr);
        ptr->socket_out = get_socket (ptr, &ptr->out_addr);

        /* Socket is invalid */
        if (ptr->socket_out < 0)
            return 0;

        /* Connect the socket */
        if (ptr->type == DS_SOCKET_TCP) {
            int connect_out = connect (ptr->socket_out,
                                       ptr->out_addr.ai_addr,
                                       ptr->out_addr.ai_addrlen);

            /* Check if connection was successfull */
            if (connect_out != 0) {
                error (ptr, "connection error", GET_ERR);
                close_socket (ptr->socket_out);
                return 0;
            }
        }
    }

    return 1;
}

/**
 * Creates and configures the given socket
 *
 * \param ptr the pointer to a \c DS_Socket structure
 */
static void* initialize_socket (void* ptr)
{
    DS_Socket* sock = (DS_Socket*) ptr;

    if (sock) {
        int in = open_socket (sock, 1);
        int ot = open_socket (sock, 0);
        sock->initialized = ((in == 1) && (ot == 1));
    }

    return NULL;
}

/**
 * Initiates the use of the Windows Sockets API for the client process
 */
void Sockets_Init()
{
#ifdef WIN32
    if (WSAStartup (MAKEWORD (2, 2), &socket_data) != 0) {
        fprintf (stderr, "Cannot initialize WSA, error: %d", WSAGetLastError());
        exit (EXIT_FAILURE);
    }
#endif
}

/**
 * Cleans up the Windows Socket API
 */
void Sockets_Close()
{
#ifdef WIN32
    WSACleanup();
#endif
}

/**
 * Initializes the data of the given socket structure
 *
 * \param ptr a pointer to a \c DS_Socket structure
 */
void DS_SocketOpen (DS_Socket* ptr)
{
    /* Check for NULL pointers */
    if (!ptr)
        return;

    /* Initialize socket in another thread */
    if (ptr->initialized != 1) {
        ptr->accepted = 0;
        ptr->initialized = 0;

        pthread_t thread;
        pthread_create (&thread, NULL, &initialize_socket, (void*) ptr);
    }
}

/**
 * Closes the given socket
 *
 * \param ptr a pointer to a \c DS_Socket structure
 */
void DS_SocketClose (DS_Socket* ptr)
{
    if (ptr) {
        close_socket (ptr->socket_in);
        close_socket (ptr->socket_out);
        close_socket (ptr->socket_tmp);

        ptr->accepted = 0;
        ptr->socket_in = 0;
        ptr->socket_out = 0;
        ptr->socket_tmp = 0;
        ptr->initialized = 0;
    }
}

/**
 * Sends the given \a buf using the information stored in the socket
 *
 * \param ptr a pointer to a \c DS_Socket structure
 * \param buf the buffer with the data to send
 *
 * \returns the number of bytes sent
 */
int DS_SocketSend (DS_Socket* ptr, sds buf)
{
    /* Invalid pointer or data buffer */
    if (!ptr || !buf)
        return 0;

    /* Buffer is empty */
    if (DS_StringIsEmpty (buf))
        return 0;

    /* Socket is disabled or uninitialized */
    if ((ptr->disabled == 1) || (ptr->initialized != 1))
        return 0;

    /* Send data using TCP */
    if (ptr->type == DS_SOCKET_TCP)
        return send (ptr->socket_out, buf, sdslen (buf), 0);

    /* Send data using UDP */
    else if (ptr->type == DS_SOCKET_UDP) {
        return sendto (ptr->socket_out,
                       buf, sdslen (buf), 0,
                       ptr->out_addr.ai_addr,
                       ptr->out_addr.ai_addrlen);
    }

    /* Socket is not TCP nor UDP */
    return 0;
}

/**
 * Copies the received data from the socket into the \a buf parameter
 *
 * \param ptr a pointer to a \c DS_Socket structure
 * \param buf the buffer in which we write received data
 *
 * \returns the number of bytes received
 */
int DS_SocketRead (DS_Socket* ptr, sds buf)
{
    /* Invalid pointer */
    if (!ptr)
        return 0;

    /* Socket is disabled or uninitialized */
    if ((ptr->disabled == 1) || (ptr->initialized != 1))
        return 0;

    /* Accept the connection (TCP, UDP is ignored) */
    accept_connection (ptr);

    /* Only receive data if connection is established */
    if (ptr->accepted != 1)
        return 0;

    /* Receive data using TCP */
    if (ptr->type == DS_SOCKET_TCP)
        return recv (ptr->socket_tmp, buf, 1024, 0);

    /* Receive data using UDP */
    else if (ptr->type == DS_SOCKET_UDP) {
        return recvfrom (ptr->socket_in,
                         buf, 1024, 0,
                         ptr->in_addr.ai_addr,
                         (int*) &ptr->in_addr.ai_addrlen);
    }

    /* Socket is not TCP nor UDP */
    return 0;
}
