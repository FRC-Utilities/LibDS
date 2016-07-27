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

#include "DS_Config.h"

static int team;
static int cpu_usage;
static int ram_usage;
static int disk_usage;
static bool robot_code;
static bool robot_enabled;
static double robot_voltage;
static bool emergency_stopped;
static bool fms_communications;
static bool radio_communications;
static bool robot_communications;
static DS_Alliance robot_alliance;
static DS_Position robot_position;
static DS_ControlMode control_mode;

void CFG_Init()
{

}

int CFG_GetTeamNumber()
{
    return team;
}

bool CFG_GetRobotCode()
{
    return robot_code;
}

bool CFG_GetRobotEnabled()
{
    return robot_enabled;
}

int CFG_GetRobotCPUUsage()
{
    return cpu_usage;
}

int CFG_GetRobotRAMUsage()
{
    return ram_usage;
}

int CFG_GetRobotDiskUsage()
{
    return disk_usage;
}

double CFG_GetRobotVoltage()
{
    return robot_voltage;
}

DS_Alliance CFG_GetAlliance()
{
    return robot_alliance;
}

DS_Position CFG_GetPosition()
{
    return robot_position;
}

bool CFG_GetEmergencyStopped()
{
    return emergency_stopped;
}

bool CFG_GetFMSCommunications()
{
    return fms_communications;
}

bool CFG_GetRadioCommunications()
{
    return radio_communications;
}

bool CFG_GetRobotCommunications()
{
    return robot_communications;
}

DS_ControlMode CFG_GetControlMode()
{
    return control_mode;
}

void CFG_SetRobotCode (const bool code)
{
    if (robot_code != code)
        robot_code = code;
}

void CFG_SetTeamNumber (const int number)
{
    if (team != number)
        team = number;
}

void CFG_SetRobotEnabled (const bool enabled)
{
    if (robot_enabled != enabled)
        robot_enabled = enabled;
}

void CFG_SetRobotCPUUsage (const int percent)
{
    if (cpu_usage != percent)
        cpu_usage = percent;
}

void CFG_SetRobotRAMUsage (const int percent)
{
    if (ram_usage != percent)
        ram_usage = percent;
}

void CFG_SetRobotDiskUsage (const int percent)
{
    if (disk_usage != percent)
        disk_usage = percent;
}

void CFG_SetRobotVoltage (const double voltage)
{
    if (robot_voltage != voltage)
        robot_voltage = voltage;
}

void CFG_SetEmergencyStopped (const bool stopped)
{
    if (emergency_stopped != stopped)
        emergency_stopped = stopped;
}

void CFG_SetAlliance (const DS_Alliance alliance)
{
    if (robot_alliance != alliance)
        robot_alliance = alliance;
}

void CFG_SetPosition (const DS_Position position)
{
    if (robot_position != position)
        robot_position = position;
}

void CFG_SetControlMode (const DS_ControlMode mode)
{
    if (control_mode != mode)
        control_mode = mode;
}

void CFG_SetFMSCommunications (const bool communications)
{
    if (fms_communications != communications)
        fms_communications = communications;
}

void CFG_SetRadioCommunications (const bool communications)
{
    if (radio_communications != communications)
        radio_communications = communications;
}

void CFG_SetRobotCommunications (const bool communications)
{
    if (robot_communications != communications)
        robot_communications = communications;
}
