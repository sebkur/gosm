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

#include "wizzard_download.h"
#include "select_use_window.h"
#include "tile_download_window.h"
#include "../tiles/tilemath.h"
#include "../tiles/tile_loader.h"

G_DEFINE_TYPE (WizzardDownload, wizzard_download, G_TYPE_OBJECT);

void wizzard_download_construct(WizzardDownload *wizzard_download);

WizzardDownload * wizzard_download_new(GtkWindow * parent_window, char * format_url, char * download_dir, Selection s)
{
	WizzardDownload * wizzard = g_object_new(GOSM_TYPE_WIZZARD_DOWNLOAD, NULL);
	memcpy(&(wizzard -> selection), &s, sizeof(Selection));
	wizzard -> parent_window = parent_window; 
	wizzard -> total = 0;
	wizzard -> ready = 0;
	wizzard -> download_dir = malloc(sizeof(char) * (strlen(download_dir) + 1));
	strcpy(wizzard -> download_dir, download_dir);
	wizzard -> format_url = malloc(sizeof(char) * (strlen(format_url) + 1));
	strcpy(wizzard -> format_url, format_url);
	return wizzard; 
}

static void wizzard_download_class_init(WizzardDownloadClass *class)
{
}

static void wizzard_download_init(WizzardDownload *wizzard_download)
{
}

gboolean loaded_cb(TileLoader * tile_loader, WizzardDownload * wizzard)
{ // TODO: insert a mutex, so that it's impossible for the wizzard to be destroyed, while something is changed on it
	gdk_threads_enter();
	printf("loaded\n");
	wizzard -> ready ++;
	GtkProgressBar * bar = GTK_PROGRESS_BAR(wizzard -> tile_download_window -> bar);
	float percent = (float)wizzard -> ready / (float)wizzard -> total;
	gtk_progress_bar_set_fraction(bar, percent);
	char buf[20];
	sprintf(buf, "%.1f %%", percent * 100);
	gtk_progress_bar_set_text(bar, buf);
	sprintf(buf, "%d", wizzard -> ready);
	gtk_label_set_text(GTK_LABEL(wizzard -> tile_download_window -> label_ready_s), buf);
	gdk_threads_leave();
}

gboolean pause_cb(GtkWidget *widget, WizzardDownload * wizzard)
{
	printf("pause\n");
	TileLoader * tl = wizzard -> tile_loader;
	GtkButton * button = GTK_BUTTON(wizzard -> tile_download_window -> button_pause);
	if (tl -> mode == TILE_LOADER_MODE_PAUSE){
		tile_loader_resume(tl);
		tile_download_window_set_pause_button_state(wizzard -> tile_download_window, TILE_DOWNLOAD_WINDOW_PAUSE_BUTTON_PAUSED);
	}else{
		tile_loader_pause(tl);
		tile_download_window_set_pause_button_state(wizzard -> tile_download_window, TILE_DOWNLOAD_WINDOW_PAUSE_BUTTON_RUNNING);
	}
}

gboolean cancel_cb(GtkWidget *widget, WizzardDownload * wizzard)
{
	printf("cancel\n");
	gtk_widget_hide_all(GTK_WIDGET(wizzard -> select_use_window));
}

gboolean cancel_download_window_cb(GtkWidget *widget, WizzardDownload * wizzard)
{
	printf("cancel\n");
	tile_loader_pause(wizzard -> tile_loader);
	g_signal_handler_disconnect(G_OBJECT(wizzard -> tile_loader), wizzard -> handler_id_tile_loaded);
	gtk_widget_hide_all(GTK_WIDGET(wizzard -> tile_download_window));
	gtk_widget_destroy(GTK_WIDGET(wizzard -> tile_download_window));
}

