/*
 * The Driver Station Library (LibDS)
 * Copyright (C) 2015-2016 Alex Spataru <alex_spataru@outlook>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software
 * is furnished to do so, subject to the following conditions:
 *
 *  1. The above copyright notice and this permission notice shall be included in
 *     all copies or substantial portions of the Software.
 *
 *  2. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 *     OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *     FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *     THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *     LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *     FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *     DEALINGS IN THE SOFTWARE.
 */

#ifndef _LIB_DS_CLIENT_H
#define _LIB_DS_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "DS_Types.h"

extern void DS_Init();
extern void DS_Close();

extern void DS_RebootRobot();
extern void DS_RestartRobotCode();
extern void DS_SetRobotEnabled (bool enabled);
extern void DS_SetEmergencyStop (bool enabled);
extern void DS_SetControlMode (DS_ControlMode mode);

extern int DS_GetTeam();
extern bool DS_GetRobotCode();
extern bool DS_GetCanBeEnabled();
extern bool DS_GetRobotEnabled();
extern bool DS_GetFMSConnected();
extern int DS_GetRobotCPUUsage();
extern int DS_GetRobotRAMUsage();
extern int DS_GetRobotDiskUsage();
extern bool DS_GetRadioConnected();
extern bool DS_GetRobotConnected();
extern double DS_GetRobotVoltage();
extern bool DS_GetRobotEmergencyStop();

#ifdef __cplusplus
}
#endif

#endif