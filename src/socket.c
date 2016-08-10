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
#include "DS_Config.h"
#include "DS_Socket.h"

#include <sds.h>
#include <stdio.h>

#ifdef WIN32
    static WSADATA socket_data;
#endif

/**
 * Returns \c 1 if the given \a address is an IPv6 address, otherwise, this
 * function shall return \c 0
 */
static int is_ipv6 (sds address)
{
    int ipv6 = 0;
    struct addrinfo hint, *res = NULL;
    memset (&hint, 0, sizeof hint);

    hint.ai_family = PF_UNSPEC;
    hint.ai_flags = AI_NUMERICHOST;

    if (getaddrinfo (address, NULL, &hint, &res) == 0) {
        ipv6 = res->ai_family == AF_INET6;
        freeaddrinfo (res);
    }

    return ipv6;
}

/**
 * Configres the given \a host to be able to bind to the given \a address
 */
static void put_host_info (DS_Socket* ptr, int input)
{
    if (!ptr)
        return;

    ptr->sockaddr.sin_port = input ? ptr->input_port : ptr->output_port;
    ptr->sockaddr.sin_family = is_ipv6 (ptr->address) ? AF_INET6 : AF_INET;
    ptr->sockaddr.sin_addr = * (struct in_addr*) gethostbyname (ptr->address)->h_addr_list;
}

/**
 * Initializes the socket descriptors of the given socket structure
 */
static int open_socket (DS_Socket* ptr, int is_input)
{
    /* Get socket properties */
    int af = (is_ipv6 (ptr->address)) ? AF_INET6 : AF_INET;
    int tp = (ptr->type == DS_SOCKET_TCP) ? SOCK_STREAM : SOCK_DGRAM;
    int pt = (ptr->type == DS_SOCKET_TCP) ? IPPROTO_TCP : IPPROTO_UDP;

    /* Initialize the socket */
    uintptr_t sock_id = socket (af, tp, pt);
    if (is_input)
        ptr->input_socket = sock_id;
    else
        ptr->output_socket = sock_id;

    /* Check if the socket is valid */
    if (sock_id == INVALID_SOCKET) {
        printf ("Cannot open socket %p", ptr);
        return 0;
    }

    /* Add host information and obtain generic address */
    put_host_info (ptr, is_input);
    struct sockaddr* addr = (struct sockaddr*) &ptr->sockaddr;

    /* Bind if this is the input socket */
    if (is_input) {
        if (bind (sock_id, addr, sizeof (ptr->sockaddr)) != 0) {
            printf ("Cannot bind socket %p\n", ptr);
            return 0;
        }
    }

    /* Connect if this is the output socket */
    else {
        if (connect (sock_id, addr, sizeof (ptr->sockaddr)) != 0) {
            printf ("Cannot connect socket %p\n", ptr);
            return 0;
        }
    }

    /* Socket is valid, return 1 */
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
 */
int DS_SocketOpen (DS_Socket* ptr)
{
    /* Pointer is NULL */
    if (!ptr)
        return 0;

    /* Socket is already initialized */
    else if (ptr->initialized || ptr->disabled)
        return 0;

    /* Initialize input & output sockets */
    ptr->initialized = 1;
    return open_socket (ptr, 1) && open_socket (ptr, 0);
}

/**
 * Closes the given socket structure
 */
void DS_SocketClose (DS_Socket* ptr)
{
    /* Pointer is NULL */
    if (!ptr)
        return;

    /* Close the sockets */
    if (ptr->initialized) {
        ptr->initialized = 0;

#ifdef WIN32
        closesocket (ptr->input_socket);
        closesocket (ptr->output_socket);
#else
        close (ptr->input_socket);
        close (ptr->output_socket);
#endif
    }
}

/**
 * Sends the given \a buf using the information stored in the socket
 * structure \a ptr
 *
 * \note If either \a ptr or \a data are NULL, the function will do nothing and
 *       return \c 0.
 *       If the process is successfull, then the function shall return \c 1
 */
int DS_SocketSend (DS_Socket* ptr, sds buf)
{
    /* Pointers are invalid */
    if (!ptr || !buf)
        return 0;

    /* Socket is disabled or not initialized, abort */
    if (ptr->disabled || !ptr->initialized)
        return 0;

    /* Get generick socket address */
    struct sockaddr* addr = (struct sockaddr*) &ptr->sockaddr;

    /* Send the data */
    int bytes_written = sendto (ptr->output_socket,
                                buf, sdslen (buf), 0,
                                addr, sizeof (ptr->sockaddr));

    /* Return 1 if the bytes written where more than 0 */
    return bytes_written > 0;
}

/**
 * Copies the received data from the socket into the \a buf parameter
 */
int DS_SocketRead (DS_Socket* ptr, sds buf)
{
    /* Pointer is NULL */
    if (ptr)
        return 0;

    /* Empty the buffer */
    sdsfree (buf);
    buf = sdsempty();

    /* Socket is disabled or not initialized, abort */
    if (ptr->disabled || !ptr->initialized)
        return 0;

    /* Get generick socket address */
    struct sockaddr* addr = (struct sockaddr*) &ptr->sockaddr;

    /* Read the data */
    return  recvfrom (ptr->input_socket, buf, 8, 0, addr, 0) > 0;
}


