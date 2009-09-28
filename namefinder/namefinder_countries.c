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

// TODO: lat / lon should not be used for positioning
// instead, the bounding box should be used to center on screen

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

#include "namefinder_countries.h"

G_DEFINE_TYPE (NamefinderCountries, namefinder_countries, GTK_TYPE_VBOX);

enum
{
        CITY_ACTIVATED,
        LAST_SIGNAL
};

static guint namefinder_countries_signals[LAST_SIGNAL] = { 0 };

enum
{
	COL_NAME = 0,
	NUM_COLS
} ;

static GtkTreeModel * namefinder_countries_create_model (country * countries, int num);
static GtkWidget * namefinder_countries_create_view (country * countries, int num);
static int cmpstringp(const void *p1, const void *p2);
void namefinder_countries_view_cb (GtkTreeView	      *treeview,
		    GtkTreePath        *path,
		    GtkTreeViewColumn  *col,
		    gpointer            namefinder_countries);

GtkWidget * namefinder_countries_new()
{
	NamefinderCountries * namefinder_countries = g_object_new(GOSM_TYPE_NAMEFINDER_COUNTRIES, NULL);
	return GTK_WIDGET(namefinder_countries);
}

static void namefinder_countries_class_init(NamefinderCountriesClass *class)
{
        namefinder_countries_signals[CITY_ACTIVATED] = g_signal_new(
                "country-activated",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (NamefinderCountriesClass, country_activated),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
}

static void namefinder_countries_init(NamefinderCountries *namefinder_countries)
{
	char * filename = GOSM_NAMEFINDER_DIR "countries.txt";
	
	struct stat sb;
	int s = stat(filename, &sb);
	int size = sb.st_size;

	char buf[size + 1];
	int fd = open(filename, O_RDONLY);
	int r = read(fd, buf, size);
	close(fd);
	buf[size] = '\0';

	gchar ** splitted = g_strsplit(buf, "\n", 250);
	int l = g_strv_length(splitted);

	namefinder_countries -> countries = malloc(sizeof(country) * (l-1));
	country * countries = namefinder_countries -> countries;

	int c;
	for (c = 0; c < l - 1; c++){
		gchar ** split2 = g_strsplit(splitted[c], "\t", 5);
		double lon_1 = strtodouble(split2[1]);
		double lat_1 = strtodouble(split2[2]);
		double lon_2 = strtodouble(split2[3]);
		double lat_2 = strtodouble(split2[4]);
		double lon1_off = 180 + lon_1;
		double lon2_off = 180 + lon_2;
		double lon_mid_off = (lon1_off + lon2_off) / 2;
		double lon_mid = lon1_off < lon2_off ? lon_mid_off - 180 : lon_mid_off;
		namefinder_countries -> countries[c].name = malloc(sizeof(char) * (strlen(split2[0]) + 1));
		strcpy(namefinder_countries -> countries[c].name, split2[0]);
		countries[c].lon = lon_mid;
		countries[c].lat = (lat_1 + lat_2) / 2;
		countries[c].lon1 = lon_1;
		countries[c].lon2 = lon_2;
		countries[c].lat1 = lat_1;
		countries[c].lat2 = lat_2;
	}
	qsort(namefinder_countries -> countries, l-1, sizeof(country), cmpstringp);
	//for (c = 0; c < l-1; c++){
		//printf("%s, %f, %f\n", countries[c].name, countries[c].lon, countries[c].lat);
	//}

	GtkWidget *view;
	view = namefinder_countries_create_view (namefinder_countries -> countries, l-1);
	g_signal_connect(view, "row-activated", (GCallback) namefinder_countries_view_cb, namefinder_countries);
	GtkWidget * scrolled = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolled), view);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled),
	                                GTK_POLICY_AUTOMATIC,
	                                GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(scrolled, 180, 200);
	gtk_container_add (GTK_CONTAINER (namefinder_countries), scrolled);
}

int namefinder_countries_get_activated_id(NamefinderCountries * namefinder_countries)
{
	return namefinder_countries -> activated;
}

gpointer namefinder_countries_get_countries(NamefinderCountries * namefinder_countries)
{
	return namefinder_countries -> countries;
}

static GtkTreeModel * namefinder_countries_create_model (country * countries, int num)
{
	GtkListStore	*store;
	GtkTreeIter	  iter;
	
	store = gtk_list_store_new (NUM_COLS, G_TYPE_STRING, G_TYPE_UINT);

	int c;
	for (c = 0; c < num; c++){
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter,
					  COL_NAME, countries[c].name,
					  -1);
	}

	return GTK_TREE_MODEL (store);
}

static GtkWidget * namefinder_countries_create_view (country * countries, int num)
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
	model = namefinder_countries_create_model (countries, num);
	gtk_tree_view_set_model (GTK_TREE_VIEW (view), model);
	g_object_unref (model);
	return view;
}

static int cmpstringp(const void *p1, const void *p2)
{
	 return strcmp(* (char * const *) p1, * (char * const *) p2);
}

void namefinder_countries_view_cb (GtkTreeView	      *treeview,
		    GtkTreePath        *path,
		    GtkTreeViewColumn  *col,
		    gpointer            namefinder_countries)
{
	GtkTreeModel *model;
	GtkTreeIter	 iter;

	model = gtk_tree_view_get_model(treeview);

	if (gtk_tree_model_get_iter(model, &iter, path))
	{
		gint * indices = gtk_tree_path_get_indices(path); 
		GOSM_NAMEFINDER_COUNTRIES(namefinder_countries) -> activated = indices[0];
		g_signal_emit (GOSM_NAMEFINDER_COUNTRIES(namefinder_countries), namefinder_countries_signals[CITY_ACTIVATED], 0);
	}
}
