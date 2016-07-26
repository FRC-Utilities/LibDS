/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "DS_Private.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int in_socket;
int out_socket;

char* data;
struct sockaddr_in in_address;
struct sockaddr_in dummy_addr;
struct sockaddr_in out_address;

DS_Bool netconsole_init()
{
    /* Initialize the sockets */
    in_socket = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    out_socket = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    /* Configure the input address */
    memset (&in_address, 0, sizeof (in_address));
    in_address.sin_family = AF_INET;
    in_address.sin_addr.s_addr = htonl (INADDR_ANY);

    /* Configure the output address */
    memset (&out_address, 0, sizeof (out_address));
    out_address.sin_family = AF_INET;
    out_address.sin_addr.s_addr = htonl (INADDR_ANY);

    /* Configure the dummy/invalid address */
    memset (&dummy_addr, 0, sizeof (dummy_addr));
    dummy_addr.sin_family = AF_UNSPEC;
    in_address.sin_addr.s_addr = htonl (INADDR_NONE);

    /* Initialize netconsole data string */
    data = "";

    /* Return DS_TRUE if both sockets have been initialzed */
    return (in_socket != DS_INVALID) && (out_socket != DS_INVALID);
}

void netconsole_close()
{
    shutdown (in_socket, SHUT_RDWR);
    shutdown (out_socket, SHUT_RDWR);
}

void netconsole_read()
{
}

const char* netconsole_data()
{
    return data;
}

void netconsole_set_input_port (const int port)
{
    in_address.sin_port = port;
    bind (in_socket, (struct sockaddr*) &dummy_addr, sizeof (dummy_addr));
    bind (in_socket, (struct sockaddr*) &in_address, sizeof (in_address));
}

void netconsole_set_output_port (const int port)
{
    out_address.sin_port = (in_port_t) port;
}

void netconsole_send_message (const char* message)
{
    if (out_address.sin_port != (in_port_t) DS_INVALID) {
        sendto (out_socket,
                message,
                strlen (message) + 1, 0,
                (struct sockaddr*) &in_address,
                sizeof (in_address));
    }
}
