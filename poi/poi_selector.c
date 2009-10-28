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

/*
 * this is derived from http://scentric.net/tutorial/sec-custom-cell-renderers.html
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "poi_selector.h"
#include "cell_renderer_colour.h"
#include "../config/color_button.h"

/****************************************************************************************************
* the PoiSelector widget let's the user select the set of PoiSets to be displayed (i.e. a list of 
* tags to be shown on the map
****************************************************************************************************/
G_DEFINE_TYPE (PoiSelector, poi_selector, GTK_TYPE_VBOX);

/****************************************************************************************************
* the columns
****************************************************************************************************/
enum
{
	COL_ACTIVE = 0,
	COL_KEY,
	COL_VALUE,
	COL_R, COL_G, COL_B, COL_A,
	NUM_COLS
};

//static guint poi_selector_signals[LAST_SIGNAL] = { 0 };

/****************************************************************************************************
* method declarations
****************************************************************************************************/
static GtkWidget * poi_selector_create_view (PoiSelector * poi_selector);
void poi_selector_add_pair(PoiSelector * poi_selector, StyledPoiSet * poi_set);
static gboolean poi_selector_check_cb(GtkCellRendererToggle * renderer, const gchar * path_string, gpointer data);
static gboolean poi_selector_color_button_cb(CellRendererColour * renderer, const gchar * path_string, gpointer data);
static gboolean poi_selector_colour_changed_cb(PoiManager * poi_manager, int index, gpointer data);
static gboolean poi_selector_layer_toggled_cb(PoiManager * poi_manager, int index, gpointer data);
static gboolean poi_selector_layer_added_cb(PoiManager * poi_manager, int index, gpointer data);
static gboolean poi_selector_layer_deleted_cb(PoiManager * poi_manager, int index, gpointer data);

/****************************************************************************************************
* constructor
****************************************************************************************************/
GtkWidget * poi_selector_new(PoiManager * poi_manager)
{
	PoiSelector * poi_selector = g_object_new(GOSM_TYPE_POI_SELECTOR, NULL);
	poi_selector -> poi_manager = poi_manager;
	g_signal_connect(
		G_OBJECT(poi_manager), "layer-added", 
		G_CALLBACK(poi_selector_layer_added_cb), (gpointer)poi_selector);
	g_signal_connect(
		G_OBJECT(poi_manager), "layer-deleted", 
		G_CALLBACK(poi_selector_layer_deleted_cb), (gpointer)poi_selector);
	g_signal_connect(
		G_OBJECT(poi_manager), "layer-toggled", 
		G_CALLBACK(poi_selector_layer_toggled_cb), (gpointer)poi_selector);
	g_signal_connect(
		G_OBJECT(poi_manager), "colour-changed",
		G_CALLBACK(poi_selector_colour_changed_cb), (gpointer)poi_selector);
	poi_selector -> view = poi_selector_create_view(poi_selector);
	GtkWidget * scrolled = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolled), poi_selector -> view);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled),
	                                GTK_POLICY_AUTOMATIC,
	                                GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(poi_selector), scrolled, TRUE, TRUE, 0);
	return GTK_WIDGET(poi_selector);
}

static void poi_selector_class_init(PoiSelectorClass *class)
{
}

static void poi_selector_init(PoiSelector *poi_selector)
{
}

/****************************************************************************************************
* when a row has been checked or unchecked
****************************************************************************************************/
static gboolean poi_selector_check_cb(GtkCellRendererToggle * renderer, const gchar * path_string, gpointer data)
{
	PoiSelector * poi_selector = GOSM_POI_SELECTOR(data);
	PoiManager * poi_manager = poi_selector -> poi_manager;
	GtkTreePath * path = gtk_tree_path_new_from_string(path_string);
	gint * indices = gtk_tree_path_get_indices(path); 
	int activated = indices[0];
	StyledPoiSet * poi_set = poi_manager_get_poi_set(poi_manager, activated);
	poi_manager_toggle_poi_set(poi_manager, activated);
	return FALSE;
}

