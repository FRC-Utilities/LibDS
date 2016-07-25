/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "DS_Private.h"

typedef struct {
    int numAxes;
    int numHats;
    int numButtons;
    DS_Hat* hatAngles;
    double* axisValues;
    DS_Button* buttonStates;
} Joystick;

Joystick* JOYSTICKS[];

void joystick_init()
{

}

int joystick_count()
{

}

int joystick_remove (const int index)
{
    (void) index;
}

int joystick_num_axes (const int index)
{
    (void) index;
}

int joystick_num_hats (const int index)
{
    (void) index;
}

int joystick_num_buttons (const int index)
{
    (void) index;
}

char* joystick_name (const int index)
{
    (void) index;
}

void joystick_update_hat (const int index, const int hat, const DS_Hat angle)
{

}

void joystick_update_axis (const int index, const int axis, const double value)
{

}

void joystick_add (const char* name, const int axes, const int hats, const int buttons)
{

}

void joystick_update_button (const int index, const int button, const DS_Button pressedState)
{

}
