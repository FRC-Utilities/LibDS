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

#ifndef _DRIVERSTATION_H
#define _DRIVERSTATION_H

#include <QObject>
#include <DS_Protocol.h>

class DriverStation : public QObject
{
    Q_OBJECT

public:
    static DriverStation* getInstance();

    enum Control {
        kControlTest = 0x00,
        kControlAutonomous = 0x02,
        kControlTeleoperated = 0x03,
    };

    enum Protocol {
        kProtocol2016 = 0x00,
        kProtocol2015 = 0x01,
        kProtocol2014 = 0x02,
    };

    enum Alliance {
        kAllianceRed = 0x00,
        kAllianceBlue = 0x01,
    };

    enum Position {
        kPosition1 = 0x00,
        kPosition2 = 0x01,
        kPosition3 = 0x02,
    };

    enum Station {
        kStationRed1 = 0x00,
        kStationRed2 = 0x01,
        kStationRed3 = 0x02,
        kStationBlue1 = 0x03,
        kStationBlue2 = 0x04,
        kStationBlue3 = 0x05,
    };

    Q_INVOKABLE int teamNumber() const;
    Q_INVOKABLE int joystickCount() const;

    Q_INVOKABLE bool isEnabled() const;
    Q_INVOKABLE bool isTestMode() const;
    Q_INVOKABLE bool canBeEnabled() const;
    Q_INVOKABLE bool hasRobotCode() const;
    Q_INVOKABLE bool isAutonomous() const;
    Q_INVOKABLE bool isTeleoperated() const;
    Q_INVOKABLE bool connectedToFMS() const;
    Q_INVOKABLE bool connectedToRadio() const;
    Q_INVOKABLE bool connectedToRobot() const;
    Q_INVOKABLE bool emergencyStopped() const;

    Q_INVOKABLE Control controlMode() const;
    Q_INVOKABLE Station teamStation() const;
    Q_INVOKABLE Alliance teamAlliance() const;
    Q_INVOKABLE Position teamPosition() const;

    Q_INVOKABLE QString appliedFMSAddress() const;
    Q_INVOKABLE QString appliedRadioAddress() const;
    Q_INVOKABLE QString appliedRobotAddress() const;

    Q_INVOKABLE QString defaultFMSAddress() const;
    Q_INVOKABLE QString defaultRadioAddress() const;
    Q_INVOKABLE QString defaultRobotAddress() const;

    Q_INVOKABLE QString generalStatus() const;
    Q_INVOKABLE QString customFMSAddress() const;
    Q_INVOKABLE QString customRadioAddress() const;
    Q_INVOKABLE QString customRobotAddress() const;

    Q_INVOKABLE QStringList stations() const;
    Q_INVOKABLE QStringList protocols() const;

public slots:
    void start();
    void rebootRobot();
    void resetJoysticks();
    void restartRobotCode();
    void switchToTestMode();
    void switchToAutonomous();
    void switchToTeleoperated();
    void setEnabled (const bool enabled);
    void setTeamNumber (const int number);
    void setProtocol (DS_Protocol* protocol);
    void setControlMode (const Control mode);
    void setProtocol (const int protocol);
    void setTeamStation (const int station);
    void setTeamAlliance (const int alliance);
    void setTeamPosition (const int position);
    void setEmergencyStopped (const bool stopped);
    void setCustomFMSAddress (const QString& address);
    void setCustomRadioAddress (const QString& address);
    void setCustomRobotAddress (const QString& address);
    void sendNetConsoleMessage (const QString& message);

    void addJoystick (int axes, int hats, int buttons);
    void setJoystickHat (int joystick, int hat, int angle);
    void setJoystickAxis (int joystick, int axis, float value);
    void setJoystickButton (int joystick, int button, bool pressed);

private slots:
    void quitDS();
    void processEvents();

private:
    bool addressIsValid (const QString address);

signals:
    void protocolChanged();
    void canUsageChanged (int usage);
    void cpuUsageChanged (int usage);
    void ramUsageChanged (int usage);
    void newMessage (QString message);
    void diskUsageChanged (int usage);
    void enabledChanged (bool enabled);
    void teamNumberChanged (int number);
    void statusChanged (QString status);
    void voltageChanged (float voltage);
    void robotCodeChanged (bool robotCode);
    void controlModeChanged (Control mode);
    void allianceChanged (Alliance alliance);
    void positionChanged (Position position);
    void fmsCommunicationsChanged (bool connected);
    void radioCommunicationsChanged (bool connected);
    void robotCommunicationsChanged (bool connected);
    void emergencyStoppedChanged (bool emergencyStopped);
};

#endif
