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

#ifndef _MENU_H_
#define _MENU_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "map_area.h"
#include "config/config.h"

#define GOSM_TYPE_MENU           (menu_get_type ())
#define GOSM_MENU(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_MENU, Menu))
#define GOSM_MENU_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_MENU, MenuClass))
#define GOSM_IS_MENU(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_MENU))
#define GOSM_IS_MENU_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_MENU))
#define GOSM_MENU_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_MENU, MenuClass))

typedef struct _Menu        Menu;
typedef struct _MenuClass   MenuClass;

struct _Menu
{
	GtkMenuBar parent;

	GtkWindow * main_window;
	MapArea * map_area;
	Config * config;
	/*
	 * Start auto-generated menu
	 * :r !./misc/menu_gen.py 1 misc/Menu.txt
	 */
	GtkWidget * menu_file_quit;
	GtkWidget * menu_view_fullscreen;
	GtkWidget * menu_control_zoom_in;
	GtkWidget * menu_control_zoom_out;
	GtkWidget * menu_control_move_up;
	GtkWidget * menu_control_move_down;
	GtkWidget * menu_control_move_left;
	GtkWidget * menu_control_move_right;
	GtkWidget * menu_tiles_mapnik;
	GtkWidget * menu_tiles_osmarender;
	GtkWidget * menu_tiles_openaerial;
	GtkWidget * menu_tiles_google;
	GtkWidget * menu_tiles_yahoo;
	GtkWidget * menu_selection_snap;
	GtkWidget * menu_selection_show;
	GtkWidget * menu_selection_export;
	GtkWidget * menu_selection_download;
	GtkWidget * menu_options_preferences;
	GtkWidget * menu_help_manual;
	GtkWidget * menu_help_about_gosm;
	GtkWidget * menu_help_about_osm;
	GtkWidget * menu_help_about_nf;
	GtkWidget * menu_help_license;
	/*
	 * End auto-generated menu
	 */
};

struct _MenuClass
{
	GtkMenuBarClass parent_class;

	//void (* function_name) (Menu *menu);
};

GtkWidget * menu_new(GtkWindow * main_window, MapArea * map_area, Config * config);

#endif /* _MENU_H_ */
