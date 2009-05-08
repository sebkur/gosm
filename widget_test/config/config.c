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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <glib.h>

#include "config.h"
#include "configuration.h"

ConfEntry ConfEntries[] = {
        {"longitude",           TYPE_DOUBLE,    "13,4",         NULL},
        {"lattitude",           TYPE_DOUBLE,    "52,52",        NULL},
        {"zoom",                TYPE_INT,       "11",           NULL},
        {"show_grid",           TYPE_BOOLEAN,   "FALSE",        NULL},
        {"set_position",        TYPE_BOOLEAN,   "FALSE",        NULL},
        {"position_x",          TYPE_INT,       "0",            NULL},
        {"position_y",          TYPE_INT,       "0",            NULL},
        {"set_size",            TYPE_BOOLEAN,   "TRUE",         NULL},
        {"size_width",          TYPE_INT,       "800",          NULL},
        {"size_height",         TYPE_INT,       "600",          NULL},
        {"fullscreen",          TYPE_BOOLEAN,   "FALSE",        NULL},
        {"cache_dir",           TYPE_DIR,       "/tmp/osm",     NULL},
        {"cache_size",          TYPE_INT,       "120",          NULL},
        {"selection_color",     TYPE_COLOR,     "#FF0044",      NULL},
        {"use_proxy",           TYPE_BOOLEAN,   "TRUE",         NULL},
        {"proxy_host",          TYPE_IP,        "123.231.12.1", NULL},
        {"proxy_port",          TYPE_INT,       "80",           NULL}
};

gboolean config_set_entry_data(ConfEntry * ce, char * data_str);
gboolean config_set_entry(Configuration * configuration, char *name, char *data);

Configuration * config_new()
{
	Configuration * configuration = malloc(sizeof(Configuration));
	configuration -> entries = malloc(sizeof(ConfEntries));
	configuration -> count = sizeof(ConfEntries) / sizeof(ConfEntry);
	memcpy(configuration -> entries, ConfEntries, sizeof(ConfEntries));
	int i;
	for (i = 0; i < configuration -> count; i++){
		ConfEntry * ce = &(configuration -> entries[i]);
		config_set_entry_data(ce, ce -> data_str);
	}	
	return configuration;
}

gboolean config_load_config_file(Configuration * configuration)
{
	char * filename = "configuration";

        struct stat sb;
        int s = stat(filename, &sb);
        if (s == -1){
                printf("config file not found\n");
		return FALSE;
        }

        int fd = open(filename, O_RDONLY);
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
	switch (ce -> type){
	case TYPE_BOOLEAN:{
		gboolean active = FALSE;
		if (strcmp(data_str, "TRUE") == 0){
			active = TRUE;
		}
		ce -> data = malloc(sizeof(gboolean));
		*((gboolean*)ce -> data) = active;
		break;
	}
	case TYPE_INT:{
		int num = (int) strtol(data_str, (char**) NULL, 10);
		ce -> data = malloc(sizeof(int));
		*((int*)ce -> data) = num;
		break;
	}
	case TYPE_DOUBLE:{
		double num = strtod(data_str, (char**) NULL);
		ce -> data = malloc(sizeof(double));
		*((double*)ce -> data) = num;
		break;
	}
	case TYPE_DIR:
	case TYPE_COLOR:
	case TYPE_IP:{
		int size = strlen(data_str) + 1;
		printf("len %d\n", size);
		ce -> data = malloc(sizeof(char) * size);
		strncpy((char*)ce -> data, data_str, size);
		break;
	}
	}
}

GtkWidget * config_widget_new(Configuration * configuration)
{
	GtkWidget *box_v = gtk_vbox_new(FALSE, 0);

	int count = configuration -> count; 

	GtkWidget * labels[count];
	GtkWidget * entries[count];

	int i; char buf[20];
	for (i = 0; i < count; i++){
		ConfEntry ce = configuration -> entries[i];
		labels[i] = gtk_label_new(ce.name);
		switch (ce.type){
		case TYPE_DIR:
		case TYPE_COLOR:
		case TYPE_IP:{
			sprintf(buf, "%s", (char*)ce.data);
			entries[i] = gtk_entry_new();
			gtk_entry_set_text(GTK_ENTRY(entries[i]), buf);
			break;
		}
		case TYPE_INT:{
			sprintf(buf, "%d", *(int*)ce.data);
			entries[i] = gtk_entry_new();
			gtk_entry_set_text(GTK_ENTRY(entries[i]), buf);
			break;
		}
		case TYPE_DOUBLE:{
			sprintf(buf, "%f", *(double*)ce.data);
			entries[i] = gtk_entry_new();
			gtk_entry_set_text(GTK_ENTRY(entries[i]), buf);
			break;
		}
		case TYPE_BOOLEAN:{ 
			entries[i] = gtk_check_button_new();
			gboolean active = *(gboolean*)ce.data;
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(entries[i]), active);
			break;
		}
		default:{
			printf("default\n");
			entries[i] = gtk_entry_new();
			gtk_entry_set_text(GTK_ENTRY(entries[i]), ce.data_str);
		}
		}
	}

	GtkWidget * table = gtk_table_new(2, 9, FALSE);
	for (i = 0; i < count; i++){
		GtkWidget * box = gtk_hbox_new(FALSE, 0);
		gtk_box_pack_start(GTK_BOX(box), labels[i], FALSE, FALSE, 0);
		gtk_table_attach(GTK_TABLE(table), box,  0, 1, i, i+1, GTK_EXPAND | GTK_FILL, 0, 0, 0);
		gtk_table_attach(GTK_TABLE(table), entries[i], 1, 2, i, i+1, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	}

	return table;
}
