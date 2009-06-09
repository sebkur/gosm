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

#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include <glib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <glib.h>

#include "customio.h"
#include "config.h"
#include "configuration.h"
#include "map_types.h"

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
        {"set_position",        TYPE_BOOLEAN,   "FALSE",        	NULL},
        {"position_x",          TYPE_INT,       "0",            	NULL},
        {"position_y",          TYPE_INT,       "0",            	NULL},
        {"set_size",            TYPE_BOOLEAN,   "TRUE",         	NULL},
        {"size_width",          TYPE_INT,       "800",          	NULL},
        {"size_height",         TYPE_INT,       "600",          	NULL},
        {"fullscreen",          TYPE_BOOLEAN,   "FALSE",        	NULL},
        {"cache_dir",           TYPE_DIR,       "/tmp/osm",     	NULL},
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

gboolean config_set_entry_data(ConfEntry * ce, char * data_str);
gboolean config_set_entry(Configuration * configuration, char *name, char *data);

Configuration * config_new()
{
	Configuration * configuration = malloc(sizeof(Configuration));
	configuration -> entries = malloc(sizeof(ConfEntries));
	configuration -> count = sizeof(ConfEntries) / sizeof(ConfEntry);
	//memcpy(configuration -> entries, ConfEntries, sizeof(ConfEntries));
	int i;
	for (i = 0; i < configuration -> count; i++){
		memcpy(&(configuration -> entries[i]), &(ConfEntries[i]), sizeof(ConfEntry));
	}
	for (i = 0; i < configuration -> count; i++){
		ConfEntry * ce = &(configuration -> entries[i]);
		config_set_entry_data(ce, ce -> data_str);
	}	
	return configuration;
}

gpointer config_get_entry_data(Configuration * configuration, char * name)
{
	int i;
	for (i = 0; i < configuration -> count; i++){
		ConfEntry * ce = &(configuration -> entries[i]);
		if (strcmp(ce -> name, name) == 0){
			return ce -> data;
		}
	}	
	return NULL;
}

gboolean config_load_config_file(Configuration * configuration)
{
	char * filename = "configuration";
	char * dir = getenv("HOME");
	if (dir == NULL){
		return FALSE;
	}
	char * gosmdir = malloc(sizeof(char) * strlen(dir) + 6);
	sprintf(gosmdir, "%s", dir);
	sprintf(gosmdir+strlen(gosmdir), "%s", "/.gosm");
	char * filepath = malloc(sizeof(char) * strlen(gosmdir) + 15);
	sprintf(filepath, "%s", gosmdir);
	sprintf(filepath+strlen(filepath), "%s", "/");
	sprintf(filepath+strlen(filepath), "%s", filename);
	printf("%s\n", filepath);

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
				config_set_entry(configuration, part1, part2);
			}
                }
                current = splitted[++i];
        }	
}

gboolean config_save_config_file(Configuration * configuration)
{
	char * filename = "configuration";
	char * dir = getenv("HOME");
	if (dir == NULL){
		return FALSE;
	}
	char * gosmdir = malloc(sizeof(char) * strlen(dir) + 6);
	sprintf(gosmdir, "%s", dir);
	sprintf(gosmdir+strlen(gosmdir), "%s", "/.gosm");
	char * filepath = malloc(sizeof(char) * strlen(gosmdir) + 15);
	sprintf(filepath, "%s", gosmdir);
	sprintf(filepath+strlen(filepath), "%s", "/");
	sprintf(filepath+strlen(filepath), "%s", filename);
	printf("%s\n", filepath);

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
	for (i = 0; i < configuration -> count; i++){
		ConfEntry * ce = &(configuration -> entries[i]);
		write(fd, ce -> name, strlen(ce->name));
		write(fd, "\t\t", 2);
		write(fd, ce -> data_str, strlen(ce->data_str));
		write(fd, "\n", 1);
	}
	close(fd); 
}

gboolean config_set_entry(Configuration * configuration, char *name, char *data)
{	
	printf("%s = %s\n", name, data);
	int i;
	for (i = 0; i < configuration -> count; i++){
		ConfEntry * ce = &(configuration -> entries[i]);
		if (strcmp(ce -> name, name) == 0){
			config_set_entry_data(ce, data);
		}
	}	
}

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
		return TRUE;
	}
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

