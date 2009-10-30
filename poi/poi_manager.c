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

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "poi_set.h"
#include "named_poi_set.h"
#include "poi_manager.h"
#include "osm_reader.h"
#include "../paths.h"
#include "../config/config.h"
#include "../customio.h"
#include "../map_area.h"
#include "../tool.h"
#include "poi_statistics.h"

/****************************************************************************************************
* PoiManager is the central management unit for Points of interests
****************************************************************************************************/
G_DEFINE_TYPE (PoiManager, poi_manager, G_TYPE_OBJECT);

/****************************************************************************************************
* signals
****************************************************************************************************/
enum
{
	LAYER_TOGGLED,
	LAYER_ADDED,
	LAYER_DELETED,
        COLOUR_CHANGED,
        KEY_CHANGED,
	VALUE_CHANGED,
	SOURCE_ACTIVATED,
	SOURCE_DEACTIVATED,
	SOURCE_ADDED,
	SOURCE_DELETED,
	FILE_PARSING_STARTED,
	FILE_PARSING_ENDED,
	API_REQUEST_STARTED,
	API_REQUEST_ENDED,
        LAST_SIGNAL
};

static guint poi_manager_signals[LAST_SIGNAL] = { 0 };

/****************************************************************************************************
* method declarations
****************************************************************************************************/
gboolean poi_manager_create_default_poi_layers();
gboolean poi_manager_read_poi_sources(PoiManager * poi_manager);
gboolean poi_manager_read_poi_layers(PoiManager * poi_manager);
void poi_manager_fill_poi_set(PoiManager * poi_manager, PoiSet * poi_set);
static gboolean poi_manager_osm_reader_finished_cb(OsmReader * osm_reader, int status, gpointer data);
static gboolean poi_manager_osm_reader_api_finished_cb(OsmReader * osm_reader, int status, gpointer data);

gint poi_manager_compare_strings(gconstpointer a, gconstpointer b, gpointer user_data)
{
        return strcmp(a, b);
}
gint poi_manager_compare_ints(gconstpointer a, gconstpointer b, gpointer user_data)
{
	        return *(int*)a - *(int*)b;
}
/****************************************************************************************************
* constructor
****************************************************************************************************/
PoiManager * poi_manager_new()
{
	PoiManager * poi_manager = g_object_new(GOSM_TYPE_POI_MANAGER, NULL);
	poi_manager -> all_pois = poi_set_new();
	poi_manager -> tag_tree = g_tree_new_full(poi_manager_compare_strings, NULL, NULL, NULL);
	poi_manager -> tree_ids = g_tree_new_full(poi_manager_compare_ints, NULL, NULL, NULL);
	poi_manager -> poi_sets = g_array_new(FALSE, FALSE, sizeof(StyledPoiSet*));
	poi_manager -> poi_sources = g_array_new(FALSE, FALSE, sizeof(PoiSource*));
	poi_manager -> active_poi_source = -1;
	poi_manager -> osm_reader = osm_reader_new();
	g_signal_connect(G_OBJECT(poi_manager -> osm_reader),"reading-finished",
			 G_CALLBACK(poi_manager_osm_reader_finished_cb), (gpointer)poi_manager);
	g_signal_connect(G_OBJECT(poi_manager -> osm_reader),"api-finished",
			 G_CALLBACK(poi_manager_osm_reader_api_finished_cb), (gpointer)poi_manager);
	gboolean layers = poi_manager_read_poi_layers(poi_manager);
	if (!layers) {
		printf("did not find poi_layers file, attempting to create it\n");
		layers = poi_manager_create_default_poi_layers();
		if (layers){
			printf("created poi_layers file\n");
			layers = poi_manager_read_poi_layers(poi_manager);
		}
		if (!layers) printf("could not read newly created poi_layers file\n");
	}
	poi_manager_read_poi_sources(poi_manager);
	return poi_manager;
}

