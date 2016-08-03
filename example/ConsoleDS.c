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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curses.h>
#include <pthread.h>

#include <sds.h>
#include <LibDS.h>

/*
 * Define window sizes
 */
#define TOP_HEIGHT     3
#define BOTTOM_HEIGHT  3
#define CENTRAL_HEIGHT 24 - BOTTOM_HEIGHT - TOP_HEIGHT

/*
 * Define basic label states
 */
#define CHECKED   "[*]"
#define UNCHECKED "[ ]"
#define NO_DATA   "--.--"
#define ENABLED   "Enabled"
#define DISABLED  "Disabled"

/*
 * Define windows
 */
static WINDOW* window = NULL;
static WINDOW* enabled_win = NULL;
static WINDOW* console_win = NULL;
static WINDOW* status_info = NULL;
static WINDOW* voltage_win = NULL;
static WINDOW* robot_status = NULL;
static WINDOW* bottom_window = NULL;

/*
 * Define window elements
 */
static sds can_str;
static sds cpu_str;
static sds ram_str;
static sds disk_str;
static sds status_str;
static sds enabled_str;
static sds voltage_str;
static sds console_str;
static sds stick_check_str;
static sds rcode_check_str;
static sds robot_check_str;

/*
 * Event loop variables
 */
static DS_Event event;
static int running = 1;

/*
 * String init & close functions
 */
static void init_strings();
static void close_strings();

/*
 * Functions used in the event loop
 */
static void draw_windows();
static void process_events();
static void refresh_windows();
static void draw_user_interface();

/*
 * Utility functions
 */
static void* get_user_input();
static sds set_checked (sds label, int checked);

/**
 * Main entry point of the application
 */
int main()
{
    init_strings();
    window = initscr();

    /* NCurses failed to start */
    if (!window) {
        printf ("Cannot initialize ncurses!\n");
        return EXIT_FAILURE;
    }

    /* Disable key echoing and enable keypad */
    noecho();
    curs_set (0);
    keypad (window, 1);

    /* Initialize LibDS */
    DS_Init();
    DS_ConfigureProtocol (DS_GetProtocolFRC_2016());

    /* Get user input in a separate thread (to avoid blocking the UI) */
    pthread_t thread;
    pthread_create (&thread, NULL, &get_user_input, NULL);

    /* Enter the UI event loop */
    while (running) {
        refresh();
        process_events();
        draw_user_interface();
        DS_Sleep (5);
    }

    /* Clean after ourselves */
    delwin (window);
    endwin();
    refresh();
    close_strings();

    /* Close LibDS */
    DS_Close();

    /* Exit the application */
    return EXIT_SUCCESS;
}

/**
 * Sets the default label texts
 */
static void init_strings()
{
    robot_check_str = set_checked (robot_check_str, 0);
    rcode_check_str = set_checked (rcode_check_str, 0);
    stick_check_str = set_checked (stick_check_str, 0);

    can_str = sdsnew (NO_DATA);
    cpu_str = sdsnew (NO_DATA);
    ram_str = sdsnew (NO_DATA);
    disk_str = sdsnew (NO_DATA);
    voltage_str = sdsnew (NO_DATA);
    enabled_str = sdsnew (DISABLED);
    status_str = sdsnew (DS_GetStatusString());
    console_str = sdsnew ("[INFO] Welcome to the ConsoleDS!");
}

/**
 * De-allocates the memory used by the strings
 */
static void close_strings()
{
    sdsfree (can_str);
    sdsfree (cpu_str);
    sdsfree (ram_str);
    sdsfree (disk_str);
    sdsfree (status_str);
    sdsfree (enabled_str);
    sdsfree (voltage_str);
    sdsfree (console_str);
    sdsfree (stick_check_str);
    sdsfree (rcode_check_str);
    sdsfree (robot_check_str);
}

/**
 * Creates the base windows of the application
 */
static void draw_windows()
{
    /* Delete the windows */
    delwin (enabled_win);
    delwin (console_win);
    delwin (status_info);
    delwin (voltage_win);
    delwin (robot_status);
    delwin (bottom_window);

    /* Create the windows */
    voltage_win   = newwin (TOP_HEIGHT, 20, 0, 0);
    enabled_win   = newwin (TOP_HEIGHT, 20, 0, 60);
    robot_status  = newwin (TOP_HEIGHT, 40, 0, 20);
    bottom_window = newwin (BOTTOM_HEIGHT, 80, 24 - BOTTOM_HEIGHT,  0);
    console_win   = newwin (CENTRAL_HEIGHT, 60, TOP_HEIGHT, 0);
    status_info   = newwin (CENTRAL_HEIGHT, 20, TOP_HEIGHT, 60);

    /* Draw borders */
    wborder (voltage_win,   0, 0, 0, 0, 0, 0, 0, 0);
    wborder (robot_status,  0, 0, 0, 0, 0, 0, 0, 0);
    wborder (enabled_win,   0, 0, 0, 0, 0, 0, 0, 0);
    wborder (console_win,   0, 0, 0, 0, 0, 0, 0, 0);
    wborder (status_info,   0, 0, 0, 0, 0, 0, 0, 0);
    wborder (bottom_window, 0, 0, 0, 0, 0, 0, 0, 0);

    /* Add topbar elements */
    mvwaddstr (console_win,  1, 1, console_str);
    mvwaddstr (enabled_win,  1, 2, enabled_str);
    mvwaddstr (robot_status, 1, 2, status_str);

    /* Add voltage elements */
    mvwaddstr (voltage_win,  1,  2, "Voltage:");
    mvwaddstr (voltage_win,  1, 12, voltage_str);

    /* Add status panel elements */
    mvwaddstr (status_info, 1, 2, "STATUS:");
    mvwaddstr (status_info, 3, 2, robot_check_str);
    mvwaddstr (status_info, 4, 2, rcode_check_str);
    mvwaddstr (status_info, 5, 2, stick_check_str);
    mvwaddstr (status_info, 3, 6, "Robot Comms");
    mvwaddstr (status_info, 4, 6, "Robot Code");
    mvwaddstr (status_info, 5, 6, "Joysticks");

    /* Add robot status elements */
    mvwaddstr (status_info,  7, 2, "ROBOT STATUS:");
    mvwaddstr (status_info,  9, 2, "CAN:");
    mvwaddstr (status_info, 10, 2, "CPU:");
    mvwaddstr (status_info, 11, 2, "RAM:");
    mvwaddstr (status_info, 12, 2, "Disk:");
    mvwaddstr (status_info,  9, 8, can_str);
    mvwaddstr (status_info, 10, 8, cpu_str);
    mvwaddstr (status_info, 11, 8, ram_str);
    mvwaddstr (status_info, 12, 8, disk_str);

    /* Add bottom bar labels */
    mvwaddstr (bottom_window, 1, 1,  "Quit (q)");
    mvwaddstr (bottom_window, 1, 12, "Set enabled (e,d)");
    mvwaddstr (bottom_window, 1, 33, "Set Control Mode (o,a,t)");
    mvwaddstr (bottom_window, 1, 61, "More Options (m)");
}

