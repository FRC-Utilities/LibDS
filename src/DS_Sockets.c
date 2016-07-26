/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "DS_Private.h"
#include "DS_Protocol.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

//==============================================================================
// Socket definition
//==============================================================================

/* FMS sockets */
int fms_udp_socket_in;
int fms_tcp_socket_in;
int fms_udp_socket_out;
int fms_tcp_socket_out;

/* Radio sockets */
int radio_udp_socket_in;
int radio_tcp_socket_in;
int radio_udp_socket_out;
int radio_tcp_socket_out;

/* Robot sockets */
int robot_udp_socket_in;
int robot_tcp_socket_in;
int robot_udp_socket_out;
int robot_tcp_socket_out;

/* TCP/UDP selectors */
DS_SocketType fms_socket_type;
DS_SocketType radio_socket_type;
DS_SocketType robot_socket_type;

//==============================================================================
// Module init & close
//==============================================================================

DS_Bool sockets_init()
{
    return DS_TRUE;
}

void sockets_close()
{

}

//==============================================================================
// FMS networking
//==============================================================================

int fms_socket_in()
{
    if (fms_socket_type == DS_SOCKET_TCP)
        return fms_tcp_socket_in;

    return fms_udp_socket_in;
}

int fms_socket_out()
{
    if (fms_socket_type == DS_SOCKET_TCP)
        return fms_tcp_socket_out;

    return fms_udp_socket_out;
}

void update_fms_callbacks()
{

}

void sockets_send_to_fms()
{
}

void sockets_set_fms_address (const char* ip)
{

}

void sockets_set_fms_input_port (const int port)
{

}

void sockets_set_fms_output_port (const int port)
{

}

void sockets_set_default_fms_address (const char* ip)
{

}

void sockets_set_fms_socket_type (const DS_SocketType type)
{

}

//==============================================================================
// Radio networking
//==============================================================================

int radio_socket_in()
{
    if (radio_socket_type == DS_SOCKET_TCP)
        return radio_tcp_socket_in;

    return radio_udp_socket_in;
}

int radio_socket_out()
{
    if (radio_socket_type == DS_SOCKET_TCP)
        return radio_tcp_socket_out;

    return radio_udp_socket_out;
}

void update_radio_callbacks()
{

}

void sockets_send_to_radio()
{

}

void sockets_set_radio_address (const char* ip)
{
}

void sockets_set_radio_input_port (const int port)
{

}

void sockets_set_radio_output_port (const int port)
{

}

void sockets_set_default_radio_address (const char* ip)
{
}

void sockets_set_radio_socket_type (const DS_SocketType type)
{

}

//==============================================================================
// Robot networking
//==============================================================================

int robot_socket_in()
{
    if (robot_socket_type == DS_SOCKET_TCP)
        return robot_tcp_socket_in;

    return robot_udp_socket_in;
}

int robot_socket_out()
{
    if (robot_socket_type == DS_SOCKET_TCP)
        return robot_tcp_socket_out;

    return robot_udp_socket_out;
}

void update_robot_callbacks()
{

}

int robot_socket()
{

}

void sockets_send_to_robot()
{

}

void sockets_set_robot_address (const char* ip)
{

}

void sockets_set_robot_input_port (const int port)
{

}

void sockets_set_robot_output_port (const int port)
{

}

void sockets_set_default_robot_address (const char* ip)
{

}

void sockets_set_robot_socket_type (const DS_SocketType type)
{

}
