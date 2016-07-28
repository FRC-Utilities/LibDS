/*
 * The Driver Station Library (LibDS)
 * Copyright (C) 2015-2016 Alex Spataru <alex_spataru@outlook>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the 'Software'),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "DS_Utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * Returns \c true if the given \a string is empty
 */
bool DS_StringIsEmpty (const char* string)
{
    if (string)
        return (string [0] == 0);

    return true;
}

/**
 * Returns a string in the format of NET.TE.AM.HOST, examples include
 *    - \c DS_GetStaticIP (10, 3794, 2) will return \c 10.37.94.2
 *    - \c DS_GetStaticIP (10, 18, 1) will return \c 10.0.18.1
 *    - and so on...
 *
 * This function is used to simplify the design and implementation of
 * different communication protocols.
 *
 * If you call this function outside the scope of the \c LibDS, remember to
 * call \c free() to avoid memory leaks.
 */
char* DS_GetStaticIP (const int net, const int team, const int host)
{
    char* string = (char*) malloc (sizeof (char) * 15);

    sprintf (string,
             "%d.%d.%d.%d",
             net,
             (team & 0xff00) >> 8,
             (team & 0xff),
             host);

    return string;
}
