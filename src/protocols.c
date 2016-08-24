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
#include "DS_Timer.h"
#include "DS_Client.h"
#include "DS_Config.h"
#include "DS_Events.h"
#include "DS_Socket.h"
#include "DS_Protocol.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define SEND_PRECISION 1  /* Update the sender timers every millisecond */
#define RECV_PRECISION 50 /* Update the watchdogs every 50 milliseconds */

/*
 * Holds a pointer to the current protocol in use
 */
static DS_Protocol* protocol;

/*
 * Define the sender watchdogs (when one expires, we send a packet)
 */
static DS_Timer fms_send_timer;
static DS_Timer radio_send_timer;
static DS_Timer robot_send_timer;

/*
 * Define the receiver watchdogs (when one expires, comms are lost)
 */
static DS_Timer fms_recv_timer;
static DS_Timer radio_recv_timer;
static DS_Timer robot_recv_timer;

/*
 * If set to anything else than 0, then the event loop will be allowed to run
 */
static int running = 0;

/*
 * Protocol read success booleans (used to feed the watchdogs)
 */
static int fms_read = 0;
static int radio_read = 0;
static int robot_read = 0;

/*
 * Holds the received data
 */
static sds fms_data = NULL;
static sds radio_data = NULL;
static sds robot_data = NULL;
static sds netcs_data = NULL;

/*
 * Event thread ID
 */
static pthread_t thread;

/**
 * Sends a new packet to the FMS, the generated data is immediatly deleted
 * once the packet has been sent
 */
static void send_fms_data()
{
    sds data = protocol->create_fms_packet();
    DS_SocketSend (&protocol->fms_socket, data);
    DS_FREESTR (data);
}

/**
 * Sends a new packet to the radio, the generated data is immediatly deleted
 * once the packet has been sent
 */
static void send_radio_data()
{
    sds data = protocol->create_radio_packet();
    DS_SocketSend (&protocol->radio_socket, data);
    DS_FREESTR (data);
}

/**
 * Sends a new packet to the robot, the generated data is immediatly deleted
 * once the packet has been sent
 */
static void send_robot_data()
{
    sds data = protocol->create_robot_packet();
    DS_SocketSend (&protocol->robot_socket, data);

    for (int i = 0; i < (int) sdslen (data); ++i)
        fprintf (stderr, "%02x ", (uint8_t) data [i]);

    fprintf (stderr, "\n");

    DS_FREESTR (data);
}

/**
 * Sends data over the network using the functions of the current protocol.
 * If there is no protocol running, then this function will do nothing.
 */
static void send_data()
{
    /* Protocol is NULL, abort */
    if (!protocol)
        return;

    /* Send FMS packet */
    if (fms_send_timer.expired) {
        send_fms_data();
        DS_TimerReset (&fms_send_timer);
    }

    /* Send radio packet */
    if (radio_send_timer.expired) {
        send_radio_data();
        DS_TimerReset (&radio_send_timer);
    }

    /* Send robot packet */
    if (robot_send_timer.expired) {
        send_robot_data();
        DS_TimerReset (&robot_send_timer);
    }
}

/**
 * Clears the strings that hold the incoming data packets
 */
static void clear_recv_data()
{
    DS_FREESTR (fms_data);
    DS_FREESTR (radio_data);
    DS_FREESTR (robot_data);
    DS_FREESTR (netcs_data);
}

/**
 * Reads the received data using the functions provided by the current protocol.
 * If there is no protocol running, then this function will do nothing.
 */
static void recv_data()
{
    /* Protocol is NULL, abort */
    if (!protocol)
        return;

    /* Reset the data pointers */
    clear_recv_data();

    /* Read data from sockets */
    fms_data = DS_SocketRead (&protocol->fms_socket);
    radio_data = DS_SocketRead (&protocol->radio_socket);
    robot_data = DS_SocketRead (&protocol->robot_socket);
    netcs_data = DS_SocketRead (&protocol->netconsole_socket);

    /* Read FMS packet */
    if (sdslen (fms_data) > 0) {
        fms_read = protocol->read_fms_packet (fms_data);
        CFG_SetFMSCommunications (fms_read);
    }

    /* Read radio packet */
    if (sdslen (radio_data) > 0) {
        radio_read = protocol->read_radio_packet (radio_data);
        CFG_SetRadioCommunications (radio_read);
    }

    /* Read robot packet */
    if (sdslen (robot_data) > 0) {
        robot_read = protocol->read_robot_packet (robot_data);
        CFG_SetRobotCommunications (robot_read);
    }

    /* Add NetConsole message to event system */
    if (sdslen (netcs_data) > 0) {
        DS_Event event;
        event.netconsole.type = DS_NETCONSOLE_NEW_MESSAGE;
        event.netconsole.message = sdsnew (netcs_data);
        DS_AddEvent (&event);
    }

    /* Reset the data pointers */
    clear_recv_data();
}

/**
 * Feeds the watchdogs, updates them and checks if any of them has expired
 */
