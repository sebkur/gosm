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

#ifndef _BOOKMARK_WIDGET_H_
#define _BOOKMARK_WIDGET_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "bookmark_manager.h"
#include "../map_area.h"

#define GOSM_TYPE_BOOKMARK_WIDGET           (bookmark_widget_get_type ())
#define GOSM_BOOKMARK_WIDGET(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_BOOKMARK_WIDGET, BookmarkWidget))
#define GOSM_BOOKMARK_WIDGET_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_BOOKMARK_WIDGET, BookmarkWidgetClass))
#define GOSM_IS_BOOKMARK_WIDGET(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_BOOKMARK_WIDGET))
#define GOSM_IS_BOOKMARK_WIDGET_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_BOOKMARK_WIDGET))
#define GOSM_BOOKMARK_WIDGET_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_BOOKMARK_WIDGET, BookmarkWidgetClass))

typedef struct _BookmarkWidget        BookmarkWidget;
typedef struct _BookmarkWidgetClass   BookmarkWidgetClass;

struct _BookmarkWidget
{
	GtkVBox parent;

	BookmarkManager * bookmark_manager;
	MapArea * map_area;
	GtkWidget * view;
};

struct _BookmarkWidgetClass
{
	GtkVBoxClass parent_class;

	//void (* function_name) (BookmarkWidget *bookmark_widget);
};

GtkWidget * bookmark_widget_new(BookmarkManager * bookmark_manager, MapArea * map_area);

#endif /* _BOOKMARK_WIDGET_H_ */
