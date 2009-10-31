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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "config.h"
#include "../customio.h"
#include "../map_types.h"

/****************************************************************************************************
* This class manages the configuration gosm uses.
* it reads in the config-file
* it writes out the config-file
* it can be queried for specific configuration-values
* see config.h for details of a single ConfEntry
****************************************************************************************************/
G_DEFINE_TYPE (Config, config, G_TYPE_OBJECT);

/****************************************************************************************************
* signals
****************************************************************************************************/
enum
{
        CONFIG_CHANGED,
        LAST_SIGNAL
};

static guint config_signals[LAST_SIGNAL] = { 0 };

/****************************************************************************************************
* method declarations
****************************************************************************************************/
gboolean config_set_entry(Config * config, char *name, char *data);

/****************************************************************************************************
* constructor
* create an array of ConfEntries, that is fille with the default values
* TODO: use g_array
* TODO: get rid of strings as default values for boolean/numeric types
****************************************************************************************************/
GObject * config_new()
{
	Config * config = g_object_new(GOSM_TYPE_CONFIG, NULL);
	/* default values */
	ConfEntry ConfEntries[] = {
		{"online_on_startup",	TYPE_BOOLEAN,	"TRUE",			NULL},
		{"longitude",           TYPE_DOUBLE,    "13.4",         	NULL},
		{"lattitude",           TYPE_DOUBLE,    "52.52",        	NULL},
		{"zoom",                TYPE_INT,       "6",	           	NULL},
		{"show_grid",           TYPE_BOOLEAN,   "FALSE",        	NULL},
		{"show_tilenumbers",	TYPE_BOOLEAN,	"FALSE",		NULL},
		{"show_menubar",	TYPE_BOOLEAN,	"TRUE",			NULL},
		{"show_toolbar",	TYPE_BOOLEAN,	"TRUE",			NULL},
		{"show_statusbar",	TYPE_BOOLEAN,	"TRUE",			NULL},
		{"show_controls",	TYPE_BOOLEAN,	"TRUE",			NULL},
		{"show_sidebar",	TYPE_BOOLEAN,	"TRUE",			NULL},
		{"show_left_sidebar",	TYPE_BOOLEAN,	"TRUE",			NULL},
		{"set_position",        TYPE_BOOLEAN,   "FALSE",        	NULL},
		{"position_x",          TYPE_INT,       "0",            	NULL},
		{"position_y",          TYPE_INT,       "0",            	NULL},
		{"tileset",		TYPE_INT,       "0",            	NULL},
		{"set_size",            TYPE_BOOLEAN,   "TRUE",         	NULL},
		{"size_width",          TYPE_INT,       "800",          	NULL},
		{"size_height",         TYPE_INT,       "600",          	NULL},
		{"fullscreen",          TYPE_BOOLEAN,   "FALSE",        	NULL},
		{"cache_dir_mapnik",    TYPE_DIR,       "/tmp/osm_mapnik",     	NULL},
		{"cache_dir_osmarender",TYPE_DIR,       "/tmp/osm_osmarender",	NULL},
		{"cache_dir_cycle",	TYPE_DIR,       "/tmp/osm_cycle",	NULL},
		{"cache_size",          TYPE_INT,       "120",          	NULL},
		{"java_binary",         TYPE_STRING,    "/usr/bin/java",     	NULL},
		{"color_selection",     TYPE_COLOR,     "1.0,1.0,0.5,0.4",	NULL},
		{"color_selection_out", TYPE_COLOR,     "1.0,1.0,0.8,0.9",	NULL},
		{"color_selection_pad", TYPE_COLOR,     "1.0,0.8,0.3,0.4",	NULL},
		{"color_atlas_lines",   TYPE_COLOR,     "0.5,0.0,0.5,0.7",	NULL},
		{"use_proxy",           TYPE_BOOLEAN,   "FALSE",		NULL},
		{"proxy_host",          TYPE_IP,        "proxy.ip.add",		NULL},
		{"proxy_port",          TYPE_INT,       "80",			NULL}
	};
	/* create the current instance of configuration */
	config -> entries = malloc(sizeof(ConfEntries));
	config -> num_entries = sizeof(ConfEntries) / sizeof(ConfEntry);
	int i;
	/* copy default values */
	for (i = 0; i < config -> num_entries; i++){
		memcpy(&(config -> entries[i]), &(ConfEntries[i]), sizeof(ConfEntry));
	}
	/* set default values to the data field */
	for (i = 0; i < config -> num_entries; i++){
		ConfEntry * ce = &(config -> entries[i]);
		config_set_entry_data(ce, ce -> data_str);
	}	
	return G_OBJECT(config);
}

