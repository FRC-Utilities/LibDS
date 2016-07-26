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

#ifndef _LIB_DS_INTERNAL_H
#define _LIB_DS_INTERNAL_H

#include <stdint.h>
#include <stdbool.h>

#include "DS_Types.h"
#include "DS_Objects.h"

/*
 * WARNING:
 *
 * Do not use this header in your application unless you are implementing your
 * own protocols. Only include this header where it is required, do not include
 * it for the lolz.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Getters */
extern bool CFG_GetRobotCode();
extern bool CFG_GetRobotEnabled();
extern int CFG_GetRobotCPUUsage();
extern int CFG_GetRobotRAMUsage();
extern int CFG_GetRobotDiskUsage();
extern double CFG_GetRobotVoltage();
extern bool CFG_GetEmergencyStopped();
extern bool CFG_GetFMSCommunications();
extern bool CFG_GetRadioCommunications();
extern bool CFG_GetRobotCommunications();

/* Setters */
extern void CFG_SetRobotCode (bool code);
extern void CFG_SetRobotEnabled (bool enabled);
extern void CFG_SetRobotCPUUsage (int percent);
extern void CFG_SetRobotRAMUsage (int percent);
extern void CFG_SetRobotDiskUsage (int percent);
extern void CFG_SetRobotVoltage (double voltage);
extern void CFG_SetEmergencyStopped (bool stopped);
extern void CFG_SetFMSCommunications (bool communications);
extern void CFG_SetRadioCommunications (bool communications);
extern void CFG_SetRobotCommunications (bool communications);

/* Networking functions */
extern int CFG_GetSocket (DS_Socket* socket);
extern int CFG_CloseSocket (DS_Socket* socket);
extern int CFG_SendData (DS_Socket* socket, char* data);
extern int CFG_ReadData (DS_Socket* socket, char* data);

#ifdef __cplusplus
}
#endif

#endif