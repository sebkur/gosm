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
#include "edit/edit_action.h"

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

static void edit_widget_action_added_cb(PoiManager * poi_manager, gpointer action_p, gpointer data);
static void edit_widget_action_undo_cb(PoiManager * poi_manager, int index, gpointer data);
static void edit_widget_action_redo_cb(PoiManager * poi_manager, int index, gpointer data);
static void edit_widget_action_remove_multiple_cb(PoiManager * poi_manager, gpointer x, gpointer data);
static gboolean edit_widget_undo_cb(GtkWidget * button, EditWidget * edit_widget);
static gboolean edit_widget_redo_cb(GtkWidget * button, EditWidget * edit_widget);
void edit_widget_cell_data_func(
	GtkTreeViewColumn * tree_column,
	GtkCellRenderer * cell,
	GtkTreeModel * tree_model,
	GtkTreeIter * iter,
	gpointer data);

GtkWidget * edit_widget_new(PoiManager * poi_manager)
{
	EditWidget * edit_widget = g_object_new(GOSM_TYPE_EDIT_WIDGET, NULL);
	edit_widget -> poi_manager = poi_manager;
	g_signal_connect(
		G_OBJECT(poi_manager), "action-added",
		G_CALLBACK(edit_widget_action_added_cb), edit_widget);
	g_signal_connect(
		G_OBJECT(poi_manager), "action-undo",
		G_CALLBACK(edit_widget_action_undo_cb), edit_widget);
	g_signal_connect(
		G_OBJECT(poi_manager), "action-redo",
		G_CALLBACK(edit_widget_action_redo_cb), edit_widget);
	g_signal_connect(
		G_OBJECT(poi_manager), "action-remove-multiple",
		G_CALLBACK(edit_widget_action_remove_multiple_cb), edit_widget);
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
	edit_widget -> button_undo = gtk_button_new();
	edit_widget -> button_redo = gtk_button_new();
	GtkWidget * image_undo = gtk_image_new_from_stock("gtk-undo", GTK_ICON_SIZE_MENU);
	GtkWidget * image_redo = gtk_image_new_from_stock("gtk-redo", GTK_ICON_SIZE_MENU);
	gtk_button_set_image(GTK_BUTTON(edit_widget -> button_undo), image_undo);
	gtk_button_set_image(GTK_BUTTON(edit_widget -> button_redo), image_redo);
	gtk_box_pack_start(GTK_BOX(toolbar), edit_widget -> button_undo, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(toolbar), edit_widget -> button_redo, FALSE, FALSE, 0);
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
	gtk_tree_view_column_set_cell_data_func(
		gtk_tree_view_get_column(view, COL_NAME),
		renderer, edit_widget_cell_data_func, edit_widget, NULL);

	edit_widget -> view = view;

	gtk_widget_set_sensitive(edit_widget -> button_undo, FALSE);
	gtk_widget_set_sensitive(edit_widget -> button_redo, FALSE);

	g_signal_connect(
		G_OBJECT(edit_widget -> button_undo), "clicked",
		G_CALLBACK(edit_widget_undo_cb), edit_widget);
	g_signal_connect(
		G_OBJECT(edit_widget -> button_redo), "clicked",
		G_CALLBACK(edit_widget_redo_cb), edit_widget);
}

static void edit_widget_action_added_cb(PoiManager * poi_manager, gpointer action_p, gpointer data)
{
	EditWidget * edit_widget = GOSM_EDIT_WIDGET(data);
	EditAction * action = GOSM_EDIT_ACTION(action_p);
	char * text = edit_action_to_string(action);
	GtkListStore * store = GTK_LIST_STORE(gtk_tree_view_get_model(edit_widget -> view));
	GtkTreeIter iter;
	gtk_list_store_prepend(store, &iter);
	gtk_list_store_set (store, &iter,
				COL_NAME, text,
				-1);
	GtkTreePath * path = gtk_tree_path_new_from_indices(0, -1);
	gtk_tree_view_scroll_to_cell(edit_widget -> view, path, NULL, FALSE, 0, 0);
	gtk_widget_set_sensitive(edit_widget -> button_undo, TRUE);
	gtk_widget_set_sensitive(edit_widget -> button_redo, FALSE);
}

static void edit_widget_action_undo_cb(PoiManager * poi_manager, int index, gpointer data)
{
	EditWidget * edit_widget = GOSM_EDIT_WIDGET(data);
	// grey out
	if (index == 0){
		gtk_widget_set_sensitive(edit_widget -> button_undo, FALSE);
	}
	gtk_widget_set_sensitive(edit_widget -> button_redo, TRUE);
	gtk_widget_queue_draw(GTK_WIDGET(edit_widget -> view));
}

static void edit_widget_action_redo_cb(PoiManager * poi_manager, int index, gpointer data)
{
	EditWidget * edit_widget = GOSM_EDIT_WIDGET(data);
	// grey out
	if (index + 1 == poi_manager -> changes -> len){
		gtk_widget_set_sensitive(edit_widget -> button_redo, FALSE);
	}
	gtk_widget_set_sensitive(edit_widget -> button_undo, TRUE);
	gtk_widget_queue_draw(GTK_WIDGET(edit_widget -> view));
}

static void edit_widget_action_remove_multiple_cb(PoiManager * poi_manager, gpointer x, gpointer data)
{
	int * is = (int*)x;
	EditWidget * edit_widget = GOSM_EDIT_WIDGET(data);
	GtkListStore * store = GTK_LIST_STORE(gtk_tree_view_get_model(edit_widget -> view));
	GtkTreeIter iter;
	int len = is[0];
	int i;
	for (i = 0; i < is[2]; i++){
		int pos = len - (is[1] + is[2]);
		printf("%d\n", pos);
		GtkTreePath * path = gtk_tree_path_new_from_indices(pos, -1);
		gtk_tree_model_get_iter(GTK_TREE_MODEL(store), &iter, path);
		gtk_list_store_remove(store, &iter);
	}
	gtk_widget_set_sensitive(edit_widget -> button_undo, 
		(poi_manager -> change_index > 0));
	gtk_widget_set_sensitive(edit_widget -> button_redo, 
		(poi_manager -> change_index + 2 < poi_manager -> changes -> len));
}

void edit_widget_cell_data_func(
	GtkTreeViewColumn * tree_column,
	GtkCellRenderer * cell,
	GtkTreeModel * tree_model,
	GtkTreeIter * iter,
	gpointer data)
{
	EditWidget * edit_widget = GOSM_EDIT_WIDGET(data);
	GtkTreePath * path = gtk_tree_model_get_path(gtk_tree_view_get_model(edit_widget -> view), iter);
	int index = edit_widget -> poi_manager -> changes -> len - gtk_tree_path_get_indices(path)[0] - 1;
	if (index <= edit_widget -> poi_manager -> change_index){
		g_object_set(cell, "foreground-gdk", gtk_widget_get_style(GTK_WIDGET(edit_widget -> view)) -> fg, NULL);
	}else{
		g_object_set(cell, "foreground-gdk", gtk_widget_get_style(GTK_WIDGET(edit_widget -> view)) -> bg, NULL);
	}
}

static gboolean edit_widget_undo_cb(GtkWidget * button, EditWidget * edit_widget)
{
	poi_manager_undo(edit_widget -> poi_manager);
}

static gboolean edit_widget_redo_cb(GtkWidget * button, EditWidget * edit_widget)
{
	poi_manager_redo(edit_widget -> poi_manager);
}