/****************************************************************************************************
* class init
****************************************************************************************************/
static void poi_manager_class_init(PoiManagerClass *class)
{
        poi_manager_signals[LAYER_TOGGLED] = g_signal_new(
                "layer-toggled",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PoiManagerClass, layer_toggled),
                NULL, NULL,
                g_cclosure_marshal_VOID__INT,
                G_TYPE_NONE, 1, G_TYPE_INT);
        poi_manager_signals[LAYER_ADDED] = g_signal_new(
                "layer-added",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PoiManagerClass, layer_added),
                NULL, NULL,
                g_cclosure_marshal_VOID__INT,
                G_TYPE_NONE, 1, G_TYPE_INT);
        poi_manager_signals[LAYER_DELETED] = g_signal_new(
                "layer-deleted",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PoiManagerClass, layer_deleted),
                NULL, NULL,
                g_cclosure_marshal_VOID__INT,
                G_TYPE_NONE, 1, G_TYPE_INT);
        poi_manager_signals[COLOUR_CHANGED] = g_signal_new(
                "colour-changed",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PoiManagerClass, colour_changed),
                NULL, NULL,
                g_cclosure_marshal_VOID__INT,
                G_TYPE_NONE, 1, G_TYPE_INT);
        poi_manager_signals[KEY_CHANGED] = g_signal_new(
                "key-changed",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PoiManagerClass, key_changed),
                NULL, NULL,
                g_cclosure_marshal_VOID__INT,
                G_TYPE_NONE, 1, G_TYPE_INT);
        poi_manager_signals[VALUE_CHANGED] = g_signal_new(
                "value-changed",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PoiManagerClass, value_changed),
                NULL, NULL,
                g_cclosure_marshal_VOID__INT,
                G_TYPE_NONE, 1, G_TYPE_INT);
        poi_manager_signals[SOURCE_ACTIVATED] = g_signal_new(
                "source-activated",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PoiManagerClass, source_activated),
                NULL, NULL,
                g_cclosure_marshal_VOID__INT,
                G_TYPE_NONE, 1, G_TYPE_INT);
        poi_manager_signals[SOURCE_DEACTIVATED] = g_signal_new(
                "source-deactivated",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PoiManagerClass, source_deactivated),
                NULL, NULL,
                g_cclosure_marshal_VOID__INT,
                G_TYPE_NONE, 1, G_TYPE_INT);
        poi_manager_signals[SOURCE_ADDED] = g_signal_new(
                "source-added",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PoiManagerClass, source_added),
                NULL, NULL,
                g_cclosure_marshal_VOID__INT,
                G_TYPE_NONE, 1, G_TYPE_INT);
        poi_manager_signals[SOURCE_DELETED] = g_signal_new(
                "source-deleted",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PoiManagerClass, source_deleted),
                NULL, NULL,
                g_cclosure_marshal_VOID__INT,
                G_TYPE_NONE, 1, G_TYPE_INT);
        poi_manager_signals[FILE_PARSING_STARTED] = g_signal_new(
                "file-parsing-started",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PoiManagerClass, file_parsing_started),
                NULL, NULL,
                g_cclosure_marshal_VOID__INT,
                G_TYPE_NONE, 1, G_TYPE_INT);
        poi_manager_signals[FILE_PARSING_ENDED] = g_signal_new(
                "file-parsing-ended",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PoiManagerClass, file_parsing_ended),
                NULL, NULL,
                g_cclosure_marshal_VOID__INT,
                G_TYPE_NONE, 1, G_TYPE_INT);
        poi_manager_signals[API_REQUEST_STARTED] = g_signal_new(
                "api-request-started",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PoiManagerClass, api_request_started),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
        poi_manager_signals[API_REQUEST_ENDED] = g_signal_new(
                "api-request-ended",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PoiManagerClass, api_request_ended),
                NULL, NULL,
                g_cclosure_marshal_VOID__INT,
                G_TYPE_NONE, 1, G_TYPE_INT);
}

static void poi_manager_init(PoiManager *poi_manager)
{
}

/****************************************************************************************************
* add a source file to the list of source files
****************************************************************************************************/
void poi_manager_add_poi_source(PoiManager * poi_manager, char * filename, gboolean load_on_startup)
{
	PoiSource * poi_source = malloc(sizeof(PoiSource));
	poi_source -> filename = malloc(sizeof(char) * (strlen(filename) + 1));
	poi_source -> basename = g_path_get_basename(filename);
	poi_source -> dirname = g_path_get_dirname(filename);
	poi_source -> load_on_startup = load_on_startup;
	strcpy(poi_source -> filename, filename);
	g_array_append_val(poi_manager -> poi_sources, poi_source);
	g_signal_emit (poi_manager, poi_manager_signals[SOURCE_ADDED], 0, poi_manager -> poi_sources -> len - 1);
}

