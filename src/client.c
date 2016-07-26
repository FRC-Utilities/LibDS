/*
 * The Driver Station Library (LibDS)
 * Copyright (C) 2015-2016 Alex Spataru <alex_spataru@outlook>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * SOftware is furnished to do so, subject to the following conditions:
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

const char* DS_GetCustomFMSAddress()
{
    return custom_fms_address;
}

const char* DS_GetCustomRadioAddress()
{
    return custom_radio_address;
}

const char* DS_GetCustomRobotAddress()
{
    return custom_robot_address;
}

const char* DS_GetDefaultFMSAddress()
{
    return DS_CurrentProtocol()->fmsAddress;
}

const char* DS_GetDefaultRadioAddress()
{
    return DS_CurrentProtocol()->radioAddress;
}

const char* DS_GetDefaultRobotAddress()
{
    return DS_CurrentProtocol()->robotAddress;
}

const char* DS_GetAppliedFMSAddress()
{
    if (DS_StringIsEmpty (DS_GetCustomFMSAddress()))
        return DS_GetDefaultFMSAddress();

    return DS_GetCustomFMSAddress();
}

const char* DS_GetAppliedRadioAddress()
{
    if (DS_StringIsEmpty (DS_GetCustomRadioAddress()))
        return DS_GetDefaultRadioAddress();

    return DS_GetCustomRadioAddress();
}

const char* DS_GetAppliedRobotAddress()
{
    if (DS_StringIsEmpty (DS_GetCustomRobotAddress()))
        return DS_GetDefaultRobotAddress();

    return DS_GetCustomRobotAddress();
}

int DS_GetTeamNumber()
{
    return CFG_GetTeamNumber();
}

bool DS_GetRobotCode()
{
    return CFG_GetRobotCode();
}

bool DS_GetCanBeEnabled()
{
    return DS_GetRobotCode()
           && !DS_GetEmergencyStopped()
           && DS_GetRobotCommunications();
}

bool DS_GetRobotEnabled()
{
    return CFG_GetRobotEnabled();
}

int DS_GetRobotCPUUsage()
{
    return CFG_GetRobotCPUUsage();
}

int DS_GetRobotRAMUsage()
{
    return CFG_GetRobotRAMUsage();
}

int DS_GetRobotDiskUsage()
{
    return CFG_GetRobotDiskUsage();
}

double DS_GetRobotVoltage()
{
    return CFG_GetRobotVoltage();
}

DS_Alliance DS_GetAlliance()
{
    return CFG_GetAlliance();
}

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
    DS_CurrentProtocol()->rebootRobot();
}

void DS_RestartRobotCode()
{
    DS_CurrentProtocol()->restartRobotCode();
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
    strcpy (custom_fms_address, address);
}

void DS_SetCustomRadioAddress (const char* address)
{
    strcpy (custom_radio_address, address);
}

void DS_SetCustomRobotAddress (const char* address)
{
    strcpy (custom_robot_address, address);
}
