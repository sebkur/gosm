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

#ifndef _BOOKMARK_LOCATION_H_
#define _BOOKMARK_LOCATION_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "bookmark.h"

#define GOSM_TYPE_BOOKMARK_LOCATION           (bookmark_location_get_type ())
#define GOSM_BOOKMARK_LOCATION(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_BOOKMARK_LOCATION, BookmarkLocation))
#define GOSM_BOOKMARK_LOCATION_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_BOOKMARK_LOCATION, BookmarkLocationClass))
#define GOSM_IS_BOOKMARK_LOCATION(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_BOOKMARK_LOCATION))
#define GOSM_IS_BOOKMARK_LOCATION_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_BOOKMARK_LOCATION))
#define GOSM_BOOKMARK_LOCATION_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_BOOKMARK_LOCATION, BookmarkLocationClass))

typedef struct _BookmarkLocation        BookmarkLocation;
typedef struct _BookmarkLocationClass   BookmarkLocationClass;

struct _BookmarkLocation
{
	Bookmark parent;

	char * name;
	double lon;
	double lat;
	int zoom;
};

struct _BookmarkLocationClass
{
	BookmarkClass parent_class;

	//void (* function_name) (BookmarkLocation *bookmark_location);
};

Bookmark * bookmark_location_new(char * name, double lon, double lat, int zoom);

#endif /* _BOOKMARK_LOCATION_H_ */
