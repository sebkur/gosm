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

#ifndef _MAP_NAVIGATOR_H_
#define _MAP_NAVIGATOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#define GOSM_TYPE_MAP_NAVIGATOR           (map_navigator_get_type ())
#define GOSM_MAP_NAVIGATOR(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_MAP_NAVIGATOR, MapNavigator))
#define GOSM_MAP_NAVIGATOR_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_MAP_NAVIGATOR, MapNavigatorClass))
#define GOSM_IS_MAP_NAVIGATOR(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_MAP_NAVIGATOR))
#define GOSM_IS_MAP_NAVIGATOR_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_MAP_NAVIGATOR))
#define GOSM_MAP_NAVIGATOR_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_MAP_NAVIGATOR, MapNavigatorClass))

typedef struct _MapNavigator        MapNavigator;
typedef struct _MapNavigatorClass   MapNavigatorClass;

struct _MapNavigator
{
	GtkVBox parent;

	GtkWidget * area;
	GtkWidget * buttons[8];
	gboolean controls_visible;
};

struct _MapNavigatorClass
{
	GtkVBoxClass parent_class;

	void (* controls_toggled) (MapNavigator *map_navigator);
};

GtkWidget * map_navigator_new(GtkWidget * area);

void map_navigator_show_controls(MapNavigator * map_navigator, gboolean show);
void map_navigator_toggle_controls(MapNavigator *map_navigator);
gboolean map_navigator_get_controls_visible(MapNavigator * map_navigator);

#endif /* _MAP_NAVIGATOR_H_ */
