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
#include "node_add_tag_dialog.h"
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
static gboolean node_tool_button_add_cb(GtkWidget * button, PoiManager * poi_manager);
static gboolean node_tool_button_remove_cb(GtkWidget * button, NodeTool * node_tool);
static gboolean node_tool_tag_added_cb(PoiManager * poi_manager, int node_id, NodeTool * node_tool);

/****************************************************************************************************
* constructor
****************************************************************************************************/
GtkWidget * node_tool_new(PoiManager * poi_manager)
{
	NodeTool * node_tool = g_object_new(GOSM_TYPE_NODE_TOOL, NULL);
	node_tool -> poi_manager = poi_manager;
	/* toolbar */
	GtkWidget * tool_bar = gtk_hbox_new(FALSE, 0);
	GtkWidget * button_add = gtk_button_new();
	GtkWidget * button_delete = gtk_button_new();
	GtkWidget * icon_add = gtk_image_new_from_stock("gtk-add", GTK_ICON_SIZE_MENU);
	GtkWidget * icon_delete = gtk_image_new_from_stock("gtk-remove", GTK_ICON_SIZE_MENU);
	gtk_button_set_image(GTK_BUTTON(button_add), icon_add);
	gtk_button_set_image(GTK_BUTTON(button_delete), icon_delete);
	gtk_box_pack_start(GTK_BOX(tool_bar), button_add, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(tool_bar), button_delete, FALSE, FALSE, 0);
	/* treeview */
	node_tool -> view = node_tool_create_view(node_tool);
	GtkWidget * scrolled = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolled), node_tool -> view);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled),
	                                GTK_POLICY_AUTOMATIC,
	                                GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(scrolled, 180, -1);
	/* layout */
	gtk_box_pack_start(GTK_BOX(node_tool), tool_bar, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(node_tool), scrolled, TRUE, TRUE, 0);
	/* signals */
	g_signal_connect(
		G_OBJECT(button_add), "clicked",
		G_CALLBACK(node_tool_button_add_cb), poi_manager);
	g_signal_connect(
		G_OBJECT(button_delete), "clicked",
		G_CALLBACK(node_tool_button_remove_cb), node_tool);
	g_signal_connect(
		G_OBJECT(poi_manager), "node-tag-added",
		G_CALLBACK(node_tool_tag_added_cb), node_tool);
	g_signal_connect(
		G_OBJECT(poi_manager), "node-tag-changed",
		G_CALLBACK(node_tool_tag_added_cb), node_tool);
	g_signal_connect(
		G_OBJECT(poi_manager), "node-tag-removed",
		G_CALLBACK(node_tool_tag_added_cb), node_tool);
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
void node_tool_set_tags(NodeTool * node_tool, int node_id)
{
	node_tool -> node_id = node_id;
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
	if (node_id != 0){
		LonLatTags * llt = poi_manager_get_node(node_tool -> poi_manager, node_id);
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
}

/****************************************************************************************************
* show a dialog asking for key/value for a new tag
****************************************************************************************************/
static gboolean node_tool_button_add_cb(GtkWidget * button, PoiManager * poi_manager)
{
	if (poi_manager_can_add_tag(poi_manager)){
		NodeAddTagDialog * natd = node_add_tag_dialog_new();
		int response = node_add_tag_dialog_run(natd);
		if (response == GTK_RESPONSE_ACCEPT){
			poi_manager_add_tag(poi_manager, TRUE,
				poi_manager_get_selected_node_id(poi_manager),
				natd -> key, natd -> value);
		}
	}
	return FALSE;
}

static gboolean node_tool_button_remove_cb(GtkWidget * button, NodeTool * node_tool)
{
	char * key;
	char * value;
	PoiManager * poi_manager = node_tool -> poi_manager;
	GtkTreeView * view = GTK_TREE_VIEW(node_tool -> view);
	GtkTreeModel * model = gtk_tree_view_get_model(view);
	GtkTreePath * path;
	gtk_tree_view_get_cursor(view, &path, NULL);
	GtkTreeIter iter;
	if (path != NULL && gtk_tree_model_get_iter(model, &iter, path)){
		gtk_tree_model_get(model, &iter,
			COL_KEY, &key,
			COL_VALUE, &value,
			-1);
		if (poi_manager_can_add_tag(poi_manager)){
			poi_manager_remove_tag(poi_manager, TRUE,
				poi_manager_get_selected_node_id(poi_manager),
				key, value);
		}
		g_free(key);
		g_free(value);
	}
	return FALSE;
}

static gboolean node_tool_tag_added_cb(PoiManager * poi_manager, int node_id, NodeTool * node_tool)
{
	if (node_tool -> node_id == node_id){
		node_tool_set_tags(node_tool, node_id);
	}
	return FALSE;
}
