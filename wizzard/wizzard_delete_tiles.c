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

#include "wizzard_delete_tiles.h"

#include "../tiles/tilesets.h"
#include "../map_types.h"
#include "../tiles/tile_manager.h"
#include "../tiles/tilemath.h"
#include "../map_area.h"

G_DEFINE_TYPE (WizzardDeleteTiles, wizzard_delete_tiles, G_TYPE_OBJECT);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint wizzard_delete_tiles_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, wizzard_delete_tiles_signals[SIGNAL_NAME_n], 0);
gboolean wizzard_delete_tiles_cancel_cb(GtkWidget *widget, WizzardDeleteTiles * wizzard);
gboolean wizzard_delete_tiles_confirm_cb(GtkWidget *widget, WizzardDeleteTiles * wizzard);

void wizzard_delete_tiles_delete(WizzardDeleteTiles * wizzard, int zoom);

WizzardDeleteTiles * wizzard_delete_tiles_new(MapArea * map_area, Tileset tileset, Selection selection)
{
	WizzardDeleteTiles * wizzard_delete_tiles = g_object_new(GOSM_TYPE_WIZZARD_DELETE_TILES, NULL);
	wizzard_delete_tiles -> map_area = map_area;
	wizzard_delete_tiles -> tileset = tileset;
	memcpy(&(wizzard_delete_tiles -> selection), &selection, sizeof(Selection));
	return wizzard_delete_tiles;
}

static void wizzard_delete_tiles_class_init(WizzardDeleteTilesClass *class)
{
        /*wizzard_delete_tiles_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (WizzardDeleteTilesClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void wizzard_delete_tiles_init(WizzardDeleteTiles *wizzard_delete_tiles)
{
}

void wizzard_delete_tiles_show(WizzardDeleteTiles * wizzard, GtkWindow * parent)
{
	wizzard -> wdtw = wizzard_delete_tiles_window_new();
	gtk_window_set_transient_for(GTK_WINDOW(wizzard -> wdtw), GTK_WINDOW(parent));
	gtk_window_set_title(GTK_WINDOW(wizzard -> wdtw), "Delete Tiles from Disk...");
	gtk_window_set_position(GTK_WINDOW(wizzard -> wdtw), GTK_WIN_POS_CENTER_ON_PARENT);
	gtk_window_set_modal(GTK_WINDOW(wizzard -> wdtw), TRUE);
	gtk_widget_show_all(GTK_WIDGET(wizzard -> wdtw));

	g_signal_connect(G_OBJECT(wizzard -> wdtw -> button_cancel), "clicked", G_CALLBACK(wizzard_delete_tiles_cancel_cb), wizzard);
	g_signal_connect(G_OBJECT(wizzard -> wdtw -> button_confirm), "clicked", G_CALLBACK(wizzard_delete_tiles_confirm_cb), wizzard);
}

gboolean wizzard_delete_tiles_cancel_cb(GtkWidget *widget, WizzardDeleteTiles * wizzard)
{
	gtk_widget_hide_all(GTK_WIDGET(wizzard -> wdtw));
}

gboolean wizzard_delete_tiles_confirm_cb(GtkWidget *widget, WizzardDeleteTiles * wizzard)
{
	int i;
	for (i = 0; i < 18; i++){
		gboolean toggled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wizzard -> wdtw -> checks[i]));
		if (toggled){
			wizzard_delete_tiles_delete(wizzard, i + 1);
		}
	}
	gtk_widget_hide_all(GTK_WIDGET(wizzard -> wdtw));
	map_area_repaint(wizzard -> map_area);
}

void wizzard_delete_tiles_delete(WizzardDeleteTiles * wizzard, int zoom)
{
	Selection * selection = &(wizzard -> selection);
	double x1 = lon_to_x(selection->lon1, zoom);
	double x2 = lon_to_x(selection->lon2, zoom);
	double y1 = lat_to_y(selection->lat1, zoom);
	double y2 = lat_to_y(selection->lat2, zoom);
	int count_x = ((int) x2) - ((int)x1) + 1;
	int count_y = ((int) y2) - ((int)y1) + 1;
	int xs = (int) x1;
	int ys = (int) y1;
	int cx, cy;
	for (cx = xs; cx < xs + count_x; cx++){
		for (cy = ys; cy < ys + count_y; cy++){
			tile_manager_delete_tile(map_area_get_tile_manager(wizzard -> map_area, wizzard -> tileset), cx, cy, zoom);
		}
	}
}
