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

#include <sys/time.h>

#include "tile_download_window.h"
#include "../tiles/tilemath.h"

G_DEFINE_TYPE (TileDownloadWindow, tile_download_window, GTK_TYPE_WINDOW);

void tile_download_window_construct(TileDownloadWindow * tile_download_window, Selection * selection, gboolean *todo);

GtkWidget * tile_download_window_new(Selection * s, gboolean * todo)
{
	TileDownloadWindow * win = g_object_new(GOSM_TYPE_TILE_DOWNLOAD_WINDOW, NULL);
	tile_download_window_construct(win, s, todo);
	return GTK_WIDGET(win);
}

static void tile_download_window_class_init(TileDownloadWindowClass *class)
{
	GtkWidgetClass *widget_class;
	widget_class = GTK_WIDGET_CLASS(class);
}

static void tile_download_window_init(TileDownloadWindow *tile_download_window)
{

}

void tile_download_window_set_pause_button_state(TileDownloadWindow * tile_download_window, int state)
{
	GtkButton * button = GTK_BUTTON(tile_download_window -> button_pause);
        if (state == TILE_DOWNLOAD_WINDOW_PAUSE_BUTTON_PAUSED){
                gtk_button_set_label(button, "Pause");
                GtkWidget * icon = gtk_image_new_from_stock("gtk-media-pause", GTK_ICON_SIZE_BUTTON);
                gtk_button_set_image(button, icon);
        }else{
                gtk_button_set_label(button, "Resume");
                GtkWidget * icon = gtk_image_new_from_stock("gtk-media-play", GTK_ICON_SIZE_BUTTON);
                gtk_button_set_image(button, icon);
        }
}

void tile_download_window_construct(TileDownloadWindow * tile_download_window, Selection * selection, gboolean *todo)
{
	int count = 0;	
	int i; for (i = 1; i <= 18; i++){
		if (todo[i-1]){
	                double x1 = lon_to_x(selection->lon1, i);
	                double x2 = lon_to_x(selection->lon2, i);
	                double y1 = lat_to_y(selection->lat1, i);
	                double y2 = lat_to_y(selection->lat2, i);
	                int count_x = ((int) x2) - ((int)x1) + 1;
	                int count_y = ((int) y2) - ((int)y1) + 1;
			count += count_x * count_y;
		}
	}
	char buf[20];
	sprintf(buf, "%d", count);

	GtkWidget *box_v = gtk_vbox_new(FALSE, 0);

	gtk_container_add(GTK_CONTAINER(tile_download_window), box_v);

	GtkWidget *head = gtk_label_new("Downloading Tiles...");
	GtkWidget *box_head = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_head), head, FALSE, FALSE, 0);

	GtkWidget *box1 = gtk_hbox_new(FALSE, 0);
	GtkWidget *box2 = gtk_hbox_new(FALSE, 0);
	GtkWidget *box3 = gtk_hbox_new(FALSE, 0);
	GtkWidget *box4 = gtk_hbox_new(FALSE, 0);

	GtkWidget *label_total = gtk_label_new("Total:");
	GtkWidget *label_ready = gtk_label_new("Completed:");
	GtkWidget *label_failed = gtk_label_new("Failed:");
	GtkWidget *label_total_s = gtk_label_new(buf);
	tile_download_window -> label_ready_s = gtk_label_new("0");
	tile_download_window -> label_failed_s = gtk_label_new("0");

	GtkWidget *label_actual = gtk_label_new("File:");
	GtkWidget *label_actual_s = gtk_label_new("11_1254_671.png");

	gtk_box_pack_start(GTK_BOX(box1), label_total, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box2), label_ready, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box3), label_failed, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box4), label_actual, FALSE, FALSE, 0);
	gtk_box_pack_end(GTK_BOX(box1), label_total_s, FALSE, FALSE, 0);
	gtk_box_pack_end(GTK_BOX(box2), tile_download_window -> label_ready_s, FALSE, FALSE, 0);
	gtk_box_pack_end(GTK_BOX(box3), tile_download_window -> label_failed_s, FALSE, FALSE, 0);
	gtk_box_pack_end(GTK_BOX(box4), label_actual_s, FALSE, FALSE, 0);

	tile_download_window -> bar = gtk_progress_bar_new();
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(tile_download_window -> bar), "0%");
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(tile_download_window -> bar), 0.0);

	tile_download_window -> button_pause = gtk_button_new_from_stock("gtk-media-pause");
	tile_download_window -> button_cancel = gtk_button_new_from_stock("gtk-cancel");
	GtkWidget * box_buttons = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_buttons), tile_download_window -> button_pause, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_buttons),   tile_download_window -> button_cancel, FALSE, FALSE, 0);

	GtkWidget * table = gtk_table_new(4, 3, FALSE);
	//gtk_table_attach(GTK_TABLE(table), box4,			0, 2, 1, 2, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), tile_download_window -> bar,	0, 2, 2, 3, GTK_EXPAND | GTK_FILL, 0, 0, 4);
	gtk_table_attach(GTK_TABLE(table), box1,			0, 2, 3, 4, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), box2,			0, 2, 4, 5, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), box3,			0, 2, 5, 6, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	
	gtk_box_pack_start(GTK_BOX(box_v), box_head, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_v), table, FALSE, FALSE, 4);
	gtk_box_pack_end(GTK_BOX(box_v), box_buttons, FALSE, FALSE, 0);

	gtk_widget_set_size_request(tile_download_window -> bar, 400, -1);
}
