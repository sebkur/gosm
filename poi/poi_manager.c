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

#include <pthread.h>

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
#include "tag_tree.h"
#include "edit/edit_action_remove_node.h"
#include "edit/edit_action_add_node.h"
#include "edit/edit_action_change_position.h"
#include "edit/edit_action.h"
#include "edit/edit_action_add_tag.h"
#include "edit/edit_action_remove_tag.h"
#include "edit/edit_action_change_tag.h"
#include "edit/edit_action_change_tag_key.h"
#include "edit/edit_action_change_tag_value.h"
#include "../data_structures/sorted_sequence.h"
#include "api_control.h"

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
	NODE_TAG_ADDED,
	NODE_TAG_REMOVED,
	NODE_TAG_CHANGED,
	ACTION_ADDED,
	ACTION_UNDO,
	ACTION_REDO,
	ACTION_UNDO_MULTIPLE,
	ACTION_REDO_MULTIPLE,
	ACTION_REMOVE,
	ACTION_REMOVE_MULTIPLE,
        LAST_SIGNAL
};

static guint poi_manager_signals[LAST_SIGNAL] = { 0 };

/****************************************************************************************************
* method declarations
****************************************************************************************************/
gboolean poi_manager_create_default_poi_layers();
gboolean poi_manager_read_poi_sources(PoiManager * poi_manager);
gboolean poi_manager_read_poi_layers(PoiManager * poi_manager);
void poi_manager_fill_poi_set(OsmDataSet * ods, PoiSet * poi_set);
static gboolean poi_manager_osm_reader_finished_cb(OsmReader * osm_reader, int status, gpointer data);
static gboolean poi_manager_osm_reader_api_finished_cb(OsmReader * osm_reader, int status, gpointer data);
void poi_manager_add_node_id(PoiManager * poi_manager, gboolean history, int node_id, double lon, double lat);
void poi_manager_apply_change_history(PoiManager * poi_manager);
void poi_manager_change_node_id(PoiManager * poi_manager, int old_id, int new_id);

/****************************************************************************************************
* constructor
****************************************************************************************************/
PoiManager * poi_manager_new()
{
	PoiManager * poi_manager = g_object_new(GOSM_TYPE_POI_MANAGER, NULL);
	pthread_mutex_init(&poi_manager -> mutex_pois, NULL);
	poi_manager -> ods_base = osm_data_set_new();
	poi_manager -> ods_edit = osm_data_set_new();
	poi_manager -> changes = g_array_new(FALSE, FALSE, sizeof(EditAction*));
	poi_manager -> change_index = -1;

	poi_manager -> poi_sources = g_array_new(FALSE, FALSE, sizeof(PoiSource*));
	poi_manager -> active_poi_source = -1;
	poi_manager -> next_node_id = -1;
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
	osm_data_set_duplicate(poi_manager -> ods_base, poi_manager -> ods_edit);
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
        poi_manager_signals[NODE_TAG_ADDED] = g_signal_new(
                "node-tag-added",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PoiManagerClass, node_tag_added),
                NULL, NULL,
                g_cclosure_marshal_VOID__INT,
                G_TYPE_NONE, 1, G_TYPE_INT);
        poi_manager_signals[NODE_TAG_REMOVED] = g_signal_new(
                "node-tag-removed",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PoiManagerClass, node_tag_removed),
                NULL, NULL,
                g_cclosure_marshal_VOID__INT,
                G_TYPE_NONE, 1, G_TYPE_INT);
        poi_manager_signals[NODE_TAG_CHANGED] = g_signal_new(
                "node-tag-changed",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PoiManagerClass, node_tag_changed),
                NULL, NULL,
                g_cclosure_marshal_VOID__INT,
                G_TYPE_NONE, 1, G_TYPE_INT);
        poi_manager_signals[ACTION_ADDED] = g_signal_new(
                "action-added",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PoiManagerClass, action_added),
                NULL, NULL,
                g_cclosure_marshal_VOID__POINTER,
                G_TYPE_NONE, 1, G_TYPE_POINTER);
        poi_manager_signals[ACTION_UNDO] = g_signal_new(
                "action-undo",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PoiManagerClass, action_undo),
                NULL, NULL,
                g_cclosure_marshal_VOID__INT,
                G_TYPE_NONE, 1, G_TYPE_INT);
        poi_manager_signals[ACTION_REDO] = g_signal_new(
                "action-redo",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PoiManagerClass, action_redo),
                NULL, NULL,
                g_cclosure_marshal_VOID__INT,
                G_TYPE_NONE, 1, G_TYPE_INT);
        poi_manager_signals[ACTION_UNDO_MULTIPLE] = g_signal_new(
                "action-undo-multiple",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PoiManagerClass, action_undo_multiple),
                NULL, NULL,
                g_cclosure_marshal_VOID__POINTER,
                G_TYPE_NONE, 1, G_TYPE_POINTER);
        poi_manager_signals[ACTION_REDO_MULTIPLE] = g_signal_new(
                "action-redo-multiple",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PoiManagerClass, action_redo_multiple),
                NULL, NULL,
                g_cclosure_marshal_VOID__POINTER,
                G_TYPE_NONE, 1, G_TYPE_POINTER);
        poi_manager_signals[ACTION_REMOVE] = g_signal_new(
                "action-remove",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PoiManagerClass, action_remove),
                NULL, NULL,
                g_cclosure_marshal_VOID__INT,
                G_TYPE_NONE, 1, G_TYPE_INT);
        poi_manager_signals[ACTION_REMOVE_MULTIPLE] = g_signal_new(
                "action-remove-multiple",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PoiManagerClass, action_remove_multiple),
                NULL, NULL,
                g_cclosure_marshal_VOID__POINTER,
                G_TYPE_NONE, 1, G_TYPE_POINTER);
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
void poi_manager_add_poi_set_ods(OsmDataSet * ods, char * key, char * value, gboolean active,
	double r, double g, double b, double a)
{
	StyledPoiSet * poi_set = styled_poi_set_new(key, value, r, g, b, a);
	poi_manager_fill_poi_set(ods, GOSM_POI_SET(poi_set));
	poi_set_set_visible(GOSM_POI_SET(poi_set), active);
	g_array_append_val(ods -> poi_sets, poi_set);
}
void poi_manager_add_poi_set(PoiManager * poi_manager, char * key, char * value, gboolean active,
	double r, double g, double b, double a)
{
	poi_manager_add_poi_set_ods(poi_manager -> ods_base, key, value, active, r, g, b, a);
	poi_manager_add_poi_set_ods(poi_manager -> ods_edit, key, value, active, r, g, b, a);
	g_signal_emit (poi_manager, poi_manager_signals[LAYER_ADDED], 0, poi_manager -> ods_base -> poi_sets -> len - 1);
}

