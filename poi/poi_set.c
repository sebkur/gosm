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

G_DEFINE_TYPE (PoiSet, poi_set, G_TYPE_OBJECT);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint poi_set_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, poi_set_signals[SIGNAL_NAME_n], 0);

PoiSet * poi_set_new()
{
	PoiSet * poi_set = g_object_new(GOSM_TYPE_POI_SET, NULL);
	poi_set -> root = RTreeNewIndex();
	poi_set -> node_index = 1;
	poi_set -> points = g_array_new(FALSE, FALSE, sizeof(LonLatPairData));
	return poi_set;
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

void poi_set_add(PoiSet * poi_set, double lon, double lat, void * data)
{
	int array_index = poi_set -> node_index - 1;
	LonLatPairData llpd = {lon, lat, data};
	g_array_append_val(poi_set -> points, llpd);
	struct Rect * rect = malloc(sizeof(struct Rect));
	rect -> boundary[0] = lon;
	rect -> boundary[1] = lat;
	rect -> boundary[2] = lon;
	rect -> boundary[3] = lat;
	RTreeInsertRect(rect, poi_set -> node_index, &(poi_set -> root), 0);
	free(rect);
	poi_set -> node_index++;
}

/*
	used as a callback for searching the RTree.
	this function adds found nodes to the resultset
*/
int poi_set_search_cb(int id, void* arg)
{
	PoiSet * poi_set = (PoiSet*) arg;
	int array_index = id - 1;
	int i = poi_set -> result_index;
	LonLatPairData * llpd = &g_array_index(poi_set -> points, LonLatPairData, array_index);
	poi_set -> results[i].lon = llpd -> lon;
	poi_set -> results[i].lat = llpd -> lat;
	poi_set -> results[i].data = llpd -> data;
	poi_set -> result_index++;
}

LonLatPairData * poi_set_get(PoiSet * poi_set, int* count, double min_lon, double min_lat, double max_lon, double max_lat)
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
	int number_of_results = RTreeSearch(poi_set -> root, &rect, NULL, 0);
	*count = number_of_results;
	poi_set -> results = malloc(sizeof(LonLatPairData) * number_of_results);
	poi_set -> result_index = 0;
	int n = RTreeSearch(poi_set -> root, &rect, poi_set_search_cb, (void*)poi_set);
	return poi_set -> results;
}
