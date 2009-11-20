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
#include <string.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "osm_data_set.h"
#include "styled_poi_set.h"
#include "named_poi_set.h"
#include "../map_types.h"
#include "../tool.h"
#include "poi_manager.h"

G_DEFINE_TYPE (OsmDataSet, osm_data_set, G_TYPE_OBJECT);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint osm_data_set_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, osm_data_set_signals[SIGNAL_NAME_n], 0);

OsmDataSet * osm_data_set_new()
{
	OsmDataSet * osm_data_set = g_object_new(GOSM_TYPE_OSM_DATA_SET, NULL);

	osm_data_set -> tree_ids = g_tree_new_full(compare_int_pointers, NULL, free, node_free);
	osm_data_set -> tag_tree = tag_tree_new();
	osm_data_set -> all_pois = poi_set_new();
	osm_data_set -> poi_sets = g_array_new(FALSE, FALSE, sizeof(PoiSet*));
	osm_data_set -> remaining_pois = GOSM_POI_SET(styled_poi_set_new("*", "*", 0, 0, 0, 0.5));

	return osm_data_set;
}

static void osm_data_set_class_init(OsmDataSetClass *class)
{
        /*osm_data_set_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (OsmDataSetClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void osm_data_set_init(OsmDataSet *osm_data_set)
{
}

void osm_data_insert_nodes(OsmDataSet * ods, GArray * poi_sets_original);

/****************************************************************************************************
* obtain a duplicate of an id-tree
****************************************************************************************************/
gboolean duplicate_id_tree__iter(gpointer key, gpointer val, gpointer data)
{
	g_tree_insert((GTree*)data, int_malloc(*(int*)key), node_copy((Node*)val));
	return FALSE;
}
GTree * duplicate_id_tree(GTree * tree_ids)
{
	GTree * id_tree = g_tree_new_full(compare_int_pointers, NULL, free, node_free);
	g_tree_foreach(tree_ids, duplicate_id_tree__iter, id_tree);
	return id_tree;
}

//TODO: this should be in StyledPoiSet
PoiSet * duplicate_poi_set(PoiSet * poi_set)
{
	StyledPoiSet * styled_poi_set = GOSM_STYLED_POI_SET(poi_set);
	StyledPoiSet * poi_set_new = styled_poi_set_new(
		named_poi_set_get_key(GOSM_NAMED_POI_SET(poi_set)),
		named_poi_set_get_value(GOSM_NAMED_POI_SET(poi_set)),
		styled_poi_set -> r,
		styled_poi_set -> g,
		styled_poi_set -> b,
		styled_poi_set -> a
	);
	poi_set_set_visible(GOSM_POI_SET(poi_set_new), poi_set_get_visible(poi_set));
	return GOSM_POI_SET(poi_set_new);
}

gboolean nodes_to_poi_set(gpointer k, gpointer node, gpointer poi_set)
{
	poi_set_add((PoiSet*)poi_set, (Node*)node);
	return FALSE;
}

void osm_data_insert_nodes(OsmDataSet * ods, GArray * poi_sets_original)
{
	GTree * tree_remaining = poi_manager_tree_intersection(ods -> tree_ids, ods -> tree_ids);
	/* add to all_pois */
	g_tree_foreach(ods -> tree_ids, nodes_to_poi_set, ods -> all_pois);
	/* add to PoiSets */
	ods -> poi_sets = g_array_new(FALSE, FALSE, sizeof(PoiSet*));
	int num_poi_sets = poi_sets_original -> len; int n;
	for (n = 0; n < num_poi_sets; n++){
		PoiSet * poi_set = duplicate_poi_set(g_array_index(poi_sets_original, PoiSet*, n));
		char * key = named_poi_set_get_key(GOSM_NAMED_POI_SET(poi_set));
		char * val = named_poi_set_get_value(GOSM_NAMED_POI_SET(poi_set));
		GSequence * elements = tag_tree_get_nodes(ods -> tag_tree, key, val);
		if (elements != NULL){
			GSequenceIter * iter = g_sequence_get_begin_iter(elements);
			while(!g_sequence_iter_is_end(iter)){
				int id = *(int*)g_sequence_get(iter);
				Node * node = g_tree_lookup(ods -> tree_ids, &id);
				poi_set_add(poi_set, node);
				g_tree_remove(tree_remaining, &id);
				iter = g_sequence_iter_next(iter);
			}
		}
		g_array_append_val(ods -> poi_sets, poi_set);
	}
	g_tree_foreach(tree_remaining, nodes_to_poi_set, ods -> remaining_pois);
}

void osm_data_set_duplicate(OsmDataSet * original, OsmDataSet * copy)
{
	copy -> tree_ids = duplicate_id_tree(original -> tree_ids);
	copy -> tag_tree = tag_tree_duplicate_tree(original -> tag_tree);
	copy -> all_pois = poi_set_new();
	copy -> remaining_pois = GOSM_POI_SET(styled_poi_set_new("*", "*", 0, 0, 0, 0.5));
	poi_set_set_visible(copy -> remaining_pois, TRUE);
	osm_data_insert_nodes(copy, original -> poi_sets);
}

void osm_data_set_duplicate_node(OsmDataSet * original, OsmDataSet * copy, int node_id)
{
	g_tree_insert(copy -> tree_ids, int_malloc(node_id), node_copy(g_tree_lookup(original -> tree_ids, &node_id)));
	Node * node = g_tree_lookup(copy -> tree_ids, &node_id);
	tag_tree_add_node(copy -> tag_tree, node_id, node);
	poi_set_add(copy -> all_pois, node);
	gboolean one = FALSE;
	int num_poi_sets = copy -> poi_sets -> len; int n;
	for (n = 0; n < num_poi_sets; n++){
		PoiSet * poi_set = g_array_index(copy -> poi_sets, PoiSet*, n);
		char * key = named_poi_set_get_key(GOSM_NAMED_POI_SET(poi_set));
		char * val = named_poi_set_get_value(GOSM_NAMED_POI_SET(poi_set));
		if (node_get_value(node, key) != NULL){
			if (strcmp(node_get_value(node, key), val) == 0){
				poi_set_add(poi_set, node);
				one = TRUE;
			}
		}
	}
	if(!one) poi_set_add(copy -> remaining_pois, node);
}
