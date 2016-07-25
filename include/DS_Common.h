/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#ifndef _LIBDS_COMMON_H
#define _LIBDS_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Used at everything to indicate that something has gone wrong
 */
static const int DS_INVALID = -1;

/**
 * Represents a simple value that can have two states
 */
typedef enum {
    DS_TRUE  = 1,
    DS_FALSE = 0,
} DS_Bool;

/**
 * Represents the available robot control modes
 */
typedef enum {
    DS_CONTROL_TEST,
    DS_CONTROL_AUTONOMOUS,
    DS_CONTROL_TELEOPERATED
} DS_Control;

/**
 * Represents the available socket types for a protocol to use
 */
typedef enum {
    DS_SOCKET_UDP,
    DS_SOCKET_TCP,
    DS_SOCKET_INVALID,
} DS_SocketType;

/**
 * Represents the available button states
 */
typedef enum {
    DS_BUTTON_PRESSED  = 0x01,
    DS_BUTTON_RELEASED = 0x00,
} DS_Button;

/**
 * Represents the available POV/hat states
 */
typedef enum {
    DS_HAT_UP        = 0x01,
    DS_HAT_DOWN      = 0x02,
    DS_HAT_LEFT      = 0x04,
    DS_HAT_RIGHT     = 0x08,
    DS_HAT_CENTERED  = 0x00,
    DS_HAT_LEFTUP    = DS_HAT_UP | DS_HAT_LEFT,
    DS_HAT_RIGHTTUP  = DS_HAT_UP | DS_HAT_RIGHT,
    DS_HAT_LEFTDOWN  = DS_HAT_DOWN | DS_HAT_LEFT,
    DS_HAT_RIGHTDOWN = DS_HAT_DOWN | DS_HAT_RIGHT,
} DS_Hat;

#ifdef __cplusplus
}
#endif

#endif
