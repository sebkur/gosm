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

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "bookmark_location.h"

G_DEFINE_TYPE (BookmarkLocation, bookmark_location, GOSM_TYPE_BOOKMARK);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint bookmark_location_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, bookmark_location_signals[SIGNAL_NAME_n], 0);

Bookmark * bookmark_location_new(char * name, double lon, double lat, int zoom)
{
	BookmarkLocation * bookmark_location = g_object_new(GOSM_TYPE_BOOKMARK_LOCATION, NULL);
	int len_name = strlen(name) + 1;
	bookmark_location -> name = malloc(sizeof(char) * len_name);
	strncpy(bookmark_location -> name, name, len_name);
	bookmark_location -> lon = lon;
	bookmark_location -> lat = lat;
	bookmark_location -> zoom = zoom;
	return GOSM_BOOKMARK(bookmark_location);
}

static void bookmark_location_class_init(BookmarkLocationClass *class)
{
        /*bookmark_location_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (BookmarkLocationClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void bookmark_location_init(BookmarkLocation *bookmark_location)
{
}

void bookmark_location_rename(BookmarkLocation * bookmark, char * name)
{
	free(bookmark -> name);
	int len_name = strlen(name) + 1;
	bookmark -> name = malloc(sizeof(char) * len_name);
	strncpy(bookmark -> name, name, len_name);
}

