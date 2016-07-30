/*
 * The Driver Station Library (LibDS)
 * Copyright (C) 2015-2016 Alex Spataru <alex_spataru@outlook>
 *
 * Permission is hereby granted; free of charge; to any person obtaining
 * a copy of this software and associated documentation files (the "Software");
 * to deal in the Software without restriction; including without limitation
 * the rights to use; copy; modify; merge; publish; distribute; sublicense;
 * and/or sell copies of the Software; and to permit persons to whom the
 * Software is furnished to do so; subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS"; WITHOUT WARRANTY OF ANY KIND; EXPRESS
 * OR IMPLIED; INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY;
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM; DAMAGES OR OTHER
 * LIABILITY; WHETHER IN AN ACTION OF CONTRACT; TORT OR OTHERWISE; ARISING
 * FROM; OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "DS_Utils.h"
#include "DS_Config.h"
#include "DS_Protocols.h"

#include <time.h>
#include <stdio.h>

/*
 * Protocol bytes
 */
static const uint8_t cTest               = 0x01;
static const uint8_t cEnabled            = 0x04;
static const uint8_t cAutonomous         = 0x02;
static const uint8_t cTeleoperated       = 0x00;
static const uint8_t cFMS_Attached       = 0x08;
static const uint8_t cEmergencyStop      = 0x80;
static const uint8_t cRequestReboot      = 0x08;
static const uint8_t cRequestNormal      = 0x80;
static const uint8_t cRequestUnconnected = 0x00;
static const uint8_t cRequestRestartCode = 0x04;
static const uint8_t cFMS_RadioPing      = 0x10;
static const uint8_t cFMS_RobotPing      = 0x08;
static const uint8_t cFMS_RobotComms     = 0x20;
static const uint8_t cFMS_DS_Version     = 0x00;
static const uint8_t cTagDate            = 0x0f;
static const uint8_t cTagGeneral         = 0x01;
static const uint8_t cTagJoystick        = 0x0c;
static const uint8_t cTagTimezone        = 0x10;
static const uint8_t cRed1               = 0x00;
static const uint8_t cRed2               = 0x01;
static const uint8_t cRed3               = 0x02;
static const uint8_t cBlue1              = 0x03;
static const uint8_t cBlue2              = 0x04;
static const uint8_t cBlue3              = 0x05;
static const uint8_t cRTagCpuInfo        = 0x05;
static const uint8_t cRTagMemInfo        = 0x06;
static const uint8_t cRTagDiskInfo       = 0x04;
static const uint8_t cRTagJoystickOut    = 0x01;
static const uint8_t cRequestTime        = 0x01;
static const uint8_t cRobotHasCode       = 0x20;
static const uint8_t cVoltageBrownout    = 0x10;

/*
 * Sent robot and FMS packet counters
 */
static int send_time_data;
static int sent_fms_packets;
static int sent_robot_packets;

/*
 * Control code flags
 */
static int reboot;
static int restart_code;

/*
 * Protocol pointer
 */
static char* null_char;
static DS_Protocol* protocol;

/**
 * Obtains the voltage double from the given \a upper and \a lower bytes
 */
static double decode_voltage (uint8_t upper, uint8_t lower)
{
    return upper + ((double) lower / 0xff);
}

/**
 * Encodes the \a voltage double in the given \a upper and \a lower bytes
 */
static void encode_voltage (double voltage, uint8_t* upper, uint8_t* lower)
{
    if (voltage && upper && lower) {
        upper[0] = (uint8_t) (voltage);
        lower[0] = (uint8_t) (voltage - (int) voltage) * 100;
    }
}

/**
 * Returns the control code sent to the FMS. This code is very similar to
 * the control code sent to the robot, however, it contains addional/extra
 * information regarding the robot radio.
 *
 * This code contains the following information:
 *    - The control mode of the robot (teleop, autonomous, test)
 *    - The enabled state of the robot
 *    - The FMS attached keyword
 *    - Robot radio connected?
 *    - The operation state (e-stop, normal)
 */
