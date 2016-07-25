/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "DS_Private.h"
#include "DS_Protocol.h"

#include <stdlib.h>

static Protocol* PROTOCOL;

int DS_ProtocolFMSInterval()
{
    if (DS_CurrentProtocol())
        return DS_CurrentProtocol()->fmsInterval;

    return DS_INVALID;
}

int DS_ProtocolRadioInterval()
{
    if (DS_CurrentProtocol())
        return DS_CurrentProtocol()->radioInterval;

    return DS_INVALID;
}

int DS_ProtocolRobotInterval()
{
    if (DS_CurrentProtocol())
        return DS_CurrentProtocol()->robotInterval;

    return DS_INVALID;
}

int DS_ProtocolFMSInputPort()
{
    if (DS_CurrentProtocol())
        return DS_CurrentProtocol()->fmsInputPort;

    return DS_INVALID;
}

int DS_ProtocolRadioInputPort()
{
    if (DS_CurrentProtocol())
        return DS_CurrentProtocol()->radioInputPort;

    return DS_INVALID;
}

int DS_ProtocolRobotInputPort()
{
    if (DS_CurrentProtocol())
        return DS_CurrentProtocol()->robotInputPort;

    return DS_INVALID;
}

int DS_ProtocolFMSOutputPort()
{
    if (DS_CurrentProtocol())
        return DS_CurrentProtocol()->fmsOutputPort;

    return DS_INVALID;
}

int DS_ProtocolRadioOutputPort()
{
    if (DS_CurrentProtocol())
        return DS_CurrentProtocol()->radioOutputPort;

    return DS_INVALID;
}

int DS_ProtocolRobotOutputPort()
{
    if (DS_CurrentProtocol())
        return DS_CurrentProtocol()->robotOutputPort;

    return DS_INVALID;
}

DS_SocketType DS_ProtocolFMSSocketType()
{
    if (DS_CurrentProtocol())
        return DS_CurrentProtocol()->fmsSocket;

    return DS_SOCKET_INVALID;
}

DS_SocketType DS_ProtocolRadioSocketType()
{
    if (DS_CurrentProtocol())
        return DS_CurrentProtocol()->radioSocket;

    return DS_SOCKET_INVALID;
}

DS_SocketType DS_ProtocolRobotSocketType()
{
    if (DS_CurrentProtocol())
        return DS_CurrentProtocol()->robotSocket;

    return DS_SOCKET_INVALID;
}

struct _protocol* DS_CurrentProtocol()
{
    return PROTOCOL->base;
}

void DS_ProtocolDecomission()
{
    if (PROTOCOL)
        free (PROTOCOL);
}

void DS_ProtocolConfigure (Protocol* protocol)
{
    if (protocol) {
        DS_ProtocolDecomission();
        PROTOCOL = protocol;

        /* Change socket types */
        sockets_set_fms_socket_type (DS_CurrentProtocol()->fmsSocket);
        sockets_set_radio_socket_type (DS_CurrentProtocol()->radioSocket);
        sockets_set_robot_socket_type (DS_CurrentProtocol()->robotSocket);

        /* Change ports */
        sockets_set_fms_input_port (DS_CurrentProtocol()->fmsInputPort);
        sockets_set_fms_output_port (DS_CurrentProtocol()->fmsOutputPort);
        sockets_set_radio_input_port (DS_CurrentProtocol()->radioInputPort);
        sockets_set_robot_input_port (DS_CurrentProtocol()->robotInputPort);
        sockets_set_radio_output_port (DS_CurrentProtocol()->radioOutputPort);
        sockets_set_robot_output_port (DS_CurrentProtocol()->robotOutputPort);

        /* Change default addresses */
        sockets_set_default_fms_address (DS_CurrentProtocol()->recommendedFMSAddress);
        sockets_set_default_radio_address (
            DS_CurrentProtocol()->recommendedRadioAddress);
        sockets_set_default_robot_address (
            DS_CurrentProtocol()->recommendedRobotAddress);

        /* Change NetConsole settings */
        netconsole_set_input_port (DS_CurrentProtocol()->netconsoleInputPort);
        netconsole_set_output_port (DS_CurrentProtocol()->netconsoleOutputPort);
    }
}