/****************************************************************************************************
* read in the list of source files from the config file
****************************************************************************************************/
//TODO: splitting in only 100 lines is stupid
gboolean poi_manager_read_poi_sources(PoiManager * poi_manager)
{
	char * filepath = config_get_poi_sources_file();
        struct stat sb;
        int s = stat(filepath, &sb);
        if (s == -1){
                printf("poi_sources file not found\n");
		return FALSE;
        }
        int fd = open(filepath, O_RDONLY);
        if (fd == -1){
                printf("poi_sources file could not be opened for reading\n");
                return FALSE;
        }
        int size = sb.st_size;
        char buf[size+1];
        read(fd, buf, size);
        close(fd);
        buf[size] = '\0';

        gchar ** splitted = g_strsplit(buf, "\n", 100);
        gchar * current = splitted[0];
        int i = 0;
        while (current != NULL){
                if (strlen(current) > 0){
                        gchar ** splitline = g_strsplit(current, "\t", 2);
			if (g_strv_length(splitline) == 2){
	                        gchar * filename = g_strstrip(splitline[0]);
				gboolean load = strcmp(splitline[1], "TRUE") == 0;
				printf("%s %d\n", filename, load);
				poi_manager_add_poi_source(poi_manager, filename, load);
				g_signal_emit (poi_manager, poi_manager_signals[SOURCE_ADDED], 0, i);
			}
			g_strfreev(splitline);
                }
                current = splitted[++i];
        }
	g_strfreev(splitted);
	return TRUE;
}

/****************************************************************************************************
* create the default config file for poi layers
****************************************************************************************************/
gboolean poi_manager_create_default_poi_layers()
{
	char * from = GOSM_POI_DIR "res/poi_layers";
	char * to = config_get_poi_layers_file();
	int ret = copy_file(from, to);
	free(to);
	return ret == 0;
}

/****************************************************************************************************
* read in the config file for poi layers
****************************************************************************************************/
gboolean poi_manager_read_poi_layers(PoiManager * poi_manager)
{
	char * filepath = config_get_poi_layers_file();

        struct stat sb;
        int s = stat(filepath, &sb);
        if (s == -1){
                printf("poi_layers file not found\n");
		return FALSE;
        }
        int fd = open(filepath, O_RDONLY);
        if (fd == -1){
                printf("poi_layers file could not be opened for reading\n");
                return FALSE;
        }
        int size = sb.st_size;
        char buf[size+1];
        read(fd, buf, size);
        close(fd);
        buf[size] = '\0';

        gchar ** splitted = g_strsplit(buf, "\n", 100);
        gchar * current = splitted[0];
        int i = 0;
        while (current != NULL){
                if (strlen(current) > 0){
                        gchar ** splitline = g_strsplit(current, "\t", 7);
			if (g_strv_length(splitline) == 7){
	                        gchar * key = g_strstrip(splitline[0]);
	                        gchar * val = g_strstrip(splitline[1]);
				gboolean visible = strcmp(splitline[2], "TRUE") == 0;
	                        double r = strtodouble(g_strstrip(splitline[3]));
	                        double g = strtodouble(g_strstrip(splitline[4]));
	                        double b = strtodouble(g_strstrip(splitline[5]));
	                        double a = strtodouble(g_strstrip(splitline[6]));
				poi_manager_add_poi_set(poi_manager, key, val, visible, r, g, b, a);
				//printf("%s %s %f %f %f %f\n", key, val, r, g, b, a);
			}
			g_strfreev(splitline);
                }
                current = splitted[++i];
        }
	g_strfreev(splitted);
	return TRUE;
}

/****************************************************************************************************
* add a new PoiSet to the set fo PoiSets
****************************************************************************************************/
void poi_manager_add_poi_set(PoiManager * poi_manager, char * key, char * value, gboolean active,
	double r, double g, double b, double a)
{
	StyledPoiSet * poi_set = styled_poi_set_new(key, value, r, g, b, a);
	poi_set_set_visible(GOSM_POI_SET(poi_set), active);
	g_array_append_val(poi_manager -> poi_sets, poi_set);
	poi_manager_fill_poi_set(poi_manager, GOSM_POI_SET(poi_set));
	g_signal_emit (poi_manager, poi_manager_signals[LAYER_ADDED], 0, poi_manager -> poi_sets -> len - 1);
}

/****************************************************************************************************
* remove a PoiSet from the set of PoiSets
****************************************************************************************************/
void poi_manager_delete_poi_set(PoiManager * poi_manager, int index)
{
	StyledPoiSet * poi_set = poi_manager_get_poi_set(poi_manager, index);
	poi_set_clear(GOSM_POI_SET(poi_set));
	g_array_remove_index(poi_manager -> poi_sets, index);
	g_signal_emit (poi_manager, poi_manager_signals[LAYER_DELETED], 0, index);
}

