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

#include "node_tool.h"
#include "../map_types.h"

/****************************************************************************************************
* NodeTool shows the key-value pairs of a node in a treeview
****************************************************************************************************/
G_DEFINE_TYPE (NodeTool, node_tool, GTK_TYPE_VBOX);

/****************************************************************************************************
* the columns of the treeview
****************************************************************************************************/
enum
{
	COL_KEY = 0,
	COL_VALUE,
	NUM_COLS
};

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint node_tool_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, node_tool_signals[SIGNAL_NAME_n], 0);

/****************************************************************************************************
* method declarations
****************************************************************************************************/
static GtkWidget * node_tool_create_view (NodeTool * node_tool);

/****************************************************************************************************
* constructor
****************************************************************************************************/
GtkWidget * node_tool_new()
{
	NodeTool * node_tool = g_object_new(GOSM_TYPE_NODE_TOOL, NULL);
	node_tool -> view = node_tool_create_view(node_tool);
	GtkWidget * scrolled = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolled), node_tool -> view);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled),
	                                GTK_POLICY_AUTOMATIC,
	                                GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(scrolled, 180, -1);
	gtk_box_pack_start(GTK_BOX(node_tool), scrolled, TRUE, TRUE, 0);
	return GTK_WIDGET(node_tool);
}

static void node_tool_class_init(NodeToolClass *class)
{
        /*node_tool_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (NodeToolClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void node_tool_init(NodeTool *node_tool)
{
}

/****************************************************************************************************
* create the treeview's model
****************************************************************************************************/
static GtkTreeModel * node_tool_create_model ()
{
	GtkListStore	*store;
	GtkTreeIter	  iter;
	store = gtk_list_store_new (NUM_COLS, G_TYPE_STRING, G_TYPE_STRING);
	return GTK_TREE_MODEL (store);
}

/****************************************************************************************************
* create the treeview
****************************************************************************************************/
static GtkWidget * node_tool_create_view (NodeTool * node_tool)
{
	GtkCellRenderer     *renderer;
	GtkTreeModel        *model;
	GtkWidget           *view;

	view = gtk_tree_view_new ();
	renderer = gtk_cell_renderer_text_new ();
	g_object_set(renderer, "width-chars", 10, NULL);
	g_object_set(renderer, "editable", TRUE, NULL);
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
	                                             -1,
	                                             "Key",
	                                             renderer,
	                                             "text", COL_KEY,
	                                             NULL);
	renderer = gtk_cell_renderer_text_new ();
	g_object_set(renderer, "editable", TRUE, NULL);
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
	                                             -1,
	                                             "Value",
	                                             renderer,
	                                             "text", COL_VALUE,
	                                             NULL);
	model = node_tool_create_model ();
	gtk_tree_view_set_model (GTK_TREE_VIEW (view), model);
	int n;
	for (n = 0; n < 2; n++){
		GtkTreeViewColumn * col = gtk_tree_view_get_column(GTK_TREE_VIEW(view), n);
		gtk_tree_view_column_set_resizable(col, TRUE);
		if (n == 1) gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_AUTOSIZE);
	}
	g_object_unref (model);
	return view;
}

/****************************************************************************************************
* set the tags (key/value-pairs) the NodeTool should display
****************************************************************************************************/
void node_tool_set_tags(NodeTool * node_tool, LonLatTags * llt)
{
	GtkTreeView * view = GTK_TREE_VIEW(node_tool -> view);
	GtkTreeModel * model = gtk_tree_view_get_model(view);
	GtkTreeIter iter;
	/* remove old entries */
	GtkTreePath * path = gtk_tree_path_new_from_indices(0, -1);
	while(gtk_tree_model_get_iter(model, &iter, path)){
		path = gtk_tree_path_new_from_indices(0, -1);
		gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
	}
	/* insert new entries */
	path = gtk_tree_path_new_from_indices(0, -1);
	gtk_tree_model_get_iter(model, &iter, path);
	GHashTableIter hash_iter;
	g_hash_table_iter_init(&hash_iter, llt -> tags);
	gpointer key, val;
	while(g_hash_table_iter_next(&hash_iter, &key, &val)){
		gtk_list_store_append (GTK_LIST_STORE(model), &iter);
		gtk_list_store_set(GTK_LIST_STORE(model), &iter,
					COL_KEY, (char*) key,
					COL_VALUE, (char*) val,
					-1);
	}
}
