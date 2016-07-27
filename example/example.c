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

#include <LibDS.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AVOID_WARNINGS(x) (void)(x + 1)

#if defined MSDOS || defined WIN32
    #define CLEAR_SCREEN AVOID_WARNINGS (system ("cls"))
#else
    #define CLEAR_SCREEN AVOID_WARNINGS (system ("clear"))
#endif

void quit();
void help();
void enable();
void disable();
void netconsole();
void updateTeam();
void startTestMode();
void setRobotAddress();
void startAutonomous();
void startTeleoperated();

int main ()
{
    CLEAR_SCREEN;

    DS_Init();
    printf ("Welcome! Type \"help\" to get started!\n");

    while (true) {
        char input [BUFSIZ];

        printf ("\n> ");

        if (scanf ("%s", input) > 0) {
            if (strcmp (input, "ip") == 0)
                setRobotAddress();

            else if (strcmp (input, "team") == 0)
                updateTeam();

            else if (strcmp (input, "enable") == 0)
                enable();

            else if (strcmp (input, "disable") == 0)
                disable();

            else if (strcmp (input, "netconsole") == 0)
                netconsole();

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
    }

    return 0;
}

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
                      "   netconsole    display netconsole messages      \n"
                      "   test          switch the robot to test mode    \n"
                      "   autonomous    switch the robot to autonomous   \n"
                      "   teleoperated  switch the robot to teleoperated \n"
                      "   quit          exit this application            \n"
                      "   help          display this menu                \n";

    printf ("%s", str);
}

void enable()
{
    DS_SetRobotEnabled (true);
}

void disable()
{
    DS_SetRobotEnabled (false);
}

void netconsole()
{
}

void updateTeam()
{
    int team = 0;
    DS_SetTeamNumber (team);
}

void startTestMode()
{
    disable();
    DS_SetControlMode (DS_CONTROL_TEST);
}

void setRobotAddress()
{
    char input [BUFSIZ];

    printf ("New address: ");

    if (scanf ("%s", input) > 0)
        DS_SetCustomRobotAddress (input);

    else
        printf ("Invalid address!\n");
}

void startAutonomous()
{
    disable();
    DS_SetControlMode (DS_CONTROL_AUTONOMOUS);
}

void startTeleoperated()
{
    disable();
    DS_SetControlMode (DS_CONTROL_TELEOPERATED);
}