/****************************************************************************************************
* insert all pois, that are currently available through OsmReader into the given PoiSet
****************************************************************************************************/
void poi_manager_fill_poi_set(PoiManager * poi_manager, PoiSet * poi_set)
{
	char * key = named_poi_set_get_key(GOSM_NAMED_POI_SET(poi_set));
	char * value = named_poi_set_get_value(GOSM_NAMED_POI_SET(poi_set));
        GTree * tree = (GTree*) g_tree_lookup(poi_manager -> tag_tree, key);
        if (tree == NULL) return;
        //GArray * ids = (GArray*) g_tree_lookup(tree, value);
	GSequence * ids = (GSequence*) g_tree_lookup(tree, value);
	if (ids == NULL) return;
	//int i;
	//for (i = 0; i < ids -> len; i++){
	GSequenceIter * iter = g_sequence_get_begin_iter(ids);
	while(!g_sequence_iter_is_end(iter)){
		int id = *(int*)g_sequence_get(iter);
		//int id = g_array_index(ids, int, i);
		int * id_p = malloc(sizeof(int));
		*id_p = id;
		LonLatTags * llt = g_tree_lookup(poi_manager -> tree_ids, (gpointer)id_p);
		poi_set_add(GOSM_POI_SET(poi_set), llt -> lon, llt -> lat, id);
		iter = g_sequence_iter_next(iter);
	}
}

/****************************************************************************************************
* add only those pois to a PoiSet that have been retrieved by an api-request recently
****************************************************************************************************/
void poi_manager_add_new_to_poi_set(PoiManager * poi_manager, StyledPoiSet * poi_set)
{
	char * key = named_poi_set_get_key(GOSM_NAMED_POI_SET(poi_set));
	char * value = named_poi_set_get_value(GOSM_NAMED_POI_SET(poi_set));
        GTree * tree = (GTree*) g_tree_lookup(poi_manager -> tag_tree_new, key);
        if (tree == NULL) return;
        //GArray * ids = (GArray*) g_tree_lookup(tree, value);
	GSequence * ids = (GSequence*) g_tree_lookup(tree, value);
	if (ids == NULL) return;
	//int i;
	//for (i = 0; i < ids -> len; i++){
	//	int id = g_array_index(ids, int, i);
	GSequenceIter * iter = g_sequence_get_begin_iter(ids);
	while(!g_sequence_iter_is_end(iter)){
		int id = *(int*)g_sequence_get(iter);
		int * id_p = malloc(sizeof(int));
		*id_p = id;
		LonLatTags * llt = g_tree_lookup(poi_manager -> tree_ids, (gpointer)id_p);
		poi_set_add(GOSM_POI_SET(poi_set), llt -> lon, llt -> lat, id);
		iter = g_sequence_iter_next(iter);
	}
}

/****************************************************************************************************
* toggle the visibility of a PoiSet
****************************************************************************************************/
void poi_manager_toggle_poi_set(PoiManager * poi_manager, int index)
{
	StyledPoiSet * poi_set = poi_manager_get_poi_set(poi_manager, index);
	gboolean visible = poi_set_get_visible(GOSM_POI_SET(poi_set));
	poi_set_set_visible(GOSM_POI_SET(poi_set), !visible);
	g_signal_emit (poi_manager, poi_manager_signals[LAYER_TOGGLED], 0, index);
}

/****************************************************************************************************
* return the number of PoiSets
****************************************************************************************************/
int poi_manager_get_number_of_poi_sets(PoiManager * poi_manager)
{
	return poi_manager -> poi_sets -> len;
}

/****************************************************************************************************
* return the index's PoiSet
****************************************************************************************************/
StyledPoiSet * poi_manager_get_poi_set(PoiManager * poi_manager, int index)
{
	return g_array_index(poi_manager -> poi_sets, StyledPoiSet*, index);
}

/****************************************************************************************************
* return the number of poi-source files
****************************************************************************************************/
int poi_manager_get_number_of_poi_sources(PoiManager * poi_manager)
{
	return poi_manager -> poi_sources -> len;
}

/****************************************************************************************************
* return the index's poi-source file
****************************************************************************************************/
PoiSource * poi_manager_get_poi_source(PoiManager * poi_manager, int index)
{
	return g_array_index(poi_manager -> poi_sources, PoiSource*, index);
}

/****************************************************************************************************
* iterator functions that are called by iterating the node_id-binarytree
****************************************************************************************************/
void remove_tags_from_tree(GTree * tag_tree, LonLatTags * llt, int node_id){
	GHashTableIter iter;
	g_hash_table_iter_init(&iter, llt -> tags);
	gpointer key, val;
	while(g_hash_table_iter_next(&iter, &key, &val)){
		GTree * tree = (GTree*) g_tree_lookup(tag_tree, key);
		if (tree == NULL) continue;
		GSequence * ids = (GSequence*) g_tree_lookup(tree, val);
		if (ids == NULL) continue;
		GSequenceIter * iter = g_sequence_get_begin_iter(ids);
		int sc = 0;
		while(!g_sequence_iter_is_end(iter)){
			int id = *(int*)g_sequence_get(iter);
			if (id == node_id){
				g_sequence_remove(iter);
				break;
			}
			sc++;
			iter = g_sequence_iter_next(iter);
		}
	}
}

