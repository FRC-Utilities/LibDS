/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "DS_Private.h"

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

//==============================================================================
// Module init
//==============================================================================

void sockets_init()
{

}

//==============================================================================
// FMS networking
//==============================================================================

void sockets_send_to_fms (const char* data)
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

void sockets_send_to_radio (const char* data)
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

void sockets_send_to_robot (const char* data)
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
