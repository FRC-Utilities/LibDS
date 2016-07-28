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

#ifndef _LIB_DS_CLIENT_H
#define _LIB_DS_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "DS_Types.h"

/* Init/close functions */
extern void Client_Init();
extern void Client_Close();

/* User-set addresses */
extern const char* DS_GetCustomFMSAddress();
extern const char* DS_GetCustomRadioAddress();
extern const char* DS_GetCustomRobotAddress();

/* Protocol-set addresses */
extern const char* DS_GetDefaultFMSAddress();
extern const char* DS_GetDefaultRadioAddress();
extern const char* DS_GetDefaultRobotAddress();

/* Used addresses */
extern const char* DS_GetAppliedFMSAddress();
extern const char* DS_GetAppliedRadioAddress();
extern const char* DS_GetAppliedRobotAddress();

/* Getters */
extern int DS_GetTeamNumber();
extern bool DS_GetRobotCode();
extern bool DS_GetCanBeEnabled();
extern bool DS_GetRobotEnabled();
extern int DS_GetRobotCPUUsage();
extern int DS_GetRobotRAMUsage();
extern int DS_GetRobotDiskUsage();
extern double DS_GetRobotVoltage();
extern DS_Alliance DS_GetAlliance();
extern DS_Position DS_GetPosition();
extern bool DS_GetEmergencyStopped();
extern bool DS_GetFMSCommunications();
extern bool DS_GetRadioCommunications();
extern bool DS_GetRobotCommunications();
extern DS_ControlMode DS_GetControlMode();

/* Setters */
extern void DS_RebootRobot();
extern void DS_RestartRobotCode();
extern void DS_SetTeamNumber (const int team);
extern void DS_SetRobotEnabled (const bool enabled);
extern void DS_SetEmergencyStopped (const bool stop);
extern void DS_SetAlliance (const DS_Alliance alliance);
extern void DS_SetPosition (const DS_Position position);
extern void DS_SetControlMode (const DS_ControlMode mode);
extern void DS_SetCustomFMSAddress (const char* address);
extern void DS_SetCustomRadioAddress (const char* address);
extern void DS_SetCustomRobotAddress (const char* address);

#ifdef __cplusplus
}
#endif

#endif
