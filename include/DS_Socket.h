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
#include "DS_Types.h"

struct DS_SocketInfo;

typedef struct {
    sds address;
    int disabled;
    int broadcast;
    int input_port;
    int output_port;
    DS_SocketType type;
    struct DS_SocketInfo* info;
} DS_Socket;

/* For socket initialization */
extern DS_Socket DS_SocketEmpty();

/* Module functions */
extern void Sockets_Init();
extern void Sockets_Close();

/* Socket initializer and destructor functions */
extern void DS_SocketOpen (DS_Socket* ptr);
extern void DS_SocketClose (DS_Socket* ptr);

/* I/O functions */
extern int DS_SocketSend (DS_Socket* ptr, sds data);
extern int DS_SocketRead (DS_Socket* ptr, sds data);
extern void DS_SocketChangeAddress (DS_Socket* ptr, sds address);

#ifdef __cplusplus
}
#endif

#endif
