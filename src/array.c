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

/**
 * Deallocates the memory used to store the \a array data and resets the
 * properties of the \a array to 0.
 */
void DS_ArrayFree (DS_Array* array)
{
    /* Array pointer is NULL */
    if (!array)
        return;

    /* De-allocate array data */
    free (array->data);

    /* Update array properties */
    array->used = 0;
    array->size = 0;
    array->data = NULL;
}

/**
 * Inserts the given \a element in the \a array and resizes the allocated
 * memory if required.
 */
void DS_ArrayInstert (DS_Array* array, void** element)
{
    /* Array pointer is NULL */
    if (!array)
        return;

    /* Resize array if required */
    if (array->used == array->size) {
        array->size *= 2;
        array->data = (void*) realloc (array->data,
                                       array->size * sizeof (void*));
    }

    /* Insert element */
    array->data [array->used++] = element;
}

/**
 * Initializes the data list and properties of the given \a array
 */
void DS_ArrayInit (DS_Array* array, size_t initial_size)
{
    /* Array pointer is NULL */
    if (!array)
        return;

    /* Allocate array data */
    array->data = (void*) realloc (array->data, initial_size);

    /* Update array data */
    array->used = 0;
    array->size = initial_size;
}
