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
#include <gdk/gdk.h>

#include <pthread.h>
#include <glib.h>
#include <glib/gthread.h>
#include <unistd.h>

GtkWidget * load_widget_new(){

	GtkWidget *box_v = gtk_vbox_new(FALSE, 0);

	GtkWidget *head = gtk_label_new("Downloading Tiles...");
	GtkWidget *box_head = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start((GtkBox*)box_head, head, FALSE, FALSE, 0);

	GtkWidget *box1 = gtk_hbox_new(FALSE, 0);
	GtkWidget *box2 = gtk_hbox_new(FALSE, 0);
	GtkWidget *box3 = gtk_hbox_new(FALSE, 0);
	GtkWidget *box4 = gtk_hbox_new(FALSE, 0);

	GtkWidget *label_total = gtk_label_new("Total:");
	GtkWidget *label_ready = gtk_label_new("Completed:");
	GtkWidget *label_failed = gtk_label_new("Failed:");
	GtkWidget *label_total_s = gtk_label_new("219");
	GtkWidget *label_ready_s = gtk_label_new("10");
	GtkWidget *label_failed_s = gtk_label_new("0");

	GtkWidget *label_actual = gtk_label_new("File:");
	GtkWidget *label_actual_s = gtk_label_new("11_1254_671.png");

	gtk_box_pack_start((GtkBox*)box1, label_total, FALSE, FALSE, 0);
	gtk_box_pack_start((GtkBox*)box2, label_ready, FALSE, FALSE, 0);
	gtk_box_pack_start((GtkBox*)box3, label_failed, FALSE, FALSE, 0);
	gtk_box_pack_start((GtkBox*)box4, label_actual, FALSE, FALSE, 0);
	gtk_box_pack_end((GtkBox*)box1, label_total_s, FALSE, FALSE, 0);
	gtk_box_pack_end((GtkBox*)box2, label_ready_s, FALSE, FALSE, 0);
	gtk_box_pack_end((GtkBox*)box3, label_failed_s, FALSE, FALSE, 0);
	gtk_box_pack_end((GtkBox*)box4, label_actual_s, FALSE, FALSE, 0);

	GtkWidget *bar = gtk_progress_bar_new();
	gtk_progress_bar_set_text((GtkProgressBar*)bar, "40%");
	gtk_progress_bar_set_fraction((GtkProgressBar*)bar, 0.4);

	GtkWidget * button_pause = gtk_button_new_from_stock("gtk-media-pause");
	GtkWidget * button_cancel = gtk_button_new_from_stock("gtk-cancel");
	GtkWidget * box_buttons = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_buttons), button_pause, FALSE, FALSE, 0);
	gtk_box_pack_end(GTK_BOX(box_buttons),   button_cancel, FALSE, FALSE, 0);

	GtkWidget * table = gtk_table_new(4, 3, FALSE);
	gtk_table_attach((GtkTable*)table, box4,		0, 2, 1, 2, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	gtk_table_attach((GtkTable*)table, bar,			0, 2, 2, 3, GTK_EXPAND | GTK_FILL, 0, 0, 4);
	gtk_table_attach((GtkTable*)table, box1,		0, 2, 3, 4, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	gtk_table_attach((GtkTable*)table, box2,		0, 2, 4, 5, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	gtk_table_attach((GtkTable*)table, box3,		0, 2, 5, 6, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	
	gtk_box_pack_start(GTK_BOX(box_v), box_head, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_v), table, FALSE, FALSE, 4);
	gtk_box_pack_end(GTK_BOX(box_v), box_buttons, FALSE, FALSE, 0);

	gtk_widget_set_size_request(bar, 400, -1);

	return box_v;
}