void add_tags_to_tree(GTree * tag_tree, LonLatTags * llt, int node_id){
	GHashTableIter iter;
	g_hash_table_iter_init(&iter, llt -> tags);
	gpointer key, val;
	while(g_hash_table_iter_next(&iter, &key, &val)){
		/* put node into tag-tree */
		gpointer lookup1 = g_tree_lookup(tag_tree, key);
		GTree * tree1 = (GTree*)lookup1;
		if (lookup1 == NULL){
			/* no key found on first level (key not present) */
			tree1 = g_tree_new_full(poi_manager_compare_strings, NULL, NULL, NULL);
			int len_k = strlen(key) + 1;
			char * key_insert = malloc(sizeof(char) * len_k);
			strcpy(key_insert, key);
			g_tree_insert(tag_tree, key_insert, tree1);
		}
		/* now tree1 exists */
		gpointer lookup2 = g_tree_lookup(tree1, val);
		GSequence * elements = (GSequence*)lookup2;
		if (lookup2 == NULL){
			elements = g_sequence_new(NULL);
			int len_v = strlen(val) + 1;
			char * val_insert = malloc(sizeof(char) * len_v);
			strcpy(val_insert, val);
			g_tree_insert(tree1, val_insert, elements);
		}
		/* now elements exists */
		//g_array_append_val(elements, id);
		int * id_p = malloc(sizeof(int));
		*id_p = node_id;
		g_sequence_insert_sorted(elements, id_p, poi_manager_compare_ints, NULL);
	}
}

gboolean handle_new_nodes(gpointer k, gpointer v, gpointer data)
{
	PoiManager * poi_manager = GOSM_POI_MANAGER(data);
	int node_id = *(int*)k;
	LonLatTags * llt = (LonLatTags*)v;
	LonLatTags * llt_old = (LonLatTags*)g_tree_lookup(poi_manager -> tree_ids, &node_id);
	if (llt_old != NULL){
		// the id has been in before, old tags have to be removed
		remove_tags_from_tree(poi_manager -> tag_tree, llt, node_id);
		// TODO: remove from PoiSets
	}else{
		int * key_insert = malloc(sizeof(int));
		*key_insert = node_id;
		g_tree_insert(poi_manager -> tree_ids, key_insert, llt);
	}
	// now, the node_id is in tree_ids either case
	// put into PoiSet that hold every node TODO: only on not in before
	//poi_set_add(poi_manager -> all_pois, llt -> lon, llt -> lat, *(int*)k);
	// add to tag tree
	add_tags_to_tree(poi_manager -> tag_tree, llt, node_id);
	//add_tags_to_tree(poi_manager -> tag_tree_new, llt, node_id);
	return FALSE;
}

// insert all nodes into the tag tree
gboolean node_to_tag_tree(gpointer k, gpointer v, gpointer data)
{
	PoiManager * poi_manager = GOSM_POI_MANAGER(data);
	int id = *(int*)k;
	LonLatTags * llt = (LonLatTags*)v;
	// iterate the hash table
	GHashTableIter iter;
	g_hash_table_iter_init(&iter, llt -> tags);
	gpointer key, val;
	while(g_hash_table_iter_next(&iter, &key, &val)){
		//printf("key %s\n", key);
		/* put node into tag-tree */
		gpointer lookup1 = g_tree_lookup(poi_manager -> tag_tree_insertion, key);
		GTree * tree1 = (GTree*)lookup1;
		if (lookup1 == NULL){
			/* no key found on first level (key not present) */
			tree1 = g_tree_new_full(poi_manager_compare_strings, NULL, NULL, NULL);
			int len_k = strlen(key) + 1;
			char * key_insert = malloc(sizeof(char) * len_k);
			strcpy(key_insert, key);
			g_tree_insert(poi_manager -> tag_tree_insertion, key_insert, tree1);
		}
		/* now tree1 exists */
		gpointer lookup2 = g_tree_lookup(tree1, val);
		GSequence * elements = (GSequence*)lookup2;
		if (lookup2 == NULL){
			elements = g_sequence_new(NULL);
			int len_v = strlen(val) + 1;
			char * val_insert = malloc(sizeof(char) * len_v);
			strcpy(val_insert, val);
			g_tree_insert(tree1, val_insert, elements);
		}
		/* now elements exists */
		//g_array_append_val(elements, id);
		int * id_p = malloc(sizeof(int));
		*id_p = id;
		g_sequence_insert_sorted(elements, id_p, poi_manager_compare_ints, NULL);
		//printf("sequence size is now %d\n", g_sequence_get_length(elements));
	}
	return FALSE;
}
// insert all nodes into the binary node_id tree
gboolean node_to_id_tree(gpointer k, gpointer v, gpointer data)
{
	PoiManager * poi_manager = GOSM_POI_MANAGER(data);
	int id = *(int*)k;
	LonLatTags * llt = (LonLatTags*)v;
	int * key_insert = malloc(sizeof(int));
	*key_insert = id;
	g_tree_insert(poi_manager -> tree_ids, key_insert, llt);
	return FALSE;
}
// insert all nodes into the main poiset, that contains evey node
gboolean node_to_poi_set_all(gpointer k, gpointer v, gpointer data)
{
	PoiSet * poi_set = GOSM_POI_SET(data);
	LonLatTags * llt = (LonLatTags*)v;
	poi_set_add(poi_set, llt -> lon, llt -> lat, *(int*)k);
	return FALSE;
}

