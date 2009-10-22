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
