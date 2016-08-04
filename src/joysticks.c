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
static DS_Array* array;

/**
 * Returns the joystick structure at the given index
 */
DS_Joystick* get_joystick (int joystick)
{
    return (DS_Joystick*) array->data [joystick];
}

/**
 * Initializes the joystick array
 */
void Joysticks_Init()
{
    array = (DS_Array*) malloc (sizeof (DS_Array));
    DS_ArrayInit (array, (sizeof (DS_Joystick) * 2));
}

/**
 * De-allocates the joystick array data
 */
void Joysticks_Close()
{
    DS_JoysticksReset();
    free (array);
}

/**
 * Returns the number of joysticks registered with the LibDS
 */
int DS_GetJoystickCount()
{
    return array->used;
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
 */
int DS_GetJoystickHat (int joystick, int hat)
{
    if (DS_GetJoystickCount() > joystick) {
        if (get_joystick (joystick)->num_hats > hat)
            return get_joystick (joystick)->hats [hat];
    }

    return 0;
}

/**
 * Returns the value that the given \a axis in the given \a joystick has.
 * If the joystick or axis do not exist, this function will return \c 0
 */
double DS_GetJoystickAxis (int joystick, int axis)
{
    if (DS_GetJoystickCount() > joystick) {
        if (get_joystick (joystick)->num_axes > axis)
            return get_joystick (joystick)->axes [axis];
    }

    return 0;
}

/**
 * Returns the value that the given \a button in the given \a joystick has.
 * If the joystick or button do not exist, this function will return \c 0
 */
int DS_GetJoystickButton (int joystick, int button)
{
    if (DS_GetJoystickCount() > joystick) {
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
    DS_ArrayFree (array);
}

/**
 * Registers a new joystick with the given number of \a axes, \a hats and
 * \a buttons. All joystick values are set to a neutral state to ensure
 * safe operation of the robot.
 */
void DS_JoysticksAdd (int axes, int hats, int buttons)
{
    /* Initialize the joystick values */
    int* v_hats    = (int*) malloc (sizeof (int) * hats);
    int* v_buttons = (int*) malloc (sizeof (int) * buttons);
    double* v_axes = (double*) malloc (sizeof (double) * axes);

    /* Ensure that hat angles are neutral */
    for (int i = 0; i < hats; ++i)
        v_hats [i] = 0;

    /* Ensure that axis values are neutral */
    for (int i = 0; i < axes; ++i)
        v_axes [i] = 0;

    /* Ensure that button states are neutral */
    for (int i = 0; i < buttons; ++i)
        v_buttons [i] = 0;

    /* Allocate memory for a new joystick */
    DS_Joystick* joystick = (DS_Joystick*) malloc (sizeof (DS_Joystick));

    /* Set joystick properties */
    joystick->num_axes = axes;
    joystick->num_hats = hats;
    joystick->num_buttons = buttons;

    /* Set joystick values */
    joystick->axes = v_axes;
    joystick->hats = v_hats;
    joystick->buttons = v_buttons;

    /* Register the new joystick in the joystick list */
    DS_ArrayInsert (array, (void*) joystick);
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
