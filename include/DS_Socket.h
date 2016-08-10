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

#ifndef _LIB_DS_SOCKET_H
#define _LIB_DS_SOCKET_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sds.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

#include "DS_Types.h"

typedef struct _socket {
    sds address;
    int disabled;
    int initialized;
    DS_SocketType type;
    uintptr_t input_socket;
    uintptr_t output_socket;
    struct sockaddr_in sockaddr;
    unsigned short int input_port;
    unsigned short int output_port;
} DS_Socket;

extern void Sockets_Init();
extern int DS_SocketOpen (DS_Socket* ptr);
extern void DS_SocketClose (DS_Socket* ptr);
extern int DS_SocketSend (DS_Socket* ptr, sds buf);
extern int DS_SocketRead (DS_Socket* ptr, sds buf);

#ifdef __cplusplus
}
#endif

#endif