/**
 * Checks if the LibDS has any new events and displays them
 * on the console screen.
 */
static void process_events()
{
    /* Check if DS has some events */
    if (!DS_PollEvent (&event))
        return;

    /* Get the event type and update variables */
    switch (event.type) {
    case DS_JOYSTICK_COUNT_CHANGED:
        stick_check_str = set_checked (stick_check_str, DS_GetJoystickCount() > 0);
        break;
    case DS_NETCONSOLE_NEW_MESSAGE:
        break;
    case DS_ROBOT_ENABLED:
        sdsfree (enabled_str);
        enabled_str = sdsnew (ENABLED);
        break;
    case DS_ROBOT_DISABLED:
        sdsfree (enabled_str);
        enabled_str = sdsnew (DISABLED);
        break;
    case DS_ROBOT_CONNECTED:
        robot_check_str = set_checked (robot_check_str, event.robot.connected);
        break;
    case DS_ROBOT_CODE_LOADED:
        rcode_check_str = set_checked (rcode_check_str, event.robot.code);
        break;
    case DS_ROBOT_DISCONNECTED:
        robot_check_str = set_checked (robot_check_str, event.robot.connected);
        break;
    case DS_ROBOT_CODE_UNLOADED:
        rcode_check_str = set_checked (rcode_check_str, event.robot.code);
        break;
    case DS_ROBOT_VOLTAGE_CHANGED:
        voltage_str = sdscatprintf (voltage_str, "%f", event.robot.voltage);
        break;
    case DS_ROBOT_CAN_UTIL_CHANGED:
        can_str = sdscatprintf (can_str, "%d %%", event.robot.can_util);
        break;
    case DS_ROBOT_CPU_INFO_CHANGED:
        cpu_str = sdscatprintf (cpu_str, "%d %%", event.robot.cpu_usage);
        break;
    case DS_ROBOT_RAM_INFO_CHANGED:
        ram_str = sdscatprintf (ram_str, "%d %%", event.robot.ram_usage);
        break;
    case DS_ROBOT_DISK_INFO_CHANGED:
        disk_str = sdscatprintf (disk_str, "%d %%", event.robot.disk_usage);
        break;
    case DS_ROBOT_EMERGENCY_STOPPED:
        break;
    default:
        break;
    }
}

/**
 * Refreshes the contents of each window
 */
static void refresh_windows()
{
    wnoutrefresh (window);
    wnoutrefresh (enabled_win);
    wnoutrefresh (console_win);
    wnoutrefresh (status_info);
    wnoutrefresh (voltage_win);
    wnoutrefresh (robot_status);
    wnoutrefresh (bottom_window);
}

/**
 * Checks if the user has pressed any key on the keyboard and
 * reacts to the given user input
 */
static void* get_user_input()
{
    while (running) {
        switch (getch()) {
        case 'q':
            running = 0;
            break;
        case 'e':
            DS_SetRobotEnabled (1);
            break;
        case 'd':
            DS_SetRobotEnabled (0);
            break;
        case 'o':
            DS_SetRobotEnabled (0);
            DS_SetControlMode (DS_CONTROL_TELEOPERATED);
            break;
        case 'a':
            DS_SetRobotEnabled (0);
            DS_SetControlMode (DS_CONTROL_AUTONOMOUS);
            break;
        case 't':
            DS_SetRobotEnabled (0);
            DS_SetControlMode (DS_CONTROL_TEST);
            break;
        case ' ':
            DS_SetEmergencyStopped (1);
            break;
        }

        DS_Sleep (10);
    }

    return NULL;
}

/**
 * Re-draws the user interface elements to represent the current
 * state of the robot and the LibDS
 */
static void draw_user_interface()
{
    draw_windows();
    refresh_windows();
}

/**
 * Changes the \a label to "[*]" if checked is greater than \c 0,
 * otherwise, the function will change the \a label to "[ ]"
 */
static sds set_checked (sds label, int checked)
{
    sdsfree (label);
    label = sdsnew (checked ? CHECKED : UNCHECKED);

    return label;
}