/****************************************************************************************************
* set the index's poi-source file as the current
* i.e. read in the selected file and fill the PoiSets with the read points
****************************************************************************************************/
void poi_manager_activate_poi_source(PoiManager * poi_manager, int index)
{
	int old = poi_manager -> active_poi_source;
	poi_manager -> active_poi_source = index;
	if (old >= 0 && old != index){
		g_signal_emit (poi_manager, poi_manager_signals[SOURCE_DEACTIVATED], 0, old);
	}
	if (index >= 0 && old != index){
		int num_poi_sets = poi_manager_get_number_of_poi_sets(poi_manager);
		osm_reader_clear(poi_manager -> osm_reader);
		int n;
//		for (n = 0; n < num_poi_sets; n++){
//			StyledPoiSet * poi_set = poi_manager_get_poi_set(poi_manager, n);
//			poi_set_clear(GOSM_POI_SET(poi_set));
//		}
		if (index >= 0){
			PoiSource * poi_source = poi_manager_get_poi_source(poi_manager, index);
			g_signal_emit (poi_manager, poi_manager_signals[FILE_PARSING_STARTED], 0, index);
			osm_reader_parse_file(poi_manager -> osm_reader, poi_source -> filename);
		}
	}
}

/****************************************************************************************************
* callback; when the OsmReader finished reading a file
****************************************************************************************************/
static gboolean poi_manager_osm_reader_finished_cb(OsmReader * osm_reader, int status, gpointer data)
{
	PoiManager * poi_manager = GOSM_POI_MANAGER(data);
//	poi_manager -> tag_tree_insertion = poi_manager -> tag_tree;
//	g_tree_foreach(osm_reader -> tree_ids, node_to_tag_tree, data);
//	g_tree_foreach(osm_reader -> tree_ids, node_to_id_tree, data);
//	g_tree_foreach(osm_reader -> tree_ids, node_to_poi_set_all, (gpointer)(poi_manager -> all_pois));
	int num_poi_sets = poi_manager_get_number_of_poi_sets(poi_manager);
	int n;
	g_tree_foreach(osm_reader -> tree_ids, handle_new_nodes, data);
	for (n = 0; n < num_poi_sets; n++){
		StyledPoiSet * poi_set = poi_manager_get_poi_set(poi_manager, n);
		poi_set_clear(GOSM_POI_SET(poi_set));
		poi_manager_fill_poi_set(poi_manager, GOSM_POI_SET(poi_set));
	}
	if (poi_manager -> active_poi_source >= 0){
		g_signal_emit (poi_manager, poi_manager_signals[FILE_PARSING_ENDED], 0);
	}
	g_signal_emit (poi_manager, poi_manager_signals[SOURCE_ACTIVATED], 0, poi_manager -> active_poi_source);
	return FALSE;
}

