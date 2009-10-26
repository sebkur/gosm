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

#include <sys/time.h>

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>

int time_diff(struct timeval * t1, struct timeval * t2);

GtkWidget * find_containing_gtk_window(GtkWidget * widget);

char * get_abs_uri(char * filename);

char * get_api_url_get(double min_lon, double min_lat, double max_lon, double max_lat);

void toggle_widget_visibility(GtkWidget * widget);
void set_widget_visibility(GtkWidget * widget, gboolean show);
