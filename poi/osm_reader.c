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

#include <expat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "osm_reader.h"
#include "../map_types.h"
#include "../customio.h"


G_DEFINE_TYPE (OsmReader, osm_reader, G_TYPE_OBJECT);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint osm_reader_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, osm_reader_signals[SIGNAL_NAME_n], 0);

void destroy_int_p(gpointer data)
{
	//printf("%d\n", (int*)data);
	free(data);
}
void destroy_string(gpointer data)
{
	//printf("%s\n", (char*)data);
	free(data);
}
void destroy_value_trees(gpointer data)
{
	//printf("DESTROY value_tree\n");
	g_tree_destroy((GTree*)data);
}
void destroy_element_arrays(gpointer data)
{	//printf("DESTROY element array\n");
	g_array_free((GArray*)data, TRUE);
}
void destroy_lon_lat_tags(gpointer data)
{	//printf("DESTROY lon lat tags\n");
	LonLatTags * llt = (LonLatTags*) data;
	g_hash_table_destroy(llt -> tags);
	free(llt);
}

gint osm_reader_compare_strings(gconstpointer a, gconstpointer b, gpointer user_data)
{
	return strcmp(a, b);
}

gint osm_reader_compare_ints(gconstpointer a, gconstpointer b, gpointer user_data)
{
	return *(int*)a - *(int*)b;
}

void osm_reader_constructor(OsmReader * osm_reader)
{
	osm_reader -> tree_tags = g_tree_new_full(osm_reader_compare_strings, NULL, destroy_string, destroy_value_trees);
	osm_reader -> tree_ids =  g_tree_new_full(osm_reader_compare_ints, NULL, destroy_int_p, destroy_lon_lat_tags);
	osm_reader -> current_level = 0;
	osm_reader -> current_element = 0;
	osm_reader -> current_id = 0;
}

OsmReader * osm_reader_new()
{
	OsmReader * osm_reader = g_object_new(GOSM_TYPE_OSM_READER, NULL);
	osm_reader_constructor(osm_reader);
	return osm_reader;
}

void osm_reader_clear(OsmReader * osm_reader)
{
	g_tree_destroy(osm_reader -> tree_tags);
	g_tree_destroy(osm_reader -> tree_ids);
	osm_reader_constructor(osm_reader);
}

static void osm_reader_class_init(OsmReaderClass *class)
{
        /*osm_reader_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (OsmReaderClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void osm_reader_init(OsmReader *osm_reader)
{
}

const char * osm_reader_get_value(const XML_Char ** atts, char * search_key)
{
	const XML_Char ** ptr = atts;
	while(*ptr != NULL){
		const XML_Char * key = *ptr;
		const XML_Char * value = *(ptr + 1);
		if (strcmp(key, search_key) == 0){
			return value;
		}
		ptr = ptr + 2;
	}
	return NULL;
}

static void XMLCALL osm_reader_StartElementCallback(	void * userData,
						const XML_Char * name,
						const XML_Char ** atts)
{
	OsmReader * osm_reader = (OsmReader*) userData;
	osm_reader -> current_level ++;
	if (osm_reader -> current_level == 2){
		if (strcmp(name, "node") == 0){
			osm_reader -> current_element = OSM_READER_ELEMENT_NODE;
			const char * id_string = osm_reader_get_value(atts, "id");
			int id = atoi(id_string);
			osm_reader -> current_id = id;
			const char * lon_s = osm_reader_get_value(atts, "lon");
			const char * lat_s = osm_reader_get_value(atts, "lat");
			double lon = strtodouble(lon_s);
			double lat = strtodouble(lat_s);
			GHashTable * tags = g_hash_table_new_full(g_str_hash, g_str_equal, destroy_string, destroy_string);
			LonLatTags * llt = malloc(sizeof(LonLatTags));
			llt -> lon = lon;
			llt -> lat = lat;
			llt -> tags = tags;
			osm_reader -> current_node = llt;
		}else{
			osm_reader -> current_element = OSM_READER_ELEMENT_OTHER;
		}
	}
	if(osm_reader -> current_level == 3 && osm_reader -> current_element == OSM_READER_ELEMENT_NODE){
		if (strcmp(name, "tag") == 0){
			const char * k = osm_reader_get_value(atts, "k");
			if (strcmp(k, "created_by") != 0){
				const char * v = osm_reader_get_value(atts, "v");
				int len_k = strlen(k) + 1;
				int len_v = strlen(v) + 1;
				/* put into node's hashmap */
				char * key_map = malloc(sizeof(char) * len_k);
				char * val_map = malloc(sizeof(char) * len_v);
				strcpy(key_map, k);
				strcpy(val_map, v);
				g_hash_table_insert(osm_reader -> current_node -> tags, key_map, val_map);
				/* put node into tag-tree */
				gpointer lookup1 = g_tree_lookup(osm_reader -> tree_tags, k);
				GTree * tree1 = (GTree*)lookup1;
				if (lookup1 == NULL){
					/* no key found on first level (k not present) */
					tree1 = g_tree_new_full(osm_reader_compare_strings, NULL, destroy_string, destroy_element_arrays);
					char * key_insert = malloc(sizeof(char) * len_k);
					strcpy(key_insert, k);
					g_tree_insert(osm_reader -> tree_tags, key_insert, tree1);
				}
				/* now tree1 exists */
				gpointer lookup2 = g_tree_lookup(tree1, v);
				GArray * elements = (GArray*)lookup2;
				if (lookup2 == NULL){
					elements = g_array_new(FALSE, FALSE, sizeof(int));
					char * val_insert = malloc(sizeof(char) * len_v);
					strcpy(val_insert, v);
					g_tree_insert(tree1, val_insert, elements);
				}
				/* now elements exists */
				g_array_append_val(elements, osm_reader -> current_id);
			}
		}
	}
	//printf("%s\n", name);
}

