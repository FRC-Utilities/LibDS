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

#include "DS_Timer.h"

void DS_TimerReset (DS_Timer* timer)
{
    if (timer) {
        timer->expired = 0;
        timer->time = timer->remaining;
    }
}

void DS_TimerUpdate (DS_Timer* timer)
{
    if (timer) {

    }
}

void DS_TimerInit (DS_Timer* timer, const int time)
{
    if (timer) {
        timer->expired = 0;
        timer->time = time;
        timer->remaining = time;
    }
}

