/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "DS_Protocol.h"
#include "../DS_Private.h"

static Protocol* instance = NULL;

Protocol* DS_Protocol2014()
{
    /* Ensure that protocol is created only once */
    if (!instance) {
        struct _protocol base;

        /* Create protocol */
        instance = (Protocol*) malloc (sizeof (Protocol));
        instance->base = &base;
    }

    /* Return protocol pointer */
    return instance;
}
