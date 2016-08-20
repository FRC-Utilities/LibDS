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

#include "DS_Utils.h"
#include "DS_Queue.h"

#include <stdlib.h>
#include <string.h>

/**
 * Removes the first item in the given \a queue
 *
 * \param queue the queue in which to operate
 * \returns \c 1 on success, \c 0 on failure
 */
extern int DS_QueuePop (DS_Queue* queue)
{
    /* Queue is NULL */
    if (!queue)
        return 0;

    /* There are no items on the queue */
    if (queue->count <= 0)
        return 0;

    /* Update queue properties */
    --queue->count;
    ++queue->front;

    /* This is a circular queue, go back to the beginning */
    if (queue->front >= queue->max_elements)
        queue->front = 0;

    return 1;
}

/**
 * Resets the properties of the given \a queue and de-allocates the memory
 * used by the queue's elements
 *
 * \param queue the queue to destroy
 */
extern void DS_QueueFree (DS_Queue* queue)
{
    /* Queue is NULL, abort */
    if (!queue)
        return;

    /* Free all the items in the queue */
    int element;
    for (element = 0; element < queue->max_elements; ++element)
        DS_FREE (queue->buffer [element]);

    /* Delete the buffer */
    DS_FREE (queue->buffer);

    /* Reset queue properties */
    queue->rear = -1;
    queue->count = 0;
    queue->front = 0;
    queue->item_size = 0;
    queue->max_elements = 0;
}

/**
 * Returns the first elemenent of the given \a queue
 *
 * \param queue the queue from which to get the first item_size
 * \returns the first element of the given \a queue, on failure, this
 *          function shall return a \c NULL pointer
 */
void* DS_QueueGetFirst (DS_Queue* queue)
{
    /* We won't crash, unless the programmer does not check return value */
    if (!queue)
        return NULL;

    /* There are no items on the queue */
    if (queue->count <= 0)
        return NULL;

    /* Get the first item in the list */
    return (void*) queue->buffer [queue->front];
}

/**
 * Adds the given \a item to the \a queue in a circular fashion
 *
 * \param queue the queue in which to operate
 * \param item the item to add, to avoid potential errors, the pointer data
 *        is duplicated by this function
 */
void DS_QueuePush (DS_Queue* queue, void* item)
{
    /* You can thank me later */
    if (!queue)
        return;

    /* Queue is full, abort */
    if (queue->count >= queue->max_elements)
        return;

    /* Update queue properties */
    ++queue->rear;
    ++queue->count;

    /* Front item should be somewhere in the middle by now */
    if (queue->rear >= queue->max_elements)
        queue->rear = 0;

    /* Since we don't want to depend on potential idiots, copy the data */
    memcpy (queue->buffer [queue->rear], item, queue->item_size);
}

/**
 * Initializes the given queue and allocates memory for its elements
 *
 * \param queue the queue to initialize
 * \param max_elements the maximum number of elements that the queue can have
 * \param item_size the size to use for each inidividual element of the queue
 */
void DS_QueueInit (DS_Queue* queue, int max_elements, int item_size)
{
    /* Set initial navitation values */
    queue->rear = -1;
    queue->count = 0;
    queue->front = 0;

    /* Set queue properties */
    queue->item_size = item_size;
    queue->max_elements = max_elements;

    /* Initialize the pointer list */
    queue->buffer = calloc (max_elements, max_elements * item_size);

    /* Initialize each item in the list */
    int item;
    for (item = 0; item < max_elements; ++item) {
        queue->buffer [item] = malloc (item_size);
        memset (queue->buffer [item], 0, item_size);
    }
}
