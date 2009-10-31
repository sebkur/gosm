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

#include "poi_set.h"

#include "RTree/index.h"
#include "../customio.h"
#include "../map_types.h"

/****************************************************************************************************
* a PoiSet is a set of points (lon/lat) that are stored in an RTree for efficient retrieval of points
* that are contained within an given area
****************************************************************************************************/
G_DEFINE_TYPE (PoiSet, poi_set, G_TYPE_OBJECT);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint poi_set_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, poi_set_signals[SIGNAL_NAME_n], 0);


/****************************************************************************************************
* functions for the binary tree
****************************************************************************************************/
gint poi_set_compare_ints(gconstpointer a, gconstpointer b, gpointer user_data)
{
        return *(int*)a - *(int*)b;
}
void destroy_just_free(gpointer data)
{
	free(data);
}

/****************************************************************************************************
* create a new PoiSet
****************************************************************************************************/
PoiSet * poi_set_new()
{
	PoiSet * poi_set = g_object_new(GOSM_TYPE_POI_SET, NULL);
	poi_set_constructor(poi_set);
	poi_set -> visible = FALSE;
	return poi_set;
}

/****************************************************************************************************
* constructor is an own function so that it can be called from *_new and from *_clear
****************************************************************************************************/
void poi_set_constructor(PoiSet * poi_set)
{
	poi_set -> root = RTreeNewIndex();
	poi_set -> points = g_tree_new_full(poi_set_compare_ints, NULL, destroy_just_free, destroy_just_free);
}

/****************************************************************************************************
* remove all points from PoiSet and call constructor, so that it can be used as if it was just
* created
****************************************************************************************************/
void poi_set_clear(PoiSet * poi_set)
{
	RTreeClear(poi_set -> root);
	// TODO: free of tree
	poi_set_constructor(poi_set);
}

static void poi_set_class_init(PoiSetClass *class)
{
        /*poi_set_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PoiSetClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void poi_set_init(PoiSet *poi_set)
{
}

/****************************************************************************************************
* add a point to the PoiSet
****************************************************************************************************/
void poi_set_add(PoiSet * poi_set, double lon, double lat, int id)
{
	/* a node might be added, that is already present */
	// TODO: this should be obsolete now
	if (g_tree_lookup(poi_set -> points, &id) == NULL){
		LonLatPair * llp = malloc(sizeof(LonLatPair));
		llp -> lon = lon;
		llp -> lat = lat;
		/* insert into binary tree */
		int * id_p = malloc(sizeof(int));
		*id_p = id;
		g_tree_insert(poi_set -> points, id_p, llp);
		/* insert into rtree */
		struct Rect * rect = malloc(sizeof(struct Rect));
		rect -> boundary[0] = lon;
		rect -> boundary[1] = lat;
		rect -> boundary[2] = lon;
		rect -> boundary[3] = lat;
		RTreeInsertRect(rect, id, &(poi_set -> root), 0);
		free(rect);
	}
}

/****************************************************************************************************
* used as a callback for searching the RTree.
* this function adds found nodes to the resultset
****************************************************************************************************/
int poi_set_search_cb(int node_id, void* arg)
{
	PoiSet * poi_set = (PoiSet*) arg;
	LonLatPair * llp = g_tree_lookup(poi_set -> points, &node_id);
	LonLatPairId llpi = {llp -> lon, llp -> lat, node_id};
	g_array_append_val(poi_set -> results, llpi);
}

/****************************************************************************************************
* removes the points found in the given area
****************************************************************************************************/
void poi_set_clear_area(PoiSet * poi_set, double min_lon, double min_lat, double max_lon, double max_lat)
{
	GArray * points = poi_set_get(poi_set, min_lon, min_lat, max_lon, max_lat);
	int i;
	for (i = 0; i < points -> len; i++){
		LonLatPairId llpi = g_array_index(points, LonLatPairId, i);
		/* remove from rtree */
		struct Rect * rect = malloc(sizeof(struct Rect));
		rect -> boundary[0] = llpi.lon;
		rect -> boundary[1] = llpi.lat;
		rect -> boundary[2] = llpi.lon;
		rect -> boundary[3] = llpi.lat;
		RTreeDeleteRect(rect, llpi.node_id, &(poi_set -> root));
		free(rect);
		/* remove from binary tree */
		g_tree_remove(poi_set -> points, &llpi.node_id);
	}
	g_array_free(points, TRUE);
}

/****************************************************************************************************
* remove a single point
****************************************************************************************************/
void poi_set_remove_point(PoiSet * poi_set, double lon, double lat, int node_id)
{
		struct Rect * rect = malloc(sizeof(struct Rect));
		rect -> boundary[0] = lon;
		rect -> boundary[1] = lat;
		rect -> boundary[2] = lon;
		rect -> boundary[3] = lat;
		RTreeDeleteRect(rect, node_id, &(poi_set -> root));
		free(rect);
		/* remove from binary tree */
		g_tree_remove(poi_set -> points, &node_id);
}

/****************************************************************************************************
* return the points found in the given area
****************************************************************************************************/
GArray * poi_set_get(PoiSet * poi_set, double min_lon, double min_lat, double max_lon, double max_lat)
{
	struct Rect rect;
	if (min_lon > max_lon){
		double tmp = min_lon;
		min_lon = max_lon;
		max_lon = tmp;
	}
	if (min_lat > max_lat){
		double tmp = min_lat;
		min_lat = max_lat;
		max_lat = tmp;
	}
	rect.boundary[0] = min_lon;
	rect.boundary[1] = min_lat;
	rect.boundary[2] = max_lon;
	rect.boundary[3] = max_lat;
	poi_set -> results = g_array_sized_new(FALSE, FALSE, sizeof(LonLatPairId), 1);
	int n = RTreeSearch(poi_set -> root, &rect, poi_set_search_cb, (void*)poi_set);
	return poi_set -> results;
}

/****************************************************************************************************
* set/get whether this PoiSet is visible
****************************************************************************************************/
void poi_set_set_visible(PoiSet * poi_set, gboolean visible)
{
	poi_set -> visible = visible;
}
gboolean poi_set_get_visible(PoiSet * poi_set)
{
	return poi_set -> visible;
}
