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
#include "poi_manager.h"
#include "osm_reader.h"
#include "../paths.h"

G_DEFINE_TYPE (PoiManager, poi_manager, G_TYPE_OBJECT);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint poi_manager_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, poi_manager_signals[SIGNAL_NAME_n], 0);

PoiManager * poi_manager_new()
{
	PoiManager * poi_manager = g_object_new(GOSM_TYPE_POI_MANAGER, NULL);
	poi_manager -> poi_sets = g_array_new(FALSE, FALSE, sizeof(StyledPoiSet*));
	char * filename = GOSM_NAMEFINDER_DIR "res/vienna.short.osm";
//	filename = GOSM_NAMEFINDER_DIR "res/berlin.short.osm";
	poi_manager -> osm_reader = osm_reader_new();
//	osm_reader_parse_file(poi_manager -> osm_reader, filename);
	poi_manager_add_poi_set(poi_manager, "shop", 	"supermarket", FALSE, 		0.0, 1.0, 0.0, 0.7);
	poi_manager_add_poi_set(poi_manager, "amenity",	"restaurant", FALSE, 		1.0, 0.0, 0.0, 0.7);
	poi_manager_add_poi_set(poi_manager, "amenity",	"cafe", FALSE, 			0.8, 0.1, 0.1, 0.7);
	poi_manager_add_poi_set(poi_manager, "amenity",	"pub", FALSE, 			0.6, 0.2, 0.2, 0.7);
	poi_manager_add_poi_set(poi_manager, "amenity",	"biergarten", FALSE, 		0.5, 0.2, 0.2, 0.7);
	poi_manager_add_poi_set(poi_manager, "amenity",	"bar", FALSE, 			0.0, 0.0, 1.0, 0.7);
	poi_manager_add_poi_set(poi_manager, "amenity",	"nightclub", FALSE, 		0.1, 0.1, 0.8, 0.7);
	poi_manager_add_poi_set(poi_manager, "amenity",	"cinema", FALSE, 		1.0, 0.0, 0.0, 0.7);
	poi_manager_add_poi_set(poi_manager, "amenity",	"theatre", FALSE, 		1.0, 0.0, 0.0, 0.7);
	poi_manager_add_poi_set(poi_manager, "amenity",	"library", FALSE, 		1.0, 0.0, 0.0, 0.7);
	poi_manager_add_poi_set(poi_manager, "amenity",	"place_of_worship", FALSE, 	1.0, 0.0, 0.0, 0.7);
	poi_manager_add_poi_set(poi_manager, "amenity",	"pharmacy", FALSE, 		1.0, 1.0, 0.0, 0.7);
	poi_manager_add_poi_set(poi_manager, "amenity",	"fuel", FALSE, 			0.0, 1.0, 1.0, 0.7);
	poi_manager_add_poi_set(poi_manager, "amenity",	"bank", FALSE, 			1.0, 0.0, 1.0, 0.7);
	poi_manager_add_poi_set(poi_manager, "amenity",	"police", FALSE,		0.0, 0.0, 1.0, 0.7);
	poi_manager_add_poi_set(poi_manager, "tourism",	"museum", FALSE, 		1.0, 0.0, 0.0, 0.7);
	poi_manager_add_poi_set(poi_manager, "tourism",	"hotel", FALSE, 		1.0, 1.0, 0.0, 0.7);
	poi_manager_add_poi_set(poi_manager, "tourism",	"hostel", FALSE, 		1.0, 0.0, 0.0, 0.7);
	poi_manager_add_poi_set(poi_manager, "tourism",	"attraction", FALSE, 		1.0, 0.0, 0.0, 0.7);
	poi_manager_add_poi_set(poi_manager, "tourism",	"viewpoint", FALSE, 		1.0, 0.0, 0.0, 0.7);
	return poi_manager;
}

static void poi_manager_class_init(PoiManagerClass *class)
{
        /*poi_manager_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PoiManagerClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void poi_manager_init(PoiManager *poi_manager)
{
}

void poi_manager_add_poi_set(PoiManager * poi_manager, char * key, char * value, gboolean active,
	double r, double g, double b, double a)
{
	StyledPoiSet * poi_set = styled_poi_set_new(key, value, r, g, b, a);
	poi_set_set_visible(GOSM_POI_SET(poi_set), active);
//	KeyValueBooleanPoiSet * kvbps = malloc(sizeof(KeyValueBooleanPoiSet));
//	kvbps -> key = key;
//	kvbps -> value = value;
//	kvbps -> active = active;
//	kvbps -> poi_set = poi_set;
	g_array_append_val(poi_manager -> poi_sets, poi_set);
	GArray * ids = osm_reader_find_ids_key_value(poi_manager -> osm_reader, key, value);
	if (ids != NULL){
		int i;
		for (i = 0; i < ids -> len; i++){
			int id = g_array_index(ids, int, i);
			LonLatTags * llt = (LonLatTags*) g_tree_lookup(poi_manager -> osm_reader -> tree_ids, &id);
			char * name = g_hash_table_lookup(llt -> tags, "name");
			if (name == NULL){
				name = malloc(sizeof(char));
				name[0] = '\0';
			}
			IdAndName * id_name = malloc(sizeof(IdAndName));
			id_name -> id = id;
			id_name -> name = name;
			poi_set_add(GOSM_POI_SET(poi_set), llt -> lon, llt -> lat, (void*)id_name);
		}
	}
}

void poi_manager_toggle_poi_set(PoiManager * poi_manager, char * key, char * value, gboolean active)
{
	int num_poi_sets = poi_manager_get_number_of_poi_sets(poi_manager);
	int poi;
	for (poi = 0; poi < num_poi_sets; poi++){
//		KeyValueBooleanPoiSet * kvbps = poi_manager_get_poi_set(poi_manager, poi);
		NamedPoiSet * poi_set = GOSM_NAMED_POI_SET(poi_manager_get_poi_set(poi_manager, poi));
		//printf("%s %s %s %s\n", key, value, kvbps -> key, kvbps -> value);
		if (strcmp(named_poi_set_get_key(poi_set), key) == 0 
		    && strcmp(named_poi_set_get_value(poi_set), value) == 0){
			poi_set_set_visible(GOSM_POI_SET(poi_set), active);
			break;
		}
	}
}

int poi_manager_get_number_of_poi_sets(PoiManager * poi_manager)
{
	return poi_manager -> poi_sets -> len;
}

StyledPoiSet * poi_manager_get_poi_set(PoiManager * poi_manager, int index)
{
	return g_array_index(poi_manager -> poi_sets, StyledPoiSet*, index);
}

