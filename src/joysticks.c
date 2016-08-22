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

#include "DS_Array.h"
#include "DS_Config.h"
#include "DS_Events.h"
#include "DS_Joysticks.h"

/**
 * Represents a joystick and its information
 */
typedef struct _joystick {
    int* hats;       /**< An array with the hat angles */
    double* axes;    /**< An array with the axis values */
    int* buttons;    /**< An array with the button states */
    int num_axes;    /**< The number of axes of the joystick */
    int num_hats;    /**< The number of hats of the joystick */
    int num_buttons; /**< The number of buttons of the joystick */
} DS_Joystick;

/**
 * Holds all the joysticks
 */
static DS_Array array;

/**
 * Registers a joystick event to the LibDS event system
 */
static void register_event()
{
    DS_Event event;
    event.joystick.count = DS_GetJoystickCount();
    event.joystick.type = DS_JOYSTICK_COUNT_CHANGED;
    DS_AddEvent (&event);
}

/**
 * Returns the joystick structure at the given index
 */
DS_Joystick* get_joystick (int joystick)
{
    if ((int) array.used > joystick)
        return (DS_Joystick*) array.data [joystick];

    return NULL;
}

/**
 * Initializes the joystick array
 */
void Joysticks_Init()
{
    DS_ArrayInit (&array, (sizeof (DS_Joystick) * 2));
}

/**
 * De-allocates the joystick array data
 */
void Joysticks_Close()
{
    DS_JoysticksReset();
}

/**
 * Returns the number of joysticks registered with the LibDS
 */
int DS_GetJoystickCount()
{
    return array.used;
}

/**
 * Returns the number of hats that the given \a joystick has.
 * If the joystick does not exist, this function will return \c 0
 */
int DS_GetJoystickNumHats (int joystick)
{
    if (DS_GetJoystickCount() > joystick)
        return get_joystick (joystick)->num_hats;

    return 0;
}

/**
 * Returns the number of axes that the given \a joystick has.
 * If the joystick does not exist, this function will return \c 0
 */
int DS_GetJoystickNumAxes (int joystick)
{
    if (DS_GetJoystickCount() > joystick)
        return get_joystick (joystick)->num_axes;

    return 0;
}

/**
 * Returns the number of buttons that the given \a joystick has.
 * If the joystick does not exist, this function will return \c 0
 */
int DS_GetJoystickNumButtons (int joystick)
{
    if (DS_GetJoystickCount() > joystick)
        return get_joystick (joystick)->num_buttons;

    return 0;
}

/**
 * Returns the value that the given \a hat in the given \a joystick has.
 * If the joystick or hat do not exist, this function will return \c 0
 *
 * \note Regardless of protocol implementation, this function will return
 *       a neutral value if the robot is disabled. This is for additional
 *       safety!
 */
int DS_GetJoystickHat (int joystick, int hat)
{
    if (DS_GetJoystickCount() > joystick && CFG_GetRobotEnabled()) {
        if (get_joystick (joystick)->num_hats > hat)
            return get_joystick (joystick)->hats [hat];
    }

    return 0;
}

/**
 * Returns the value that the given \a axis in the given \a joystick has.
 * If the joystick or axis do not exist, this function will return \c 0
 *
 * \note Regardless of protocol implementation, this function will return
 *       a neutral value if the robot is disabled. This is for additional
 *       safety!
 */
double DS_GetJoystickAxis (int joystick, int axis)
{
    if (DS_GetJoystickCount() > joystick && CFG_GetRobotEnabled()) {
        if (get_joystick (joystick)->num_axes > axis)
            return get_joystick (joystick)->axes [axis];
    }

    return 0;
}

/**
 * Returns the value that the given \a button in the given \a joystick has.
 * If the joystick or button do not exist, this function will return \c 0
 *
 * \note Regardless of protocol implementation, this function will return
 *       a neutral value if the robot is disabled. This is for additional
 *       safety!
 */
int DS_GetJoystickButton (int joystick, int button)
{
    if (DS_GetJoystickCount() > joystick && CFG_GetRobotEnabled()) {
        if (get_joystick (joystick)->num_buttons > button)
            return get_joystick (joystick)->buttons [button];
    }

    return 0;
}

/**
 * Removes all the registered joysticks from the LibDS
 */
void DS_JoysticksReset()
{
    DS_ArrayFree (&array);
    register_event();
}

/**
 * Registers a new joystick with the given number of \a axes, \a hats and
 * \a buttons. All joystick values are set to a neutral state to ensure
 * safe operation of the robot.
 */
void DS_JoysticksAdd (const int axes, const int hats, const int buttons)
{
    /* Allocate memory for a new joystick */
    DS_Joystick* joystick = calloc (1, sizeof (DS_Joystick));

    /* Set joystick properties */
    joystick->num_axes = axes;
    joystick->num_hats = hats;
    joystick->num_buttons = buttons;

    /* Set joystick value arrays */
    joystick->hats = calloc (hats, sizeof (int));
    joystick->axes = calloc (axes, sizeof (double));
    joystick->buttons = calloc (buttons, sizeof (int));

    /* Register the new joystick in the joystick list */
    DS_ArrayInsert (&array, (void*) joystick);

    /* Emit the joystick count changed event */
    register_event();
}

/**
 * Updates the \a angle of the given \a hat in the given \a joystick
 */
void DS_SetJoystickHat (int joystick, int hat, int angle)
{
    if (DS_GetJoystickCount() > joystick) {
        if (get_joystick (joystick)->num_hats > hat)
            get_joystick (joystick)->hats [hat] = angle;
    }
}

/**
 * Updates the \a value of the given \a axis in the given \a joystick
 */
void DS_SetJoystickAxis (int joystick, int axis, double value)
{
    if (DS_GetJoystickCount() > joystick) {
        if (get_joystick (joystick)->num_axes > axis)
            get_joystick (joystick)->axes [axis] = value;
    }
}

/**
 * Updates the \a pressed state of the given \a button in the given \a joystick
 */
void DS_SetJoystickButton (int joystick, int button, int pressed)
{
    if (DS_GetJoystickCount() > joystick) {
        if (get_joystick (joystick)->num_buttons > button)
            get_joystick (joystick)->buttons [button] = pressed;
    }
}
