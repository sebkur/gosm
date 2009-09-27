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

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "wizzard_delete_tiles_window.h"

G_DEFINE_TYPE (WizzardDeleteTilesWindow, wizzard_delete_tiles_window, GTK_TYPE_WINDOW);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint wizzard_delete_tiles_window_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, wizzard_delete_tiles_window_signals[SIGNAL_NAME_n], 0);

void wizzard_delete_tiles_window_construct(WizzardDeleteTilesWindow * wdtw);

WizzardDeleteTilesWindow * wizzard_delete_tiles_window_new()
{
	WizzardDeleteTilesWindow * wizzard_delete_tiles_window = g_object_new(GOSM_TYPE_WIZZARD_DELETE_TILES_WINDOW, NULL);
	wizzard_delete_tiles_window_construct(wizzard_delete_tiles_window);
	return wizzard_delete_tiles_window;
}

static void wizzard_delete_tiles_window_class_init(WizzardDeleteTilesWindowClass *class)
{
        /*wizzard_delete_tiles_window_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (WizzardDeleteTilesWindowClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void wizzard_delete_tiles_window_init(WizzardDeleteTilesWindow *wizzard_delete_tiles_window)
{
}

void wizzard_delete_tiles_window_construct(WizzardDeleteTilesWindow * wdtw)
{
	GtkWidget * table = gtk_table_new(19, 2, FALSE);
	GtkWidget * label_zoom = gtk_label_new("Zoomlevel");
	GtkWidget * label_select = gtk_label_new("Delete");
	gtk_table_attach(GTK_TABLE(table), label_zoom, 0, 1, 0, 1, 0, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), label_select, 0, 1, 1, 2, 0, 0, 0, 0);
	int i = 0;
	for (i = 1; i <= 18; i++){
		char buf[3];
		sprintf(buf, "%d", i);
		GtkWidget * label = gtk_label_new(buf);
		wdtw -> checks[i-1] = gtk_check_button_new();
		gtk_table_attach(GTK_TABLE(table), label, i, i+1, 0, 1, 0, 0, 0, 0);
		gtk_table_attach(GTK_TABLE(table), wdtw -> checks[i-1], i, i+1, 1, 2, 0, 0, 0, 0);
	}
	wdtw -> button_confirm = gtk_button_new_from_stock("gtk-delete");
	wdtw -> button_cancel = gtk_button_new_from_stock("gtk-cancel");

	GtkWidget * box_v = gtk_vbox_new(0, FALSE);
	gtk_container_add(GTK_CONTAINER(wdtw), box_v);

	gtk_box_pack_start(GTK_BOX(box_v), table, FALSE, FALSE, 0);
	GtkWidget * box_buttons = gtk_hbox_new(0, FALSE);
	gtk_box_pack_start(GTK_BOX(box_buttons), wdtw -> button_confirm, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_buttons), wdtw -> button_cancel, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_v), box_buttons, FALSE, FALSE, 0);
}