gboolean download_cb(GtkWidget *widget, WizzardDownload * wizzard)
{
	printf("ok\n");

	Selection * selection = &(wizzard -> select_use_window -> selection);
	gboolean to_load[18];
	int i;
	for (i = 0; i < 18; i++){
		to_load[i] = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wizzard -> select_use_window -> to_load_wid[i]));
	}

	gboolean one_selected = FALSE;
	for (i = 0; i < 18; i++){
		if (to_load[i]){
			one_selected = TRUE;
			break;
		}
	}	
	if (!one_selected){
		return;
	}

	gtk_widget_hide_all(GTK_WIDGET(wizzard -> select_use_window));

	int count = 0;
        for (i = 1; i <= 18; i++){
                if (to_load[i-1]){
                        double x1 = lon_to_x(selection->lon1, i);
                        double x2 = lon_to_x(selection->lon2, i);
                        double y1 = lat_to_y(selection->lat1, i);
                        double y2 = lat_to_y(selection->lat2, i);
                        int count_x = ((int) x2) - ((int)x1) + 1;
                        int count_y = ((int) y2) - ((int)y1) + 1;
                        count += count_x * count_y;
                }
        }
	wizzard -> total = count;

	// destroy?

	wizzard -> tile_download_window = GOSM_TILE_DOWNLOAD_WINDOW(tile_download_window_new(selection, to_load));
	gtk_window_set_title(GTK_WINDOW(wizzard ->  tile_download_window), "Loading Tiles...");
	gtk_window_set_position(GTK_WINDOW(wizzard ->  tile_download_window), GTK_WIN_POS_CENTER_ON_PARENT);
	gtk_window_set_transient_for(GTK_WINDOW(wizzard ->  tile_download_window), GTK_WINDOW(wizzard -> parent_window));
	gtk_window_set_modal(GTK_WINDOW(wizzard ->  tile_download_window), FALSE);
	gtk_widget_show_all(GTK_WIDGET(wizzard ->  tile_download_window));
	
	g_signal_connect(G_OBJECT(wizzard -> tile_download_window -> button_pause), "clicked", G_CALLBACK(pause_cb), wizzard);
	g_signal_connect(G_OBJECT(wizzard -> tile_download_window -> button_cancel), "clicked", G_CALLBACK(cancel_download_window_cb), wizzard);

	// download tiles, update window
	wizzard -> tile_loader = GOSM_TILE_LOADER(tile_loader_new());
	tile_loader_set_cache_directory(wizzard -> tile_loader, wizzard -> download_dir);
	tile_loader_set_url_format(wizzard -> tile_loader, wizzard -> format_url);

	wizzard -> handler_id_tile_loaded = g_signal_connect(G_OBJECT(wizzard -> tile_loader), "tile-loaded-succesfully", G_CALLBACK(loaded_cb), wizzard);

	int x1, x2, y1, y2, ix, iy, zoom;
	for (i = 0; i < 18; i++){
		zoom = i+1;
		if (to_load[i]){
			x1 = (int)lon_to_x(selection->lon1, zoom);
			x2 = (int)lon_to_x(selection->lon2, zoom);
			y1 = (int)lat_to_y(selection->lat1, zoom);
			y2 = (int)lat_to_y(selection->lat2, zoom);
			for (ix = x1; ix <= x2; ix++){
				for (iy = y1; iy <= y2; iy++){
					MapTile map_tile = {ix, iy, zoom};
					tile_loader_add_tile(wizzard -> tile_loader, map_tile);
				}
			}
		}
	}
	tile_loader_start(wizzard -> tile_loader);
}

void wizzard_download_show(WizzardDownload * wizzard)
{
	GtkWidget * win = select_use_window_new(&(wizzard -> selection));
	gtk_window_set_transient_for(GTK_WINDOW(win), GTK_WINDOW(wizzard -> parent_window));
	gtk_window_set_title(GTK_WINDOW(win), "Download Selection...");
	gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER_ON_PARENT);
	gtk_window_set_modal(GTK_WINDOW(win), TRUE);
	gtk_widget_show_all(win);

	wizzard -> select_use_window = GOSM_SELECT_USE_WINDOW(win);
	g_signal_connect(G_OBJECT(wizzard -> select_use_window -> button_cancel), "clicked", G_CALLBACK(cancel_cb), wizzard);
	g_signal_connect(G_OBJECT(wizzard -> select_use_window -> button_load), "clicked", G_CALLBACK(download_cb), wizzard);
}

void wizzard_download_set_active(WizzardDownload * wizzard, int zoomlevel, gboolean state)
{
	if(wizzard -> select_use_window != NULL){
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wizzard -> select_use_window -> to_load_wid[zoomlevel-1]), state);
	}
}
