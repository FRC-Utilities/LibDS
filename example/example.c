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

void read_user_input();
void read_libds_events();

int main ()
{
    CLEAR_SCREEN;

    DS_Init();
    printf ("Welcome! Type \"help\" to get started!\n");

    while (true) {
        read_user_input();
        read_libds_events();
    }

    return 0;
}

void read_user_input()
{
    printf ("\n> ");

    /* Get user input */
    char input [BUFSIZ];
    if (scanf ("%s", input) <= 0)
        return;

    /* User wants to change robot IP */
    if (strcmp (input, "ip") == 0) {
        char ip [BUFSIZ];
        if (scanf ("%s", ip) > 0)
            DS_SetCustomRobotAddress (ip);
    }

    /* User wants to change team number */
    else if (strcmp (input, "team") == 0) {
        int team = 0;
        if (scanf ("%d", &team) > 0)
            DS_SetTeamNumber (team);
    }

    /* User wants to enable the robot */
    else if (strcmp (input, "enable") == 0)
        DS_SetRobotEnabled (true);

    /* User wants to disable the robot */
    else if (strcmp (input, "disable") == 0)
        DS_SetRobotEnabled (false);

    /* User wants to see NetConsole output */
    else if (strcmp (input, "netconsole") == 0) {

    }

    /* User switched to test mode */
    else if (strcmp (input, "test") == 0) {
        DS_SetRobotEnabled (false);
        DS_SetControlMode (DS_CONTROL_TEST);
    }

    /* User switched to autonomous */
    else if (strcmp (input, "autonomous") == 0) {
        DS_SetRobotEnabled (false);
        DS_SetControlMode (DS_CONTROL_AUTONOMOUS);
    }

    /* User switched to teleoperated */
    else if (strcmp (input, "teleoperated") == 0) {
        DS_SetRobotEnabled (false);
        DS_SetControlMode (DS_CONTROL_TELEOPERATED);
    }

    /* User wants to quit the application */
    else if (strcmp (input, "quit") == 0) {
        DS_Close();
        exit (0);
    }

    /* User inputed an illegal command */
    else {
        const char* str = "Available commands are:                      \n"
                          "   ip         change robot IP                \n"
                          "   team       set team number                \n"
                          "   enable     enable the robot               \n"
                          "   disable    disable the robot              \n"
                          "   netconsole display netconsole messages    \n"
                          "   test       switch the robot to test mode  \n"
                          "   autonomous switch the robot to autonomous \n"
                          "   operator   switch the robot to teleop     \n"
                          "   quit       exit this application          \n"
                          "   help       display this menu              \n";

        printf ("%s", str);
    }
}

void read_libds_events()
{
    DS_Event event;

    /* Get one item from the event list until the list is empty */
    while (DS_PollEvent (&event) != 0) {

    }
}
