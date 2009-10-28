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

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#define GOSM_TYPE_CONFIG           (config_get_type ())
#define GOSM_CONFIG(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_CONFIG, Config))
#define GOSM_CONFIG_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_CONFIG, ConfigClass))
#define GOSM_IS_CONFIG(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_CONFIG))
#define GOSM_IS_CONFIG_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_CONFIG))
#define GOSM_CONFIG_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_CONFIG, ConfigClass))

typedef struct _Config        Config;
typedef struct _ConfigClass   ConfigClass;

/****************************************************************************************************
* 'name' is a string that identifies the configuration-entry
* 'type' is one of those TYPE_* below and indicates the type of data found in 'data'
* 'data_str' is a string representation of the value, e.g. "TRUE", "13.42", or "/tmp/osm_mapnik"
* 'data' is the actual value stored in the entry, e.g. 1, 13.42, "/tmp/osm_mapnik"
****************************************************************************************************/
typedef struct ConfEntry{
	char * name;
	int type;
	gpointer data_str;
	gpointer data;
} ConfEntry;

/****************************************************************************************************
* this is used to discriminate between different types of values stored in configuration entries
****************************************************************************************************/
enum {
        TYPE_BOOLEAN,
        TYPE_INT,
        TYPE_DOUBLE,
        TYPE_STRING,
        TYPE_COLOR,
	TYPE_DIR,
	TYPE_IP
};

/****************************************************************************************************
* header for config-class
****************************************************************************************************/
struct _Config
{
	GObject parent;

	ConfEntry * entries;
	int num_entries;
};

struct _ConfigClass
{
	GObjectClass parent_class;

	void (* config_changed) (Config *config);
};

GObject * config_new();

char * config_get_config_dir();
char * config_get_config_file();
char * config_get_poi_sources_file();
char * config_get_poi_layers_file();

gpointer config_get_entry_data(Config * config, char * name);

gboolean config_load_config_file(Config * config);

gboolean config_save_config_file(Config * config);

gboolean config_set_entry_data(ConfEntry * ce, char * data_str);

#endif /* _CONFIG_H_ */
