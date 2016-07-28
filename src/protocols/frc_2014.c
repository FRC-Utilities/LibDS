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

#include <math.h>

#include "DS_Utils.h"
#include "DS_Config.h"
#include "DS_Joysticks.h"
#include "DS_Protocols.h"

enum Control {
    cEnabled          = 0x20,
    cTestMode         = 0x02,
    cAutonomous       = 0x10,
    cTeleoperated     = 0x00,
    cFMS_Attached     = 0x08,
    cResyncComms      = 0x04,
    cRebootRobot      = 0x80,
    cEmergencyStopOn  = 0x00,
    cEmergencyStopOff = 0x40,
};

enum Stations {
    cPosition1    = 0x31,
    cPosition2    = 0x32,
    cPosition3    = 0x33,
    cAllianceRed  = 0x52,
    cAllianceBlue = 0x42,
};

enum FMS {
    cFMSAutonomous   = 0x53,
    cFMSTeleoperated = 0x43,
};

static uint32_t CRC;
static int SENT_FMS_PACKETS;
static int SENT_ROBOT_PACKETS;

static bool RESYNC;
static bool REBOOT_ROBOT;
static bool RESTART_CODE;
static DS_Protocol* PROTOCOL;

static uint8_t get_control_code()
{
    uint8_t code = cEmergencyStopOff;
    uint8_t enabled = CFG_GetRobotEnabled() ? cEnabled : 0x00;

    /* Get the control mode (Test, Auto or TeleOp) */
    switch (CFG_GetControlMode()) {
    case DS_CONTROL_TEST:
        code |= enabled + cTestMode;
        break;
    case DS_CONTROL_AUTONOMOUS:
        code |= enabled + cAutonomous;
        break;
    case DS_CONTROL_TELEOPERATED:
        code |= enabled + cTeleoperated;
        break;
    default:
        code = cEmergencyStopOff;
        break;
    }

    /* Resync robot communications */
    if (RESYNC)
        code |= cResyncComms;

    /* Let robot know if we are connected to FMS */
    if (CFG_GetFMSCommunications())
        code |= cFMS_Attached;

    /* Set the emergency stop state */
    if (CFG_GetEmergencyStopped())
        code = cEmergencyStopOn;

    /* Send the reboot code if required */
    if (REBOOT_ROBOT)
        code = cRebootRobot;

    return code;
}

static uint8_t get_alliance_code()
{
    if (CFG_GetAlliance() == DS_ALLIANCE_RED)
        return cAllianceRed;

    return cAllianceBlue;
}

static uint8_t get_position_code()
{
    uint8_t code = cPosition1;

    switch (CFG_GetPosition()) {
    case DS_POSITION_1:
        code = cPosition1;
        break;
    case DS_POSITION_2:
        code = cPosition2;
        break;
    case DS_POSITION_3:
        code = cPosition3;
        break;
    }

    return code;
}

static uint8_t get_digital_inputs()
{
    return 0x00;
}

static void add_joystick_data (char* data, int offset)
{
    int pos = offset;

    for (int i = 0; i < PROTOCOL->maxJoysticks; ++i) {
        /* Get joystick properties */
        int num_axes = DS_GetJoystickNumAxes (i);
        int num_buttons = DS_GetJoystickNumButtons (i);
        bool joystick_exists = DS_GetJoystickCount() > i;

        /* Add axis data */
        for (int axis = 0; axis < PROTOCOL->maxAxisCount; ++i) {
            /* Add real axis value */
            if (joystick_exists && axis < num_axes)
                data [pos] = (uint8_t) (DS_GetJoystickAxis (i, axis) * 127);

            /* Add neutral values */
            else
                data [pos] = (uint8_t) 0x00;

            /* Increase offset */
            ++pos;
        }

        /* Calculate button code */
        int button_flags = 0x00;
        for (int button = 0; button < num_buttons; ++button) {
            if (joystick_exists && button < num_buttons)
                button_flags |= (uint8_t) pow (2, button);
        }

        /* Add button data */
        data [pos] = (button_flags & 0xff00) >> 8;
        data [pos + 1] = (button_flags & 0xff);

        /* Increase offset for next joystick */
        pos += 2;
    }
}

static char* fms_address()
{
    return "";
}

static char* radio_address()
{
    return DS_GetStaticIP (10, CFG_GetTeamNumber(), 1);
}

static char* robot_address()
{
    return DS_GetStaticIP (10, CFG_GetTeamNumber(), 2);
}

static uint8_t* create_fms_packet()
{
    return (uint8_t*) malloc (sizeof (uint8_t));
}

static uint8_t* create_radio_packet()
{
    return (uint8_t*) malloc (sizeof (uint8_t));
}

