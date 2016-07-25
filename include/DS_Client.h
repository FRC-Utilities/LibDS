/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#ifndef _LIBDS_CLIENT_H
#define _LIBDS_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "DS_Common.h"

/**
 * Initializes all the modules of the LibDS, you should call this function
 * as soon as possible.
 */
extern void DS_Init();

/**
 * Closes all the modules of the LibDS, call this before quiting the
 * application
 */
extern void DS_Close();

/**
 * Returns the currently assigned team number
 */
extern int DS_Team();

/**
 * Returns the current voltage of the robot
 */
extern double DS_Voltage();

/**
 * Returns the current CPU usage of the robot
 */
extern int DS_RobotCPUUsage();

/**
 * Returns the current RAM usage of the robot
 */
extern int DS_RobotRAMUsage();

/**
 * Returns the current disk usage of the robot
 */
extern int DS_RobotDiskUsage();

/**
 * Returns the current PDP version of the robot
 */
extern char* DS_RobotPDPVersion();

/**
 * Returns the current PCM version of the robot
 */
extern char* DS_RobotPCMVersion();

/**
 * Returns the current library version of the robot
 */
extern char* DS_RobotLibVersion();

/**
 * Returns \c DS_TRUE if the robot is enabled, otherwise, it returns \c DS_FALSE
 */
extern DS_Bool DS_Enabled();

/**
 * Returns \c DS_TRUE if it is possible to enable the robot, otherwise, it
 * returns \c DS_FALSE.
 *
 * The following conditions must be met in order to enable the robot:
 *    - Robot communications need to be established
 *    - Robot code must be running
 *    - Robot must not be in emergency stop
 */
extern DS_Bool DS_CanBeEnabled();

/**
 * Returns \c DS_TRUE if the robot is emergency stopped, otherwise, it returns
 * \c DS_FALSE
 */
extern DS_Bool DS_EmergencyStop();

/**
 * Returns \c DS_TRUE if the client is connected to the FMS.
 * Otherwise, it returns \c DS_FALSE.
 */
extern DS_Bool DS_FMSConnected();

/**
 * Returns \c DS_TRUE if the client is connected to the radio/bridge.
 * Otherwise, it returns \c DS_FALSE.
 */
extern DS_Bool DS_RadioConnected();

/**
 * Returns \c DS_TRUE if the client is connected to the robot.
 * Otherwise, it returns \c DS_FALSE.
 */
extern DS_Bool DS_RobotConnected();

/**
 * Returns \c DS_TRUE if the robot reports that its user code is running.
 * Otherwise, it returns \c DS_FALSE.
 */
extern DS_Bool DS_RobotCodeLoaded();

/**
 * Returns the current control mode of the robot.
 * Possible values are:
 *    - \c DS_CONTROL_TEST
 *    - \c DS_CONTROL_AUTONOMOUS
 *    - \c DS_CONTROL_TELEOPERATED
 *
 * \note This function will return a valid control mode, even if the client is
 *       not connected to the robot.
 */
extern DS_Control DS_GetControlMode();

/**
 * Updates the team number.
 * \note When you update the team number, the IP addresses of the FMS, radio
 *       and robot will be updated automatically
 */
extern void DS_SetTeamNumber (const int team);

/**
 * Updates the enabled state of the robot.
 * \note If \c DS_CanBeEnabled() returns \c DS_FALSE, this function will
 *       automatically disable the robot
 */
extern void DS_SetEnabled (const DS_Bool enabled);

/**
 * Changes the control mode of the robot
 */
extern void DS_SetControlMode (const DS_Control mode);

/**
 * Changes the emergency stop state of the robot.
 */
extern void DS_SetEmergencyStopped (const DS_Bool stopped);

/**
 * Changes the IP address of the FMS.
 * \note The \a ip must be resolved, you cannot input an mDNS address
 */
extern void DS_SetFMSAddress (const char* ip);

/**
 * Changes the IP address of the radio.
 * \note The \a ip must be resolved, you cannot input an mDNS address
 */
extern void DS_SetRadioAddress (const char* ip);

/**
 * Changes the IP address of the robot.
 * \note The \a ip must be resolved, you cannot input an mDNS address
 */
extern void DS_SetRobotAddress (const char* ip);

/**
 * Registers a new joystick with the given \a data
 */
extern void DS_RegisterJoystick (const char* name,
                                 const int axes,
                                 const int hats,
                                 const int buttons);

/**
 * Returns the number of joysticks attached to the DS
 */
extern int DS_JoystickCount();

/**
 * Returns the number of axes managed by the given \a joystick
 */
extern int DS_GetNumAxes (const int joystick);

/**
 * Returns the number of hats managed by the given \a joystick
 */
extern int DS_GetNumHats (const int joystick);

/**
 * Returns the number of button managed by the given \a joystick
 */
extern int DS_GetNumButtons (const int joystick);

/**
 * Returns the name of the given joystick
 */
extern char* DS_GetJoystickName (const int joystick);

/**
 * Removes the given \a joystick
 */
extern int DS_RemoveJoystick (const int joystick);

/**
 * Sends the given \a message to the NetConsole
 */
extern void DS_SendNetConsoleMessage (const char* message);

#ifdef __cplusplus
}
#endif

#endif
