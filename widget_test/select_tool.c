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

GtkWidget * select_tool_new(){

	GtkWidget *head = gtk_label_new("Selection");
	GtkWidget *box_head = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start((GtkBox*)box_head, head, FALSE, FALSE, 0);

	GtkWidget *lon1 = gtk_entry_new();
	GtkWidget *lon2 = gtk_entry_new();
	GtkWidget *lat1 = gtk_entry_new();
	GtkWidget *lat2 = gtk_entry_new();

	GtkWidget *icon1 = gtk_image_new_from_file("icons/square_top_left.png");
	GtkWidget *icon2 = gtk_image_new_from_file("icons/square_bottom_right.png");

	GtkWidget *check_show = gtk_check_button_new_with_label("show");
	GtkWidget *box_check_show = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start((GtkBox*)box_check_show, check_show, FALSE, FALSE, 0);

	GtkWidget * hbox = gtk_table_new(4, 3, FALSE);
	gtk_table_attach((GtkTable*)hbox, box_head,		0, 3, 0, 1, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	gtk_table_attach((GtkTable*)hbox, icon1,		0, 1, 1, 2, GTK_SHRINK, 0, 0, 0);
	gtk_table_attach((GtkTable*)hbox, icon2,		0, 1, 2, 3, GTK_SHRINK, 0, 0, 0);
	gtk_table_attach((GtkTable*)hbox, lon1,			1, 2, 1, 2, GTK_FILL | GTK_EXPAND, 0, 0, 0);
	gtk_table_attach((GtkTable*)hbox, lon2,			1, 2, 2, 3, GTK_FILL | GTK_EXPAND, 0, 0, 0);
	gtk_table_attach((GtkTable*)hbox, lat1,			2, 3, 1, 2, GTK_FILL | GTK_EXPAND, 0, 0, 0);
	gtk_table_attach((GtkTable*)hbox, lat2,			2, 3, 2, 3, GTK_FILL | GTK_EXPAND, 0, 0, 0);
	gtk_table_attach((GtkTable*)hbox, box_check_show,	0, 3, 3, 4, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	
	gtk_widget_set_size_request(hbox, 200, -1);
	gtk_widget_set_size_request(lon1, 0 , -1);
	gtk_widget_set_size_request(lon2, 0 , -1);
	gtk_widget_set_size_request(lat1, 0 , -1);
	gtk_widget_set_size_request(lat2, 0 , -1);
	
	return hbox;
}
