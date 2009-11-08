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

#include "bookmark_widget.h"
#include "bookmark_location.h"

G_DEFINE_TYPE (BookmarkWidget, bookmark_widget, GTK_TYPE_VBOX);

/****************************************************************************************************
* the columns of the treeview
****************************************************************************************************/
enum
{	
	COL_BOOKMARK = 0,
	COL_NAME,
	NUM_COLS
} ;

static GtkTreeModel * bookmarks_widget_create_model (BookmarkManager * bookmark_manager);
static GtkWidget * bookmarks_widget_create_view ();
static gboolean bookmark_widget_view_cb (GtkTreeView * treeview,
				GtkTreePath * path,
				GtkTreeViewColumn * col,
				gpointer bookmark_widget_p);
static gboolean bookmark_widget_bookmark_added_cb(BookmarkManager * bookmark_manager,
				gpointer bookmark_p,
				gpointer bookmark_widget_p);
static gboolean bookmark_widget_bookmark_removed_cb(BookmarkManager * bookmark_manager,
				int index,
				gpointer bookmark_widget_p);
static gboolean bookmark_widget_bookmark_moved_cb(BookmarkManager * bookmark_manager,
				gpointer indices,
				gpointer bookmark_widget_p);
static gboolean bookmark_widget_remove_cb(GtkWidget * button,
				BookmarkWidget * bookmark_widget);
static gboolean bookmark_widget_save_cb(GtkWidget * button,
				BookmarkManager * bookmark_manager);
static void bookmark_widget_drag_data_received(
	GtkWidget * widget,
	GdkDragContext * context,
	gint x,
	gint y,
	GtkSelectionData * selection_data,
	guint info,
	guint time,
	BookmarkWidget * bookmark_widget);
static void bookmark_widget_drag_data_get(
	GtkWidget * widget,
	GdkDragContext * context,
	GtkSelectionData * selection_data,
	guint info,
	guint time);

GtkWidget * bookmark_widget_new(BookmarkManager * bookmark_manager, MapArea * map_area)
{
	BookmarkWidget * bookmark_widget = g_object_new(GOSM_TYPE_BOOKMARK_WIDGET, NULL);
	bookmark_widget -> bookmark_manager = bookmark_manager;
	bookmark_widget -> map_area = map_area;

	GtkWidget * toolbar = gtk_hbox_new(FALSE, 0);
	GtkWidget * button_add = gtk_button_new();
	GtkWidget * button_delete = gtk_button_new();
	GtkWidget * button_save = gtk_button_new();
	GtkWidget * button_revert = gtk_button_new();
	GtkWidget * icon_add = gtk_image_new_from_stock("gtk-add", GTK_ICON_SIZE_MENU);
	GtkWidget * icon_delete = gtk_image_new_from_stock("gtk-remove", GTK_ICON_SIZE_MENU);
	GtkWidget * icon_save = gtk_image_new_from_stock("gtk-save", GTK_ICON_SIZE_MENU);
	GtkWidget * icon_revert = gtk_image_new_from_stock("gtk-revert-to-saved", GTK_ICON_SIZE_MENU);
	gtk_button_set_image(GTK_BUTTON(button_add), icon_add);
	gtk_button_set_image(GTK_BUTTON(button_delete), icon_delete);
	gtk_button_set_image(GTK_BUTTON(button_save), icon_save);
	gtk_button_set_image(GTK_BUTTON(button_revert), icon_revert);
	gtk_box_pack_start(GTK_BOX(toolbar), button_add, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(toolbar), button_delete, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(toolbar), button_save, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(toolbar), button_revert, FALSE, FALSE, 0);
	gtk_widget_set_tooltip_text(button_add, "add bookmark");
	gtk_widget_set_tooltip_text(button_delete, "remove bookmark");
	gtk_widget_set_tooltip_text(button_save, "save bookmarks to file");
	gtk_widget_set_tooltip_text(button_revert, "revert to saved state");

	bookmark_widget -> view = bookmarks_widget_create_view();
	GtkTreeModel * model = bookmarks_widget_create_model(bookmark_manager);
	gtk_tree_view_set_model (GTK_TREE_VIEW (bookmark_widget -> view), model);
	g_object_unref (model);

	GtkTargetEntry * target = malloc(sizeof(GtkTargetEntry));
	target -> target = "foo-test";
	target -> flags = GTK_TARGET_SAME_WIDGET;
	target -> info = 23;
	gtk_tree_view_enable_model_drag_source(
		GTK_TREE_VIEW(bookmark_widget -> view), 
		GDK_BUTTON1_MASK, target, 1, GDK_ACTION_MOVE);
	gtk_tree_view_enable_model_drag_dest(
		GTK_TREE_VIEW(bookmark_widget -> view), 
		target, 1, GDK_ACTION_MOVE);

        GtkWidget * scrolled = gtk_scrolled_window_new(NULL, NULL);
        gtk_container_add(GTK_CONTAINER(scrolled), bookmark_widget -> view);
        gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled),
                                        GTK_POLICY_AUTOMATIC,
                                        GTK_POLICY_AUTOMATIC);

	gtk_box_pack_start(GTK_BOX(bookmark_widget), toolbar, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(bookmark_widget), scrolled, TRUE, TRUE, 0);

	g_signal_connect(
		G_OBJECT(bookmark_widget -> view), "row-activated", 
		G_CALLBACK(bookmark_widget_view_cb), (gpointer) bookmark_widget);
	g_signal_connect(
		G_OBJECT(bookmark_widget -> view), "drag-data-get", 
		G_CALLBACK(bookmark_widget_drag_data_get), (gpointer) bookmark_widget);
	g_signal_connect(
		G_OBJECT(bookmark_widget -> view), "drag-data-received", 
		G_CALLBACK(bookmark_widget_drag_data_received), (gpointer) bookmark_widget);
	g_signal_connect(
		G_OBJECT(bookmark_manager), "bookmark-location-added", 
		G_CALLBACK(bookmark_widget_bookmark_added_cb), (gpointer) bookmark_widget);
	g_signal_connect(
		G_OBJECT(bookmark_manager), "bookmark-location-removed", 
		G_CALLBACK(bookmark_widget_bookmark_removed_cb), (gpointer) bookmark_widget);
	g_signal_connect(
		G_OBJECT(bookmark_manager), "bookmark-location-moved", 
		G_CALLBACK(bookmark_widget_bookmark_moved_cb), (gpointer) bookmark_widget);
	g_signal_connect(
		G_OBJECT(button_delete), "clicked", 
		G_CALLBACK(bookmark_widget_remove_cb), (gpointer) bookmark_widget);
	g_signal_connect(
		G_OBJECT(button_save), "clicked", 
		G_CALLBACK(bookmark_widget_save_cb), (gpointer) bookmark_manager);
	return GTK_WIDGET(bookmark_widget);
}

