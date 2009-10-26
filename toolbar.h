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

#ifndef _TOOLBAR_H_
#define _TOOLBAR_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "map_area.h"
#include "map_navigator.h"
#include "config/config.h"

#define GOSM_TYPE_TOOLBAR           (toolbar_get_type ())
#define GOSM_TOOLBAR(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_TOOLBAR, Toolbar))
#define GOSM_TOOLBAR_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_TOOLBAR, ToolbarClass))
#define GOSM_IS_TOOLBAR(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_TOOLBAR))
#define GOSM_IS_TOOLBAR_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_TOOLBAR))
#define GOSM_TOOLBAR_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_TOOLBAR, ToolbarClass))

typedef struct _Toolbar        Toolbar;
typedef struct _ToolbarClass   ToolbarClass;

struct _Toolbar
{
	GtkToolbar parent;

	MapArea * map_area;
	Config * config;
	MapNavigator * map_navigator;
	GtkWidget * side_left;
	GtkWidget * side_right;

	GtkWidget ** 	toolbar_buttons;
	GtkWidget * 	button_network;
	GtkWidget * 	button_map_controls;
	GtkWidget * 	button_side_bar_left;
	GtkWidget * 	button_side_bar_right;
	GtkWidget * 	combo_tiles;
	GtkWidget *	button_grid;
	GtkWidget * 	button_font;
};

struct _ToolbarClass
{
	GtkToolbarClass parent_class;

	//void (* function_name) (Toolbar *toolbar);
};

GtkWidget * toolbar_new(
	MapArea * map_area, 
	Config * config, 
	MapNavigator * map_navigator, 
	GtkWidget * side_left, 
	GtkWidget * side_right);

#endif /* _TOOLBAR_H_ */