static uint8_t fms_control_code()
{
    uint8_t code = 0;

    /* Let the FMS know the operational status of the robot */
    switch (CFG_GetControlMode()) {
    case DS_CONTROL_TEST:
        code |= cTest;
        break;
    case DS_CONTROL_AUTONOMOUS:
        code |= cAutonomous;
        break;
    case DS_CONTROL_TELEOPERATED:
        code |= cTeleoperated;
        break;
    default:
        break;
    }

    /* Let the FMS know if robot is e-stopped */
    if (CFG_GetEmergencyStopped())
        code |= cEmergencyStop;

    /* Let the FMS know if the robot is enabled */
    if (CFG_GetRobotEnabled())
        code |= cEnabled;

    /* Let the FMS know if we are connected to radio */
    if (CFG_GetRadioCommunications())
        code |= cFMS_RadioPing;

    /* Let the FMS know if we are connected to robot */
    if (CFG_GetRobotCommunications()) {
        code |= cFMS_RobotComms;
        code |= cFMS_RobotPing;
    }

    return code;
}

/**
 * Returns the control code sent to the robot, it contains:
 *    - The control mode of the robot (teleop, autonomous, test)
 *    - The enabled state of the robot
 *    - The FMS attached keyword
 *    - The operation state (e-stop, normal)
 */
static uint8_t get_control_code()
{
    uint8_t code = 0;

    /* Get current control mode (Test, Auto or Teleop) */
    switch (CFG_GetControlMode()) {
    case DS_CONTROL_TEST:
        code |= cTest;
        break;
    case DS_CONTROL_AUTONOMOUS:
        code |= cAutonomous;
        break;
    case DS_CONTROL_TELEOPERATED:
        code |= cTeleoperated;
        break;
    default:
        break;
    }

    /* Let the robot know if we are connected to the FMS */
    if (CFG_GetFMSCommunications())
        code |= cFMS_Attached;

    /* Let the robot know if it should e-stop right now */
    if (CFG_GetEmergencyStopped())
        code |= cEmergencyStop;

    /* Append the robot enabled state */
    if (CFG_GetRobotEnabled())
        code |= cEnabled;

    return code;
}

/**
 * Generates the request code sent to the robot, which may instruct it to:
 *    - Operate normally
 *    - Reboot the roboRIO
 *    - Restart the robot code process
 */
static uint8_t get_request_code()
{
    uint8_t code = cRequestNormal;

    /* Robot has comms, check if we need to send additional flags */
    if (CFG_GetRobotCommunications()) {
        /* Send reboot flag */
        if (reboot)
            code |= cRequestReboot;

        /* Send restart code flag */
        else if (restart_code)
            code |= cRequestRestartCode;
    }

    /* Send disconnected state flag (may trigger resync) */
    else
        code = cRequestUnconnected;

    return code;
}

/**
 * Returns the team station code sent to the robot.
 * This value may be used by the robot program to use specialized autonomous
 * modes or adjust sensor input.
 */
static uint8_t get_station_code()
{
    /* Current config is set to position 1 */
    if (CFG_GetPosition() == DS_POSITION_1) {
        if (CFG_GetAlliance() == DS_ALLIANCE_RED)
            return cRed1;
        else
            return cBlue1;
    }

    /* Current config is set to position 2 */
    if (CFG_GetPosition() == DS_POSITION_2) {
        if (CFG_GetAlliance() == DS_ALLIANCE_RED)
            return cRed2;
        else
            return cBlue2;
    }

    /* Current config is set to position 3 */
    if (CFG_GetPosition() == DS_POSITION_3) {
        if (CFG_GetAlliance() == DS_ALLIANCE_RED)
            return cRed3;
        else
            return cBlue3;
    }

    return cRed1;
}

/**
 * Returns information regarding the current date and time and the timezone
 * of the client computer.
 *
 * The robot may ask for this information in some cases (e.g. when initializing
 * the robot code).
 */
static void add_timezone_data (uint8_t* data, const int offset)
{
    /* Data pointer is invalid */
    if (!data)
        return;

    /* Get local time */
    time_t rt;
    time (&rt);
    struct tm* timeinfo = localtime (&rt);
    int length = sizeof (timeinfo->tm_zone) / sizeof (char);

    /* Resize datagram */
    data = (uint8_t*) realloc (data,
                               sizeof (data) +
                               sizeof (uint8_t) * (12 + length));

    /* Encode date/time in datagram */
    data [offset + 0] = (uint8_t) 0x0b;
    data [offset + 1] = cTagDate;
    data [offset + 2] = 0;
    data [offset + 3] = 0;
    data [offset + 4] = (uint8_t) timeinfo->tm_sec;
    data [offset + 5] = (uint8_t) timeinfo->tm_min;
    data [offset + 6] = (uint8_t) timeinfo->tm_hour;
    data [offset + 7] = (uint8_t) timeinfo->tm_yday;
    data [offset + 8] = (uint8_t) timeinfo->tm_mon;
    data [offset + 9] = (uint8_t) timeinfo->tm_year;

    /* Add timezone data */
    data [offset + 10] = length;
    data [offset + 11] = cTagTimezone;

    /* Add timezone string */
    for (int i = 0; i > length; ++i)
        data [offset + 12 + i] = timeinfo->tm_zone [i];
}