/****************************************************************************************************
* remove a PoiSet from the set of PoiSets
****************************************************************************************************/
gboolean poi_manager_tree_delete__iter(gpointer node_id_p, gpointer node_p, gpointer data_p);

void poi_manager_delete_poi_set(PoiManager * poi_manager, int index)
{
	StyledPoiSet * poi_set = poi_manager_get_poi_set(poi_manager, index);
	StyledPoiSet * poi_set_edit = poi_manager_get_poi_set_edit(poi_manager, index);
	g_tree_foreach(GOSM_POI_SET(poi_set) -> points, poi_manager_tree_delete__iter, (gpointer)poi_manager -> ods_base);
	g_tree_foreach(GOSM_POI_SET(poi_set_edit) -> points, poi_manager_tree_delete__iter, (gpointer)poi_manager -> ods_edit);
	poi_set_clear(GOSM_POI_SET(poi_set));
	poi_set_clear(GOSM_POI_SET(poi_set_edit));
	g_array_remove_index(poi_manager -> ods_base -> poi_sets, index);
	g_array_remove_index(poi_manager -> ods_edit -> poi_sets, index);
	g_signal_emit (poi_manager, poi_manager_signals[LAYER_DELETED], 0, index);
}

gboolean poi_manager_tree_delete__iter(gpointer node_id_p, gpointer node_p, gpointer data_p)
{
	OsmDataSet * ods = GOSM_OSM_DATA_SET(data_p);
	Node * node = (Node*) node_p;
	node -> refs -= 1;
	if (node -> refs == 0){
		poi_set_add(ods -> remaining_pois, node);
	}
	return FALSE;
}

/****************************************************************************************************
* insert all pois, that are currently available into the given PoiSet
****************************************************************************************************/
void poi_manager_fill_poi_set(OsmDataSet * ods, PoiSet * poi_set)
{
	char * key = named_poi_set_get_key(GOSM_NAMED_POI_SET(poi_set));
	char * value = named_poi_set_get_value(GOSM_NAMED_POI_SET(poi_set));
	GSequence * ids = tag_tree_get_nodes(ods -> tag_tree, key, value);
	if (ids == NULL) return;
	GSequenceIter * iter = g_sequence_get_begin_iter(ids);
	while(!g_sequence_iter_is_end(iter)){
		int id = *(int*)g_sequence_get(iter);
		Node * node = g_tree_lookup(ods -> tree_ids, &id);
		node -> refs += 1;
		poi_set_add(GOSM_POI_SET(poi_set), node);
		//TODO: is it right, that it's not checked whether this is in remaining pois?
		poi_set_remove_point(ods -> remaining_pois, id, node -> lon, node -> lat);
		iter = g_sequence_iter_next(iter);
	}
}

/****************************************************************************************************
* toggle the visibility of a PoiSet
****************************************************************************************************/
void poi_manager_toggle_poi_set(PoiManager * poi_manager, int index)
{
	StyledPoiSet * poi_set = poi_manager_get_poi_set(poi_manager, index);
	StyledPoiSet * poi_set_edit = poi_manager_get_poi_set_edit(poi_manager, index);
	gboolean visible = poi_set_get_visible(GOSM_POI_SET(poi_set));
	poi_set_set_visible(GOSM_POI_SET(poi_set), !visible);
	poi_set_set_visible(GOSM_POI_SET(poi_set_edit), !visible);
	g_signal_emit (poi_manager, poi_manager_signals[LAYER_TOGGLED], 0, index);
}

/****************************************************************************************************
* return the number of PoiSets
****************************************************************************************************/
int poi_manager_get_number_of_poi_sets(PoiManager * poi_manager)
{
	return poi_manager -> ods_base -> poi_sets -> len;
}

/****************************************************************************************************
* return the index's PoiSet
****************************************************************************************************/
StyledPoiSet * poi_manager_get_poi_set(PoiManager * poi_manager, int index)
{
	return g_array_index(poi_manager -> ods_base -> poi_sets, StyledPoiSet*, index);
}

/****************************************************************************************************
* return the index's PoiSet
****************************************************************************************************/
StyledPoiSet * poi_manager_get_poi_set_edit(PoiManager * poi_manager, int index)
{
	return g_array_index(poi_manager -> ods_edit -> poi_sets, StyledPoiSet*, index);
}

