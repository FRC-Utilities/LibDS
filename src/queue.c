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
#include <string.h>

/**
 * Removes an item from the \a queue
 */
int DS_QueuePop (DS_Queue* queue)
{
    /* Pointer is NULL */
    if (!queue)
        return 0;

    /* There are no items in the queue */
    if (queue->count < 1)
        return 0;

    /* Re-assign the pointer of the last item */
    if (queue->tail == queue->buffer_end)
        queue->tail = queue->buffer;

    /* Decrease the number of items in queue */
    queue->count--;
    return 1;
}

/**
 * De-allocates the memory used by the \a queue and resets its properties
 */
void DS_QueueFree (DS_Queue* queue)
{
    /* Pointer is NULL */
    if (!queue)
        return;

    /* De-allocate the queue data */
    free (queue->buffer);

    /* Reset the queue's properties */
    queue->count = 0;
    queue->capacity = 0;
    queue->item_size = 0;
}

/**
 * Inserts the given \a item into the \a queue
 */
void DS_QueuePush (DS_Queue* queue, void** item)
{
    /* Pointer is NULL */
    if (!queue)
        return;

    /* We have reached the queue's capacity */
    if (queue->count == queue->capacity)
        return;

    /* Copy the given item to the first element */
    memcpy (queue->head, item, queue->item_size);

    /* Re-assign the pointer of the first element */
    if (queue->head == queue->buffer_end)
        queue->head = queue->buffer;

    /* Increase the items in queue */
    queue->count++;
}

/**
 * Initializes the given \a queue with the given \a capacity and \a item size
 */
void DS_QueueInit (DS_Queue* queue, size_t capacity, size_t item)
{
    /* Pointer is NULL */
    if (!queue)
        return;

    /* Set general properties */
    queue->count = 0;
    queue->item_size = item;
    queue->capacity = capacity;

    /* Initialize the data buffer */
    queue->buffer = malloc (capacity * item);
    queue->buffer_end = (char*) queue->buffer + capacity * item;

    /* Initialize the first and last item pointer */
    queue->head = queue->buffer;
    queue->tail = queue->buffer;
}