static uint8_t* create_robot_packet()
{
    /* Create a 1024-byte long packet */
    uint8_t* data = (uint8_t*) malloc (sizeof (uint8_t) * 1024);

    /* Add packet index */
    data [0] = (SENT_ROBOT_PACKETS & 0xff00) >> 8;
    data [1] = (SENT_ROBOT_PACKETS & 0xff);

    /* Add control code and digital inputs */
    data [2] = get_control_code();
    data [3] = get_digital_inputs();

    /* Add team number */
    data [4] = (CFG_GetTeamNumber() & 0xff00) >> 8;
    data [5] = (CFG_GetTeamNumber() & 0xff);

    /* Add alliance and position */
    data [6] = get_alliance_code();
    data [6] = get_position_code();

    /* Add joystick data */
    add_joystick_data (data, 8);

    /* Add FRC Driver Station version (same as the one sent by 16.0.1) */
    data [72] = (uint8_t) 0x30;
    data [73] = (uint8_t) 0x34;
    data [74] = (uint8_t) 0x30;
    data [75] = (uint8_t) 0x31;
    data [76] = (uint8_t) 0x31;
    data [77] = (uint8_t) 0x36;
    data [78] = (uint8_t) 0x30;
    data [79] = (uint8_t) 0x30;

    /* Add CRC32 checksum */
    uint8_t checksum = DS_CRC32 (CRC, data, sizeof (data));
    data[1020] = (checksum & 0xff000000) >> 24;
    data[1021] = (checksum & 0xff0000) >> 16;
    data[1022] = (checksum & 0xff00) >> 8;
    data[1023] = (checksum & 0xff);

    /* Return address of data */
    return data;
}

static bool read_fms_packet (const uint8_t* data)
{
    /* Data pointer is invalid */
    if (data == NULL)
        return false;

    /* The packet is long enough to be read */
    if (sizeof (data) > 8)
        return true;

    return false;
}

static bool read_radio_packet (const uint8_t* data)
{
    (void) data;
    return false;
}

bool read_robot_packet (const uint8_t* data)
{
    /* Data pointer is invalid */
    if (data == NULL)
        return false;

    /* The packet is long enough to be read */
    if (sizeof (data) >= 1024) {
        uint8_t opcode = data [0];
        uint8_t integer = data [1];
        uint8_t decimal = data [2];

        /* Parse the voltage */

        /* Check if robot is e-stopepd */
        CFG_SetEmergencyStopped (opcode == cEmergencyStopOn);

        /* Update code status */
        CFG_SetRobotCode ((integer != 0x37) && (decimal != 0x37));

        /* Packet read successfully */
        return true;
    }

    return false;
}

static void reset_fms()
{
    /* Nothing to do */
}

static void reset_radio()
{
    /* Nothing to do */
}

static void reset_robot()
{
    RESYNC = 1;
    RESTART_CODE = 0;
    REBOOT_ROBOT = 0;
}

static void reboot_robot()
{
    REBOOT_ROBOT = 1;
}

void restart_robot_code()
{
    RESTART_CODE = 1;
}

DS_Protocol* DS_GetProtocolFRC_2014()
{
    if (!PROTOCOL) {
        /* Initialize protocol variables */
        CRC = 0;
        RESYNC = 0;
        RESTART_CODE = 0;
        REBOOT_ROBOT = 0;
        SENT_FMS_PACKETS = 0;
        SENT_ROBOT_PACKETS = 0;

        /* Initialize protocol */
        PROTOCOL = (DS_Protocol*) malloc (sizeof (DS_Protocol));

        /* Set address functions */
        PROTOCOL->fms_address = &fms_address;
        PROTOCOL->radio_address = &radio_address;
        PROTOCOL->robot_address = &robot_address;

        /* Set packet generator functions */
        PROTOCOL->create_fms_packet = &create_fms_packet;
        PROTOCOL->create_radio_packet = &create_radio_packet;
        PROTOCOL->create_robot_packet = &create_robot_packet;

        /* Set packet interpretation functions */
        PROTOCOL->read_fms_packet = &read_fms_packet;
        PROTOCOL->read_radio_packet = &read_radio_packet;
        PROTOCOL->read_robot_packet = &read_robot_packet;

        /* Set reset functions */
        PROTOCOL->reset_fms = &reset_fms;
        PROTOCOL->reset_radio = &reset_radio;
        PROTOCOL->reset_robot = &reset_robot;

        /* Set misc. functions */
        PROTOCOL->reboot_robot = &reboot_robot;
        PROTOCOL->restart_robot_code = &restart_robot_code;

        /* Set packet intervals */
        PROTOCOL->fmsInterval = 500;
        PROTOCOL->radioInterval = 0;
        PROTOCOL->robotInterval = 20;

        /* Set joystick properties */
        PROTOCOL->maxJoysticks = 4;
        PROTOCOL->maxAxisCount = 6;
        PROTOCOL->maxHatsCount = 0;
        PROTOCOL->maxButtonCount = 10;

        /* Define FMS socket properties */
        DS_Socket fms_socket;
        fms_socket.input_port = 1120;
        fms_socket.output_port = 1160;
        fms_socket.type = DS_SOCKET_UDP;

        /* Define radio socket properties */
        DS_Socket radio_socket;
        radio_socket.type = DS_SOCKET_INVALID;

        /* Define robot socket properties */
        DS_Socket robot_socket;
        robot_socket.input_port = 1150;
        robot_socket.output_port = 1110;
        robot_socket.type = DS_SOCKET_UDP;

        /* Assign socket objects */
        PROTOCOL->fmsSocket = fms_socket;
        PROTOCOL->radioSocket = radio_socket;
        PROTOCOL->robotSocket = robot_socket;
    }

    return PROTOCOL;
}
