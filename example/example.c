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

/*
 * Not nice, but it works on GCC 4.9+
 */
#define AVOID_WARNINGS(x) (void)(x + 1)

/*
 * Clears the console screen
 */
#if defined MSDOS || defined WIN32
    #define CLEAR_SCREEN AVOID_WARNINGS (system ("cls"))
#else
    #define CLEAR_SCREEN AVOID_WARNINGS (system ("clear"))
#endif

/*
 * Forward-declaration of function
 */
void read_user_input();
void read_libds_events();

/*
 * Main entry point of the application
 */
int main ()
{
    CLEAR_SCREEN;

    DS_Init();
    DS_ConfigureProtocol (DS_GetProtocolFRC_2016());
    printf ("Welcome! Type \"help\" to get started!\n\n");

    while (1) {
        read_user_input();
        read_libds_events();
    }

    return 0;
}

/*
 * Reads the user input and calls the appropiate commands
 */
void read_user_input()
{
    printf ("> ");

    /* Get user input */
    char input [BUFSIZ];
    if (scanf ("%s", input) <= 0)
        return;

    /* User wants to change robot IP */
    if (strcmp (input, "ip") == 0) {
        printf ("Set robot address: ");

        char ip [BUFSIZ];
        if (scanf ("%s", ip)) {
            DS_SetCustomRobotAddress (ip);
            printf ("Robot address set to %s\n", DS_GetAppliedRobotAddress());
        }
    }

    /* User wants to change team number */
    else if (strcmp (input, "team") == 0) {
        int team = 0;
        printf ("New team number: ");
        if (scanf ("%d", &team)) {
            DS_SetTeamNumber (team);
            printf ("Team number set to %d\n", DS_GetTeamNumber());
        }
    }

    /* User wants to enable the robot */
    else if (strcmp (input, "enable") == 0)
        DS_SetRobotEnabled (1);

    /* User wants to disable the robot */
    else if (strcmp (input, "disable") == 0)
        DS_SetRobotEnabled (0);

    /* User switched to test mode */
    else if (strcmp (input, "test") == 0) {
        DS_SetRobotEnabled (0);
        DS_SetControlMode (DS_CONTROL_TEST);
    }

    /* User switched to autonomous */
    else if (strcmp (input, "autonomous") == 0) {
        DS_SetRobotEnabled (0);
        DS_SetControlMode (DS_CONTROL_AUTONOMOUS);
    }

    /* User switched to teleoperated */
    else if (strcmp (input, "operator") == 0) {
        DS_SetRobotEnabled (0);
        DS_SetControlMode (DS_CONTROL_TELEOPERATED);
    }

    /* Clear the console screen */
    else if (strcmp (input, "clear") == 0)
        CLEAR_SCREEN;

    /* User wants to quit the application */
    else if (strcmp (input, "quit") == 0) {
        DS_Close();
        exit (0);
    }

    /* User requested an illegal command */
    else {
        const char* str = "Available commands are:                      \n"
                          "   ip         change robot IP                \n"
                          "   team       set team number                \n"
                          "   enable     enable the robot               \n"
                          "   disable    disable the robot              \n"
                          "   test       switch the robot to test mode  \n"
                          "   autonomous switch the robot to autonomous \n"
                          "   operator   switch the robot to teleop     \n"
                          "   quit       exit this application          \n"
                          "   help       display this menu              \n"
                          "   clear      clear the console screen       \n";

        printf ("%s", str);
    }
}

/**
 * Informs the user about the different LibDS events
 */
void read_libds_events()
{
    DS_Event event;

    /* Get one item from the event list until the list is empty */
    while (DS_PollEvent (&event) != 0) {
        switch (event.type) {
        case DS_FMS_CONNECTED:
            printf ("[INFO] Connected to FMS\n");
            break;
        case DS_FMS_DISCONNECTED:
            printf ("[INFO] Disconnected from FMS\n");
            break;
        case DS_RADIO_CONNECTED:
            printf ("[INFO] Connected to radio\n");
            break;
        case DS_RADIO_DISCONNECTED:
            printf ("[INFO] Disconnected from radio\n");
            break;
        case DS_NETCONSOLE_NEW_MESSAGE:
            printf ("[NETCONSOLE] %s\n", event.netconsole.message);
            break;
        case DS_ROBOT_ENABLED:
            printf ("[INFO] Robot enabled\n");
            break;
        case DS_ROBOT_DISABLED:
            printf ("[INFO] Robot disabled\n");
            break;
        case DS_ROBOT_REBOOTED:
            printf ("[INFO] Robot rebooted\n");
            break;
        case DS_ROBOT_CONNECTED:
            printf ("[INFO] Connected to robot\n");
            break;
        case DS_ROBOT_EXIT_ESTOP:
            printf ("[INFO] Exited from emergency stop\n");
            break;
        case DS_ROBOT_CODE_LOADED:
            printf ("[INFO] Robot code loaded\n");
            break;
        case DS_ROBOT_DISCONNECTED:
            printf ("[INFO] Disconnected from robot\n");
            break;
        case DS_ROBOT_CODE_UNLOADED:
            printf ("[INFO] Robot code unloaded\n");
            break;
        case DS_ROBOT_CODE_RESTARTED:
            printf ("[INFO] Robot code restarted\n");
            break;
        case DS_ROBOT_VOLTAGE_CHANGED:
            printf ("[INFO] Robot voltage set to %f\n", event.robot.voltage);
            break;
        case DS_ROBOT_EMERGENCY_STOPPED:
            printf ("[INFO] Robot entered emergency stop\n");
            break;
        }
    }
}
