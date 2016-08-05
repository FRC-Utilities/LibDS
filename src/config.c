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
#include <string.h>

/*
 * These variables hold the state(s) of the LibDS and its modules
 */
static int team = 0;
static int cpu_usage = 0;
static int ram_usage = 0;
static int disk_usage = 0;
static int robot_code = 0;
static int robot_enabled = 0;
static int can_utilization = 0;
static double robot_voltage = 0;
static int emergency_stopped = 0;
static int fms_communications = 0;
static int radio_communications = 0;
static int robot_communications = 0;
static DS_Position robot_position = DS_POSITION_1;
static DS_Alliance robot_alliance = DS_ALLIANCE_RED;
static DS_ControlMode control_mode = DS_CONTROL_TELEOPERATED;

/*
 * Initialize the default netconsole message
 */
static sds netconsole_data;

/**
 * Ensures that the given \a input number is either \c 0 or \c 1.
 */
static int to_boolean (const int input)
{
    if (input < 1)
        return 0;

    return 1;
}

/**
 * Ensures that the given \a input stays within the range established by the
 * \a min and \a max numbers
 */
static int respect_range (const int input, const int min, const int max)
{
    if (input < min)
        return min;

    else if (input > max)
        return max;

    return input;
}

/**
 * Returns the current team number, which may be used by the protocols to
 * specifiy the default addresses and generate specialized packets
 */
int CFG_GetTeamNumber()
{
    return team;
}

/**
 * Returns \c 0 if there is no robot code running, otherwise, it returns \c 1
 */
int CFG_GetRobotCode()
{
    return robot_code;
}

/**
 * Returns \c 1 if the robot is enabled, otherwise, it returns \c 0
 */
int CFG_GetRobotEnabled()
{
    return robot_enabled;
}

/**
 * Returns the current CPU usage of the robot
 */
int CFG_GetRobotCPUUsage()
{
    return cpu_usage;
}

/**
 * Returns the current RAM usage of the robot
 */
int CFG_GetRobotRAMUsage()
{
    return ram_usage;
}

/**
 * Returns the current utilization of the robot's CAN-BUS
 */
int CFG_GetCANUtilization()
{
    return can_utilization;
}

/**
 * Returns the current disk usage of the robot
 */
int CFG_GetRobotDiskUsage()
{
    return disk_usage;
}

/**
 * Returns the current voltage of the robot
 */
double CFG_GetRobotVoltage()
{
    return robot_voltage;
}

/**
 * Returns the current alliance of the robot, possible values are:
 *    - \c DS_ALLIANCE_RED
 *    - \c DS_ALLIANCE_BLUE
 */
DS_Alliance CFG_GetAlliance()
{
    return robot_alliance;
}

/**
 * Returns the current team position of the robot, possible values are:
 *    - \c DS_POSITION_1
 *    - \c DS_POSITION_2
 *    - \c DS_POSITION_3
 */
DS_Position CFG_GetPosition()
{
    return robot_position;
}

/**
 * Returns the current NetConsole messages to send
 */
sds CFG_GetNetConsoleData()
{
    return netconsole_data;
}

/**
 * Returns \c 1 if the robot is emergency stopped, otherwise, it returns \c 0
 */
int CFG_GetEmergencyStopped()
{
    return emergency_stopped;
}

/**
 * Returns \c 1 if the client has communications with the FMS, otherwise,
 * it returns \c 0
 */
int CFG_GetFMSCommunications()
{
    return fms_communications;
}

/**
 * Returns \c 1 if the client has communications with the radio, otherwise,
 * it returns \c 0
 */
int CFG_GetRadioCommunications()
{
    return radio_communications;
}

/**
 * Returns \c 1 if the client has communications with the robot, otherwise,
 * it returns \c 0
 */
int CFG_GetRobotCommunications()
{
    return robot_communications;
}

/**
 * Returns the current control mode of the robot, possible values are:
 *    - \c DS_CONTROL_TEST
 *    - \c DS_CONTROL_AUTONOMOUS
 *    - \c DS_CONTROL_TELEOPERATED
 */
DS_ControlMode CFG_GetControlMode()
{
    return control_mode;
}

/**
 * Updates the available state of the robot code
 */
