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

#include "poi_statistics.h"
#include "poi_manager.h"

/****************************************************************************************************
* let the user inspect what tags are present in the currently loaded PoiSource
****************************************************************************************************/
G_DEFINE_TYPE (PoiStatistics, poi_statistics, G_TYPE_OBJECT);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint poi_statistics_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, poi_statistics_signals[SIGNAL_NAME_n], 0);

/****************************************************************************************************
* constructor
****************************************************************************************************/
PoiStatistics * poi_statistics_new()
{
	PoiStatistics * poi_statistics = g_object_new(GOSM_TYPE_POI_STATISTICS, NULL);
	poi_statistics -> key_value = g_array_new(FALSE, FALSE, sizeof(KeyValueCount));
	return poi_statistics;
}

static void poi_statistics_class_init(PoiStatisticsClass *class)
{
        /*poi_statistics_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PoiStatisticsClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void poi_statistics_init(PoiStatistics *poi_statistics)
{
}

/****************************************************************************************************
* iterator function for the key-value-tree on level 2(values), add tag and count to resultset
****************************************************************************************************/
gboolean tree_inspect2 (gpointer k, gpointer v, gpointer data)
{
	PoiStatistics * poi_stats = (PoiStatistics*) data;
	char * val = (char*)k;
	GSequence * elements = (GSequence*)v;
	KeyValueCount kvc = {poi_stats -> current_key, val, g_sequence_get_length(elements)};
	g_array_append_val(poi_stats -> key_value, kvc);
	return FALSE;
}

/****************************************************************************************************
* iterator function for the key-value-tree on level 1(keys), iterate values
****************************************************************************************************/
gboolean tree_inspect1 (gpointer k, gpointer v, gpointer data)
{
	PoiStatistics * poi_stats = (PoiStatistics*) data;
	poi_stats -> current_key = (char*)k;
	g_tree_foreach((GTree*)v, tree_inspect2, data);
	return FALSE;
}

/****************************************************************************************************
* comparism function for sorting tags by number of occurences
****************************************************************************************************/
int key_val_count_compare(gconstpointer a, gconstpointer b)
{
	KeyValueCount * c1 = (KeyValueCount*)a;
	KeyValueCount * c2 = (KeyValueCount*)b;
	return c2 -> count - c1 -> count;
}

/****************************************************************************************************
* analyse the currently loaded pois
* inspect all tags and save each tag associated with the number of occurences
****************************************************************************************************/
void poi_statistics_analyze(PoiStatistics * poi_statistics, PoiManager * poi_manager)
{
	g_tree_foreach(poi_manager -> tag_tree -> tree, tree_inspect1, (gpointer)poi_statistics);
	g_array_sort(poi_statistics -> key_value, key_val_count_compare);
	printf("size of key-val-list: %d\n", poi_statistics -> key_value -> len);
//	int i;
//	for (i = 0; i < poi_statistics -> key_value -> len; i++){
//		KeyValueCount * kvc = &g_array_index(poi_statistics -> key_value, KeyValueCount, i);
//		printf("%d %s::%s\n", kvc -> count, kvc -> key, kvc -> val);
//		if (i == 100) break;
//	}
}

/****************************************************************************************************
* columns for presentation of tags + number of occurences
****************************************************************************************************/
enum {
	COL_KEY = 0,
	COL_VAL,
	COL_COUNT,
	NUM_COLS
};

/****************************************************************************************************
* show a window; treeview showing tags + number of occurences
****************************************************************************************************/
void poi_statistics_show_window(PoiStatistics * poi_statistics)
{
	GtkWidget * window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	/* treeview */
	GtkWidget * view = gtk_tree_view_new ();
	GtkCellRenderer * renderer;
	renderer = gtk_cell_renderer_text_new ();
	g_object_set(renderer, "width-chars", 20, NULL);
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
							-1, "key", renderer,
							"text", COL_KEY, NULL);
	renderer = gtk_cell_renderer_text_new ();
	g_object_set(renderer, "width-chars", 30, NULL);
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
							-1, "value", renderer,
							"text", COL_VAL, NULL);
	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
							-1, "count", renderer,
							"text", COL_COUNT, NULL);
	GtkListStore * store = gtk_list_store_new (NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	GtkTreeIter iter;
	int i;
	for (i = 0; i < poi_statistics -> key_value -> len; i++){
		KeyValueCount * kvc = &g_array_index(poi_statistics -> key_value, KeyValueCount, i);
		char buf[10];
		sprintf(buf, "%d", kvc -> count);
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter,
					COL_KEY, kvc -> key,
					COL_VAL, kvc -> val,
					COL_COUNT, buf,
					-1);
		if (kvc -> count == 1) break;
	}
	gtk_tree_view_set_model (GTK_TREE_VIEW (view), GTK_TREE_MODEL(store));
	g_object_unref (store);
	/* col resizablitiy */
	int n;
	for (n = 0; n < COL_COUNT; n++){
		GtkTreeViewColumn * col = gtk_tree_view_get_column(GTK_TREE_VIEW(view), n);
		gtk_tree_view_column_set_resizable(col, TRUE);
		//if (n == 1) gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_AUTOSIZE);
	}
	/* scroll-area */
	GtkWidget * scrolled = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolled), view);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled),
	                                GTK_POLICY_AUTOMATIC,
	                                GTK_POLICY_AUTOMATIC);
	/* show window */
	gtk_container_add(GTK_CONTAINER(window), scrolled);
	gtk_widget_set_size_request(window, 400, 300);
	gtk_widget_show_all(window);
}
