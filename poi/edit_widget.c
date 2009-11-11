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

#include "edit_widget.h"

G_DEFINE_TYPE (EditWidget, edit_widget, GTK_TYPE_VBOX);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint edit_widget_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, edit_widget_signals[SIGNAL_NAME_n], 0);

enum {
	COL_NAME = 0,
	NUM_COLS
};

GtkWidget * edit_widget_new()
{
	EditWidget * edit_widget = g_object_new(GOSM_TYPE_EDIT_WIDGET, NULL);
	return GTK_WIDGET(edit_widget);
}

static void edit_widget_class_init(EditWidgetClass *class)
{
        /*edit_widget_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (EditWidgetClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void edit_widget_init(EditWidget *edit_widget)
{
	GtkWidget * vbox = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(edit_widget), vbox, TRUE, TRUE, 0);
	GtkWidget * toolbar = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);
	GtkWidget * button_undo = gtk_button_new();
	GtkWidget * button_redo = gtk_button_new();
	GtkWidget * image_undo = gtk_image_new_from_stock("gtk-undo", GTK_ICON_SIZE_MENU);
	GtkWidget * image_redo = gtk_image_new_from_stock("gtk-redo", GTK_ICON_SIZE_MENU);
	gtk_button_set_image(GTK_BUTTON(button_undo), image_undo);
	gtk_button_set_image(GTK_BUTTON(button_redo), image_redo);
	gtk_box_pack_start(GTK_BOX(toolbar), button_undo, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(toolbar), button_redo, FALSE, FALSE, 0);
	GtkListStore * store = gtk_list_store_new(1, G_TYPE_STRING);
	GtkTreeView * view = GTK_TREE_VIEW(gtk_tree_view_new());
	gtk_tree_view_set_model(view, GTK_TREE_MODEL(store));
	GtkCellRenderer * renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(view, -1, "Edit", renderer, "text", COL_NAME, NULL);
	GtkWidget * scrolled = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolled), GTK_WIDGET(view));
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled),
	                                GTK_POLICY_AUTOMATIC,
	                                GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);
}