/****************************************************************************************************
* when the colour-column has been clicked in a row
****************************************************************************************************/
static gboolean poi_selector_color_button_cb(CellRendererColour * renderer, const gchar * path_string, gpointer data)
{
	PoiSelector * poi_selector = GOSM_POI_SELECTOR(data);
	PoiManager * poi_manager = poi_selector -> poi_manager;
	GtkTreePath * path = gtk_tree_path_new_from_string(path_string);
	gint * indices = gtk_tree_path_get_indices(path); 
	int activated = indices[0];
	StyledPoiSet * poi_set = poi_manager_get_poi_set(poi_manager, activated);

	GtkWidget * dialog = gtk_color_selection_dialog_new("Select color");
	GtkColorSelection * sel = GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(dialog) -> colorsel);
	gtk_color_selection_set_has_opacity_control(sel, TRUE);

	double r, g, b, a;
	styled_poi_set_get_colour(poi_set, &r, &g, &b, &a);
	GdkColor color;
	color.red = r * 65535;
	color.green = g * 65535;
	color.blue = b * 65535;
	guint16 alpha = a * 65535;

	gtk_color_selection_set_current_color(sel, &color);
	gtk_color_selection_set_current_alpha(sel, alpha);

	gint response = gtk_dialog_run(GTK_DIALOG(dialog));
	if (response != GTK_RESPONSE_OK){
		gtk_widget_destroy(dialog);
		return FALSE;
	}
	gtk_color_selection_get_current_color(GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(dialog) -> colorsel), &color);
	guint16 opacity = gtk_color_selection_get_current_alpha(GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(dialog) -> colorsel));
	gtk_widget_destroy(dialog);

	r = (double)color.red / 65535; 
	g = (double)color.green / 65535; 
	b = (double)color.blue / 65535; 
	a = (double)opacity / 65535; 

	poi_manager_set_poi_set_colour(poi_selector -> poi_manager, activated, r, g, b, a);
	return FALSE;
}

/****************************************************************************************************
* when the colour of a row changed
****************************************************************************************************/
static gboolean poi_selector_colour_changed_cb(PoiManager * poi_manager, int index, gpointer data)
{
	PoiSelector * poi_selector = GOSM_POI_SELECTOR(data);
	GtkTreeView * view = GTK_TREE_VIEW(poi_selector -> view);
	GtkTreeModel * model = gtk_tree_view_get_model(view);
	GtkTreeIter iter;

	GtkTreePath * path = gtk_tree_path_new_from_indices(index, -1);
	gtk_tree_model_get_iter(model, &iter, path);
	StyledPoiSet * poi_set = poi_manager_get_poi_set(poi_manager, index);
	double r, g, b, a;
	styled_poi_set_get_colour(poi_set, &r, &g, &b, &a);
	gtk_list_store_set(GTK_LIST_STORE(model), &iter,
				COL_R, r,
				COL_G, g,
				COL_B, b,
				COL_A, a,
				-1);
	return FALSE;
}

/****************************************************************************************************
* when a tag (key-value-pair) has been added
****************************************************************************************************/
static gboolean poi_selector_layer_added_cb(PoiManager * poi_manager, int index, gpointer data)
{
	PoiSelector * poi_selector = (PoiSelector*) data;
	GtkTreeView * view = GTK_TREE_VIEW(poi_selector -> view);
	GtkTreeModel * model = gtk_tree_view_get_model(view);
	GtkTreeIter iter;

	GtkTreePath * path = gtk_tree_path_new_from_indices(index, -1);
	gtk_tree_model_get_iter(model, &iter, path);

	StyledPoiSet * poi_set = poi_manager_get_poi_set(poi_manager, index);
	gtk_list_store_append (GTK_LIST_STORE(model), &iter);
	double r, g, b, a;
	styled_poi_set_get_colour(poi_set, &r, &g, &b, &a);
	gtk_list_store_set (GTK_LIST_STORE(model), &iter,
				COL_ACTIVE, poi_set_get_visible(GOSM_POI_SET(poi_set)),
				COL_KEY, named_poi_set_get_key(GOSM_NAMED_POI_SET(poi_set)),
				COL_VALUE, named_poi_set_get_value(GOSM_NAMED_POI_SET(poi_set)),
				COL_R, r, COL_G, g, COL_B, b, COL_A, a,
				-1);
	return FALSE;
}

/****************************************************************************************************
* when a tag has been removed
****************************************************************************************************/
static gboolean poi_selector_layer_deleted_cb(PoiManager * poi_manager, int index, gpointer data)
{
	PoiSelector * poi_selector = (PoiSelector*) data;
	GtkTreeView * view = GTK_TREE_VIEW(poi_selector -> view);
	GtkTreeModel * model = gtk_tree_view_get_model(view);
	GtkTreeIter iter;

	GtkTreePath * path = gtk_tree_path_new_from_indices(index, -1);
	gtk_tree_model_get_iter(model, &iter, path);

	gtk_list_store_remove (GTK_LIST_STORE(model), &iter);
	return FALSE;
}

