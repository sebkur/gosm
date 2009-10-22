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

#include "select_use_window.h"
#include "../tiles/tilemath.h"

G_DEFINE_TYPE (SelectUseWindow, select_use_window, GTK_TYPE_WINDOW);

void select_use_window_construct(SelectUseWindow *select_use_window);

GtkWidget * select_use_window_new(Selection * s)
{
	SelectUseWindow * win = g_object_new(GOSM_TYPE_SELECT_USE_WINDOW, NULL);
	memcpy(&(win -> selection), s, sizeof(Selection));
	select_use_window_construct(win);
	return GTK_WIDGET(win);
}

static void select_use_window_class_init(SelectUseWindowClass *class)
{
	GtkWidgetClass *widget_class;
	widget_class = GTK_WIDGET_CLASS(class);
}

static void select_use_window_init(SelectUseWindow *select_use_window)
{
	printf("selection use window\n");
}

void select_use_window_construct(SelectUseWindow *select_use_window)
{
	Selection * selection = &(select_use_window -> selection);
	GtkWidget * table = gtk_table_new(19, 9, FALSE);
	gtk_container_add(GTK_CONTAINER(select_use_window), table);
	GtkWidget * label_level = gtk_label_new("Level");
	GtkWidget * label_x1 = gtk_label_new("x1");
	GtkWidget * label_x2 = gtk_label_new("x2");
	GtkWidget * label_y1 = gtk_label_new("y1");
	GtkWidget * label_y2 = gtk_label_new("y2");
	GtkWidget * label_count_x = gtk_label_new("#x");
	GtkWidget * label_count_y = gtk_label_new("#y");
	GtkWidget * label_mult = gtk_label_new("#xy");
	GtkWidget * label_load = gtk_label_new("load");
	
	gtk_table_attach(GTK_TABLE(table), label_level,		0, 1, 0, 1, 0, 0, 8, 0);
	gtk_table_attach(GTK_TABLE(table), label_x1,		1, 2, 0, 1, 0, 0, 8, 0);
	gtk_table_attach(GTK_TABLE(table), label_x2,		2, 3, 0, 1, 0, 0, 8, 0);
	gtk_table_attach(GTK_TABLE(table), label_y1,		3, 4, 0, 1, 0, 0, 8, 0);
	gtk_table_attach(GTK_TABLE(table), label_y2,		4, 5, 0, 1, 0, 0, 8, 0);
	gtk_table_attach(GTK_TABLE(table), label_count_x,	5, 6, 0, 1, 0, 0, 8, 0);
	gtk_table_attach(GTK_TABLE(table), label_count_y,	6, 7, 0, 1, 0, 0, 8, 0);
	gtk_table_attach(GTK_TABLE(table), label_mult,		7, 8, 0, 1, 0, 0, 8, 0);
	gtk_table_attach(GTK_TABLE(table), label_load,		8, 9, 0, 1, 0, 0, 8, 0);

	int i; for (i = 1; i <= 18; i++){
		char buf[16];
		sprintf(buf, "%d", i);
		GtkWidget * lab_lev = gtk_label_new(buf);
		double x1 = lon_to_x(selection->lon1, i);
		double x2 = lon_to_x(selection->lon2, i);
		double y1 = lat_to_y(selection->lat1, i);
		double y2 = lat_to_y(selection->lat2, i);
		int count_x = ((int) x2) - ((int)x1) + 1;
		int count_y = ((int) y2) - ((int)y1) + 1;
		sprintf(buf, "%d", (int)x1);
		GtkWidget * lab_x1 = gtk_label_new(buf);
		sprintf(buf, "%d", (int)x2);
		GtkWidget * lab_x2 = gtk_label_new(buf);
		sprintf(buf, "%d", (int)y1);
		GtkWidget * lab_y1 = gtk_label_new(buf);
		sprintf(buf, "%d", (int)y2);
		GtkWidget * lab_y2 = gtk_label_new(buf);
		sprintf(buf, "%d", count_x);
		GtkWidget * lab_count_x = gtk_label_new(buf);
		sprintf(buf, "%d", count_y);
		GtkWidget * lab_count_y = gtk_label_new(buf);
		sprintf(buf, "%d", count_x * count_y);
		GtkWidget * lab_mult = gtk_label_new(buf);
		//GtkWidget * check_load = gtk_check_button_new();
		select_use_window -> to_load_wid[i-1] = gtk_check_button_new();
		GtkWidget * check_load = select_use_window -> to_load_wid[i-1];

		gtk_table_attach(GTK_TABLE(table), lab_lev,	0, 1, i, i+1, 0, 0, 8, 0);
		gtk_table_attach(GTK_TABLE(table), lab_x1,	1, 2, i, i+1, 0, 0, 8, 0);
		gtk_table_attach(GTK_TABLE(table), lab_x2,	2, 3, i, i+1, 0, 0, 8, 0);
		gtk_table_attach(GTK_TABLE(table), lab_y1,	3, 4, i, i+1, 0, 0, 8, 0);
		gtk_table_attach(GTK_TABLE(table), lab_y2,	4, 5, i, i+1, 0, 0, 8, 0);
		gtk_table_attach(GTK_TABLE(table), lab_count_x,	5, 6, i, i+1, 0, 0, 8, 0);
		gtk_table_attach(GTK_TABLE(table), lab_count_y,	6, 7, i, i+1, 0, 0, 8, 0);
		gtk_table_attach(GTK_TABLE(table), lab_mult,	7, 8, i, i+1, 0, 0, 8, 0);
		gtk_table_attach(GTK_TABLE(table), check_load,	8, 9, i, i+1, 0, 0, 8, 0);

		//g_signal_connect(G_OBJECT(check_load),  "toggled", G_CALLBACK(toggle_check_cb), GINT_TO_POINTER(i));
	}
	GtkWidget * hbox = gtk_hbox_new(FALSE, 0);
	gtk_table_attach(GTK_TABLE(table), hbox, 0, 9, 19, 20, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	select_use_window -> button_load = gtk_button_new_with_label("Load to Disk");
	GtkWidget * icon_load = gtk_image_new_from_stock("gtk-harddisk", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(select_use_window -> button_load), icon_load);
	select_use_window -> button_cancel = gtk_button_new_from_stock("gtk-cancel");
	gtk_box_pack_start(GTK_BOX(hbox), select_use_window -> button_load, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), select_use_window -> button_cancel, FALSE, FALSE, 0);

	//g_signal_connect(G_OBJECT(button_cancel), "clicked", G_CALLBACK(cancel_cb), select_use_window);
	//g_signal_connect(G_OBJECT(button_load), "clicked", G_CALLBACK(download_cb), select_use_window);

}
