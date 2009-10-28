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

#include "poi_source_selector.h"

/****************************************************************************************************
* let the user manage the poi-source on harddisk
****************************************************************************************************/
G_DEFINE_TYPE (PoiSourceSelector, poi_source_selector, GTK_TYPE_VBOX);

/****************************************************************************************************
* columns
****************************************************************************************************/
enum
{
	COL_ACTIVE = 0,
	COL_NAME,
	COL_DIR,
	NUM_COLS
};

/****************************************************************************************************
* method declarations
****************************************************************************************************/
static GtkTreeModel * poi_source_selector_create_model (PoiManager * poi_manager);
static GtkWidget * poi_source_selector_create_view (PoiManager * poi_manager);
void poi_source_selector_list_cb(
		GtkTreeView		*treeview,
		GtkTreePath		*path,
		GtkTreeViewColumn	*col,
		gpointer		poi_manager_p);
static gboolean poi_source_selector_added_cb(PoiManager * poi_manager, int index, gpointer data);
static gboolean poi_source_selector_deleted_cb(PoiManager * poi_manager, int index, gpointer data);
static gboolean poi_source_selector_activated_cb(PoiManager * poi_manager, int index, gpointer data);
static gboolean poi_source_selector_deactivated_cb(PoiManager * poi_manager, int index, gpointer data);

/****************************************************************************************************
* constructor
****************************************************************************************************/
PoiSourceSelector * poi_source_selector_new(PoiManager * poi_manager)
{
	PoiSourceSelector * poi_source_selector = g_object_new(GOSM_TYPE_POI_SOURCE_SELECTOR, NULL);
	/* connect to the poi_manager signals, so that this class acts as a view */
	g_signal_connect(
		G_OBJECT(poi_manager), "source-added", 
		G_CALLBACK(poi_source_selector_added_cb), (gpointer)poi_source_selector);
	g_signal_connect(
		G_OBJECT(poi_manager), "source-deleted", 
		G_CALLBACK(poi_source_selector_deleted_cb), (gpointer)poi_source_selector);
	g_signal_connect(
		G_OBJECT(poi_manager), "source-activated",
		G_CALLBACK(poi_source_selector_activated_cb), (gpointer)poi_source_selector);
	g_signal_connect(
		G_OBJECT(poi_manager), "source-deactivated",
		G_CALLBACK(poi_source_selector_deactivated_cb), (gpointer)poi_source_selector);
	/* create tree view */
	poi_source_selector -> view = poi_source_selector_create_view(poi_manager);
	/* take care of activation of rows (double-click, ...) */
	g_signal_connect(
		G_OBJECT(poi_source_selector -> view), "row-activated",
		G_CALLBACK(poi_source_selector_list_cb), (gpointer)poi_manager);
	/* insert into a scrollarea */
	GtkWidget * scrolled = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolled), GTK_WIDGET(poi_source_selector -> view));
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled),
	                                GTK_POLICY_AUTOMATIC,
	                                GTK_POLICY_AUTOMATIC);
	/* add scrollarea to this widget itself */
	gtk_box_pack_start(GTK_BOX(poi_source_selector), scrolled, TRUE, TRUE, 0);
	return poi_source_selector;
}

static void poi_source_selector_class_init(PoiSourceSelectorClass *class)
{
}

static void poi_source_selector_init(PoiSourceSelector *poi_source_selector)
{
}

/****************************************************************************************************
* creates the model and fills it with the poi_managers' current sources
****************************************************************************************************/
static GtkTreeModel * poi_source_selector_create_model (PoiManager * poi_manager)
{
	GtkListStore * store;
	GtkTreeIter iter;
	store = gtk_list_store_new (NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	int count = poi_manager_get_number_of_poi_sources(poi_manager);
	int c;
	for (c = 0; c < count; c++){
		PoiSource * poi_source = poi_manager_get_poi_source(poi_manager, c);
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter,
					COL_ACTIVE, " ",
					COL_NAME, poi_source -> basename,
					COL_DIR, poi_source -> dirname,
					-1);
	}
	return GTK_TREE_MODEL (store);
}

/****************************************************************************************************
* creates the view with three columns (active, basename, directory)
****************************************************************************************************/
static GtkWidget * poi_source_selector_create_view (PoiManager * poi_manager)
{
	GtkCellRenderer     *renderer;
	GtkTreeModel        *model;
	GtkWidget           *view;

	view = gtk_tree_view_new ();
	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
	                                             -1,      
	                                             "x",  
	                                             renderer,
	                                             "text", COL_ACTIVE,
	                                             NULL);
	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
	                                             -1,      
	                                             "Name",  
	                                             renderer,
	                                             "text", COL_NAME,
	                                             NULL);
	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
	                                             -1,      
	                                             "Directory",  
	                                             renderer,
	                                             "text", COL_DIR,
	                                             NULL);
	model = poi_source_selector_create_model (poi_manager);
	gtk_tree_view_set_model (GTK_TREE_VIEW (view), model);
	g_object_unref (model);
	return view;
}