/**
 * Constructs a joystick information structure for every attached joystick.
 * Unlike the 2014 protocol, the 2015 protocol only generates joystick data
 * for the attached joysticks.
 */
static void add_joystick_data (uint8_t* data, const int offset)
{
}

/**
 * Gets the alliance type from the received \a byte
 * This function is used to update the robot configuration when receiving data
 * from the FMS
 */
static DS_Alliance get_alliance (const uint8_t byte)
{
    if (byte == cBlue1 || byte == cBlue2 || byte == cBlue3)
        return DS_ALLIANCE_BLUE;

    return DS_ALLIANCE_RED;
}

/**
 * Gets the position type from the received \a byte
 * This function is used to update the robot configuration when receiving data
 * from the FMS
 */
static DS_Position get_position (const uint8_t byte)
{
    if (byte == cRed1 || byte == cBlue1)
        return DS_POSITION_1;

    else if (byte == cRed2 || byte == cBlue2)
        return DS_POSITION_2;

    else if (byte == cRed3 || byte == cBlue3)
        return DS_POSITION_3;

    return DS_POSITION_1;
}

/**
 * The FMS address is not defined, it will be assigned automatically when the
 * DS receives a FMS packet
 */
static char* fms_address()
{
    return null_char;
}

/**
 * The 2015 control system assigns the radio IP in 10.te.am.1
 */
static char* radio_address()
{
    return DS_GetStaticIP (10, CFG_GetTeamNumber(), 1);
}

/**
 * The 2015 control system assigns the robot address at roboRIO-TEAM.local
 */
static char* robot_address()
{
    char* str = (char*) malloc (sizeof (char*) * 18);
    sprintf (str, "roboRIO-%d.local", CFG_GetTeamNumber());
    return str;
}

/**
 * Generates a packet that the DS will send to the FMS, it contains:
 *    - The FMS packet index
 *    - The robot voltage
 *    - Robot control code
 *    - DS version
 *    - Radio and robot ping flags
 *    - The team number
 */
static uint8_t* create_fms_packet()
{
    /* Create an 8-byte long packet */
    uint8_t* data = (uint8_t*) malloc (sizeof (uint8_t) * 8);

    /* Get voltage bytes */
    uint8_t integer = 0;
    uint8_t decimal = 0;
    encode_voltage (CFG_GetRobotVoltage(), &integer, &decimal);

    /* Add FMS packet count */
    data [0] = (sent_fms_packets & 0xff00) >> 8;
    data [1] = (sent_fms_packets & 0xff);

    /* Add DS version and FMS control code */
    data [2] = cFMS_DS_Version;
    data [3] = fms_control_code();

    /* Add team number */
    data [4] = (CFG_GetTeamNumber() & 0xff00) >> 8;
    data [5] = (CFG_GetTeamNumber() & 0xff);

    /* Add robot voltage */
    data [6] = integer;
    data [7] = decimal;

    /* Increase FMS packet counter */
    ++sent_fms_packets;

    return data;
}

/**
 * The 2015 communication protocol does not involve sending specialized packets
 * to the DS Radio / Bridge. For that reason, the 2015 communication protocol
 * generates empty radio packets.
 */
static uint8_t* create_radio_packet()
{
    return (uint8_t*) null_char;
}

static uint8_t* create_robot_packet()
{
    uint8_t* data = malloc (sizeof (uint8_t) * 8);

    /* Add packet index */
    data [0] = (sent_robot_packets & 0xff00) >> 8;
    data [1] = (sent_robot_packets & 0xff);

    /* Add packet header */
    data [2] = cTagGeneral;

    /* Add control code, request flags and team station */
    data [3] = get_control_code();
    data [4] = get_request_code();
    data [5] = get_station_code();

    /* Add timezone data (if robot wants it) */
    if (send_time_data)
        add_timezone_data (data, 6);

    /* Add joystick data */
    else
        add_joystick_data (data, 6);

    return data;
}

