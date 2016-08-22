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
#include "DS_Timer.h"

#include <stdio.h>
#include <pthread.h>

#if defined _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

static int running = 1;

/**
 * Updates the properties of the given \a timer
 * This function is called in a separate thread for each timer that
 * we use.
 */
static void* update_timer (void* ptr)
{
    if (!ptr)
        return NULL;

    DS_Timer* timer = (DS_Timer*) ptr;

    while (running == 1) {
        if (timer->enabled && timer->time > 0 && !timer->expired) {
            timer->elapsed += timer->precision;

            if (timer->elapsed >= timer->time)
                timer->expired = 1;
        }

        DS_Sleep (timer->precision);
    }

    pthread_join (timer->thread, NULL);
    return NULL;
}

/**
 * Breaks all the timer loops
 */
void Timers_Close()
{
    running = 0;
}

/**
 * Pauses the execution state of the program/thread for the given
 * number of \a millisecs.
 *
 * We use this function to update each timer based on its precision
 */
void DS_Sleep (const int millisecs)
{
#if defined _WIN32
    Sleep (millisecs);
#else
    usleep (millisecs * 1000);
#endif
}

/**
 * Resets and disables the given \a timer
 */
void DS_TimerStop (DS_Timer* timer)
{
    if (timer) {
        timer->enabled = 0;
        timer->expired = 0;
        timer->elapsed = 0;
    }
}

/**
 * Resets and enables the given \a timer
 */
void DS_TimerStart (DS_Timer* timer)
{
    if (timer) {
        timer->enabled = 1;
        timer->expired = 0;
        timer->elapsed = 0;
    }
}

/**
 * Resets the elapsed time and expired state of the given \a timer
 */
void DS_TimerReset (DS_Timer* timer)
{
    if (timer) {
        timer->expired = 0;
        timer->elapsed = 0;
    }
}

/**
 * Initializes the given \a timer with the given \a time and \a precision.
 * The timers are updated using a threaded while loop (that sleeps the number
 * of milliseconds specified with \a precision).
 *
 * This function will return \c 0 if the timer thread fails to start.
 */
int DS_TimerInit (DS_Timer* timer, const int time, const int precision)
{
    if (timer) {
        /* Timer has already been initialized, fuck off */
        if (timer->initialized)
            return 0;

        /* Configure the timer */
        timer->enabled = 0;
        timer->expired = 0;
        timer->elapsed = 0;
        timer->time = time;
        timer->initialized = 1;
        timer->precision = precision;

        /* Configure the thread */
        int error = pthread_create (&timer->thread, NULL,
                                    &update_timer, (void*) timer);

        /* Report thread creation errors */
        if (error)
            fprintf (stderr, "Cannot create timer thread, error %d\n", error);

        /* Return 1 if there are no errors */
        return (error == 0);
    }

    return 0;
}