/****************************************************************************************************
* when the visibility of a PoiSet has been changed
****************************************************************************************************/
static gboolean poi_selector_layer_toggled_cb(PoiManager * poi_manager, int index, gpointer data)
{
	PoiSelector * poi_selector = (PoiSelector*) data;
	GtkTreeView * view = GTK_TREE_VIEW(poi_selector -> view);
	GtkTreeModel * model = gtk_tree_view_get_model(view);
	GtkTreeIter iter;

	GtkTreePath * path = gtk_tree_path_new_from_indices(index, -1);
	gtk_tree_model_get_iter(model, &iter, path);
	StyledPoiSet * poi_set = poi_manager_get_poi_set(poi_manager, index);
	gtk_list_store_set(GTK_LIST_STORE(model), &iter,
				COL_ACTIVE, poi_set_get_visible(GOSM_POI_SET(poi_set)),
				-1);
	return FALSE;
}

/****************************************************************************************************
* create the treeview's model
****************************************************************************************************/
static GtkTreeModel * poi_selector_create_model (PoiManager * poi_manager)
{
	GtkListStore	*store;
	GtkTreeIter	  iter;
	
	store = gtk_list_store_new (NUM_COLS, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_STRING,
			 G_TYPE_DOUBLE, G_TYPE_DOUBLE, G_TYPE_DOUBLE, G_TYPE_DOUBLE);

	int num_poi_sets = poi_manager_get_number_of_poi_sets(poi_manager);
	int poi;
	for (poi = 0; poi < num_poi_sets; poi++){
		StyledPoiSet * poi_set = poi_manager_get_poi_set(poi_manager, poi);
		gtk_list_store_append (store, &iter);
		double r, g, b, a;
		styled_poi_set_get_colour(poi_set, &r, &g, &b, &a);
		gtk_list_store_set (store, &iter,
					COL_ACTIVE, poi_set_get_visible(GOSM_POI_SET(poi_set)),
					COL_KEY, named_poi_set_get_key(GOSM_NAMED_POI_SET(poi_set)),
					COL_VALUE, named_poi_set_get_value(GOSM_NAMED_POI_SET(poi_set)),
					COL_R, r, COL_G, g, COL_B, b, COL_A, a,
					-1);
	}

	return GTK_TREE_MODEL (store);
}

/****************************************************************************************************
* create the treeview
****************************************************************************************************/
static GtkWidget * poi_selector_create_view (PoiSelector * poi_selector)
{
	GtkCellRenderer     *renderer;
	GtkTreeModel        *model;
	GtkWidget           *view;

	PoiManager * poi_manager = poi_selector -> poi_manager;
	view = gtk_tree_view_new ();
	renderer = gtk_cell_renderer_toggle_new ();
	g_signal_connect(G_OBJECT(renderer), "toggled", G_CALLBACK(poi_selector_check_cb), poi_selector);
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
	                                             -1,
	                                             "",
	                                             renderer,
	                                             "active", COL_ACTIVE,
	                                             NULL);
	renderer = cell_renderer_colour_new();
	g_signal_connect(G_OBJECT(renderer), "edit-colour", G_CALLBACK(poi_selector_color_button_cb), poi_selector);
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
	                                             -1,
	                                             "",
	                                             renderer,
	                                             "r", COL_R,
	                                             "g", COL_G,
	                                             "b", COL_B,
	                                             "a", COL_A,
	                                             NULL);
	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
	                                             -1,
	                                             "Key",
	                                             renderer,
	                                             "text", COL_KEY,
	                                             NULL);
	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
	                                             -1,
	                                             "Value",
	                                             renderer,
	                                             "text", COL_VALUE,
	                                             NULL);
	model = poi_selector_create_model (poi_manager);
	gtk_tree_view_set_model (GTK_TREE_VIEW (view), model);
	g_object_unref (model);
	return view;
}

/****************************************************************************************************
* this function returns the currently selected rows' index, or -1 if none is selected
****************************************************************************************************/
int poi_selector_get_active_item_index(PoiSelector * poi_selector)
{
	GtkTreeView * view = GTK_TREE_VIEW(poi_selector -> view);
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