static void XMLCALL osm_reader_EndElementCallback(	void * userData,
					const XML_Char * name)
{
	OsmReader * osm_reader = (OsmReader*) userData;
	osm_reader -> current_level --;
	if (osm_reader -> current_level == 1 && osm_reader -> current_element == OSM_READER_ELEMENT_NODE){
		if (g_hash_table_size(osm_reader -> current_node -> tags) > 0){
			/* insert finished node into tree holding nodes, sorted by ids */
			int * id_insert = malloc(sizeof(int));
			*id_insert = osm_reader -> current_id;
			g_tree_insert(osm_reader -> tree_ids, id_insert, osm_reader -> current_node);
		}else{
			/* the last node had no relevant tags
			   -> it is not inserted into tree */
			g_hash_table_destroy(osm_reader -> current_node -> tags);
			free(osm_reader -> current_node);
		}
	}
}

int osm_reader_parse_file(OsmReader * osm_reader, char * filename)
{
	XML_Parser parser = XML_ParserCreate(NULL);
	XML_SetUserData(parser, (void*)osm_reader);
	XML_SetStartElementHandler(parser, osm_reader_StartElementCallback);
	XML_SetEndElementHandler(parser, osm_reader_EndElementCallback);

	int f = open(filename, O_RDONLY);
	if (f == 0) return 1;
	ssize_t size;
	int n = 1000;
	char buf[n];
	do{
		size = read(f, buf, n);
		XML_Parse(parser, buf, size, 0);
	}while(size > 0);
	XML_Parse(parser, NULL, 0, 1);
	return 0;
}

GArray * osm_reader_find_ids_key_value(OsmReader * osm_reader, char * key, char * value)
{
	GTree * tree = (GTree*) g_tree_lookup(osm_reader -> tree_tags, key);
	if (tree == NULL){
		return NULL;
	}
	GArray * array = (GArray*) g_tree_lookup(tree, value);
	if (array == NULL){
		return NULL;
	}
	return array;
}

//int main(int argc, char * argv[])
//{
//	printf("foo\n");
//	g_type_init();
//	OsmReader * osm_reader = osm_reader_new();
//	osm_reader_parse_file(osm_reader, "vienna.osm");
//	osm_reader_find_ids_key_value(osm_reader, "shop", "supermarket");
//	g_tree_foreach(osm_reader->tree_ids, traverse, NULL);
//}
