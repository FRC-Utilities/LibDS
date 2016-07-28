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
#include "DS_Protocols.h"

#include <string.h>

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
 * Returns \c true if the robot code is running
 */
bool DS_GetRobotCode()
{
    return CFG_GetRobotCode();
}

/**
 * Returns \c true if the conditions to enable a robot are met
 */
bool DS_GetCanBeEnabled()
{
    return DS_GetRobotCode()
           && !DS_GetEmergencyStopped()
           && DS_GetRobotCommunications();
}

/**
 * Returns \c true if the robot can be enabled, otherwise, this function will
 * return \c false.
 */
bool DS_GetRobotEnabled()
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

bool DS_GetEmergencyStopped()
{
    return CFG_GetEmergencyStopped();
}

bool DS_GetFMSCommunications()
{
    return CFG_GetFMSCommunications();
}

bool DS_GetRadioCommunications()
{
    return CFG_GetRadioCommunications();
}

bool DS_GetRobotCommunications()
{
    return CFG_GetRobotCommunications();
}

DS_ControlMode DS_GetControlMode()
{
    return CFG_GetControlMode();
}

void DS_RebootRobot()
{
    DS_CurrentProtocol()->reboot_robot();
}

void DS_RestartRobotCode()
{
    DS_CurrentProtocol()->restart_robot_code();
}

void DS_SetTeamNumber (const int team)
{
    CFG_SetTeamNumber (team);
}

void DS_SetRobotEnabled (const bool enabled)
{
    CFG_SetRobotEnabled (enabled);
}

void DS_SetEmergencyStopped (const bool stop)
{
    CFG_SetEmergencyStopped (stop);
}

void DS_SetAlliance (const DS_Alliance alliance)
{
    CFG_SetAlliance (alliance);
}

void DS_SetPosition (const DS_Position position)
{
    CFG_SetPosition (position);
}

void DS_SetControlMode (const DS_ControlMode mode)
{
    CFG_SetControlMode (mode);
}

void DS_SetCustomFMSAddress (const char* address)
{
    if (address != NULL)
        strcpy (custom_fms_address, address);
}

void DS_SetCustomRadioAddress (const char* address)
{
    if (address != NULL)
        strcpy (custom_radio_address, address);
}

void DS_SetCustomRobotAddress (const char* address)
{
    if (address != NULL)
        strcpy (custom_robot_address, address);
}
