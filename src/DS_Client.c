/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "DS_Client.h"
#include "DS_Private.h"

#include <stdio.h>

void DS_Init()
{
    cfg_init();
    sockets_init();
    joystick_init();
    netconsole_init();
}

void DS_Close()
{
}

int DS_Team()
{
    return cfg_team();
}

double DS_Voltage()
{
    return cfg_voltage();
}

int DS_RobotCPUUsage()
{
    return cfg_cpu_usage();
}

int DS_RobotRAMUsage()
{
    return cfg_ram_usage();
}

int DS_RobotDiskUsage()
{
    return cfg_disk_usage();
}

char* DS_RobotPDPVersion()
{
    return cfg_pdp_version();
}

char* DS_RobotPCMVersion()
{
    return cfg_pcm_version();
}

char* DS_RobotLibVersion()
{
    return cfg_lib_version();
}

DS_Bool DS_Enabled()
{
    return cfg_robot_enabled();
}

DS_Bool DS_CanBeEnabled()
{
    return DS_RobotConnected() && DS_RobotCodeLoaded() && !DS_EmergencyStop();
}

DS_Bool DS_EmergencyStop()
{
    return cfg_emergency_stop();
}

DS_Bool DS_FMSConnected()
{
    return cfg_fms_connected();
}

DS_Bool DS_RadioConnected()
{
    return cfg_radio_connected();
}

DS_Bool DS_RobotConnected()
{
    return cfg_robot_connected();
}

DS_Bool DS_RobotCodeLoaded()
{
    return cfg_robot_code();
}

DS_Control DS_GetControlMode()
{
    return cfg_control_mode();
}

void DS_SetTeamNumber (const int team)
{
    cfg_update_team (team);
}

void DS_SetEnabled (const DS_Bool enabled)
{
    cfg_update_enabled (DS_CanBeEnabled() ? enabled : DS_FALSE);
}

void DS_SetControlMode (const DS_Control mode)
{
    cfg_update_control_mode (mode);
}

void DS_SetEmergencyStopped (const DS_Bool stopped)
{
    cfg_update_emergency_stop (stopped);
}

void DS_SetFMSAddress (const char* ip)
{
    sockets_set_fms_address (ip);
}

void DS_SetRadioAddress (const char* ip)
{
    sockets_set_radio_address (ip);
}

void DS_SetRobotAddress (const char* ip)
{
    sockets_set_robot_address (ip);
}

int DS_RegisterJoystick (const char* name,
                         const int axes,
                         const int hats,
                         const int buttons)
{
    return joystick_add (name, axes, hats, buttons);
}

int DS_JoystickCount()
{
    return joystick_count();
}

int DS_GetNumAxes (const int joystick)
{
    return joystick_num_axes (joystick);
}

int DS_GetNumHats (const int joystick)
{
    return joystick_num_hats (joystick);
}

int DS_GetNumButtons (const int joystick)
{
    return joystick_num_buttons (joystick);
}

char* DS_GetJoystickName (const int joystick)
{
    return joystick_name (joystick);
}

int DS_RemoveJoystick (const int joystick)
{
    return joystick_remove (joystick);
}

void DS_SendNetConsoleMessage (const char* message)
{
    netconsole_send_message (message);
}