/****************************************************************************************************
* make a request to the OSM-API
****************************************************************************************************/
void poi_manager_api_request(PoiManager * poi_manager)
{
	/* find out which bbox to query */
	double min_lon, min_lat, max_lon, max_lat;
	map_area_get_visible_area(poi_manager -> map_area, &min_lon, &min_lat, &max_lon, &max_lat);
	/* and remember the queried bbox for later usage, when query is ready */
	poi_manager -> bbox_api_query.min_lon = min_lon;
	poi_manager -> bbox_api_query.min_lat = min_lat;
	poi_manager -> bbox_api_query.max_lon = max_lon;
	poi_manager -> bbox_api_query.max_lat = max_lat;
	/* find out the api-query-url */
	char * buf = get_api_url_get(min_lon, min_lat, max_lon, max_lat);
	printf("getting osm-data for bbox: %e %e %e %e\n", min_lon, min_lat, max_lon, max_lat);
	printf("downloading %s\n", buf);
	/* deactivate any poi-source */
	poi_manager_activate_poi_source(poi_manager, -1);
	g_signal_emit (poi_manager, poi_manager_signals[API_REQUEST_STARTED], 0);
	osm_reader_clear(poi_manager -> osm_reader);
	osm_reader_parse_api_url(poi_manager -> osm_reader, buf);
}

/****************************************************************************************************
* callback; when the OsmReader finished reading in an api-request
****************************************************************************************************/
static gboolean poi_manager_osm_reader_api_finished_cb(OsmReader * osm_reader, int status, gpointer data)
{
	PoiManager * poi_manager = GOSM_POI_MANAGER(data);
//	double min_lon = poi_manager -> bbox_api_query.min_lon;
//	double min_lat = poi_manager -> bbox_api_query.min_lat;
//	double max_lon = poi_manager -> bbox_api_query.max_lon;
//	double max_lat = poi_manager -> bbox_api_query.max_lat;
	/* remove all pois that have been in the requested area before */
	int num_poi_sets = poi_manager_get_number_of_poi_sets(poi_manager);
	int n;
//	for (n = 0; n < num_poi_sets; n++){
//		PoiSet * poi_set = GOSM_POI_SET(poi_manager_get_poi_set(poi_manager, n));
//		poi_set_clear_area(poi_set, min_lon, min_lat, max_lon, max_lat);
//	}
	/* insert new pois */
	//poi_manager -> tag_tree_new = g_tree_new_full(poi_manager_compare_strings, NULL, NULL, NULL);
	g_tree_foreach(osm_reader -> tree_ids, handle_new_nodes, data);
//	poi_manager -> tag_tree_insertion = poi_manager -> tag_tree;
//	g_tree_foreach(osm_reader -> tree_ids, node_to_tag_tree, data);
//
//	poi_manager -> tag_tree_insertion = poi_manager -> tag_tree_new;
//	g_tree_foreach(osm_reader -> tree_ids, node_to_tag_tree, data);
//
//	g_tree_foreach(osm_reader -> tree_ids, node_to_id_tree, data);
//	printf("%d nodes\n", g_tree_nnodes(poi_manager -> tree_ids));
//	g_tree_foreach(osm_reader -> tree_ids, node_to_poi_set_all, (gpointer)(poi_manager -> all_pois));
	for (n = 0; n < num_poi_sets; n++){
		StyledPoiSet * poi_set = poi_manager_get_poi_set(poi_manager, n);
		//poi_manager_add_new_to_poi_set(poi_manager, poi_set);
		//TODO: instead of clear & reload, the double node's old values could be removed from PoiSets
		// and new values inserted
		poi_set_clear(GOSM_POI_SET(poi_set));
		poi_manager_fill_poi_set(poi_manager, GOSM_POI_SET(poi_set));
	}
	g_signal_emit (poi_manager, poi_manager_signals[API_REQUEST_ENDED], 0, status);
	g_signal_emit (poi_manager, poi_manager_signals[SOURCE_ACTIVATED], 0, poi_manager -> active_poi_source);
	return FALSE;
}

/****************************************************************************************************
* change a PoiSets' colour
****************************************************************************************************/
void poi_manager_set_poi_set_colour(PoiManager * poi_manager, int index, double r, double g, double b, double a)
{
	StyledPoiSet * poi_set = g_array_index(poi_manager -> poi_sets, StyledPoiSet*, index);
	styled_poi_set_set_colour(poi_set, r, g, b, a);
	g_signal_emit (poi_manager, poi_manager_signals[COLOUR_CHANGED], 0, index);
}

