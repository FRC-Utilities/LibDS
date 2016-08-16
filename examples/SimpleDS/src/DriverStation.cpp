/*
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

#include "DriverStation.h"

#include <QTimer>
#include <LibDS.h>
#include <QApplication>

static bool initialized = false;

static sds qstring_to_sds (const QString& string)
{
    return sdsnew (string.toStdString().c_str());
}

static QString sds_to_qstring (const sds string)
{
    if (string)
        return QString (string);

    return QString ("");
}

DriverStation* DriverStation::getInstance()
{
    static DriverStation instance;
    return &instance;
}

int DriverStation::teamNumber() const
{
    return DS_GetTeamNumber();
}

bool DriverStation::isEnabled() const
{
    return DS_GetRobotEnabled();
}

bool DriverStation::isTestMode() const
{
    return (controlMode() == kControlTest);
}

bool DriverStation::canBeEnabled() const
{
    return DS_GetCanBeEnabled();
}

bool DriverStation::hasRobotCode() const
{
    return DS_GetRobotCode();
}

bool DriverStation::isAutonomous() const
{
    return (controlMode() == kControlAutonomous);
}

bool DriverStation::isTeleoperated() const
{
    return (controlMode() == kControlTeleoperated);
}

bool DriverStation::connectedToFMS() const
{
    return DS_GetFMSCommunications();
}

bool DriverStation::connectedToRadio() const
{
    return DS_GetRadioCommunications();
}

bool DriverStation::connectedToRobot() const
{
    return DS_GetRobotCommunications();
}

bool DriverStation::emergencyStopped() const
{
    return DS_GetEmergencyStopped();
}

DriverStation::Control DriverStation::controlMode() const
{
    switch (DS_GetControlMode()) {
    case DS_CONTROL_TEST:
        return kControlTest;
        break;
    case DS_CONTROL_AUTONOMOUS:
        return kControlAutonomous;
        break;
    case DS_CONTROL_TELEOPERATED:
        return kControlTeleoperated;
        break;
    default:
        return kControlTeleoperated;
        break;
    }
}

DriverStation::Station DriverStation::teamStation() const
{
    if (teamAlliance() == kAllianceRed) {
        switch (teamPosition()) {
        case kPosition1:
            return kStationRed1;
            break;
        case kPosition2:
            return kStationRed2;
            break;
        case kPosition3:
            return kStationRed3;
            break;
        default:
            return kStationRed1;
            break;
        }
    }

    else {
        switch (teamPosition()) {
        case kPosition1:
            return kStationBlue1;
            break;
        case kPosition2:
            return kStationBlue2;
            break;
        case kPosition3:
            return kStationBlue3;
            break;
        default:
            return kStationBlue1;
            break;
        }
    }
}

DriverStation::Alliance DriverStation::teamAlliance() const
{
    switch (DS_GetAlliance()) {
    case DS_ALLIANCE_RED:
        return kAllianceRed;
        break;
    case DS_ALLIANCE_BLUE:
        return kAllianceBlue;
        break;
    default:
        return kAllianceRed;
        break;
    }
}

DriverStation::Position DriverStation::teamPosition() const
{
    switch (DS_GetPosition()) {
    case DS_POSITION_1:
        return kPosition1;
        break;
    case DS_POSITION_2:
        return kPosition2;
        break;
    case DS_POSITION_3:
        return kPosition3;
        break;
    default:
        return kPosition1;
        break;
    }
}

QString DriverStation::appliedFMSAddress() const
{
    return sds_to_qstring (DS_GetAppliedFMSAddress());
}

QString DriverStation::appliedRadioAddress() const
{
    return sds_to_qstring (DS_GetAppliedRadioAddress());
}

QString DriverStation::appliedRobotAddress() const
{
    return sds_to_qstring (DS_GetAppliedRobotAddress());
}

QString DriverStation::defaultFMSAddress() const
{
    return sds_to_qstring (DS_GetDefaultFMSAddress());
}

QString DriverStation::defaultRadioAddress() const
{
    return sds_to_qstring (DS_GetDefaultRadioAddress());
}

QString DriverStation::defaultRobotAddress() const
{
    return sds_to_qstring (DS_GetDefaultRobotAddress());
}

QString DriverStation::customFMSAddress() const
{
    return sds_to_qstring (DS_GetCustomFMSAddress());
}

QString DriverStation::customRadioAddress() const
{
    return sds_to_qstring (DS_GetCustomRadioAddress());
}

QString DriverStation::customRobotAddress() const
{
    return sds_to_qstring (DS_GetCustomRobotAddress());
}

QStringList DriverStation::stations() const
{
    QStringList list;

    list.append (tr ("Red 1"));
    list.append (tr ("Red 2"));
    list.append (tr ("Red 3"));
    list.append (tr ("Blue 1"));
    list.append (tr ("Blue 2"));
    list.append (tr ("Blue 3"));

    return list;
}

QStringList DriverStation::protocols() const
{
    QStringList list;

    list.append (tr ("FRC 2016"));
    list.append (tr ("FRC 2015"));
    list.append (tr ("FRC 2014"));

    return list;
}

void DriverStation::start()
{
    if (!initialized) {
        DS_Init();
        processEvents();
        connect (qApp, SIGNAL (aboutToQuit()), this, SLOT (quitDS()));
    }
}

void DriverStation::rebootRobot()
{
    DS_RebootRobot();
}

void DriverStation::resetJoysticks()
{
    DS_JoysticksReset();
}

void DriverStation::restartRobotCode()
{
    DS_RestartRobotCode();
}

void DriverStation::switchToTestMode()
{
    setEnabled (false);
    setControlMode (kControlTest);
}

void DriverStation::switchToAutonomous()
{
    setEnabled (false);
    setControlMode (kControlAutonomous);
}

void DriverStation::switchToTeleoperated()
{
    setEnabled (false);
    setControlMode (kControlTeleoperated);
}

void DriverStation::setEnabled (const bool  enabled)
{
    DS_SetRobotEnabled (enabled);
}

void DriverStation::setTeamNumber (const int number)
{
    DS_SetTeamNumber (number);
}

void DriverStation::setProtocol (DS_Protocol* protocol)
{
    if (protocol)
        DS_ConfigureProtocol (protocol);
}

void DriverStation::setControlMode (const Control mode)
{
    switch (mode) {
    case kControlTest:
        DS_SetControlMode (DS_CONTROL_TEST);
        break;
    case kControlAutonomous:
        DS_SetControlMode (DS_CONTROL_AUTONOMOUS);
        break;
    case kControlTeleoperated:
        DS_SetControlMode (DS_CONTROL_TELEOPERATED);
        break;
    default:
        DS_SetControlMode (DS_CONTROL_TELEOPERATED);
        break;
    }
}

void DriverStation::setProtocol (const int protocol)
{
    switch ((Protocol) protocol) {
    case kProtocol2014:
        setProtocol (DS_GetProtocolFRC_2014());
        break;
    case kProtocol2015:
        setProtocol (DS_GetProtocolFRC_2015());
        break;
    case kProtocol2016:
        setProtocol (DS_GetProtocolFRC_2016());
        break;
    default:
        break;
    }
}

void DriverStation::setTeamStation (const int station)
{
    switch ((Station) station) {
    case kStationRed1:
        setTeamPosition (kPosition1);
        setTeamAlliance (kAllianceRed);
        break;
    case kStationRed2:
        setTeamPosition (kPosition2);
        setTeamAlliance (kAllianceRed);
        break;
    case kStationRed3:
        setTeamPosition (kPosition3);
        setTeamAlliance (kAllianceRed);
        break;
    case kStationBlue1:
        setTeamPosition (kPosition1);
        setTeamAlliance (kAllianceBlue);
        break;
    case kStationBlue2:
        setTeamPosition (kPosition2);
        setTeamAlliance (kAllianceBlue);
        break;
    case kStationBlue3:
        setTeamPosition (kPosition3);
        setTeamAlliance (kAllianceBlue);
        break;
    default:
        break;
    }
}

void DriverStation::setTeamAlliance (const int alliance)
{
    switch ((Alliance) alliance) {
    case kAllianceRed:
        DS_SetAlliance (DS_ALLIANCE_RED);
        break;
    case kAllianceBlue:
        DS_SetAlliance (DS_ALLIANCE_BLUE);
        break;
    }
}

void DriverStation::setTeamPosition (const int position)
{
    switch ((Position) position) {
    case kPosition1:
        DS_SetPosition (DS_POSITION_1);
        break;
    case kPosition2:
        DS_SetPosition (DS_POSITION_2);
        break;
    case kPosition3:
        DS_SetPosition (DS_POSITION_3);
        break;
    }
}

void DriverStation::setEmergencyStopped (const bool stopped)
{
    DS_SetEmergencyStopped (stopped);
}


void DriverStation::setCustomFMSAddress (const QString& address)
{
    if (!address.isEmpty())
        DS_SetCustomFMSAddress (qstring_to_sds (address));
}

void DriverStation::setCustomRadioAddress (const QString& address)
{
    if (!address.isEmpty())
        DS_SetCustomRadioAddress (qstring_to_sds (address));
}

void DriverStation::setCustomRobotAddress (const QString& address)
{
    if (!address.isEmpty())
        DS_SetCustomRobotAddress (qstring_to_sds (address));
}

void DriverStation::sendNetConsoleMessage (const QString& message)
{
    if (!message.isEmpty())
        DS_SendNetConsoleMessage (qstring_to_sds (message));
}

void DriverStation::addJoystick (int axes, int hats, int buttons)
{
    DS_JoysticksAdd (axes, hats, buttons);
}

void DriverStation::setJoystickHat (int joystick, int hat, int angle)
{
    DS_SetJoystickHat (joystick, hat, angle);
}

void DriverStation::setJoystickAxis (int joystick, int axis, double value)
{
    DS_SetJoystickAxis (joystick, axis, value);
}

void DriverStation::setJoystickButton (int joystick, int button, bool pressed)
{
    DS_SetJoystickButton (joystick, button, pressed);
}

void DriverStation::quitDS()
{
    if (initialized)
        DS_Close();
}

void DriverStation::processEvents()
{
    DS_Event event;
    while (DS_PollEvent (&event)) {
        switch (event.type) {
        case DS_FMS_COMMS_CHANGED:
            emit fmsCommunicationsChanged (event.fms.connected);
            break;
        case DS_RADIO_COMMS_CHANGED:
            emit radioCommunicationsChanged (event.radio.connected);
            break;
        case DS_NETCONSOLE_NEW_MESSAGE:
            emit newMessage (sds_to_qstring (event.netconsole.message));
            break;
        case DS_ROBOT_ENABLED_CHANGED:
            emit enabledChanged (event.robot.enabled);
            break;
        case DS_ROBOT_MODE_CHANGED:
            emit controlModeChanged (controlMode());
            break;
        case DS_ROBOT_COMMS_CHANGED:
            emit robotCommunicationsChanged (event.robot.connected);
            break;
        case DS_ROBOT_CODE_CHANGED:
            emit robotCodeChanged (event.robot.code);
            break;
        case DS_ROBOT_VOLTAGE_CHANGED:
            emit voltageChanged (event.robot.voltage);
            break;
        case DS_ROBOT_CAN_UTIL_CHANGED:
            emit canUsageChanged (event.robot.can_util);
            break;
        case DS_ROBOT_CPU_INFO_CHANGED:
            emit cpuUsageChanged (event.robot.cpu_usage);
            break;
        case DS_ROBOT_RAM_INFO_CHANGED:
            emit ramUsageChanged (event.robot.ram_usage);
            break;
        case DS_ROBOT_DISK_INFO_CHANGED:
            emit diskUsageChanged (event.robot.disk_usage);
            break;
        case DS_ROBOT_STATION_CHANGED:
            emit allianceChanged (teamAlliance());
            emit positionChanged (teamPosition());
            break;
        case DS_ROBOT_ESTOP_CHANGED:
            emit emergencyStoppedChanged (event.robot.estopped);
            break;
        case DS_STATUS_STRING_CHANGED:
            emit statusChanged (sds_to_qstring (DS_GetStatusString()));
            break;
        default:
            break;
        }
    }

    QTimer::singleShot (5, Qt::PreciseTimer, this, SLOT (processEvents()));
}
