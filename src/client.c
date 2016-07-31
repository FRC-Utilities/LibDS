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

#include "DS_Utils.h"
#include "DS_Client.h"
#include "DS_Config.h"
#include "DS_Protocol.h"

#include <string.h>

/*
 * These variables hold the user-set addresses
 */
static char* custom_fms_address;
static char* custom_radio_address;
static char* custom_robot_address;

/**
 * Allocates memory for the members of the client module
 */
void Client_Init()
{
    custom_fms_address = (char*) malloc (sizeof (char));
    custom_radio_address = (char*) malloc (sizeof (char));
    custom_robot_address = (char*) malloc (sizeof (char));
}

/**
 * Frees up the memory used by the members of the client module
 */
void Client_Close()
{
    free (custom_fms_address);
    free (custom_radio_address);
    free (custom_robot_address);
}

/**
 * Returns the user-set FMS address
 */
const char* DS_GetCustomFMSAddress()
{
    return custom_fms_address;
}

/**
 * Returns the user-set radio address
 */
const char* DS_GetCustomRadioAddress()
{
    return custom_radio_address;
}

/**
 * Returns the user-set robot address
 */
const char* DS_GetCustomRobotAddress()
{
    return custom_robot_address;
}

/**
 * Returns the protocol-set FMS address
 */
const char* DS_GetDefaultFMSAddress()
{
    return DS_CurrentProtocol()->fms_address();
}

/**
 * Returns the protocol-set radio address
 */
const char* DS_GetDefaultRadioAddress()
{
    return DS_CurrentProtocol()->radio_address();
}

/**
 * Returns the protocol-set robot address
 */
const char* DS_GetDefaultRobotAddress()
{
    return DS_CurrentProtocol()->robot_address();
}

/**
 * Returns the address used to communicate with the FMS
 */
const char* DS_GetAppliedFMSAddress()
{
    if (DS_StringIsEmpty (DS_GetCustomFMSAddress()))
        return DS_GetDefaultFMSAddress();

    return DS_GetCustomFMSAddress();
}

/**
 * Returns the address used to communicate with the radios
 */
const char* DS_GetAppliedRadioAddress()
{
    if (DS_StringIsEmpty (DS_GetCustomRadioAddress()))
        return DS_GetDefaultRadioAddress();

    return DS_GetCustomRadioAddress();
}

/**
 * Returns the address used to communicate with the robot
 */
const char* DS_GetAppliedRobotAddress()
{
    if (DS_StringIsEmpty (DS_GetCustomRobotAddress()))
        return DS_GetDefaultRobotAddress();

    return DS_GetCustomRobotAddress();
}

/**
 * Returns the current team number
 */
int DS_GetTeamNumber()
{
    return CFG_GetTeamNumber();
}

/**
 * Returns \c 1 if the robot code is running
 */
int DS_GetRobotCode()
{
    return CFG_GetRobotCode();
}

/**
 * Returns \c 1 if the conditions to enable a robot are met
 */
int DS_GetCanBeEnabled()
{
    return DS_GetRobotCode()
           && !DS_GetEmergencyStopped()
           && DS_GetRobotCommunications();
}

/**
 * Returns \c 1 if the robot can be enabled, otherwise, this function will
 * return \c 0.
 */
int DS_GetRobotEnabled()
{
    return CFG_GetRobotEnabled();
}

/**
 * Returns the current CPU usage of the robot
 */
int DS_GetRobotCPUUsage()
{
    return CFG_GetRobotCPUUsage();
}

/**
 * Returns the current RAM usage of the robot
 */
int DS_GetRobotRAMUsage()
{
    return CFG_GetRobotRAMUsage();
}

/**
 * Returns the current disk usage of the robot
 */
int DS_GetRobotDiskUsage()
{
    return CFG_GetRobotDiskUsage();
}

/**
 * Returns the current voltage of the robot
 */
double DS_GetRobotVoltage()
{
    return CFG_GetRobotVoltage();
}

/**
 * Returns the current alliance of the robot.
 * This value can be changed by the user or the FMS.
 */
DS_Alliance DS_GetAlliance()
{
    return CFG_GetAlliance();
}

/**
 * Returns the current position of the robot.
 * This value can be changed by the user or the FMS.
 */
DS_Position DS_GetPosition()
{
    return CFG_GetPosition();
}

/**
 * Returns \c 1 if the robot is emergency stopped
 */
int DS_GetEmergencyStopped()
{
    return CFG_GetEmergencyStopped();
}

/**
 * Returns \c 1 if the LibDS has communications with the FMS
 */
int DS_GetFMSCommunications()
{
    return CFG_GetFMSCommunications();
}

/**
 * Returns \c 1 if the LibDS has communications with the radio
 */
int DS_GetRadioCommunications()
{
    return CFG_GetRadioCommunications();
}

/**
 * Returns \c 1 if the LibDS has communications with the robot
 */
int DS_GetRobotCommunications()
{
    return CFG_GetRobotCommunications();
}

/**
 * Returns the current control mode of the robot
 */
DS_ControlMode DS_GetControlMode()
{
    return CFG_GetControlMode();
}

/**
 * Instructs the current protocol to reboot the robot
 */
void DS_RebootRobot()
{
    DS_CurrentProtocol()->reboot_robot();
}

/**
 * Instructs the current protocol to restart the robot code
 */
void DS_RestartRobotCode()
{
    DS_CurrentProtocol()->restart_robot_code();
}

/**
 * Changes the \a team number, which is used by the protocols to find the robot
 * and (in some cases), to construct the packet
 */
void DS_SetTeamNumber (const int team)
{
    CFG_SetTeamNumber (team);
}

/**
 * Changes the \a enabled state of the robot
 */
void DS_SetRobotEnabled (const int enabled)
{
    CFG_SetRobotEnabled (enabled);
}

/**
 * Changes the emergency stop state of the robot
 */
void DS_SetEmergencyStopped (const int stop)
{
    CFG_SetEmergencyStopped (stop);
}

/**
 * Changes the team \a alliance of the robot
 */
void DS_SetAlliance (const DS_Alliance alliance)
{
    CFG_SetAlliance (alliance);
}

/**
 * Changes the robot's \a position in the field
 */
void DS_SetPosition (const DS_Position position)
{
    CFG_SetPosition (position);
}

/**
 * Changes the control \a mode of the robot
 */
void DS_SetControlMode (const DS_ControlMode mode)
{
    CFG_SetControlMode (mode);
}

/**
 * Changes the \a address used to communicate with the FMS
 */
void DS_SetCustomFMSAddress (const char* address)
{
    if (address != NULL)
        strcpy (custom_fms_address, address);
}

/**
 * Changes the \a address used to communicate with the radio
 */
void DS_SetCustomRadioAddress (const char* address)
{
    if (address != NULL)
        strcpy (custom_radio_address, address);
}

/**
 * Changes the \a address used to communicate with the robot
 */
void DS_SetCustomRobotAddress (const char* address)
{
    if (address != NULL)
        strcpy (custom_robot_address, address);
}

/**
 * Sends the given \a message to the NetConsole of the robot
 */
void DS_SendNetConsoleMessage (const char* message)
{
    CFG_SetNetConsoleData (message);
}