static void bookmark_widget_class_init(BookmarkWidgetClass *class)
{
}

static void bookmark_widget_init(BookmarkWidget *bookmark_widget)
{
}

/****************************************************************************************************
* create the treeview
****************************************************************************************************/
static GtkWidget * bookmarks_widget_create_view ()
{
	GtkCellRenderer * renderer;
	GtkWidget * view = gtk_tree_view_new ();

	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
	                                             -1,      
	                                             "Name",  
	                                             renderer,
	                                             "text", COL_NAME,
	                                             NULL);
	return view;
}

/****************************************************************************************************
* create the treeview's model
****************************************************************************************************/
static GtkTreeModel * bookmarks_widget_create_model (BookmarkManager * bookmark_manager)
{
	GtkListStore * store = gtk_list_store_new (NUM_COLS, GOSM_TYPE_BOOKMARK, G_TYPE_STRING);
	GtkTreeIter iter;
	int i;
	for (i = 0; i < bookmark_manager -> bookmarks_location -> len; i++){
		BookmarkLocation * bookmark = 
			GOSM_BOOKMARK_LOCATION(g_array_index(bookmark_manager -> bookmarks_location, Bookmark*, i));
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter,
					  COL_BOOKMARK, bookmark,
					  COL_NAME, bookmark -> name,
					  -1);
	}
	return GTK_TREE_MODEL (store);
}

static gboolean bookmark_widget_view_cb (GtkTreeView * treeview,
				GtkTreePath * path,
				GtkTreeViewColumn * col,
				gpointer bookmark_widget_p)
{
	BookmarkWidget * bookmark_widget = GOSM_BOOKMARK_WIDGET(bookmark_widget_p);
	GtkTreeModel * model = gtk_tree_view_get_model(treeview);
	GtkTreeIter iter;
	if (gtk_tree_model_get_iter(model, &iter, path))
	{
		GValue value = {0};
		gtk_tree_model_get_value(model, &iter, COL_BOOKMARK, &value);
		BookmarkLocation * bookmark = g_value_get_object(&value);
		map_area_goto_lon_lat_zoom(bookmark_widget -> map_area,
			bookmark -> lon,
			bookmark -> lat,
			bookmark -> zoom);
		map_area_repaint(bookmark_widget -> map_area);
	}
	return FALSE;
}

static gboolean bookmark_widget_bookmark_added_cb(BookmarkManager * bookmark_manager,
				gpointer bookmark_p,
				gpointer bookmark_widget_p)
{
	Bookmark * bookmark = GOSM_BOOKMARK(bookmark_p);
	BookmarkWidget * bookmark_widget = GOSM_BOOKMARK_WIDGET(bookmark_widget_p);

	GtkTreeView * view = GTK_TREE_VIEW(bookmark_widget -> view);
	GtkTreeModel * model = gtk_tree_view_get_model(view);
	GtkTreeIter iter;
	gtk_list_store_append (GTK_LIST_STORE(model), &iter);
	gtk_list_store_set (GTK_LIST_STORE(model), &iter,
				COL_NAME, GOSM_BOOKMARK_LOCATION(bookmark) -> name,
				COL_BOOKMARK, bookmark,
				-1);
	return FALSE;
}