/****************************************************************************************************
* save the list of poi layers
****************************************************************************************************/
gboolean poi_manager_layers_save(PoiManager * poi_manager)
{
	printf("saving poi layers\n");
	char * filepath = config_get_poi_layers_file();
        int fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1){
                printf("poi_layers file could not be opened for writing\n");
                return FALSE;
        }
	int i;
	char * true_s = "TRUE";
	char * false_s = "FALSE";
	for (i = 0; i < poi_manager -> poi_sets -> len; i++){
		StyledPoiSet * poi_set = poi_manager_get_poi_set(poi_manager, i);
		char * key = named_poi_set_get_key(GOSM_NAMED_POI_SET(poi_set));
		char * val = named_poi_set_get_value(GOSM_NAMED_POI_SET(poi_set));
		double r, g, b, a;
		styled_poi_set_get_colour(poi_set, &r, &g, &b, &a);
		char r_s[6], g_s[6], b_s[6], a_s[6];
		sprintdouble(r_s, r, 2);
		sprintdouble(g_s, g, 2);
		sprintdouble(b_s, b, 2);
		sprintdouble(a_s, a, 2);
		write(fd, key, strlen(key));
		write(fd, "\t", 1);
		write(fd, val, strlen(val));
		write(fd, "\t", 1);
		if (poi_set_get_visible(GOSM_POI_SET(poi_set))){
			write(fd, true_s, strlen(true_s));
		}else{
			write(fd, false_s, strlen(false_s));
		}
		write(fd, "\t", 1);
		write(fd, r_s, strlen(r_s));
		write(fd, "\t", 1);
		write(fd, g_s, strlen(g_s));
		write(fd, "\t", 1);
		write(fd, b_s, strlen(b_s));
		write(fd, "\t", 1);
		write(fd, a_s, strlen(a_s));
		write(fd, "\n", 1);
	}
	close(fd);
	return TRUE;
}

/****************************************************************************************************
* save the list of poi sources
****************************************************************************************************/
gboolean poi_manager_sources_save(PoiManager * poi_manager)
{
	printf("saving poi sources\n");
	char * filepath = config_get_poi_sources_file();
        int fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1){
                printf("poi_sources file could not be opened for writing\n");
                return FALSE;
        }
	int i;
	char * true_s = "TRUE";
	char * false_s = "FALSE";
	for (i = 0; i < poi_manager -> poi_sources -> len; i++){
		PoiSource * poi_source = poi_manager_get_poi_source(poi_manager, i);
		char * filename = poi_source -> filename;
		write(fd, filename, strlen(filename));
		write(fd, "\t", 1);
		if (poi_source -> load_on_startup){
			write(fd, true_s, strlen(true_s));
		}else{
			write(fd, false_s, strlen(false_s));
		}
		write(fd, "\n", 1);
	}
	close(fd);
	return TRUE;
}

/****************************************************************************************************
* revert the poi layers to the last saved state
* TODO: unimplemented
****************************************************************************************************/
gboolean poi_manager_layers_revert(PoiManager * poi_manager)
{
	printf("reverting poi layers\n");
	return TRUE;
}

/****************************************************************************************************
* add a poi-source file
****************************************************************************************************/
gboolean poi_manager_sources_add(PoiManager * poi_manager, char * path)
{
	poi_manager_add_poi_source(poi_manager, path, FALSE);
}

/****************************************************************************************************
* remove a poi-source file
****************************************************************************************************/
gboolean poi_manager_sources_delete(PoiManager * poi_manager, int index)
{
	if (index == poi_manager -> active_poi_source){
		poi_manager -> active_poi_source = -1;
		//TODO: free resources bound by the deleted, active poi_source, unshow
	}
	PoiSource * poi_source = g_array_index(poi_manager -> poi_sources, PoiSource*, index);
	g_array_remove_index(poi_manager -> poi_sources, index);
	free(poi_source -> filename);
	free(poi_source -> basename);
	free(poi_source -> dirname);
	g_signal_emit (poi_manager, poi_manager_signals[SOURCE_DELETED], 0, index);
}

/****************************************************************************************************
* the PoiManager needs a reference to the MapArea to be able to request the currently visible area
* via the OSM-API
****************************************************************************************************/
void poi_manager_set_map_area(PoiManager * poi_manager, MapArea * map_area)
{
	poi_manager -> map_area = map_area;
}

/****************************************************************************************************
* print out to stdout the information about a node
****************************************************************************************************/
void poi_manager_print_node_information(PoiManager * poi_manager, int node_id)
{
	printf("Node id = %d\n", node_id);
	LonLatTags * llt = (LonLatTags*) g_tree_lookup(poi_manager-> tree_ids, &node_id);
	GHashTableIter iter;
	g_hash_table_iter_init(&iter, llt -> tags);
	gpointer key, val;
	while(g_hash_table_iter_next(&iter, &key, &val)){
		printf("%s:%s\n", (char*)key, (char*)val);
	}
}

/****************************************************************************************************
* find a node
****************************************************************************************************/
LonLatTags * poi_manager_get_node(PoiManager * poi_manager, int node_id)
{
	LonLatTags * llt = (LonLatTags*) g_tree_lookup(poi_manager -> tree_ids, &node_id);
	return llt;
}
