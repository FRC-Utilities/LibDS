/*
 * The Driver Station Library (LibDS)
 * Copyright (c) 2015-2017 Alex Spataru <alex_spataru@outlook>
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
#include "DS_Protocol.h"
#include "DS_Joysticks.h"
#include "DS_DefaultProtocols.h"

//----------------------------------------------------------------------------//
// Recomendations for the devs who WILL implement this:                       //
//   - To reverse engineer the protocol, launch Wireshark and see what the    //
//     FRC DS sends and receives from the robot under different conditions,   //
//     take note of the port info and timings of each event to know how the   //
//     RX/TX bytes changed.                                                   //
//   - Update the target addresses/IPs, I have used the ones defined by the   //
//     FRC 2016 protocol, but things may have changed over these two years    //
//   - Check the functions of DS_String.h, LibDS passes byte arrays (strings) //
//     using the structures and functions defined in that header/module.      //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Network address section, someone please verify these functions             //
//----------------------------------------------------------------------------//

/**
 * The FMS address is not defined, it will be assigned automatically when the
 * DS receives a FMS packet
 */
static DS_String fms_address (void)
{
    return DS_StrNew (DS_FallBackAddress);
}

/**
 * The 2015 control system assigns the radio IP in 10.te.am.1
 */
static DS_String radio_address (void)
{
    return DS_GetStaticIP (10, CFG_GetTeamNumber(), 1);
}

/**
 * The 2016 control system assigns the robot address at roboRIO-TEAM-FRC.local,
 */
static DS_String robot_address (void)
{
    return DS_StrFormat ("roboRIO-%d-FRC.local", CFG_GetTeamNumber());
}

//----------------------------------------------------------------------------//
// DS to Robot/Radio/FMS packet generation                                    //
//----------------------------------------------------------------------------//

static DS_String create_fms_packet (void)
{
    /* Return empty (0-length) string */
    return DS_StrNewLen (0);
}

static DS_String create_radio_packet (void)
{
    /* Return empty (0-length) string */
    return DS_StrNewLen (0);
}

static DS_String create_robot_packet (void)
{
    /* Return empty (0-length) string */
    return DS_StrNewLen (0);
}

//----------------------------------------------------------------------------//
// Robot/Radio/FMS to DS packet reading & interpretation                      //
//----------------------------------------------------------------------------//

static int read_fms_packet (const DS_String* data)
{
    /* Check if data pointer is valid */
    if (!data)
        return 0;

    /* NOTES:
     * - You can verify packet length with DS_StrLen() function
     * - This function should update global variables accordingly, check
     *   previous FRC Comm. protocol implementations for more info, you should
     *   use LibDS functions that start with "CFG_" to update these variables.
     * - If the packet is read and interpreted successfully, this function
     *   should return 1 (considered as "true" in C), otherwise, just
     *   return 0 if you find any error
     */
    return 0;
}

static int read_radio_packet (const DS_String* data)
{
    /* Check if data pointer is valid */
    if (!data)
        return 0;

    /* NOTES:
     * - You can verify packet length with DS_StrLen() function
     * - This function should update global variables accordingly, check
     *   previous FRC Comm. protocol implementations for more info, you should
     *   use LibDS functions that start with "CFG_" to update these variables.
     * - If the packet is read and interpreted successfully, this function
     *   should return 1 (considered as "true" in C), otherwise, just
     *   return 0 if you find any error
     */
    return 0;
}

static int read_robot_packet (const DS_String* data)
{
    /* Check if data pointer is valid */
    if (!data)
        return 0;

    /* NOTES:
     * - You can verify packet length with DS_StrLen() function
     * - This function should update global variables accordingly, check
     *   previous FRC Comm. protocol implementations for more info, you should
     *   use LibDS functions that start with "CFG_" to update these variables.
     * - If the packet is read and interpreted successfully, this function
     *   should return 1 (considered as "true" in C), otherwise, just
     *   return 0 if you find any error
     */
    return 0;
}

//----------------------------------------------------------------------------//
// The implementation of these functions is up to you, and how the protocol   //
// works...                                                                   //
// In previous protocol implementations, these functions just changed the     //
// value of global variables that were used by the packet generation          //
// functions.                                                                 //
//----------------------------------------------------------------------------//

static void reset_fms (void) {}
static void reset_radio (void) {}
static void reset_robot (void) {}
static void reboot_robot (void) {}
static void restart_robot_code (void) {}

//----------------------------------------------------------------------------//
// Finally, to use the protocol with the rest of LibDS, create a DS_Protocol  //
// structure, which contains pointers to the functions implemented above and  //
// other properties, such as:                                                 //
//   - Maximum joystick count, and individual joystick button and axis limits //
//   - DS to Robot/Radio/FMS packet sending intervals (in milliseconds)       //
//   - Network socket properties for robot, radio and FMS                     //
//   - Input/output network ports for robot, radio and FMS                    //
//----------------------------------------------------------------------------//

/**
 * Initializes and configures the FRC 2018 communication protocol
 */
DS_Protocol DS_GetProtocolFRC_2018 (void)
{
    /* Initialize pointers */
    DS_Protocol protocol;

    /* Set protocol name */
    protocol.name = DS_StrNew ("FRC 2018");

    /* Set address functions */
    protocol.fms_address = &fms_address;
    protocol.radio_address = &radio_address;
    protocol.robot_address = &robot_address;

    /* Set packet generator functions */
    protocol.create_fms_packet = &create_fms_packet;
    protocol.create_radio_packet = &create_radio_packet;
    protocol.create_robot_packet = &create_robot_packet;

    /* Set packet interpretation functions */
    protocol.read_fms_packet = &read_fms_packet;
    protocol.read_radio_packet = &read_radio_packet;
    protocol.read_robot_packet = &read_robot_packet;

    /* Set reset functions */
    protocol.reset_fms = &reset_fms;
    protocol.reset_radio = &reset_radio;
    protocol.reset_robot = &reset_robot;

    /* Set misc. functions */
    protocol.max_battery_voltage = 13;
    protocol.reboot_robot = &reboot_robot;
    protocol.restart_robot_code = &restart_robot_code;

    /* Set packet intervals */
    protocol.fms_interval = 500;
    protocol.radio_interval = 0;
    protocol.robot_interval = 20;

    /* Set joystick properties */
    protocol.max_hat_count = 0;
    protocol.max_axis_count = 0;
    protocol.max_joysticks = 0;
    protocol.max_button_count = 0;

    /* Define FMS socket properties */
    protocol.fms_socket = *DS_SocketEmpty();
    protocol.fms_socket.disabled = 0;
    protocol.fms_socket.in_port = 1120;
    protocol.fms_socket.out_port = 1160;
    protocol.fms_socket.type = DS_SOCKET_UDP;

    /* Define radio socket properties */
    protocol.radio_socket = *DS_SocketEmpty();
    protocol.radio_socket.disabled = 1;

    /* Define robot socket properties */
    protocol.robot_socket = *DS_SocketEmpty();
    protocol.robot_socket.disabled = 0;
    protocol.robot_socket.in_port = 1150;
    protocol.robot_socket.out_port = 1110;
    protocol.robot_socket.type = DS_SOCKET_UDP;

    /* Define netconsole socket properties */
    protocol.netconsole_socket = *DS_SocketEmpty();
    protocol.netconsole_socket.disabled = 1;

    /* Return the pointer */
    return protocol;
}