static gboolean bookmark_widget_bookmark_removed_cb(BookmarkManager * bookmark_manager,
				int index,
				gpointer bookmark_widget_p)
{
	BookmarkWidget * bookmark_widget = GOSM_BOOKMARK_WIDGET(bookmark_widget_p);
	GtkTreeView * view = GTK_TREE_VIEW(bookmark_widget -> view);
	GtkTreeModel * model = gtk_tree_view_get_model(view);
	GtkTreeIter iter;

	GtkTreePath * path = gtk_tree_path_new_from_indices(index, -1);
	gtk_tree_model_get_iter(model, &iter, path);
	gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
	gtk_tree_path_free(path);
	return FALSE;
}

static gboolean bookmark_widget_bookmark_moved_cb(BookmarkManager * bookmark_manager,
				gpointer indices,
				gpointer bookmark_widget_p)
{
	int * ind = (int*) indices;
	int pos_old = ind[0];
	int pos_new = ind[1];
	if (pos_new > pos_old) pos_new--;

	BookmarkWidget * bookmark_widget = GOSM_BOOKMARK_WIDGET(bookmark_widget_p);
	GtkTreeView * view = GTK_TREE_VIEW(bookmark_widget -> view);
	GtkTreeModel * model = gtk_tree_view_get_model(view);
	GtkTreeIter iter1, iter2;

	GtkTreePath * path1 = gtk_tree_path_new_from_indices(pos_old, -1);
	gtk_tree_model_get_iter(model, &iter1, path1);
	gtk_tree_path_free(path1);
	GtkTreePath * path2 = gtk_tree_path_new_from_indices(pos_new, -1);
	gtk_tree_model_get_iter(model, &iter2, path2);
	gtk_tree_path_free(path2);
	if (pos_new > pos_old){
		gtk_list_store_move_after(GTK_LIST_STORE(model), &iter1, &iter2);
	}else{
		gtk_list_store_move_before(GTK_LIST_STORE(model), &iter1, &iter2);
	}
}

static gboolean bookmark_widget_save_cb(GtkWidget * button,
				BookmarkManager * bookmark_manager)
{
	bookmark_manager_save(bookmark_manager);
	return FALSE;
}

static gboolean bookmark_widget_remove_cb(GtkWidget * button,
				BookmarkWidget * bookmark_widget)
{
	GtkTreeView * view = GTK_TREE_VIEW(bookmark_widget -> view);
	GtkTreePath * path;
	GtkTreeViewColumn * cols;
	gtk_tree_view_get_cursor(view, &path, &cols);
	if (path != NULL){
		int * indices = gtk_tree_path_get_indices(path);
		int index = indices[0];
		gtk_tree_path_free(path);
		bookmark_manager_remove_bookmark_location(bookmark_widget -> bookmark_manager, index);
	}
	return FALSE;
}

static void bookmark_widget_drag_data_get(
	GtkWidget * widget,
	GdkDragContext * context,
	GtkSelectionData * selection_data,
	guint info,
	guint time)
{
	GtkTreeView * view = GTK_TREE_VIEW(widget);
	GtkTreePath * path;
	gtk_tree_view_get_cursor(view, &path, NULL);
	int * indices = gtk_tree_path_get_indices(path);
	int index = indices[0];
	int * int_data = malloc(sizeof(int));
	*int_data = index;
	gtk_selection_data_set(selection_data, GDK_SELECTION_TYPE_INTEGER, 32, (guchar*)int_data, 32);
}

static void bookmark_widget_drag_data_received(
	GtkWidget * widget,
	GdkDragContext * context,
	gint x,
	gint y,
	GtkSelectionData * selection_data,
	guint info,
	guint time,
	BookmarkWidget * bookmark_widget)
{
	guchar * data = gtk_selection_data_get_data(selection_data);
	int old_pos = *(int*)data;
	GtkTreePath * path;
	GtkTreeViewDropPosition pos;
	gtk_tree_view_get_dest_row_at_pos(GTK_TREE_VIEW(widget), x, y, &path, &pos);
	int index = -1;
	if (path == NULL){
		GtkTreeModel * model = gtk_tree_view_get_model(GTK_TREE_VIEW(widget));
		index = gtk_tree_model_iter_n_children(model, NULL);
	}else{
		int * indices = gtk_tree_path_get_indices(path);
		index = indices[0];
		if (pos == GTK_TREE_VIEW_DROP_AFTER || pos == GTK_TREE_VIEW_DROP_INTO_OR_AFTER){
			index ++;
		}
	}
	if (old_pos != index && old_pos + 1 != index){
		bookmark_manager_move_bookmark_location(bookmark_widget -> bookmark_manager, old_pos, index);
	}
}

