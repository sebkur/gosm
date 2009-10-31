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

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <glib.h>
#include <string.h>

#include "../paths.h"
#include "../customio.h"

#include "namefinder_cities.h"

G_DEFINE_TYPE (NamefinderCities, namefinder_cities, GTK_TYPE_VBOX);

/****************************************************************************************************
* signals
****************************************************************************************************/
enum
{
        CITY_ACTIVATED,
        LAST_SIGNAL
};

static guint namefinder_cities_signals[LAST_SIGNAL] = { 0 };

/****************************************************************************************************
* the columns of the treeview
****************************************************************************************************/
enum
{
	COL_NAME = 0,
	NUM_COLS
} ;

/****************************************************************************************************
* method declarations
****************************************************************************************************/
static GtkTreeModel * namefinder_cities_create_model (city * cities, int num);
static GtkWidget * namefinder_cities_create_view (city * cities, int num);
static int cmpstringp(const void *p1, const void *p2);
void namefinder_cities_view_cb (GtkTreeView	      *treeview,
		    GtkTreePath        *path,
		    GtkTreeViewColumn  *col,
		    gpointer            namefinder_cities);

/****************************************************************************************************
* constructor
****************************************************************************************************/
GtkWidget * namefinder_cities_new()
{
	NamefinderCities * namefinder_cities = g_object_new(GOSM_TYPE_NAMEFINDER_CITIES, NULL);
	return GTK_WIDGET(namefinder_cities);
}

/****************************************************************************************************
* class init
****************************************************************************************************/
static void namefinder_cities_class_init(NamefinderCitiesClass *class)
{
        namefinder_cities_signals[CITY_ACTIVATED] = g_signal_new(
                "city-activated",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (NamefinderCitiesClass, city_activated),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
}

/****************************************************************************************************
* object init
****************************************************************************************************/
static void namefinder_cities_init(NamefinderCities *namefinder_cities)
{
	char * filename = GOSM_NAMEFINDER_DIR "res/cities15000.crop.txt";
	
	struct stat sb;
	int s = stat(filename, &sb);
	int size = sb.st_size;

	char buf[size + 1];
	int fd = open(filename, O_RDONLY);
	int r = read(fd, buf, size);
	close(fd);
	buf[size] = '\0';

	gchar ** splitted = g_strsplit(buf, "\n", 21500);
	int l = g_strv_length(splitted);

	namefinder_cities -> cities = malloc(sizeof(city) * (l-1));

	int c;
	for (c = 0; c < l - 1; c++){
		gchar ** split2 = g_strsplit(splitted[c], "\t", 3);
		//printf("%s a %s b %s\n", split2[0], split2[1], split2[2]);
		double lat = strtodouble(split2[1]);
		double lon = strtodouble(split2[2]);
		namefinder_cities -> cities[c].name = malloc(sizeof(char) * (strlen(split2[0]) + 1));
		strcpy(namefinder_cities -> cities[c].name, split2[0]);
		namefinder_cities -> cities[c].lon = lon;
		namefinder_cities -> cities[c].lat = lat;
		g_strfreev(split2);
	}
	g_strfreev(splitted);
	qsort(namefinder_cities -> cities, l-1, sizeof(city), cmpstringp);
	//for (c = 0; c < l-1; c++){
		//printf("%s, %f, %f\n", cities[c].name, cities[c].lon, cities[c].lat);
	//}

	GtkWidget *view;
	view = namefinder_cities_create_view (namefinder_cities -> cities, l-1);
	g_signal_connect(view, "row-activated", (GCallback) namefinder_cities_view_cb, namefinder_cities);
	GtkWidget * scrolled = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolled), view);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled),
	                                GTK_POLICY_AUTOMATIC,
	                                GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(scrolled, 180, 200);
	gtk_container_add (GTK_CONTAINER (namefinder_cities), scrolled);
}

/****************************************************************************************************
* return the currently activated item's index
****************************************************************************************************/
int namefinder_cities_get_activated_id(NamefinderCities * namefinder_cities)
{
	return namefinder_cities -> activated;
}

/****************************************************************************************************
* return the array of city-structs
****************************************************************************************************/
gpointer namefinder_cities_get_cities(NamefinderCities * namefinder_cities)
{
	return namefinder_cities -> cities;
}

/****************************************************************************************************
* create the treeview's model
****************************************************************************************************/
static GtkTreeModel * namefinder_cities_create_model (city * cities, int num)
{
	GtkListStore	*store;
	GtkTreeIter	  iter;
	
	store = gtk_list_store_new (NUM_COLS, G_TYPE_STRING);

	int c;
	for (c = 0; c < num; c++){
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter,
					  COL_NAME, cities[c].name,
					  -1);
	}

	return GTK_TREE_MODEL (store);
}

/****************************************************************************************************
* create the treeview
****************************************************************************************************/
static GtkWidget * namefinder_cities_create_view (city * cities, int num)
{
	GtkCellRenderer     *renderer;
	GtkTreeModel        *model;
	GtkWidget           *view;

	view = gtk_tree_view_new ();
	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
	                                             -1,      
	                                             "Name",  
	                                             renderer,
	                                             "text", COL_NAME,
	                                             NULL);
	model = namefinder_cities_create_model (cities, num);
	gtk_tree_view_set_model (GTK_TREE_VIEW (view), model);
	g_object_unref (model);
	return view;
}

/****************************************************************************************************
* function used to sort the cities
****************************************************************************************************/
static int cmpstringp(const void *p1, const void *p2)
{
	 return strcmp(* (char * const *) p1, * (char * const *) p2);
}

/****************************************************************************************************
* when an entry has been activated
****************************************************************************************************/
void namefinder_cities_view_cb (GtkTreeView	      *treeview,
		    GtkTreePath        *path,
		    GtkTreeViewColumn  *col,
		    gpointer            namefinder_cities)
{
	GtkTreeModel *model;
	GtkTreeIter iter;

	model = gtk_tree_view_get_model(treeview);

	if (gtk_tree_model_get_iter(model, &iter, path))
	{
		gint * indices = gtk_tree_path_get_indices(path); 
		GOSM_NAMEFINDER_CITIES(namefinder_cities) -> activated = indices[0];
		g_signal_emit (GOSM_NAMEFINDER_CITIES(namefinder_cities), namefinder_cities_signals[CITY_ACTIVATED], 0);
	}
}