void CFG_SetRobotCode (const int code)
{
    if (robot_code != code)
        robot_code = code;
}

/**
 * Updates the team \a number
 */
void CFG_SetTeamNumber (const int number)
{
    if (team != number)
        team = number;
}

/**
 * Updates the robot's \a enabled state
 */
void CFG_SetRobotEnabled (const int enabled)
{
    int boolean = to_boolean (enabled);

    if (robot_enabled != boolean)
        robot_enabled = boolean;
}

/**
 * Updates the NetConsole message to send
 */
void CFG_SetNetConsoleData (const sds data)
{
    if (!netconsole_data)
        netconsole_data = sdsempty();

    sdscpy (netconsole_data, data);
}

/**
 * Updates the robot's CPU usage.
 * You must input a value between \c 0 and \c 100
 */
void CFG_SetRobotCPUUsage (const int percent)
{
    int value = respect_range (percent, 0, 100);

    if (cpu_usage != value)
        cpu_usage = value;
}

/**
 * Updates the robot's RAM/memory usage.
 * You must input a value between \c 0 and \c 100
 */
void CFG_SetRobotRAMUsage (const int percent)
{
    int value = respect_range (percent, 0, 100);

    if (ram_usage != value)
        ram_usage = value;
}

/**
 * Updates the robot's disk usage.
 * You must input a value between \c 0 and \c 100
 */
void CFG_SetRobotDiskUsage (const int percent)
{
    int value = respect_range (percent, 0, 100);

    if (disk_usage != value)
        disk_usage = value;
}

/**
 * Updates the robot's \a voltage, there are no range limits
 */
void CFG_SetRobotVoltage (const double voltage)
{
    if (robot_voltage != voltage)
        robot_voltage = voltage;
}

/**
 * Updates the emergency \a stopped state of the robot.
 */
void CFG_SetEmergencyStopped (const int stopped)
{
    int boolean = to_boolean (stopped);

    if (emergency_stopped != boolean)
        emergency_stopped = boolean;
}

/**
 * Changes the \a alliance of the robot
 */
void CFG_SetAlliance (const DS_Alliance alliance)
{
    if (robot_alliance != alliance)
        robot_alliance = alliance;
}

/**
 * Changes the \a position of the robot
 */
void CFG_SetPosition (const DS_Position position)
{
    if (robot_position != position)
        robot_position = position;
}

/**
 * Updates the CAN \a utilization
 */
void CFG_SetCANUtilization (const int utilization)
{
    if (can_utilization != utilization)
        can_utilization = utilization;
}

/**
 * Changes the control \a mode of the robot
 */
void CFG_SetControlMode (const DS_ControlMode mode)
{
    if (control_mode != mode)
        control_mode = mode;
}

/**
 * Updates the state of the FMS communications.
 */
void CFG_SetFMSCommunications (const int communications)
{
    int boolean = to_boolean (communications);

    if (fms_communications != boolean)
        fms_communications = boolean;
}

/**
 * Updates the state of the radio communications.
 */
void CFG_SetRadioCommunications (const int communications)
{
    int boolean = to_boolean (communications);

    if (radio_communications != boolean)
        radio_communications = boolean;
}

/**
 * Updates the state of the robot communications.
 */
void CFG_SetRobotCommunications (const int communications)
{
    int boolean = to_boolean (communications);

    if (robot_communications != boolean)
        robot_communications = boolean;
}

/**
 * Called when the FMS watchdog expires
 */
void CFG_FMSWatchdogExpired()
{
    CFG_SetFMSCommunications (0);
}

/**
 * Called when the radio watchdog expires
 */
void CFG_RadioWatchdogExpired()
{
    CFG_SetRadioCommunications (0);
}

/**
 * Called when the robot watchdog expires
 */
void CFG_RobotWatchdogExpired()
{
    CFG_SetRobotCode (0);
    CFG_SetRobotVoltage (0);
    CFG_SetRobotEnabled (0);
    CFG_SetRobotCPUUsage (0);
    CFG_SetRobotRAMUsage (0);
    CFG_SetRobotDiskUsage (0);
    CFG_SetEmergencyStopped (0);
    CFG_SetRobotCommunications (0);
    CFG_SetControlMode (DS_CONTROL_TELEOPERATED);
}