static void config_class_init(ConfigClass *class)
{
        config_signals[CONFIG_CHANGED] = g_signal_new(
                "config-changed",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (ConfigClass, config_changed),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
}

static void config_init(Config *config)
{
}

/****************************************************************************************************
* return the data of a specific config-entry
****************************************************************************************************/
gpointer config_get_entry_data(Config * config, char * name)
{
	int i;
	for (i = 0; i < config -> num_entries; i++){
		ConfEntry * ce = &(config -> entries[i]);
		if (strcmp(ce -> name, name) == 0){
			return ce -> data;
		}
	}	
	return NULL;
}

/****************************************************************************************************
* find and return the configuration-directory (~/.config/gosm)
* TODO: use g_build_filename instead of self-made string-concat
****************************************************************************************************/
char * config_get_config_dir()
{
	char * dir = getenv("HOME");
	if (dir == NULL){
		return FALSE;
	}
	char * afterhomedir = "/.config/gosm";
	char * gosmdir = malloc(sizeof(char) * (strlen(dir) + strlen(afterhomedir) + 1 ));
	sprintf(gosmdir, "%s%s", dir,afterhomedir);
	return gosmdir;
}
/****************************************************************************************************
* find and retrun the path to a file in the configuration-directory
****************************************************************************************************/
char * config_get_config_dir_sub_file(char * filename)
{
	char * gosmdir = config_get_config_dir();
	char * filepath = malloc(sizeof(char) * (strlen(gosmdir) + strlen(filename)+ 2 ));
	sprintf(filepath, "%s/%s", gosmdir,filename);
	printf("%s\n", filepath);
	free(gosmdir);
	return filepath;
}
/****************************************************************************************************
* find the filenames for config-files that are used by gosm
****************************************************************************************************/
char * config_get_config_file()
{
	return config_get_config_dir_sub_file("configuration");
}
char * config_get_poi_sources_file()
{
	return config_get_config_dir_sub_file("poi_sources");
}
char * config_get_poi_layers_file()
{
	return config_get_config_dir_sub_file("poi_layers");
}

/****************************************************************************************************
* read and parse the config-file; fill read values into array of config-values
****************************************************************************************************/
gboolean config_load_config_file(Config * config)
{
	char * filepath = config_get_config_file();

        struct stat sb;
        int s = stat(filepath, &sb);
        if (s == -1){
                printf("config file not found\n");
		return FALSE;
        }

        int fd = open(filepath, O_RDONLY);
        if (fd == -1){
                printf("config file not found\n");
                return FALSE;
        }

        int size = sb.st_size;
        char buf[size+1];
        read(fd, buf, size);
        close(fd);
        buf[size] = '\0';

        gchar ** splitted = g_strsplit(buf, "\n", 100);
        gchar * current = splitted[0];
        int i = 0;
        while (current != NULL){
                if (strlen(current) > 0){
                        gchar ** splitline = g_strsplit(current, "\t", 2);
			if (splitline[0] != NULL && splitline[1] != NULL){
	                        gchar * part1 = g_strstrip(splitline[0]);
	                        gchar * part2 = g_strstrip(splitline[1]);
				config_set_entry(config, part1, part2);
			}
			g_strfreev(splitline);
                }
                current = splitted[++i];
        }
	g_strfreev(splitted);
	//TODO: free ?
}

/****************************************************************************************************
* write the current config into the config-file
****************************************************************************************************/
gboolean config_save_config_file(Config * config)
{
	char * filename = "configuration";
	char * gosmdir = config_get_config_dir();
	char * filepath = config_get_config_file();

        struct stat sb;
        int s = stat(gosmdir, &sb);
        if (s == -1){
                printf("gosm dir not found, trying to create\n");
		mkdir(gosmdir, 0755);
		s = stat(gosmdir, &sb);
		if (s == -1){
			return FALSE;
		}else{
			printf("successfully created gosm dir: %s\n", gosmdir);
		}
        }

	int fd = open(filepath, O_RDWR | O_CREAT | O_TRUNC, 0644);
	int i;
	for (i = 0; i < config -> num_entries; i++){
		ConfEntry * ce = &(config -> entries[i]);
		write(fd, ce -> name, strlen(ce->name));
		write(fd, "\t\t", 2);
		write(fd, ce -> data_str, strlen(ce->data_str));
		write(fd, "\n", 1);
	}
	close(fd); 
	g_signal_emit (config, config_signals[CONFIG_CHANGED], 0);
	return TRUE;
}

/****************************************************************************************************
* sets the config entry given by 'name' to the value given by 'data'
* only if 'data' has changed in comparison to the old value, the hold data will be changed
****************************************************************************************************/
gboolean config_set_entry(Config * config, char *name, char *data)
{	
	printf("%s = %s\n", name, data);
	int i;
	for (i = 0; i < config -> num_entries; i++){
		ConfEntry * ce = &(config -> entries[i]);
		if (strcmp(ce -> name, name) == 0){
			config_set_entry_data(ce, data);
		}
	}	
}

/****************************************************************************************************
* set the data-entry of ConfEntry according to the value in 'data_str'
****************************************************************************************************/
gboolean config_set_entry_data(ConfEntry * ce, char * data_str)
{	
	if (ce -> data != NULL){
		free(ce -> data_str);
	}
	int size = strlen(data_str) + 1;
	ce -> data_str = malloc(sizeof(char) * size);
	strncpy((char*)ce -> data_str, data_str, size);

	switch (ce -> type){
	case TYPE_BOOLEAN:{
		if (ce -> data == NULL){
			ce -> data = malloc(sizeof(gboolean));
		}
		gboolean active = FALSE;
		if (strcmp(data_str, "TRUE") == 0){
			active = TRUE;
		}
		if (*((gboolean*)ce -> data) == active) return FALSE;
		*((gboolean*)ce -> data) = active;
		return TRUE;
	}
	case TYPE_INT:{
		if (ce -> data == NULL){
			ce -> data = malloc(sizeof(int));
		}
		errno = 0;		
		int num = (int) strtol(data_str, (char**) NULL, 10);
		if (errno != 0) return FALSE;
		if (*((int*)ce -> data) == num) return FALSE;
		*((int*)ce -> data) = num;
		return TRUE;
	}
	case TYPE_DOUBLE:{
		if (ce -> data == NULL){
			ce -> data = malloc(sizeof(double));
		}
		errno = 0;
		double num = strtodouble(data_str);
		if (errno != 0) return FALSE;
		if (*((double*)ce -> data) == num) return FALSE;
		*((double*)ce -> data) = num;
		char b[20];
		//sprintf(b, "%f", num);
		sprintdouble(b, num, 4);
		strncpy(ce -> data_str, b, strlen(b));
		return TRUE;
	}
	case TYPE_COLOR:{
		if (ce -> data == NULL){
			ce -> data = malloc(sizeof(ColorQuadriple));
		}
                gchar ** split = g_strsplit(data_str, ",", 4);
		if (split[0] == NULL || split[1] == NULL || split[2] == NULL || split[3] == NULL) return FALSE;
		((ColorQuadriple*)ce -> data) -> r = strtodouble(split[0]);
		((ColorQuadriple*)ce -> data) -> g = strtodouble(split[1]);
		((ColorQuadriple*)ce -> data) -> b = strtodouble(split[2]);
		((ColorQuadriple*)ce -> data) -> a = strtodouble(split[3]);
		g_strfreev(split);
		return TRUE;
	}
	case TYPE_STRING:
	case TYPE_DIR:
	case TYPE_IP:{
		int size_new = strlen(data_str) + 1;
		if (ce -> data == NULL){
			ce -> data = malloc(sizeof(char) * size_new);
			strncpy((char*)ce -> data, data_str, size_new);
			//ce -> data_str = malloc(sizeof(char) * size_new);
			//strncpy((char*)ce -> data_str, data_str, size_new);
			return TRUE;
		}
		int diff = strcmp(ce -> data, data_str);
		if (diff == 0) return FALSE;
		free(ce -> data);
		//free(ce -> data_str);
		ce -> data = malloc(sizeof(char) * size_new);
		//ce -> data_str = malloc(sizeof(char) * size_new);
		strncpy((char*)ce -> data, data_str, size_new);
		//strncpy((char*)ce -> data_str, data_str, size_new);
		return TRUE;
	}
	}
}

