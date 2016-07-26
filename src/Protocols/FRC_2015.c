/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "DS_Protocol.h"
#include "../DS_Private.h"

enum Protocol2015 {
    cTest               = 0x01,
    cEnabled            = 0x04,
    cAutonomous         = 0x02,
    cTeleoperated       = 0x00,
    cFMS_Attached       = 0x08,
    cEmergencyStop      = 0x80,
    cRequestReboot      = 0x08,
    cRequestNormal      = 0x80,
    cRequestUnconnected = 0x00,
    cRequestRestartCode = 0x04,
    cFMS_RadioPing      = 0x10,
    cFMS_RobotPing      = 0x08,
    cFMS_RobotComms     = 0x20,
    cFMS_DS_Version     = 0x00,
    cTagDate            = 0x0f,
    cTagGeneral         = 0x01,
    cTagJoystick        = 0x0c,
    cTagTimezone        = 0x10,
    cRed1               = 0x00,
    cRed2               = 0x01,
    cRed3               = 0x02,
    cBlue1              = 0x03,
    cBlue2              = 0x04,
    cBlue3              = 0x05,
    cRequestTime        = 0x01,
    cRobotHasCode       = 0x20,
    cVoltageBrownout    = 0x10,
};

static Protocol* instance = NULL;

static int sent_fms_packets = 0;
static int sent_radio_packets = 0;
static int sent_robot_packets = 0;

DS_Bool read_fms_packet (const char* data)
{
    return DS_TRUE;
}

DS_Bool read_radio_packet (const char* data)
{
    return DS_TRUE;
}

DS_Bool read_robot_packet (const char* data)
{
    return DS_TRUE;
}

char* create_fms_packet (const char* data)
{
    return data;
}

char* create_radio_packet (const char* data)
{
    return data;
}

char* create_robot_packet (const char* data)
{
    return data;
}

char* recommended_fms_address (const char* string)
{
    return string;
}

char* recommended_radio_address (const char* string)
{
    return string;
}

char* recommended_robot_address (const char* string)
{
    return string;
}

Protocol* DS_Protocol2015()
{
    /* Ensure that protocol is created only once */
    if (!instance) {
        struct _protocol* base = (struct _protocol*) malloc (sizeof (struct _protocol));

        /* Setup the intervals */
        base->fmsInterval = 500;
        base->robotInterval = 20;
        base->radioInterval = DS_INVALID;

        /* Setup input ports */
        base->fmsInputPort = 1120;
        base->robotInputPort = 1150;
        base->radioInputPort = DS_INVALID;

        /* Setup output ports */
        base->fmsOutputPort = 1160;
        base->robotOutputPort = 1110;
        base->radioOutputPort = DS_INVALID;

        /* Setup netconsole */
        base->netconsoleInputPort = 6666;
        base->netconsoleOutputPort = DS_INVALID;

        /* Setup socket types */
        base->fmsSocket = DS_SOCKET_UDP;
        base->robotSocket = DS_SOCKET_UDP;
        base->radioSocket = DS_SOCKET_INVALID;

        /* Setup joystick properties */
        base->maxHats = 1;
        base->maxAxes = 12;
        base->maxButtons = 12;
        base->maxJoysticks = 6;

        /* Setup interpreter functions */
        base->readFMSPacket = &read_fms_packet;
        base->readRadioPacket = &read_radio_packet;
        base->readRobotPacket = &read_robot_packet;

        /* Setup generator functions */
        base->createFMSPacket = &create_fms_packet;
        base->createRadioPacket = &create_radio_packet;
        base->createRobotPacket = &create_robot_packet;

        /* Setup default address functions */
        base->recommendedFMSAddress = &recommended_fms_address;
        base->recommendedRadioAddress = &recommended_radio_address;
        base->recommendedRobotAddress = &recommended_robot_address;

        /* Create protocol */
        instance = (Protocol*) malloc (sizeof (Protocol));
        instance->base = base;
    }

    /* Return the protocol pointer */
    return instance;
}