/****************************************************************************************************
* this callback is invoked, when an item in the list is activated (i.e. double-click)
* and then tells the poi_manager to open this file
****************************************************************************************************/
void poi_source_selector_list_cb(
		GtkTreeView		*treeview,
		GtkTreePath		*path,
		GtkTreeViewColumn	*col,
		gpointer		poi_manager_p)
{	
	PoiManager * poi_manager = GOSM_POI_MANAGER(poi_manager_p);
	GtkTreeModel * model = gtk_tree_view_get_model(treeview);
	GtkTreeIter iter;
	if (gtk_tree_model_get_iter(model, &iter, path))
	{
		gint * indices = gtk_tree_path_get_indices(path); 
		int activated = indices[0];
		int old = poi_manager -> active_poi_source;
		poi_manager_activate_poi_source(poi_manager, activated);
	}
}

/****************************************************************************************************
* this callback adds a row to the view
****************************************************************************************************/
static gboolean poi_source_selector_added_cb(PoiManager * poi_manager, int index, gpointer data)
{
	PoiSourceSelector * poi_source_selector = GOSM_POI_SOURCE_SELECTOR(data);
	GtkTreeView * view = GTK_TREE_VIEW(poi_source_selector -> view);
	GtkTreeModel * model = gtk_tree_view_get_model(view);
	GtkTreeIter iter;

	GtkTreePath * path = gtk_tree_path_new_from_indices(index, -1);
	gtk_tree_model_get_iter(model, &iter, path);
	PoiSource * poi_source = poi_manager_get_poi_source(poi_manager, index);
	gtk_list_store_insert(GTK_LIST_STORE(model), &iter, index);
	gtk_list_store_set(GTK_LIST_STORE(model), &iter,
				COL_ACTIVE, " ",
				COL_NAME, poi_source -> basename,
				COL_DIR, poi_source -> dirname,
				-1);
	gtk_tree_path_free(path);
	return FALSE;
}

/****************************************************************************************************
* this callback deletes the row with given index from the view
****************************************************************************************************/
static gboolean poi_source_selector_deleted_cb(PoiManager * poi_manager, int index, gpointer data)
{
	PoiSourceSelector * poi_source_selector = GOSM_POI_SOURCE_SELECTOR(data);
	GtkTreeView * view = GTK_TREE_VIEW(poi_source_selector -> view);
	GtkTreeModel * model = gtk_tree_view_get_model(view);
	GtkTreeIter iter;

	GtkTreePath * path = gtk_tree_path_new_from_indices(index, -1);
	gtk_tree_model_get_iter(model, &iter, path);
	gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
	gtk_tree_path_free(path);
	return FALSE;
}

/****************************************************************************************************
* this function returns the currently selected rows' index, or -1 if none is selected
****************************************************************************************************/
int poi_source_selector_get_active_item_index(PoiSourceSelector * poi_source_selector)
{
	GtkTreeView * view = GTK_TREE_VIEW(poi_source_selector -> view);
	GtkTreePath * path;
	GtkTreeViewColumn * cols;
	gtk_tree_view_get_cursor(view, &path, &cols);
	if (path != NULL){
		int * indices = gtk_tree_path_get_indices(path);
		int index = indices[0];
		gtk_tree_path_free(path);
		return index;
	}
	return -1;
}

/****************************************************************************************************
* this callback marks this (indictated by index) poi-source as active
* at the moment, this is a "x" character in the left-most column
* -1 may be given as index to activate none
****************************************************************************************************/
static gboolean poi_source_selector_activated_cb(PoiManager * poi_manager, int index, gpointer data)
{
	if (index >= 0){
		PoiSourceSelector * poi_source_selector = GOSM_POI_SOURCE_SELECTOR(data);
		GtkTreeView * view = GTK_TREE_VIEW(poi_source_selector -> view);
		GtkTreeModel * model = gtk_tree_view_get_model(view);
		GtkTreeIter iter;
		GtkTreePath * path = gtk_tree_path_new_from_indices(index, -1);
		gtk_tree_model_get_iter(model, &iter, path);
		gtk_list_store_set(GTK_LIST_STORE(model), &iter,
					COL_ACTIVE, "x",
					-1);
	}
	return FALSE;
}

/****************************************************************************************************
* this callback marks this (indicated by index) poi-source as deactive
* -1 may be given as index to deactivate none
****************************************************************************************************/
static gboolean poi_source_selector_deactivated_cb(PoiManager * poi_manager, int index, gpointer data)
{
	if (index >= 0){
		PoiSourceSelector * poi_source_selector = GOSM_POI_SOURCE_SELECTOR(data);
		GtkTreeView * view = GTK_TREE_VIEW(poi_source_selector -> view);
		GtkTreeModel * model = gtk_tree_view_get_model(view);
		GtkTreeIter iter;
		GtkTreePath * path = gtk_tree_path_new_from_indices(index, -1);
		gtk_tree_model_get_iter(model, &iter, path);
		gtk_list_store_set(GTK_LIST_STORE(model), &iter,
					COL_ACTIVE, " ",
					-1);
	}
	return FALSE;
}
