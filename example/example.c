/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include <LibDS.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//==============================================================================
// Function definitions
//==============================================================================

void quit();
void help();
void enable();
void disable();
void setRobotIP();
void updateTeam();
void startTestMode();
void startAutonomous();
void startTeleoperated();

//==============================================================================
// Entry point function & event loop
//==============================================================================

int main()
{
    DS_Init();

    printf ("Welcome! Type \"help\" to get started!\n");

    DS_ProtocolConfigure (DS_Protocol2015());

    while (DS_TRUE) {
        char input [BUFSIZ];

        printf ("\n> ");
        scanf ("%s", input);

        if (strcmp (input, "ip") == 0)
            setRobotIP();

        else if (strcmp (input, "team") == 0)
            updateTeam();

        else if (strcmp (input, "enable") == 0)
            enable();

        else if (strcmp (input, "disable") == 0)
            disable();

        else if (strcmp (input, "test") == 0)
            startTestMode();

        else if (strcmp (input, "autonomous") == 0)
            startAutonomous();

        else if (strcmp (input, "teleoperated") == 0)
            startTeleoperated();

        else if (strcmp (input, "quit") == 0)
            quit();

        else
            help();
    }

    return 0;
}

//==============================================================================
// Function implementation
//==============================================================================

void quit()
{
    DS_Close();
    exit (0);
}

void help()
{
    const char* str = "Available commands are:                           \n"
                      "   ip            change robot IP                  \n"
                      "   team          set team number                  \n"
                      "   enable        enable the robot                 \n"
                      "   disable       disable the robot                \n"
                      "   test          switch the robot to test mode    \n"
                      "   autonomous    switch the robot to autonomous   \n"
                      "   teleoperated  switch the robot to teleoperated \n"
                      "   quit          exit this application            \n"
                      "   help          display this menu                \n";

    printf (str);
}

void enable()
{
    DS_SetEnabled (DS_TRUE);
}

void disable()
{
    DS_SetEnabled (DS_FALSE);
}

void setRobotIP()
{
    char input [BUFSIZ];

    printf ("Set robot IP (static): ");
    scanf ("%s", input);

    DS_SetRobotAddress (input);
}

void updateTeam()
{
    int team = 0;
    DS_SetTeamNumber (team);
}

void startTestMode()
{
    DS_SetEnabled (DS_FALSE);
    DS_SetControlMode (DS_CONTROL_TEST);
}

void startAutonomous()
{
    DS_SetEnabled (DS_FALSE);
    DS_SetControlMode (DS_CONTROL_AUTONOMOUS);
}

void startTeleoperated()
{
    DS_SetEnabled (DS_FALSE);
    DS_SetControlMode (DS_CONTROL_TELEOPERATED);
}