/****************************************************************************************************
* return the PoiSet by key/value. NULL if there is none
****************************************************************************************************/
StyledPoiSet * poi_manager_get_poi_set_by_tag(PoiManager * poi_manager, OsmDataSet * ods, char * key, char * value)
{
	int nop = poi_manager_get_number_of_poi_sets(poi_manager);
	int n;
	for (n = 0; n < nop; n++){
		NamedPoiSet * poi_set = g_array_index(ods -> poi_sets, NamedPoiSet*, n);
		if (strcmp(poi_set -> key, key) == 0 && strcmp(poi_set -> value, value) == 0){
			return GOSM_STYLED_POI_SET(poi_set);
		}
	}
	return NULL;
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

// insert all nodes into the binary node_id tree
gboolean node_to_id_tree(gpointer k, gpointer v, gpointer data)
{
	PoiManager * poi_manager = GOSM_POI_MANAGER(data);
	g_tree_insert(poi_manager -> ods_base -> tree_ids, int_malloc_p(k), (Node*)v);
	return FALSE;
}
// insert nodes into the poiset
gboolean node_to_poi_set(gpointer k, gpointer v, gpointer data)
{
	PoiSet * poi_set = GOSM_POI_SET(data);
	Node * node = (Node*)v;
	poi_set_add(poi_set, node);
	return FALSE;
}
/****************************************************************************************************
* return a new GTree.
* the key-set of the returned tree is the intersection of both tree's keysets.
* the values in the tree are those, that are referenced by tree1's keys.
****************************************************************************************************/
//TODO: this could be moved into a g_tree_helper-file
gboolean poi_manager_tree_intersection__iter(gpointer k, gpointer v, gpointer data_p);

GTree * poi_manager_tree_intersection(GTree * tree1, GTree * tree2)
{
	GTree * tree = g_tree_new_full(compare_int_pointers, NULL, NULL, NULL);
	gpointer data[2] = {tree, tree1};
	g_tree_foreach(tree2, poi_manager_tree_intersection__iter, (gpointer)data);
	return tree;
}

gboolean poi_manager_tree_intersection__iter(gpointer k, gpointer v, gpointer data_p)
{
	gpointer * data = (gpointer*) data_p;
	GTree * tree = data[0];
	GTree * tree1 = data[1];
	gpointer v1 = g_tree_lookup(tree1, k);
	if (v1 != NULL){
		g_tree_insert(tree, k, v1);
	}
	return FALSE;
}

/****************************************************************************************************
* create a TagTree from a tree of Nodes
****************************************************************************************************/
gboolean poi_manager_build_tag_tree__iter(gpointer node_id_p, gpointer node_p, gpointer tag_tree_p);

TagTree * poi_manager_build_tag_tree(GTree * tree_ids)
{
	TagTree * tag_tree = tag_tree_new();
	g_tree_foreach(tree_ids, poi_manager_build_tag_tree__iter, (gpointer)tag_tree);
	return tag_tree;
}

gboolean poi_manager_build_tag_tree__iter(gpointer node_id_p, gpointer node_p, gpointer tag_tree_p)
{
	int node_id = *(int*) node_id_p;
	Node * node = (Node*) node_p;
	TagTree * tag_tree = GOSM_TAG_TREE(tag_tree_p);
	//TODO: let TagTree use Node instead of id+tags
	tag_tree_add_node(tag_tree, node_id, node);
	return FALSE;
}

/****************************************************************************************************
* remove the nodes contained in tree_ids from poi_set
****************************************************************************************************/
gboolean poi_manager_remove_nodes_from_poi_set__iter(gpointer node_id_p, gpointer node_p, gpointer poi_set_p);

poi_manager_remove_nodes_from_poi_set(PoiSet * poi_set, GTree * tree_ids)
{
	g_tree_foreach(tree_ids, poi_manager_remove_nodes_from_poi_set__iter, poi_set);
}

gboolean poi_manager_remove_nodes_from_poi_set__iter(gpointer node_id_p, gpointer node_p, gpointer poi_set_p)
{
	int node_id = *(int*) node_id_p;
	Node * node = (Node*) node_p;
	PoiSet * poi_set = GOSM_POI_SET(poi_set_p);
	//TODO: add a fast flag for not checking. useful for removing from all_pois
	if (poi_set_contains_point(poi_set, node_id)){
		poi_set_remove_point(poi_set, node_id, node -> lon, node -> lat);
	}
	return FALSE;
}

/****************************************************************************************************
* remove the given nodes from poi_manager's tree_ids
****************************************************************************************************/
gboolean poi_manager_remove_nodes_from_tree_ids__iter(gpointer node_id_p, gpointer node_p, gpointer tree_ids_p);

poi_manager_remove_nodes_from_tree_ids(PoiManager * poi_manager, GTree * tree_ids)
{
	g_tree_foreach(tree_ids, poi_manager_remove_nodes_from_tree_ids__iter,
		(gpointer)poi_manager -> ods_base -> tree_ids);
}

gboolean poi_manager_remove_nodes_from_tree_ids__iter(gpointer node_id_p, gpointer node_p, gpointer tree_ids_p)
{
	GTree * tree_ids = (GTree*) tree_ids_p;
	g_tree_remove(tree_ids, node_id_p);
	return FALSE;
}

/****************************************************************************************************
* remove a tree of nodes from PoiManager
****************************************************************************************************/
void poi_manager_remove_nodes(PoiManager * poi_manager, GTree * tree_ids_old)
{
	TagTree * tag_tree_old = poi_manager_build_tag_tree(tree_ids_old);
	/* remove from PoiSets */
	int num_poi_sets = poi_manager_get_number_of_poi_sets(poi_manager); int n;
	for (n = 0; n < num_poi_sets; n++){
		PoiSet * poi_set = GOSM_POI_SET(poi_manager_get_poi_set(poi_manager, n));
		char * key = named_poi_set_get_key(GOSM_NAMED_POI_SET(poi_set));
		char * val = named_poi_set_get_value(GOSM_NAMED_POI_SET(poi_set));
		GSequence * elements = tag_tree_get_nodes(tag_tree_old, key, val);
		if (elements != NULL){
			GSequenceIter * iter = g_sequence_get_begin_iter(elements);
			while(!g_sequence_iter_is_end(iter)){
				gpointer id_p = g_sequence_get(iter);
				int id = *(int*)id_p;
				Node * node = g_tree_lookup(tree_ids_old, id_p);
				node -> refs -= 1;
				poi_set_remove_point(poi_set, id, node -> lon, node -> lat);
				iter = g_sequence_iter_next(iter);
			}
		}
	}
	/* remove from all_pois */
	poi_manager_remove_nodes_from_poi_set(poi_manager -> ods_base -> all_pois, tree_ids_old);
	poi_manager_remove_nodes_from_poi_set(poi_manager -> ods_base -> remaining_pois, tree_ids_old);
	/* remove from tag_tree */
	tag_tree_subtract_tag_tree(poi_manager -> ods_base -> tag_tree, tag_tree_old);
	/* remove from tree_ids */
	poi_manager_remove_nodes_from_tree_ids(poi_manager, tree_ids_old);
	tag_tree_destroy(tag_tree_old);
}

/****************************************************************************************************
* add a tree of nodes to PoiManager
****************************************************************************************************/
poi_manager_add_nodes(PoiManager * poi_manager, GTree * tree_ids_new)
{
	/* add to tree_ids */
	g_tree_foreach(tree_ids_new, node_to_id_tree, (gpointer)poi_manager);
	/* keep track of remaining nodes TODO: copy of tree inefficient like this*/
	GTree * tree_remaining = poi_manager_tree_intersection(tree_ids_new, tree_ids_new);
	printf("size of remaining %d\n", g_tree_nnodes(tree_remaining));
	/* add to tag_tree */
	TagTree * tag_tree = poi_manager_build_tag_tree(tree_ids_new);
	tag_tree_add_tag_tree(poi_manager -> ods_base -> tag_tree, tag_tree);
	/* add to all_pois */
	g_tree_foreach(tree_ids_new, node_to_poi_set, (gpointer) poi_manager -> ods_base -> all_pois);
	/* add to PoiSets */
	int num_poi_sets = poi_manager_get_number_of_poi_sets(poi_manager); int n;
	for (n = 0; n < num_poi_sets; n++){
		PoiSet * poi_set = GOSM_POI_SET(poi_manager_get_poi_set(poi_manager, n));
		char * key = named_poi_set_get_key(GOSM_NAMED_POI_SET(poi_set));
		char * val = named_poi_set_get_value(GOSM_NAMED_POI_SET(poi_set));
		GSequence * elements = tag_tree_get_nodes(tag_tree, key, val);
		if (elements != NULL){
			GSequenceIter * iter = g_sequence_get_begin_iter(elements);
			while(!g_sequence_iter_is_end(iter)){
				gpointer id_p = g_sequence_get(iter);
				int id = *(int*)id_p;
				Node * node = g_tree_lookup(poi_manager -> ods_base -> tree_ids, id_p);
				node -> refs += 1;
				poi_set_add(poi_set, node);
				g_tree_remove(tree_remaining, id_p);
				iter = g_sequence_iter_next(iter);
			}
		}
	}
	printf("size of remaining %d\n", g_tree_nnodes(tree_remaining));
	g_tree_foreach(tree_remaining, node_to_poi_set, (gpointer) poi_manager -> ods_base -> remaining_pois);
	tag_tree_destroy(tag_tree);
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
	/* build intersection tree to update already present node_ids */
	GTree * tree_ids_old = poi_manager_tree_intersection(poi_manager -> ods_base -> tree_ids, osm_reader -> tree_ids);
	printf("number of reloaded ids %d\n", g_tree_nnodes(tree_ids_old));
	/* remove old nodes */
	poi_manager_remove_nodes(poi_manager, tree_ids_old);
	g_tree_destroy(tree_ids_old);
	/* add new nodes */
	poi_manager_add_nodes(poi_manager, osm_reader -> tree_ids);
	/* copy to edit layer */
	osm_data_set_duplicate(poi_manager -> ods_base, poi_manager -> ods_edit);
	poi_manager_apply_change_history(poi_manager);
	/* emit signals */
	g_signal_emit (poi_manager, poi_manager_signals[FILE_PARSING_ENDED], 0);
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
	pthread_mutex_lock(&(poi_manager -> mutex_pois));
	poi_manager -> map_area -> poi_active_id = 0;
	if (status != 0){
		pthread_mutex_unlock(&(poi_manager -> mutex_pois));
		g_signal_emit (poi_manager, poi_manager_signals[API_REQUEST_ENDED], 0, status);
		return FALSE;
	}
	double min_lon = poi_manager -> bbox_api_query.min_lon;
	double min_lat = poi_manager -> bbox_api_query.min_lat;
	double max_lon = poi_manager -> bbox_api_query.max_lon;
	double max_lat = poi_manager -> bbox_api_query.max_lat;
	/* remove all pois that have been in the requested area before */
	GArray * points = poi_set_get(poi_manager -> ods_base -> all_pois, min_lon, min_lat, max_lon, max_lat);
	GTree * tree_ids_area = g_tree_new_full(compare_int_pointers, NULL, NULL, NULL);
	int i; for (i = 0; i < points -> len; i++){
		LonLatPairId * llpi = &g_array_index(points, LonLatPairId, i);
		Node * node = g_tree_lookup(poi_manager -> ods_base -> tree_ids, &(llpi -> node_id));
		if (node == NULL) printf("NULL\n");
		g_tree_insert(tree_ids_area, &(llpi -> node_id), node);
	}
	printf("number of area ids removed %d\n", g_tree_nnodes(tree_ids_area));
	/* remove area nodes */
	poi_manager_remove_nodes(poi_manager, tree_ids_area);
	g_array_free(points, TRUE);
	g_tree_destroy(tree_ids_area);
	/* build intersection tree to update already present node_ids */
	GTree * tree_ids_old = poi_manager_tree_intersection(poi_manager -> ods_base -> tree_ids, osm_reader -> tree_ids);
	printf("number of reloaded ids %d\n", g_tree_nnodes(tree_ids_old));
	/* remove old nodes */
	poi_manager_remove_nodes(poi_manager, tree_ids_old);
	g_tree_destroy(tree_ids_old);
	/* add new nodes */
	poi_manager_add_nodes(poi_manager, osm_reader -> tree_ids);
	/* copy to edit layer */
	osm_data_set_duplicate(poi_manager -> ods_base, poi_manager -> ods_edit);
	poi_manager_apply_change_history(poi_manager);
	printf("finished processing\n");
	pthread_mutex_unlock(&(poi_manager -> mutex_pois));
	/* emit signals */
	g_signal_emit (poi_manager, poi_manager_signals[API_REQUEST_ENDED], 0, status);
	g_signal_emit (poi_manager, poi_manager_signals[SOURCE_ACTIVATED], 0, poi_manager -> active_poi_source);
	return FALSE;
}

/****************************************************************************************************
* remove all nodes that are in the PoiManager currently
****************************************************************************************************/
void poi_manager_clear_pois(PoiManager * poi_manager)
{
	// TODO: this could abviously be done more efficient, since we don't have to build the tag_tree
	// for just removing all nodes. but this currently is kind of a test of consistency of data-
	// structures
	pthread_mutex_lock(&(poi_manager -> mutex_pois));
	poi_manager_remove_nodes(poi_manager, poi_manager -> ods_base -> tree_ids);
	osm_data_set_duplicate(poi_manager -> ods_base, poi_manager -> ods_edit);
	//poi_manager_apply_change_history(poi_manager);
	pthread_mutex_unlock(&(poi_manager -> mutex_pois));
	poi_manager_activate_poi_source(poi_manager, -1);
	g_signal_emit (poi_manager, poi_manager_signals[SOURCE_ACTIVATED], 0, poi_manager -> active_poi_source);
	int len = poi_manager -> changes -> len;
	if (len > 0){
		g_array_remove_range(poi_manager -> changes, 0, len);
		poi_manager -> change_index = -1;
		int x[3] = {len, 0, len};
		g_signal_emit (poi_manager, poi_manager_signals[ACTION_REMOVE_MULTIPLE], 0, x);
	}
}

/****************************************************************************************************
* change a PoiSets' colour
****************************************************************************************************/
void poi_manager_set_poi_set_colour(PoiManager * poi_manager, int index, double r, double g, double b, double a)
{
	StyledPoiSet * poi_set = g_array_index(poi_manager -> ods_base -> poi_sets, StyledPoiSet*, index);
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
	for (i = 0; i < poi_manager -> ods_base -> poi_sets -> len; i++){
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
	Node * node = (Node*) g_tree_lookup(poi_manager-> ods_edit -> tree_ids, &node_id);
	GHashTableIter iter;
	g_hash_table_iter_init(&iter, node -> tags);
	gpointer key, val;
	while(g_hash_table_iter_next(&iter, &key, &val)){
		printf("%s:%s\n", (char*)key, (char*)val);
	}
}

/****************************************************************************************************
* find a node
****************************************************************************************************/
Node * poi_manager_get_node(PoiManager * poi_manager, int node_id)
{
	Node * node = (Node*) g_tree_lookup(poi_manager -> ods_edit -> tree_ids, &node_id);
	return node;
}

/****************************************************************************************************
* update the position of a node
****************************************************************************************************/
void poi_manager_reposition(PoiManager * poi_manager, int node_id, double lon, double lat)
{
	Node * node = (Node*) g_tree_lookup(poi_manager -> ods_edit -> tree_ids, &(node_id));
	poi_set_reposition(poi_manager -> ods_edit -> all_pois, node_id, lon, lat);
	int num_poi_sets = poi_manager_get_number_of_poi_sets(poi_manager);
	int poi;
	for (poi = 0; poi <= num_poi_sets; poi++){
		PoiSet * poi_set = poi < num_poi_sets
			? GOSM_POI_SET(poi_manager_get_poi_set_edit(poi_manager, poi))
			: GOSM_POI_SET(poi_manager -> ods_edit -> remaining_pois);
		if (poi_set_contains_point(poi_set, node_id)){
			poi_set_reposition(poi_set, node_id, lon, lat);
		}
	}
	node -> lon = lon;
	node -> lat = lat;
}

/****************************************************************************************************
*****************************************************************************************************
* Here are functions that deal with the change-history
*****************************************************************************************************
****************************************************************************************************/

void poi_manager_apply_change(PoiManager * poi_manager, EditAction * action)
{
	int id = G_OBJECT_TYPE(action);
	if(id == GOSM_TYPE_EDIT_ACTION_ADD_NODE){
		EditActionAddNode * eaan = GOSM_EDIT_ACTION_ADD_NODE(action);
		poi_manager_add_node_id(poi_manager, FALSE, action -> node_id, eaan -> lon, eaan -> lat);
	}
	if(id == GOSM_TYPE_EDIT_ACTION_REMOVE_NODE){
		EditActionRemoveNode * earn = GOSM_EDIT_ACTION_REMOVE_NODE(action);
		if (poi_set_contains_point(poi_manager -> ods_edit -> all_pois, action -> node_id)){
			poi_manager_remove_node(poi_manager, FALSE, action -> node_id);
		}
	}
	if(id == GOSM_TYPE_EDIT_ACTION_CHANGE_POSITION){
		EditActionChangePosition * eacp = GOSM_EDIT_ACTION_CHANGE_POSITION(action);
		if (poi_set_contains_point(poi_manager -> ods_edit -> all_pois, action -> node_id)){
			poi_manager_reposition(poi_manager, action -> node_id, eacp -> lon, eacp -> lat);
		}
	}
	if(id == GOSM_TYPE_EDIT_ACTION_ADD_TAG){
		EditActionAddTag * eaat = GOSM_EDIT_ACTION_ADD_TAG(action);
		if (poi_set_contains_point(poi_manager -> ods_edit -> all_pois, action -> node_id)){
			poi_manager_add_tag(poi_manager, FALSE, action -> node_id, eaat -> key, eaat -> value);
		}
	}
	if(id == GOSM_TYPE_EDIT_ACTION_CHANGE_TAG_KEY){
		EditActionChangeTagKey * eactk = GOSM_EDIT_ACTION_CHANGE_TAG_KEY(action);
		if (poi_set_contains_point(poi_manager -> ods_edit -> all_pois, action -> node_id)){
			poi_manager_change_tag_key(poi_manager, FALSE, action -> node_id, eactk -> old_key, eactk -> new_key);
		}
	}
	if(id == GOSM_TYPE_EDIT_ACTION_CHANGE_TAG_VALUE){
		EditActionChangeTagValue * eactv = GOSM_EDIT_ACTION_CHANGE_TAG_VALUE(action);
		if (poi_set_contains_point(poi_manager -> ods_edit -> all_pois, action -> node_id)){
			poi_manager_change_tag_value(poi_manager, FALSE, action -> node_id, eactv -> key, eactv -> value);
		}
	}
	if(id == GOSM_TYPE_EDIT_ACTION_REMOVE_TAG){
		EditActionRemoveTag * eart = GOSM_EDIT_ACTION_REMOVE_TAG(action);
		if (poi_set_contains_point(poi_manager -> ods_edit -> all_pois, action -> node_id)){
			poi_manager_remove_tag(poi_manager, FALSE, action -> node_id, eart -> key, eart -> value);
		}
	}
}

/****************************************************************************************************
* the list of changes will be applied to the base dataset to reflect the changes made by the user
****************************************************************************************************/
void poi_manager_apply_change_history(PoiManager * poi_manager)
{
	int s;
	for (s = 0; s < poi_manager -> changes -> len; s++){
		EditAction * action = g_array_index(poi_manager -> changes, EditAction*, s);
		poi_manager_apply_change(poi_manager, action);
	}
}

/****************************************************************************************************
* just print out the stack
****************************************************************************************************/
void poi_manager_print_change_stack(PoiManager * poi_manager)
{
	printf("CHANGE STACK\n");
	int s;
	for (s = 0; s < poi_manager -> changes -> len; s++){
		EditAction * action = g_array_index(poi_manager -> changes, EditAction*, s);
		edit_action_print(action);
	}
}

/****************************************************************************************************
*****************************************************************************************************
* Here follow the function that are used for making changes to the original data
*****************************************************************************************************
****************************************************************************************************/

/****************************************************************************************************
* add an action to the list of changes
****************************************************************************************************/
void poi_manager_add_action(PoiManager * poi_manager, EditAction * action)
{
	printf("%d %d\n", poi_manager -> change_index, poi_manager -> changes -> len);
	//TODO: weird: this doesn't work all time if "-1" on the right instead of "+1" on the left
	if (poi_manager -> change_index + 1 < poi_manager -> changes -> len){
		int i = poi_manager -> change_index + 1;
		int l = poi_manager -> changes -> len - 1 - poi_manager -> change_index;
		int x[3] = {poi_manager -> changes -> len, i, l};
		printf("removing %d %d\n", i, l);
		g_array_remove_range(poi_manager -> changes, i, l);
		g_signal_emit (poi_manager, poi_manager_signals[ACTION_REMOVE_MULTIPLE], 0, x);
	}
	g_array_append_val(poi_manager -> changes, action);
	poi_manager_print_change_stack(poi_manager);
	poi_manager -> change_index++;
	g_signal_emit (poi_manager, poi_manager_signals[ACTION_ADDED], 0, action);
}

/****************************************************************************************************
* add a node, use the next available id
****************************************************************************************************/
int poi_manager_add_node(PoiManager * poi_manager, double lon, double lat)
{
	/* get the next available node_id */
	int node_id = poi_manager -> next_node_id --;
	/* and insert the new node with this id */
	poi_manager_add_node_id(poi_manager, TRUE, node_id, lon, lat);
	return node_id;
}

/****************************************************************************************************
* add a node, use the given id
****************************************************************************************************/
void poi_manager_add_node_id(PoiManager * poi_manager, gboolean history, int node_id, double lon, double lat)
{
	Node * node = node_new(node_id, 1, lon, lat);
	g_tree_insert(poi_manager -> ods_edit -> tree_ids, int_malloc(node_id), node);
	poi_set_add(poi_manager -> ods_edit -> all_pois, node);
	poi_set_add(poi_manager -> ods_edit -> remaining_pois, node);
	if (history){
		EditAction * action = edit_action_add_node_new(node_id, lon, lat);
		poi_manager_add_action(poi_manager, action);
	}
}

/****************************************************************************************************
* remove the given node
****************************************************************************************************/
void poi_manager_remove_node(PoiManager * poi_manager, gboolean history, int node_id)
{
	Node * node = g_tree_lookup(poi_manager -> ods_edit -> tree_ids, &node_id);
	int num_poi_sets = poi_manager_get_number_of_poi_sets(poi_manager);
	int poi;
	poi_set_remove_point(GOSM_POI_SET(poi_manager -> ods_edit -> all_pois), node_id, node -> lon, node -> lat);
	for (poi = 0; poi <= num_poi_sets; poi++){
		PoiSet * poi_set = poi < num_poi_sets
			? GOSM_POI_SET(poi_manager_get_poi_set_edit(poi_manager, poi))
			: GOSM_POI_SET(poi_manager -> ods_edit -> remaining_pois);
		if (poi_set_contains_point(poi_set, node_id)){
			poi_set_remove_point(poi_set, node_id, node -> lon, node -> lat);
		}
	}
        tag_tree_subtract_node(poi_manager -> ods_edit -> tag_tree, node_id, node);
	g_tree_remove(poi_manager -> ods_edit -> tree_ids, &node_id);
	if (history){
		EditAction * action = edit_action_remove_node_new(node_id);
		poi_manager_add_action(poi_manager, action);
	}
}

/****************************************************************************************************
* utility function that is used, when an edit has been made:
* add the node to the poi-set referenced by key/value, if it exists. if the node has been in the
* special poi-set remaining-pois, remove it from there, since it's in a poi-set now.
****************************************************************************************************/
void poi_manager_add_node_to_poi_set_if_exists(PoiManager * poi_manager, int node_id, char * key, char * value)
{
	StyledPoiSet * poi_set = poi_manager_get_poi_set_by_tag(poi_manager, poi_manager -> ods_edit, key, value);
	if (poi_set != NULL){
		Node * node = (Node*)g_tree_lookup(poi_manager -> ods_edit -> tree_ids, &node_id);
		poi_set_add(GOSM_POI_SET(poi_set), node);
		node -> refs += 1;
		if (poi_set_contains_point(poi_manager -> ods_edit -> remaining_pois, node_id)){
			poi_set_remove_node(GOSM_POI_SET(poi_manager -> ods_edit -> remaining_pois), node_id);
		}
	}
}

/****************************************************************************************************
* utility function that is used, when an edit has been made:
* remove the node from the poi-set referenced by key/value, if it exists. if the node ist not in any
* other poi-set anymore, put it into the special poi-set remaining-pois.
****************************************************************************************************/
void poi_manager_remove_node_from_poi_set(PoiManager * poi_manager, int node_id, char * key, char * value)
{
	StyledPoiSet * poi_set = poi_manager_get_poi_set_by_tag(poi_manager, poi_manager -> ods_edit, key, value);
	if (poi_set != NULL){
		Node * node = (Node*)g_tree_lookup(poi_manager -> ods_edit -> tree_ids, &node_id);
		poi_set_remove_node(GOSM_POI_SET(poi_set), node_id);
		node -> refs -= 1;
		if (node -> refs == 0){
			poi_set_add(GOSM_POI_SET(poi_manager -> ods_edit -> remaining_pois), node);
		}
	}
}

/****************************************************************************************************
* add a tag to an existing node. if the key already exists, replace it
****************************************************************************************************/
void poi_manager_add_tag(PoiManager * poi_manager, gboolean history, int node_id, char * key, char * value)
{
	Node * node = g_tree_lookup(poi_manager -> ods_edit -> tree_ids, &node_id);
	gpointer lookup = g_hash_table_lookup(node -> tags, key);
	//TODO: move some things to Node
	if (lookup != NULL){
		/* if the key was already used in this node */
		poi_manager_change_tag_value(poi_manager, history, node_id, key, value);
	}else{
		/* if the key was not used in this node before */
		g_hash_table_insert(node -> tags, g_strdup(key), g_strdup(value));
		tag_tree_add_node_tag(poi_manager -> ods_edit -> tag_tree, node_id, key, value);
		poi_manager_add_node_to_poi_set_if_exists(poi_manager, node_id, key, value);
		if (history){
			EditAction * action = edit_action_add_tag_new(node_id, key, value);
			poi_manager_add_action(poi_manager, action);
			g_signal_emit (poi_manager, poi_manager_signals[NODE_TAG_ADDED], 0, node_id);
		}
	}
}

/****************************************************************************************************
* given a tag, change the key. i.e: remove the old tag and add a new one for the new key with the
* old value
****************************************************************************************************/
void poi_manager_change_tag_key(PoiManager * poi_manager, gboolean history, int node_id, char * old_key, char * key)
{
	Node * node = g_tree_lookup(poi_manager -> ods_edit -> tree_ids, &node_id);
	char * value = g_strdup((char*)g_hash_table_lookup(node -> tags, old_key));
	tag_tree_subtract_node_tag(poi_manager -> ods_edit -> tag_tree, node_id, old_key, value);
	g_hash_table_remove(node -> tags, old_key);
	poi_manager_remove_node_from_poi_set(poi_manager, node_id, old_key, value);
	poi_manager_add_tag(poi_manager, FALSE, node_id, key, value);
	if (history){
		EditAction * action = edit_action_change_tag_key_new(node_id, g_strdup(old_key), g_strdup(key));
		poi_manager_add_action(poi_manager, action);
		g_signal_emit (poi_manager, poi_manager_signals[NODE_TAG_CHANGED], 0, node_id);
	}
}

/****************************************************************************************************
* given a tag, change the value.
****************************************************************************************************/
void poi_manager_change_tag_value(PoiManager * poi_manager, gboolean history, int node_id, char * key, char * value)
{
	Node * node = g_tree_lookup(poi_manager -> ods_edit -> tree_ids, &node_id);
	char * old_value = (char*)g_hash_table_lookup(node -> tags, key);
	tag_tree_subtract_node_tag(poi_manager -> ods_edit -> tag_tree, node_id, key, old_value);
	poi_manager_remove_node_from_poi_set(poi_manager, node_id, key, old_value);
	g_hash_table_replace(node -> tags, g_strdup(key), g_strdup(value)); /* value and key are replaced */
	tag_tree_add_node_tag(poi_manager -> ods_edit -> tag_tree, node_id, g_strdup(key), g_strdup(value));
	poi_manager_add_node_to_poi_set_if_exists(poi_manager, node_id, key, value);
	if (history){
		EditAction * action = edit_action_change_tag_value_new(node_id, g_strdup(key), g_strdup(value));
		poi_manager_add_action(poi_manager, action);
		g_signal_emit (poi_manager, poi_manager_signals[NODE_TAG_CHANGED], 0, node_id);
	}
}

/****************************************************************************************************
* remove the given tag from this node
****************************************************************************************************/
void poi_manager_remove_tag(PoiManager * poi_manager, gboolean history, int node_id, char * key, char * value)
{
	Node * node = g_tree_lookup(poi_manager -> ods_edit -> tree_ids, &node_id);
	g_hash_table_remove(node -> tags, key);
	tag_tree_subtract_node_tag(poi_manager -> ods_edit -> tag_tree, node_id, key, value);
	poi_manager_remove_node_from_poi_set(poi_manager, node_id, key, value);
	if (history){
		EditAction * action = edit_action_remove_tag_new(node_id, key, value);
		poi_manager_add_action(poi_manager, action);
		g_signal_emit (poi_manager, poi_manager_signals[NODE_TAG_REMOVED], 0, node_id);
	}
}

void poi_manager_reposition_finished(PoiManager * poi_manager, int node_id, double lon, double lat)
{
	EditAction * action = edit_action_change_position_new(node_id, lon, lat);
	poi_manager_add_action(poi_manager, action);
}

gboolean poi_manager_can_add_tag(PoiManager * poi_manager)
{
	return poi_manager -> map_area -> poi_selected_id != 0;
}

int poi_manager_get_selected_node_id(PoiManager * poi_manager)
{
	return poi_manager -> map_area -> poi_selected_id;
}

void poi_manager_undo_action_by_invoking_actions(PoiManager * poi_manager, int index, gboolean remove)
{
	EditAction * action = g_array_index(poi_manager -> changes, EditAction*, index);
	int node_id = action -> node_id;
	int i;
	if(remove){
		tag_tree_subtract_node(poi_manager -> ods_edit -> tag_tree, node_id, 
			g_tree_lookup(poi_manager -> ods_edit -> tree_ids, &node_id));
		for (i = 0; i < poi_manager -> ods_edit -> poi_sets -> len; i++){
			PoiSet * poi_set = g_array_index(poi_manager -> ods_edit -> poi_sets, PoiSet*, i);
			if (poi_set_contains_point(poi_set, node_id)){
				poi_set_remove_node(poi_set, node_id);
			}
		}
		if (poi_set_contains_point(poi_manager -> ods_edit -> remaining_pois, node_id)){
			poi_set_remove_node(GOSM_POI_SET(poi_manager -> ods_edit -> remaining_pois), node_id);
		}
		if (poi_set_contains_point(poi_manager -> ods_edit -> all_pois, node_id)){
			poi_set_remove_node(GOSM_POI_SET(poi_manager -> ods_edit -> all_pois), node_id);
		}
	}
	if (node_id > 0){
		osm_data_set_duplicate_node(poi_manager -> ods_base, poi_manager -> ods_edit, node_id);
	}
	for (i = 0; i < index; i++){
		EditAction * action = g_array_index(poi_manager -> changes, EditAction*, i);
		if (action -> node_id == node_id){
			poi_manager_apply_change(poi_manager, action);
		}
	}
}

void poi_manager_undo_action(PoiManager * poi_manager, int index)
{
	EditAction * action = g_array_index(poi_manager -> changes, EditAction*, index);
	int id = G_OBJECT_TYPE(action);
	if(id == GOSM_TYPE_EDIT_ACTION_ADD_NODE){
		EditActionAddNode * eaan = GOSM_EDIT_ACTION_ADD_NODE(action);
		poi_set_remove_node(poi_manager -> ods_edit -> all_pois, action -> node_id);
		poi_set_remove_node(poi_manager -> ods_edit -> remaining_pois, action -> node_id);
		g_tree_remove(poi_manager -> ods_edit -> tree_ids, &(action -> node_id));
	}
	if(id == GOSM_TYPE_EDIT_ACTION_REMOVE_NODE){
		poi_manager_undo_action_by_invoking_actions(poi_manager, index, FALSE);
	}
	if(id == GOSM_TYPE_EDIT_ACTION_CHANGE_POSITION){
		poi_manager_undo_action_by_invoking_actions(poi_manager, index, TRUE);
	}
	if(id == GOSM_TYPE_EDIT_ACTION_ADD_TAG){
		poi_manager_undo_action_by_invoking_actions(poi_manager, index, TRUE);
	}
	if(id == GOSM_TYPE_EDIT_ACTION_CHANGE_TAG_KEY){
		poi_manager_undo_action_by_invoking_actions(poi_manager, index, TRUE);
	}
	if(id == GOSM_TYPE_EDIT_ACTION_CHANGE_TAG_VALUE){
		poi_manager_undo_action_by_invoking_actions(poi_manager, index, TRUE);
	}
	if(id == GOSM_TYPE_EDIT_ACTION_REMOVE_TAG){
		poi_manager_undo_action_by_invoking_actions(poi_manager, index, TRUE);
	}
}
void poi_manager_undo(PoiManager * poi_manager)
{
	if (poi_manager -> change_index > -1){
		poi_manager_undo_action(poi_manager, poi_manager -> change_index);
		poi_manager -> change_index--;
		g_signal_emit(poi_manager, poi_manager_signals[ACTION_UNDO], 0, poi_manager -> change_index + 1);
	}
}

void poi_manager_redo(PoiManager * poi_manager)
{
	if (poi_manager -> change_index + 1 < poi_manager -> changes -> len){
		poi_manager -> change_index ++;
		EditAction * action = g_array_index(poi_manager -> changes, EditAction*, poi_manager -> change_index);
		poi_manager_apply_change(poi_manager, action);
		g_signal_emit(poi_manager, poi_manager_signals[ACTION_REDO], 0, poi_manager -> change_index);
	}
}

gboolean poi_manager_node_exists(PoiManager * poi_manager, int node_id)
{
	return poi_set_contains_point(poi_manager -> ods_edit -> all_pois, node_id);
}

void poi_manager_save(PoiManager * poi_manager)
{
	SortedSequence * nodes_new = sorted_sequence_new(free, compare_int_pointers);
	SortedSequence * nodes_mod = sorted_sequence_new(free, compare_int_pointers);
	SortedSequence * nodes_del = sorted_sequence_new(free, compare_int_pointers);
	int i;
	for (i = 0; i <= poi_manager -> change_index; i++){
		EditAction * action = g_array_index(poi_manager -> changes, EditAction*, i);
		int type = G_OBJECT_TYPE(action);
		int node_id = action -> node_id;
		if(type == GOSM_TYPE_EDIT_ACTION_REMOVE_NODE){
			if (node_id < 0){
				sorted_sequence_remove(nodes_new, &node_id);
			}else{
				sorted_sequence_remove(nodes_mod, &node_id);
				sorted_sequence_insert(nodes_del, int_malloc(node_id));
			}
		}else{
			if (node_id < 0){
				if (!sorted_sequence_contains(nodes_new, &node_id)){
					sorted_sequence_insert(nodes_new, int_malloc(node_id));
				}
			}else{
				if (!sorted_sequence_contains(nodes_mod, &node_id)){
					sorted_sequence_insert(nodes_mod, int_malloc(node_id));
				}
			}
		}
	}
	printf("NEW\n");
	for (i = 0; i < sorted_sequence_get_length(nodes_new); i++){
		int node_id = *(int*)sorted_sequence_get(nodes_new, i);
		printf("%d\n", node_id);
	}
	printf("MOD\n");
	for (i = 0; i < sorted_sequence_get_length(nodes_mod); i++){
		int node_id = *(int*)sorted_sequence_get(nodes_mod, i);
		printf("%d\n", node_id);
	}
	printf("DELETE\n");
	for (i = 0; i < sorted_sequence_get_length(nodes_del); i++){
		int node_id = *(int*)sorted_sequence_get(nodes_del, i);
		printf("%d\n", node_id);
	}
	ApiControl * api_control = api_control_new();
	api_control_initialize(api_control);
	int cs = api_control_create_changeset(api_control);
	if (cs < 0) return;
	for (i = 0; i < sorted_sequence_get_length(nodes_new); i++){
		int node_id = *(int*)sorted_sequence_get(nodes_new, i);
		Node * node = g_tree_lookup(poi_manager -> ods_edit -> tree_ids, &node_id);
		printf("id: %d, version: %d\n", node_id, node -> version);
		int api = api_control_create_node(api_control, cs, node);
		if (api < 0){
			//TODO: error
		}else{
			//TODO: update node id
			int new_id = api; int old_id = node_id;
			poi_manager_change_node_id(poi_manager, old_id, new_id);
		}
	}
	for (i = 0; i < sorted_sequence_get_length(nodes_mod); i++){
		int node_id = *(int*)sorted_sequence_get(nodes_mod, i);
		Node * node = g_tree_lookup(poi_manager -> ods_edit -> tree_ids, &node_id);
		printf("id: %d, version: %d\n", node_id, node -> version);
		int api = api_control_change_node(api_control, cs, node);
		if (api < 0){
			//TODO: on failure, inspect changes in remotely changed version
		}else{
		}
	}
	for (i = 0; i < sorted_sequence_get_length(nodes_del); i++){
		int node_id = *(int*)sorted_sequence_get(nodes_del, i);
		Node * node = g_tree_lookup(poi_manager -> ods_base -> tree_ids, &node_id);
		printf("id: %d, version: %d\n", node_id, node -> version);
		int api = api_control_delete_node(api_control, cs, node);
		if (api < 0){
			//TODO: on failure, inspect changes in remotely changed version
		}else{
		}
	}
	api_control_close_changeset(api_control, cs);

	int len = poi_manager -> changes -> len;
	int x[3] = {len, 0, len};
	g_array_remove_range(poi_manager -> changes, 0, len);
	poi_manager -> change_index = -1;
	g_signal_emit (poi_manager, poi_manager_signals[ACTION_REMOVE_MULTIPLE], 0, x);
}

void poi_manager_change_node_id(PoiManager * poi_manager, int old_id, int new_id)
{
	osm_data_set_change_node_id(poi_manager -> ods_edit, old_id, new_id);
}