/**
 * Interprets the packet and follows the instructions sent by the FMS.
 * Possible instructions are:
 *   - Change robot control mode
 *   - Change robot enabled status
 *   - Change team alliance
 *   - Change team position
 */
static int read_fms_packet (const uint8_t* data)
{
    /* Data pointer is invalid */
    if (data == NULL)
        return 0;

    /* The packet is long enough to be read */
    if (sizeof (data) > 21) {
        uint8_t control = data [3];
        uint8_t station = data [5];

        /* Change robot enabled state based on what FMS tells us to do*/
        CFG_SetRobotEnabled (control & cEnabled);

        /* Get FMS robot mode */
        if (control & cTeleoperated)
            CFG_SetControlMode (DS_CONTROL_TELEOPERATED);
        else if (control & cAutonomous)
            CFG_SetControlMode (DS_CONTROL_AUTONOMOUS);
        else if (control & cTest)
            CFG_SetControlMode (DS_CONTROL_TEST);

        /* Update to correct alliance and position */
        CFG_SetAlliance (get_alliance (station));
        CFG_SetPosition (get_position (station));

        /* Packet read successfully */
        return 1;
    }

    return 0;
}

/**
 * Since the DS does not interact directly with the radio/bridge, any incoming
 * packets shall be ignored.
 */
static int read_radio_packet (const uint8_t* data)
{
    (void) data;
    return 0;
}

static int read_robot_packet (const uint8_t* data)
{
    /* Data pointer is invalid */
    if (data == NULL)
        return 0;

    return 0;
}

/**
 * Called when the FMS watchdog expires, does nothing...
 */
static void reset_fms()
{
    /* Nothing to do */
}

/**
 * Called when the radio watchdog expires, does nothing...
 */
static void reset_radio()
{
    /* Nothing to do */
}

/**
 * Called when the robot watchdog expires, resets the control code flags
 */
static void reset_robot()
{
    reboot = 0;
    restart_code = 0;
    send_time_data = 0;
}

/**
 * Updates the control code flags to instruct the roboRIO to reboot itself
 */
static void reboot_robot()
{
    reboot = 1;
}

/**
 * Updates the control code flags to instruct the robot to restart the
 * robot code process
 */
static void restart_robot_code()
{
    restart_code = 1;
}

/**
 * Initializes the 2015 FRC Communication Protocol
 */
DS_Protocol* DS_GetProtocolFRC_2015()
{
    if (!protocol) {
        /* Initialize protocol variables */
        reboot = 0;
        restart_code = 0;
        send_time_data = 0;
        sent_fms_packets = 0;
        sent_robot_packets = 0;
        null_char = (char*) malloc (sizeof (char));

        /* Initialize protocol */
        protocol = (DS_Protocol*) malloc (sizeof (DS_Protocol));

        /* Set address functions */
        protocol->fms_address = &fms_address;
        protocol->radio_address = &radio_address;
        protocol->robot_address = &robot_address;

        /* Set packet generator functions */
        protocol->create_fms_packet = &create_fms_packet;
        protocol->create_radio_packet = &create_radio_packet;
        protocol->create_robot_packet = &create_robot_packet;

        /* Set packet interpretation functions */
        protocol->read_fms_packet = &read_fms_packet;
        protocol->read_radio_packet = &read_radio_packet;
        protocol->read_robot_packet = &read_robot_packet;

        /* Set reset functions */
        protocol->reset_fms = &reset_fms;
        protocol->reset_radio = &reset_radio;
        protocol->reset_robot = &reset_robot;

        /* Set misc. functions */
        protocol->reboot_robot = &reboot_robot;
        protocol->restart_robot_code = &restart_robot_code;

        /* Set packet intervals */
        protocol->fmsInterval = 500;
        protocol->radioInterval = 0;
        protocol->robotInterval = 20;

        /* Set joystick properties */
        protocol->maxJoysticks = 6;
        protocol->maxAxisCount = 6;
        protocol->maxHatsCount = 1;
        protocol->maxButtonCount = 10;

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

        /* Define netconsole socket properties */
        DS_Socket netconsole_socket;
        netconsole_socket.input_port = 6666;
        netconsole_socket.output_port = 6668;
        netconsole_socket.type = DS_SOCKET_UDP;

        /* Assign socket objects */
        protocol->fmsSocket = fms_socket;
        protocol->radioSocket = radio_socket;
        protocol->robotSocket = robot_socket;
        protocol->netconsoleSocket = netconsole_socket;
    }

    return protocol;
}
