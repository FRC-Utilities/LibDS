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

#ifndef _LIB_DS_EVENTS_H
#define _LIB_DS_EVENTS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief The types of events that can be delivered.
 */
typedef enum {
    DS_FIRST_EVENT             = 0x000, /**< Unused (do not remove) */
    DS_QUIT                    = 0x100, /**< User-requested quit */

    /* FMS events */
    DS_FMS_EVENT               = 0x200, /**< FMS event */
    DS_FMS_CONNECTED,                   /**< DS connected with FMS */
    DS_FMS_DISCONNECTED,                /**< DS disconnected from FMS */

    /* Radio/bridge events */
    DS_RADIO_EVENT             = 0x300, /**< Radio event */
    DS_RADIO_CONNECTED,                 /**< DS connected with radio */
    DS_RADIO_DISCONNECTED,              /**< DS disconnected from radio */

    /* Joystick events */
    DS_JOYSTICK_EVENT          = 0x400, /**< Joystick event */
    DS_JOYSTICK_COUNT_CHANGED,          /**< Joystick count changed */

    /* NetConsole events */
    DS_NETCONSOLE_EVENT        = 0x500, /**< NetConsole event */
    DS_NETCONSOLE_NEW_MESSAGE,          /**< DS received a message from NC */

    /* Robot events */
    DS_ROBOT_EVENT             = 0x600, /**< Robot event */
    DS_ROBOT_ENABLED,                   /**< Robot has been enabled */
    DS_ROBOT_DISABLED,                  /**< Robot has been disabled */
    DS_ROBOT_REBOOTED,                  /**< DS-led robot reboot */
    DS_ROBOT_CONNECTED,                 /**< DS conencted with robot */
    DS_ROBOT_EXIT_ESTOP,                /**< Robot exited e-stop */
    DS_ROBOT_CODE_LOADED,               /**< Robot code is available */
    DS_ROBOT_DISCONNECTED,              /**< DS disconnected from robot */
    DS_ROBOT_CODE_UNLOADED,             /**< Robot code is not available */
    DS_ROBOT_CODE_RESTARTED,            /**< Robot code re-loaded */
    DS_ROBOT_VOLTAGE_CHANGED,           /**< Robot voltage changed */
    DS_ROBOT_CPU_INFO_CHANGED,          /**< Robot CPU usage updated */
    DS_ROBOT_RAM_INFO_CHANGED,          /**< Robot RAM usage updated */
    DS_ROBOT_DISK_INFO_CHANGED,         /**< Robot disk usage updated */
    DS_ROBOT_EMERGENCY_STOPPED,         /**< Robot entered e-stop */
} DS_EventType;

/**
 * \brief Fields shared by every event
 */
typedef struct {
    uint32_t type;      /**< Event type */
    uint32_t timestamp; /**< Timestamp of event */
} SDL_CommonEvent;

/**
 * \brief FMS event fields
 */
typedef struct {
    uint32_t type;      /**< ::DS_FMS_EVENT */
    uint32_t timestamp; /**< Timestamp of event */
    int connected;      /**< Set to \c 1 if FMS is connected */
} DS_FMSEvent;

/**
 * \brief Radio event fields
 */
typedef struct {
    uint32_t type;      /**< ::DS_RADIO_EVENT */
    uint32_t timestamp; /**< Timestamp of event */
    int connected;      /**< Set to \c 1 if radio is connected */
} DS_RadioEvent;

/**
 * \brief Robot event fields
 */
typedef struct {
    uint32_t type;      /**< ::DS_ROBOT_EVENT */
    uint32_t timestamp; /**< Timestamp of event */
    int code;           /**< Set to \c 1 if robot code is loaded */
    int enabled;        /**< Set to \c 1 if the robot is enabled */
    int cpu_usage;      /**< Represents the CPU usage (from 0 to 100) */
    int ram_usage;      /**< Represents the RAM usage (from 0 to 100) */
    int estopped;       /**< Set to \c 1 if robot is emergency stopped */
    int connected;      /**< Set to \c 1 if robot is connected */
    int disk_usage;     /**< Represents the disk usage (from 0 to 100) */
    double voltage;     /**< Holds the current robot voltage */
} DS_RobotEvent;

/**
 * \brief Joystick event fields
 */
typedef struct {
    uint32_t type;      /**< ::DS_JOYSTICK_EVENT */
    uint32_t timestamp; /**< Timestamp of event */
    int count;          /**< Holds the new joystick count */
} DS_JoystickEvent;

/**
 * \brief NetConsole event fields
 */
typedef struct {
    uint32_t type;      /**< ::DS_FMS_EVENT */
    uint32_t timestamp; /**< Timestamp of event */
    char* message;      /**< Holds the received message */
} DS_NetConsoleEvent;

/**
 * \brief General event structure
 */
typedef union {
    uint32_t type;                 /**< Event type */
    DS_FMSEvent fms;               /**< FMS event data */
    DS_RobotEvent robot;           /**< Robot event data */
    DS_RadioEvent radio;           /**< Radio event data */
    DS_JoystickEvent joystick;     /**< Joystick event data */
    DS_NetConsoleEvent netconsole; /**< NetConsole event data */
} DS_Event;

extern int DS_PollEvent (DS_Event* event);

#ifdef __cplusplus
}
#endif

#endif

