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

#ifndef _WIZZARD_DELETE_TILES_WINDW_H_
#define _WIZZARD_DELETE_TILES_WINDW_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#define GOSM_TYPE_WIZZARD_DELETE_TILES_WINDOW           (wizzard_delete_tiles_window_get_type ())
#define GOSM_WIZZARD_DELETE_TILES_WINDOW(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_WIZZARD_DELETE_TILES_WINDOW, WizzardDeleteTilesWindow))
#define GOSM_WIZZARD_DELETE_TILES_WINDOW_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_WIZZARD_DELETE_TILES_WINDOW, WizzardDeleteTilesWindowClass))
#define GOSM_IS_WIZZARD_DELETE_TILES_WINDOW(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_WIZZARD_DELETE_TILES_WINDOW))
#define GOSM_IS_WIZZARD_DELETE_TILES_WINDOW_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_WIZZARD_DELETE_TILES_WINDOW))
#define GOSM_WIZZARD_DELETE_TILES_WINDOW_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_WIZZARD_DELETE_TILES_WINDOW, WizzardDeleteTilesWindowClass))

typedef struct _WizzardDeleteTilesWindow        WizzardDeleteTilesWindow;
typedef struct _WizzardDeleteTilesWindowClass   WizzardDeleteTilesWindowClass;

struct _WizzardDeleteTilesWindow
{
	GtkWindow parent;

	GtkWidget* checks[18];
	GtkWidget * button_confirm;
	GtkWidget * button_cancel;
};

struct _WizzardDeleteTilesWindowClass
{
	GtkWindowClass parent_class;

	//void (* function_name) (WizzardDeleteTilesWindow *wizzard_delete_tiles_window);
};

WizzardDeleteTilesWindow * wizzard_delete_tiles_window_new();

#endif /* _WIZZARD_DELETE_TILES_WINDW_H_ */
