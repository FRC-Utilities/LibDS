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

#include <sds.h>
#include <stdio.h>
#include <string.h>

#ifdef WIN32
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

#ifdef WIN32
    static WSADATA socket_data;
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
    if (ptr->type == DS_SOCKET_TCP)
        return SOCK_STREAM;

    return SOCK_DGRAM;
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
    return AF_INET6;
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
    int name = 1;
    int reuse_port = setsockopt (sock,
                                 SOL_SOCKET,
                                 SO_REUSEPORT,
                                 &name, sizeof (name));

    if (reuse_port != 0) {
        printf ("Socket %p: error %d while setting SO_REUSEPORT\n", ptr, errno);
        return 0;
    }

    if (ptr->broadcast && (ptr->type == DS_SOCKET_UDP)) {
        int broadcast = setsockopt (sock,
                                    SOL_SOCKET,
                                    SO_BROADCAST,
                                    &name, sizeof (name));

        if (broadcast != 0) {
            printf ("Socket %p error %d while settings SO_BROADCAST\n", ptr, errno);
            return 0;
        }
    }

    return 1;
}

/**
 * Fills in information from the local address and the input port
 *
 * \param ptr a pointer to \c DS_Socket structure, used to get the
 *        server port number
 * \param addr the address in which we should put the obtained data
 */
static void get_local_address (DS_Socket* ptr, struct addrinfo* addr)
{
    struct addrinfo hints;
    memset (&hints, 0, sizeof (hints));

    /* Set hints */
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = get_domain();
    hints.ai_socktype = get_type (ptr);

    /* Get port string */
    char* port = malloc (sizeof (char) * 5);
    sprintf (port, "%d", ptr->input_port);

    /* Get address information */
    getaddrinfo (NULL, port, &hints, &addr);

    /* De-allocate memory for the port string */
    free (port);
}

/**
 * Allows the given socket to acccept any incoming connections
 */
static void accept_connection (DS_Socket* ptr)
{
    if (!ptr)
        return;

    if (ptr->initialized && !ptr->accepted) {
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
 * \param addr the address in which we should put the obtained data
 */
static void get_remote_address (DS_Socket* ptr, struct addrinfo* addr)
{
    int error;
    int sockfd;
    struct addrinfo hints, *info;
    memset (&hints, 0, sizeof hints);

    /* Set hints */
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = get_domain();
    hints.ai_socktype = get_type (ptr);

    /* Get port string */
    char* port = malloc (sizeof (char) * 5);
    sprintf (port, "%d", ptr->output_port);

    /* Get the address info */
    error = getaddrinfo (NULL, port, &hints, &info);

    /* Something went wrong */
    if (error) {
        printf ("Socket %p: remote address error %s\n", ptr, gai_strerror (error));
        return;
    }

    /* Loop the found addresses until one responds to connection */
    for (addr = info; addr != NULL; addr = addr->ai_next) {
        if ((sockfd = socket (addr->ai_family, addr->ai_socktype,
                              addr->ai_protocol)) == -1)
            continue;

        if (connect (sockfd, addr->ai_addr, addr->ai_addrlen) == -1) {
            close_socket (sockfd);
            continue;
        }

        break;
    }

    /* De-allocate memory */
    free (port);
    freeaddrinfo (info);
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

    /* Socket is already initialized, abort */
    if (ptr->initialized) {
        printf ("Socket %p is already initialized!\n", ptr);
        return 0;
    }

    /* Open the socket descriptor */
    int sockfd = socket (get_domain(), get_type (ptr), 0);

    /* Check that the socket is valid */
    if (sockfd < 0) {
        printf ("Socket %p: open error %d\n", ptr, errno);
        return 0;
    }

    /* Set socket options */
    if (!set_socket_flags (ptr, sockfd)) {
        printf ("Socket %p: error settings socket flags\n", ptr);
        return 0;
    }

    /* Configure the server/input socket */
    if (is_input) {
        struct addrinfo addr;
        get_local_address (ptr, &addr);

        /* Save the descriptor to the socket structure */
        ptr->socket_in = sockfd;

        /* Bind the socket */
        if (bind (sockfd, addr.ai_addr, addr.ai_addrlen) != 0) {
            printf ("Socket %p: bind error %d\n", ptr, errno);
            return 0;
        }

        /* Allow 5 connections on the incoming queue */
        if (listen (sockfd, 5)) {
            printf ("Socket %p: listen error %d\n", ptr, errno);
            return 0;
        }
    }

    /* Configure the client/output socket */
    else {
        struct addrinfo addr;
        get_remote_address (ptr, &addr);

        /* Save the descriptor to the socket structure */
        ptr->socket_out = sockfd;
    }

    return 1;
}

/**
 * Initiates the use of the Windows Sockets API for the client process
 */
void Sockets_Init()
{
#ifdef WIN32
    if (WSAStartup (MAKEWORD (2, 2), &socket_data) != 0) {
        printf ("Cannot initialize WSA, error code: %d", WSAGetLastError());
        exit (EXIT_FAILURE);
    }
#endif
}

/**
 * Initializes the data of the given socket structure
 *
 * \param ptr a pointer to a \c DS_Socket structure
 */
int DS_SocketOpen (DS_Socket* ptr)
{
    /* Pointer is NULL */
    if (!ptr)
        return 0;

    /* Socket is already initialized */
    else if (ptr->initialized)
        return 0;

    /* Initialize input & output sockets */
    int in = open_socket (ptr, 1);
    int ot = open_socket (ptr, 0);

    /* Update the socket variables */
    ptr->initialized = 1;
    return in && ot;
}

/**
 * Closes the given socket
 *
 * \param ptr a pointer to a \c DS_Socket structure
 */
void DS_SocketClose (DS_Socket* ptr)
{
    if (ptr) {
        ptr->initialized = 0;
        close_socket (ptr->socket_in);
        close_socket (ptr->socket_out);
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
    if (!ptr || DS_StringIsEmpty (buf))
        return 0;

    if (!ptr->initialized || ptr->disabled)
        return 0;

    return send (ptr->socket_out, buf, sdslen (buf), 0);
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
    if (!ptr)
        return 0;

    if (ptr->disabled)
        return 0;

    accept_connection (ptr);
    return recv (ptr->socket_tmp, buf, 1024, 0);
}
