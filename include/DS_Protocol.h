/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#ifndef _LIBDS_PROTOCOL_H
#define _LIBDS_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "DS_Common.h"

/**
 * Defines the base variables that any protocol must define in order to
 * interact with the LibDS system
 */
struct _protocol {
    /* Packet generation intervals */
    int fmsInterval;
    int radioInterval;
    int robotInterval;

    /* Input ports */
    int fmsInputPort;
    int radioInputPort;
    int robotInputPort;
    int netconsoleInputPort;

    /* Output ports */
    int fmsOutputPort;
    int radioOutputPort;
    int robotOutputPort;
    int netconsoleOutputPort;

    /* Joystick information */
    int maxAxes;
    int maxHats;
    int maxButtons;
    int maxJoysticks;

    /* Socket types */
    DS_SocketType fmsSocket;
    DS_SocketType radioSocket;
    DS_SocketType robotSocket;

    /* Packet interpretation functions */
    DS_Bool (*readFMSPacket) (char* data);
    DS_Bool (*readRadioPacket) (char* data);
    DS_Bool (*readRobotPacket) (char* data);

    /* Packet generation functions */
    char* (*createFMSPacket) (char* data);
    char* (*createRadioPacket) (char* data);
    char* (*createRobotPacket) (char* data);

    /* Recommended addresses */
    char* (*recommendedFMSAddress) (char* string);
    char* (*recommendedRadioAddress) (char* string);
    char* (*recommendedRobotAddress) (char* string);
};

/**
 * Used for actually using protocol info in LibDS
 */
typedef struct {
    struct _protocol* base;
} Protocol;

/**
 * Returns an instance of the 2014 protocol
 */
Protocol* DS_Protocol2014();

/**
 * Returns an instance of the 2015 protocol
 */
Protocol* DS_Protocol2015();

/**
 * Returns an instance of the 2016 protocol
 */
Protocol* DS_Protocol2016();

/**
 * Constructs an static IP using the given \a team number.
 *
 * For example:
 *    - \c DS_StaticAddress(10, 3794, 2) would return \c "10.37.94.2"
 *    - \c DS_StaticAddress(10, 3794, 1) would return \c "10.37.94.1"
 */
extern char* DS_GetStaticAddress (int net, int team, int host);

/**
 * Returns the FMS packet sending interval specified by the current protocol.
 * If there is no protocol loaded, this function will return \c DS_INVALID
 */
extern int DS_ProtocolFMSInterval();

/**
 * Returns the radio packet sending interval specified by the current protocol.
 * If there is no protocol loaded, this function will return \c DS_INVALID
 */
extern int DS_ProtocolRadioInterval();

/**
 * Returns the robot packet sending interval specified by the current protocol.
 * If there is no protocol loaded, this function will return \c DS_INVALID
 */
extern int DS_ProtocolRobotInterval();

/**
 * Returns the port in which the LibDS receives FMS data.
 * If there is no protocol loaded, this function will return \c DS_INVALID
 */
extern int DS_ProtocolFMSInputPort();

/**
 * Returns the port in which the LibDS receives radio data.
 * If there is no protocol loaded, this function will return \c DS_INVALID
 */
extern int DS_ProtocolRadioInputPort();

/**
 * Returns the port in which the LibDS receives robot data.
 * If there is no protocol loaded, this function will return \c DS_INVALID
 */
extern int DS_ProtocolRobotInputPort();

/**
 * Returns the port in which the LibDS sends data to the FMS.
 * If there is no protocol loaded, this function will return \c DS_INVALID
 */
extern int DS_ProtocolFMSOutputPort();

/**
 * Returns the port in which the LibDS sends data to the radio.
 * If there is no protocol loaded, this function will return \c DS_INVALID
 */
extern int DS_ProtocolRadioOutputPort();

/**
 * Returns the port in which the LibDS sends data to the robot.
 * If there is no protocol loaded, this function will return \c DS_INVALID
 */
extern int DS_ProtocolRobotOutputPort();

/**
 * Returns the type of socket used to communicate with the FMS.
 * If there is no protocol loaded, this function will return \c DS_SOCKET_INVALID
 */
extern DS_SocketType DS_ProtocolFMSSocketType();

/**
 * Returns the type of socket used to communicate with the radio.
 * If there is no protocol loaded, this function will return \c DS_SOCKET_INVALID
 */
extern DS_SocketType DS_ProtocolRadioSocketType();

/**
 * Returns the type of socket used to communicate with the robot.
 * If there is no protocol loaded, this function will return \c DS_SOCKET?INVALID
 */
extern DS_SocketType DS_ProtocolRobotSocketType();

/**
 * Returns a pointer to the current protocol.
 * \warning This function will return a null pointer if there is no protocol
 *          configured with the LibDS
 */
extern struct _protocol* DS_CurrentProtocol();

/**
 * Disables and deletes the current protocol (if any)
 */
extern void DS_ProtocolDecomission();

/**
 * Decomissions the current protocol (if any) and configures the LibDS to use
 * the given \a protocol.
 */
extern void DS_ProtocolConfigure (Protocol* protocol);

#ifdef __cplusplus
}
#endif

#endif
