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

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "tool.h"

int time_diff(struct timeval * t1, struct timeval * t2)
{
	int sec_diff  = t2 -> tv_sec - t1 -> tv_sec;
	int msec = sec_diff * 1000000 + t2 -> tv_usec - t1 -> tv_usec;
	return msec;
}

GtkWidget * find_containing_gtk_window(GtkWidget * widget)
{
	GtkWidget * current = widget;
	GtkWidget * cparent = gtk_widget_get_parent(current);
	while(cparent != NULL && !GTK_IS_WINDOW(cparent)){
		current = cparent;
		cparent = gtk_widget_get_parent(current);
	}
	return cparent;
}

char * get_abs_uri(char * filename)
{
	if (filename == NULL) return NULL;
	char * abs = realpath(filename, NULL);
	if (abs == NULL) return NULL;
	char * uri_prefix = "file://";
	char * uri = malloc(sizeof(char) * (strlen(uri_prefix) + strlen(abs) + 1));
	uri[0] = '\0';
	strcat(uri, uri_prefix);
	strcat(uri, abs);
	free(abs);
	return uri;
}

char * get_api_url_get(double min_lon, double min_lat, double max_lon, double max_lat)
{
	char * api_prefix = "http://api.openstreetmap.org/api/0.6/map?bbox=";
	int len = 60 + strlen(api_prefix);

	char * buf = malloc(sizeof(char) * len);
	sprintf(buf, "%s", api_prefix);
	sprintdouble(buf+strlen(buf), min_lon, 7);
	sprintf(buf+strlen(buf), "%s", ",");
	sprintdouble(buf+strlen(buf), min_lat, 7); 
	sprintf(buf+strlen(buf), "%s", ",");
	sprintdouble(buf+strlen(buf), max_lon, 7); 
	sprintf(buf+strlen(buf), "%s", ",");
	sprintdouble(buf+strlen(buf), max_lat, 7); 
	
	return buf;
}

void toggle_widget_visibility(GtkWidget * widget)
{
	gboolean visible = GTK_WIDGET_VISIBLE(widget);
	if (visible){
		gtk_widget_hide(widget);
	}else{
		gtk_widget_show(widget);
	}
}

void set_widget_visibility(GtkWidget * widget, gboolean show)
{
	if (show){
		gtk_widget_show(widget);
	}else{
		gtk_widget_hide(widget);
	}
}
