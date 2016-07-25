/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "DS_Private.h"

static int TEAM       = 0;
static int CPU_USAGE  = 0;
static int RAM_USAGE  = 0;
static int DISK_USAGE = 0;
static double VOLTAGE = 0;

static char* LIB_VERSION = "";
static char* PDP_VERSION = "";
static char* PCM_VERSION = "";

static DS_Bool ROBOT_CODE      = DS_FALSE;
static DS_Bool ENABLED         = DS_FALSE;
static DS_Bool EMERGENCY_STOP  = DS_FALSE;
static DS_Bool FMS_CONNECTED   = DS_FALSE;
static DS_Bool RADIO_CONNECTED = DS_FALSE;
static DS_Bool ROBOT_CONNECTED = DS_FALSE;

static DS_Control CONTROL_MODE = DS_CONTROL_TELEOPERATED;

void cfg_init()
{

}

int cfg_team()
{
    return TEAM;
}

double cfg_voltage()
{
    return VOLTAGE;
}

int cfg_cpu_usage()
{
    return CPU_USAGE;
}

int cfg_ram_usage()
{
    return RAM_USAGE;
}

int cfg_disk_usage()
{
    return DISK_USAGE;
}

char* cfg_pdp_version()
{
    return PDP_VERSION;
}

char* cfg_pcm_version()
{
    return PCM_VERSION;
}

char* cfg_lib_version()
{
    return LIB_VERSION;
}
DS_Bool cfg_robot_code()
{
    return ROBOT_CODE;
}

DS_Bool cfg_robot_enabled()
{
    return ENABLED;
}

DS_Bool cfg_fms_connected()
{
    return FMS_CONNECTED;
}
DS_Bool cfg_emergency_stop()
{
    return EMERGENCY_STOP;
}

DS_Bool cfg_radio_connected()
{
    return RADIO_CONNECTED;
}

DS_Bool cfg_robot_connected()
{
    return ROBOT_CONNECTED;
}

DS_Control cfg_control_mode()
{
    return CONTROL_MODE;
}

void cfg_update_team (const int team)
{
    TEAM = team;
}

void cfg_update_enabled (const DS_Bool enabled)
{
    ENABLED = enabled;
}

void cfg_update_cpu_usage (const int usage)
{
    CPU_USAGE = usage;
}

void cfg_update_ram_usage (const int usage)
{
    RAM_USAGE = usage;
}

void cfg_update_disk_usage (const int usage)
{
    DISK_USAGE = usage;
}

void cfg_update_emergency_stop (const DS_Bool stop)
{
    EMERGENCY_STOP = stop;
}

void cfg_update_control_mode (const DS_Control mode)
{
    CONTROL_MODE = mode;
}

void cfg_update_robot_code (const DS_Bool available)
{
    ROBOT_CODE = available;
}

void cfg_update_voltage (const double voltage)
{
    VOLTAGE = voltage;
}

void cfg_update_pdp_version (const char* version)
{
    strcpy (PDP_VERSION, version);
}

void cfg_update_pcm_version (const char* version)
{
    strcpy (PCM_VERSION, version);
}

void cfg_update_lib_version (const char* version)
{
    strcpy (LIB_VERSION, version);
}

void cfg_update_fms_connected (const DS_Bool connected)
{
    FMS_CONNECTED = connected;
}

void cfg_update_radio_connected (const DS_Bool connected)
{
    RADIO_CONNECTED = connected;
}

void cfg_update_robot_connected (const DS_Bool connected)
{
    ROBOT_CONNECTED = connected;
}