static void update_watchdogs()
{
    /* Feed the watchdogs if packets are read */
    if (fms_read)   DS_TimerReset (&fms_recv_timer);
    if (radio_read) DS_TimerReset (&radio_recv_timer);
    if (robot_read) DS_TimerReset (&robot_recv_timer);

    /* Clear the read success values */
    fms_read = 0;
    radio_read = 0;
    robot_read = 0;

    /* Reset the FMS if the watchdog expires */
    if (fms_recv_timer.expired) {
        CFG_FMSWatchdogExpired();
        DS_TimerReset (&fms_recv_timer);
    }

    /* Reset the radio if the watchdog expires */
    if (radio_recv_timer.expired) {
        CFG_RadioWatchdogExpired();
        DS_TimerReset (&radio_recv_timer);
    }

    /* Reset the robot if the watchdog expires */
    if (robot_recv_timer.expired) {
        CFG_RobotWatchdogExpired();
        DS_TimerReset (&robot_recv_timer);
    }
}

/**
 * This function is executed periodically, the function does the following:
 *    - Send data to the FMS, robot and radio
 *    - Read received data from the FMS, robot and radio
 *    - Feed/reset the watchdogs
 *    - Check if any of the watchdogs has expired
 */
static void* run_event_loop()
{
    while (running) {
        send_data();
        recv_data();
        update_watchdogs();

        DS_Sleep (5);
    }

    return NULL;
}

/**
 * Returns a pointer to the current protocol
 */
DS_Protocol* DS_CurrentProtocol()
{
    return protocol;
}

/**
 * Initializes the protocol sender/receiver thread and the timers
 */
void Protocols_Init()
{
    /* Initialize sender timers */
    DS_TimerInit (&fms_send_timer,   0, SEND_PRECISION);
    DS_TimerInit (&radio_send_timer, 0, SEND_PRECISION);
    DS_TimerInit (&robot_send_timer, 0, SEND_PRECISION);

    /* Initialize watchdog timers */
    DS_TimerInit (&fms_recv_timer,   0, RECV_PRECISION);
    DS_TimerInit (&radio_recv_timer, 0, RECV_PRECISION);
    DS_TimerInit (&robot_recv_timer, 0, RECV_PRECISION);

    /* Allow the event loop to run */
    running = 1;

    /* Configure the event thread */
    int error = pthread_create (&thread, NULL, &run_event_loop, NULL);

    /* Quit if the thread fails to start */
    if (error) {
        fprintf (stderr, "Cannot initialize event thread");
        exit (EXIT_FAILURE);
    }
}

/**
 * De-allocates the current protocol and closes its sockets
 */
static void close_protocol()
{
    /* Protocol is NULL, abort */
    if (!protocol)
        return;

    /* Close the sockets */
    DS_SocketClose (&protocol->fms_socket);
    DS_SocketClose (&protocol->radio_socket);
    DS_SocketClose (&protocol->robot_socket);
    DS_SocketClose (&protocol->netconsole_socket);

    /* Stop sender timers */
    DS_TimerStop (&fms_send_timer);
    DS_TimerStop (&radio_send_timer);
    DS_TimerStop (&robot_send_timer);

    /* Stop receiver timers */
    DS_TimerStop (&fms_recv_timer);
    DS_TimerStop (&radio_recv_timer);
    DS_TimerStop (&robot_recv_timer);

    /* De-allocate the protocol */
    DS_FREE (protocol);
}

/**
 * Stops the sender/receiver thread and deletes the allocated protocol
 */
void Protocols_Close()
{
    running = 0;
    close_protocol();
    pthread_join (thread, NULL);
}

/**
 * De-allocates the current protocol and loads the given protocol
 *
 * \param ptr pointer to the new protocol implementation to load
 */
void DS_ConfigureProtocol (DS_Protocol* ptr)
{
    /* Pointer is NULL, abort */
    if (!ptr)
        return;

    /* Close previous protocol */
    close_protocol();

    /* Re-assign the protocol */
    protocol = ptr;

    /* Update sockets */
    DS_SocketOpen (&ptr->fms_socket);
    DS_SocketOpen (&ptr->radio_socket);
    DS_SocketOpen (&ptr->robot_socket);
    DS_SocketOpen (&ptr->netconsole_socket);

    /* Update sender timers */
    fms_send_timer.time = ptr->fms_interval;
    radio_send_timer.time = ptr->radio_interval;
    robot_send_timer.time = ptr->robot_interval;

    /* Update watchdogs */
    fms_recv_timer.time = DS_Min (ptr->fms_interval * 50, 1000);
    radio_recv_timer.time = DS_Min (ptr->radio_interval * 50, 1000);
    robot_recv_timer.time = DS_Min (ptr->robot_interval * 50, 1000);

    /* Start the timers */
    DS_TimerStart (&fms_send_timer);
    DS_TimerStart (&fms_recv_timer);
    DS_TimerStart (&radio_send_timer);
    DS_TimerStart (&radio_recv_timer);
    DS_TimerStart (&robot_send_timer);
    DS_TimerStart (&robot_recv_timer);
}
