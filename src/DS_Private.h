/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#ifndef _LIBDS_PRIVATE_H
#define _LIBDS_PRIVATE_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "DS_Common.h"

/* init functions */
extern DS_Bool cfg_init();
extern DS_Bool events_init();
extern DS_Bool sockets_init();
extern DS_Bool joystick_init();
extern DS_Bool netconsole_init();

/* termination functions */
extern void cfg_close();
extern void events_close();
extern void sockets_close();
extern void joystick_close();
extern void netconsole_close();

/* config.c - getters */
extern int cfg_team();
extern double cfg_voltage();
extern int cfg_cpu_usage();
extern int cfg_ram_usage();
extern int cfg_disk_usage();
extern char* cfg_pdp_version();
extern char* cfg_pcm_version();
extern char* cfg_lib_version();
extern DS_Bool cfg_robot_code();
extern DS_Bool cfg_robot_enabled();
extern DS_Bool cfg_fms_connected();
extern DS_Bool cfg_emergency_stop();
extern DS_Bool cfg_radio_connected();
extern DS_Bool cfg_robot_connected();
extern DS_Control cfg_control_mode();

/* config.c - setters */
extern void cfg_update_team (const int team);
extern void cfg_update_enabled (const DS_Bool enabled);
extern void cfg_update_cpu_usage (const int usage);
extern void cfg_update_ram_usage (const int usage);
extern void cfg_update_disk_usage (const int usage);
extern void cfg_update_emergency_stop (const DS_Bool stop);
extern void cfg_update_control_mode (const DS_Control mode);
extern void cfg_update_robot_code (const DS_Bool available);
extern void cfg_update_voltage (const double voltage);
extern void cfg_update_pdp_version (const char* version);
extern void cfg_update_pcm_version (const char* version);
extern void cfg_update_lib_version (const char* version);
extern void cfg_update_fms_connected (const DS_Bool connected);
extern void cfg_update_radio_connected (const DS_Bool connected);
extern void cfg_update_robot_connected (const DS_Bool connected);

/* joysticks.c */
extern int joystick_count();
extern int joystick_remove (const int index);
extern int joystick_num_axes (const int index);
extern int joystick_num_hats (const int index);
extern int joystick_num_buttons (const int index);
extern char* joystick_name (const int index);
extern void joystick_update_hat (const int index, const int hat, const DS_Hat angle);
extern void joystick_update_axis (const int index, const int axis, const double value);
extern void joystick_add (const char* name, const int axes, const int hats, const int buttons);
extern void joystick_update_button (const int index, const int button, const DS_Button pressed);

/* netconsole.c */
extern void netconsole_read();
extern const char* netconsole_data();
extern void netconsole_set_input_port (const int port);
extern void netconsole_set_output_port (const int port);
extern void netconsole_send_message (const char* message);

/* sockets.c - FMS functions */
extern void sockets_send_to_fms();
extern void sockets_set_fms_address (const char* ip);
extern void sockets_set_fms_input_port (const int port);
extern void sockets_set_fms_output_port (const int port);
extern void sockets_set_default_fms_address (const char* ip);
extern void sockets_set_fms_socket_type (const DS_SocketType type);

/* sockets.c - Radio/Bridge functions */
extern void sockets_send_to_radio();
extern void sockets_set_radio_address (const char* ip);
extern void sockets_set_radio_input_port (const int port);
extern void sockets_set_radio_output_port (const int port);
extern void sockets_set_default_radio_address (const char* ip);
extern void sockets_set_radio_socket_type (const DS_SocketType type);

/* sockets.c - Robot functions */
extern void sockets_send_to_robot();
extern void sockets_set_robot_address (const char* ip);
extern void sockets_set_robot_input_port (const int port);
extern void sockets_set_robot_output_port (const int port);
extern void sockets_set_default_robot_address (const char* ip);
extern void sockets_set_robot_socket_type (const DS_SocketType type);

#ifdef __cplusplus
}
#endif

#endif
