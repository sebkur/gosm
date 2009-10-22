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

#include <gtk/gtk.h>
//#include <glib.h>

#include <unistd.h>
#include <string.h>

#include "wizzard_export.h"
#include "select_export_window.h"
#include "../tiles/tilemath.h"
#include "../tiles/tile_loader.h"
#include "../imageglue/image_glue.h"

G_DEFINE_TYPE (WizzardExport, wizzard_export, G_TYPE_OBJECT);

// TODO: let a thread perform the real work
// TODO: load images from net, if they are not there or corrupted

WizzardExport * wizzard_export_new(GtkWindow * parent_window, char * cache_dir, Selection s, gint zoom)
{
	WizzardExport * wizzard = g_object_new(GOSM_TYPE_WIZZARD_EXPORT, NULL);
	memcpy(&(wizzard -> selection), &s, sizeof(Selection));
	wizzard -> zoom = zoom;
	wizzard -> parent_window = parent_window; 
	wizzard -> total = 0;
	wizzard -> ready = 0;
	wizzard -> cache_dir = malloc(sizeof(char) * (strlen(cache_dir) + 1));
	strcpy(wizzard -> cache_dir, cache_dir);
	return wizzard; 
}

static void wizzard_export_class_init(WizzardExportClass *class)
{
}

static void wizzard_export_init(WizzardExport *wizzard_export)
{
}

static gboolean wizzard_export_cancel_cb(GtkWidget *widget, WizzardExport * wizzard);
static gboolean wizzard_export_export_cb(GtkWidget *widget, WizzardExport * wizzard);

void wizzard_export_show(WizzardExport * wizzard)
{
	GtkWidget * win = select_export_window_new(&(wizzard -> selection), wizzard -> zoom);
	wizzard -> select_export_window = GOSM_SELECT_EXPORT_WINDOW(win);
	gtk_window_set_transient_for(GTK_WINDOW(win), GTK_WINDOW(wizzard -> parent_window));
	gtk_window_set_title(GTK_WINDOW(win), "Export Selection as Image...");
	gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER_ON_PARENT);
	gtk_window_set_modal(GTK_WINDOW(win), TRUE);
	gtk_widget_show_all(win);

	//wizzard -> select_use_window = GOSM_SELECT_USE_WINDOW(win);
	g_signal_connect(G_OBJECT(wizzard -> select_export_window -> button_cancel), "clicked", G_CALLBACK(wizzard_export_cancel_cb), wizzard);
	g_signal_connect(G_OBJECT(wizzard -> select_export_window -> button_export), "clicked", G_CALLBACK(wizzard_export_export_cb), wizzard);
}

static gboolean tile_cb(gpointer image_glue, int n, WizzardExport * wizzard)
{
	wizzard -> ready += 1;
	//printf("%f\n", ((double) wizzard -> ready) / wizzard -> total);
	gdk_threads_enter();
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(wizzard -> select_export_window -> progress_bar), ((double) wizzard -> ready) / wizzard -> total);
	gdk_threads_leave();
}

static gboolean wizzard_export_cancel_cb(GtkWidget *widget, WizzardExport * wizzard)
{
	printf("cancel\n");
	gtk_widget_hide_all(GTK_WIDGET(wizzard -> select_export_window));
}

static gboolean wizzard_export_export_cb(GtkWidget *widget, WizzardExport * wizzard)
{
	printf("ok\n");
	wizzard -> zoom = gtk_combo_box_get_active(GTK_COMBO_BOX(wizzard -> select_export_window -> combo)) + 1;
	const char * filename = gtk_entry_get_text(GTK_ENTRY(wizzard -> select_export_window -> entry_filename));
	char * filen = malloc(sizeof(char) * (1 + strlen(filename)));
	strcpy(filen, filename);
	select_export_window_set_inactive(wizzard -> select_export_window);
	Selection selection = wizzard -> selection;
	//printf("%d %s\n", wizzard -> zoom, filename, wizzard -> selection.lon1);
	ImageGlue * image_glue = image_glue_new();
	g_signal_connect(G_OBJECT(image_glue), "tile-completed", G_CALLBACK(tile_cb), wizzard);
	wizzard -> total = image_glue_single_get_number_of_tiles(wizzard -> zoom, selection.lon1, selection.lon2, selection.lat1, selection.lat2);
	image_glue_single_setup(image_glue, filen, wizzard -> cache_dir, wizzard -> zoom, selection.lon1, selection.lon2, selection.lat1, selection.lat2);
	image_glue_single_process(image_glue);
}

gboolean wizzard_export_loaded_cb(TileLoader * tile_loader, WizzardExport * wizzard)
{
	gdk_threads_enter();
	/*printf("loaded\n");
	wizzard -> ready ++;
	GtkProgressBar * bar = GTK_PROGRESS_BAR(wizzard -> tile_export_window -> bar);
	float percent = (float)wizzard -> ready / (float)wizzard -> total;
	gtk_progress_bar_set_fraction(bar, percent);
	char buf[20];
	sprintf(buf, "%.1f %%", percent * 100);
	gtk_progress_bar_set_text(bar, buf);*/
	gdk_threads_leave();
}
