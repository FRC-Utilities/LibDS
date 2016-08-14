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

#include "DS_Queue.h"
#include "DS_Events.h"

#include <string.h>

static DS_Queue events;

/**
 * Initializes the event queue
 */
void Events_Init()
{
    DS_QueueInit (&events, sizeof (DS_Event) * 20, sizeof (DS_Event));
}

/**
 * Frees the event queue
 */
void Events_Close()
{
    DS_QueueFree (&events);
}

/**
 * Adds the given \a event to the event queue
 *
 * \param event the event to register in the event queue
 */
void DS_AddEvent (DS_Event* event)
{
    if (event)
        DS_QueuePush (&events, (void*) event);
}

/**
 * Polls for currently pending events.
 * Returns 1 if there are any pending events, or 0 if there are none available.
 */
int DS_PollEvent (DS_Event* event)
{
    if (DS_QueuePop (&events)) {
        DS_Event* front = (DS_Event*) events.head;

        if (front) {
            memcpy (event, front, sizeof (DS_Event));
            return 1;
        }
    }

    return 0;
}
