/* GOSM - the Gtk OpenStreetMap Tool
 *
 * Copyright (C) 2009  Sebastian Kuerten
 *
 * This file is part of Gosm.
 *
 * Gosm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Gosm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Gosm.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtk/gtk.h>
#include <glib.h>
#include "configuration.h"

#ifndef _CONFIG_H_
#define _CONFIG_H_

//static Configuration * global_config;
//static gboolean network_state = FALSE;

Configuration * config_new();

char * config_get_config_dir();
char * config_get_config_file();

gpointer config_get_entry_data(Configuration * configuration, char * name);

gboolean config_load_config_file(Configuration * configuration);

gboolean config_save_config_file(Configuration * configuration);

gboolean config_set_entry_data(ConfEntry * ce, char * data_str);

#endif
