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

G_DEFINE_TYPE (PoiSourceSelector, poi_source_selector, GTK_TYPE_VBOX);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint poi_source_selector_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, poi_source_selector_signals[SIGNAL_NAME_n], 0);

enum
{
	COL_ACTIVE = 0,
	COL_NAME,
	COL_DIR,
	NUM_COLS
};

static GtkTreeModel * poi_source_selector_create_model (PoiManager * poi_manager);
static GtkWidget * poi_source_selector_create_view (PoiManager * poi_manager);
void poi_source_selector_list_cb(GtkTreeView	      *treeview,
		    GtkTreePath        *path,
		    GtkTreeViewColumn  *col,
		    gpointer            poi_manager);
static gboolean poi_source_selector_deactivated_cb(PoiManager * poi_manager, int index, gpointer data);
static gboolean poi_source_selector_activated_cb(PoiManager * poi_manager, int index, gpointer data);

PoiSourceSelector * poi_source_selector_new(PoiManager * poi_manager)
{
	PoiSourceSelector * poi_source_selector = g_object_new(GOSM_TYPE_POI_SOURCE_SELECTOR, NULL);
	g_signal_connect(G_OBJECT(poi_manager),"source-deactivated", 
		G_CALLBACK(poi_source_selector_deactivated_cb), (gpointer)poi_source_selector);
	g_signal_connect(G_OBJECT(poi_manager),"source-activated", 
		G_CALLBACK(poi_source_selector_activated_cb), (gpointer)poi_source_selector);
	poi_source_selector -> view = poi_source_selector_create_view(poi_manager);
	g_signal_connect(poi_source_selector -> view, "row-activated", (GCallback) poi_source_selector_list_cb, poi_manager);
	GtkWidget * scrolled = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolled), GTK_WIDGET(poi_source_selector -> view));
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled),
	                                GTK_POLICY_AUTOMATIC,
	                                GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(poi_source_selector), scrolled, TRUE, TRUE, 0);
	return poi_source_selector;
}

static void poi_source_selector_class_init(PoiSourceSelectorClass *class)
{
        /*poi_source_selector_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PoiSourceSelectorClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void poi_source_selector_init(PoiSourceSelector *poi_source_selector)
{
}

static GtkTreeModel * poi_source_selector_create_model (PoiManager * poi_manager)
{
	GtkListStore	*store;
	GtkTreeIter	  iter;
	
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


static gboolean poi_source_selector_deactivated_cb(PoiManager * poi_manager, int index, gpointer data)
{
	PoiSourceSelector * poi_source_selector = GOSM_POI_SOURCE_SELECTOR(data);
	GtkTreeView * view = GTK_TREE_VIEW(poi_source_selector -> view);
	GtkTreeModel * model = gtk_tree_view_get_model(view);
	GtkTreeIter iter;

	GtkTreePath * path = gtk_tree_path_new_from_indices(index, -1);
	gtk_tree_model_get_iter(model, &iter, path);
	PoiSource * poi_source = poi_manager_get_poi_source(poi_manager, index);
	gtk_list_store_set(GTK_LIST_STORE(model), &iter,
				COL_ACTIVE, " ",
				COL_NAME, poi_source -> basename,
				COL_DIR, poi_source -> dirname,
				-1);
}

static gboolean poi_source_selector_activated_cb(PoiManager * poi_manager, int index, gpointer data)
{
	PoiSourceSelector * poi_source_selector = GOSM_POI_SOURCE_SELECTOR(data);
	GtkTreeView * view = GTK_TREE_VIEW(poi_source_selector -> view);
	GtkTreeModel * model = gtk_tree_view_get_model(view);
	GtkTreeIter iter;

	GtkTreePath * path = gtk_tree_path_new_from_indices(index, -1);
	gtk_tree_model_get_iter(model, &iter, path);
	PoiSource * poi_source = poi_manager_get_poi_source(poi_manager, index);
	gtk_list_store_set(GTK_LIST_STORE(model), &iter,
				COL_ACTIVE, "x",
				COL_NAME, poi_source -> basename,
				COL_DIR, poi_source -> dirname,
				-1);
}

void poi_source_selector_list_cb(GtkTreeView	      *treeview,
		    GtkTreePath        *path,
		    GtkTreeViewColumn  *col,
		    gpointer            poi_manager_p)
{
	GtkTreeModel *model;
	GtkTreeIter	 iter;

	model = gtk_tree_view_get_model(treeview);

	PoiManager * poi_manager = GOSM_POI_MANAGER(poi_manager_p);

	if (gtk_tree_model_get_iter(model, &iter, path))
	{
		gint * indices = gtk_tree_path_get_indices(path); 
		int activated = indices[0];
		int old = poi_manager -> active_poi_source;
		poi_manager_activate_poi_source(poi_manager, activated);
		//poi_manager -> active_poi_source = activated;
		if (old != activated){
			PoiSource * poi_source_a = poi_manager_get_poi_source(poi_manager, activated);
			gtk_list_store_set(GTK_LIST_STORE(model), &iter,
						COL_ACTIVE, "x",
						COL_NAME, poi_source_a -> basename,
						COL_DIR, poi_source_a -> dirname,
						-1);
			if (old >= 0){
				GtkTreePath * path = gtk_tree_path_new_from_indices(old, -1);
				gtk_tree_model_get_iter(model, &iter, path);
				PoiSource * poi_source_o = poi_manager_get_poi_source(poi_manager, old);
				gtk_list_store_set(GTK_LIST_STORE(model), &iter,
							COL_ACTIVE, " ",
							COL_NAME, poi_source_o -> basename,
							COL_DIR, poi_source_o -> dirname,
							-1);
			}
			//GOSM_NAMEFINDER_COUNTRIES(namefinder_countries) -> activated = indices[0];
			//g_signal_emit (GOSM_NAMEFINDER_COUNTRIES(namefinder_countries), namefinder_countries_signals[CITY_ACTIVATED], 0);
		}
	}
}
