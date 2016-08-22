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
 * Appends the given \a data to the \a string
 *
 * \param string the original string
 * \param data the byte to append
 *
 * \example data = DS_Append (data, 'a')
 *
 * \warning the original string will be deleted to avoid memory leaks,
 *          just use the value returned by this function afterwards!
 *
 * \returns the pointer to the newly obtained string
 */
sds DS_Append (sds string, char data)
{
    /* Initialize the variables */
    sds temp;
    size_t len = sdslen (string);

    /* Duplicate string and append data */
    temp = sdsnewlen (string, len + 1);
    temp [len] = data;

    /* Delete original string */
    DS_FREESTR (string);

    /* Return new string */
    return temp;
}

/**
 * Returns \c 1 if the given \a string is empty
 * \note This function will also return \c 1 if the string is \c NULL
 */
int DS_StringIsEmpty (const sds string)
{
    if (string != NULL) {
        if (sdslen (string) <= 0)
            return 1;
        else
            return 0;
    }

    return 1;
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
 * call \c DS_FREESTR() to avoid memory leaks.
 *
 * \param net the desired first octet of the IP
 * \param team the team number, used in second and third octets
 * \param host the host byte (or the last octet) of the IP
 */
sds DS_GetStaticIP (const int net, const int team, const int host)
{
    int te = team / 100;
    int am = team - (te * 100);
    return sdscatprintf (sdsempty(), "%d.%d.%d.%d", net, te, am, host);
}
