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

G_DEFINE_TYPE (PoiManager, poi_manager, G_TYPE_OBJECT);

enum
{
	LAYER_TOGGLED,
        COLOUR_CHANGED,
        KEY_CHANGED,
	VALUE_CHANGED,
	SOURCE_ACTIVATED,
	SOURCE_DEACTIVATED,
	SOURCE_ADDED,
	SOURCE_DELETED,
	FILE_PARSING_STARTED,
	FILE_PARSING_ENDED,
        LAST_SIGNAL
};

static guint poi_manager_signals[LAST_SIGNAL] = { 0 };

gboolean poi_manager_create_default_poi_layers();
gboolean poi_manager_read_poi_sources(PoiManager * poi_manager);
gboolean poi_manager_read_poi_layers(PoiManager * poi_manager);
void poi_manager_fill_poi_set(PoiManager * poi_manager, StyledPoiSet * poi_set);
static gboolean poi_manager_osm_reader_finished_cb(OsmReader * osm_reader, int status, gpointer data);

PoiManager * poi_manager_new()
{
	PoiManager * poi_manager = g_object_new(GOSM_TYPE_POI_MANAGER, NULL);
	poi_manager -> poi_sets = g_array_new(FALSE, FALSE, sizeof(StyledPoiSet*));
	poi_manager -> poi_sources = g_array_new(FALSE, FALSE, sizeof(PoiSource*));
	poi_manager -> active_poi_source = -1;
//	char * filename = GOSM_NAMEFINDER_DIR "res/vienna.short.osm";
//	filename = GOSM_NAMEFINDER_DIR "res/berlin.short.osm";
//	filename = GOSM_NAMEFINDER_DIR "res/vienna.50000.osm";
	poi_manager -> osm_reader = osm_reader_new();
	g_signal_connect(G_OBJECT(poi_manager -> osm_reader),"reading-finished",
			 G_CALLBACK(poi_manager_osm_reader_finished_cb), (gpointer)poi_manager);
//	osm_reader_parse_file(poi_manager -> osm_reader, filename);
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
}

static void poi_manager_init(PoiManager *poi_manager)
{
}

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

gboolean poi_manager_create_default_poi_layers()
{
	char * from = GOSM_POI_DIR "res/poi_layers";
	char * to = config_get_poi_layers_file();
	int ret = copy_file(from, to);
	free(to);
	return ret == 0;
}

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

void poi_manager_add_poi_set(PoiManager * poi_manager, char * key, char * value, gboolean active,
	double r, double g, double b, double a)
{
	StyledPoiSet * poi_set = styled_poi_set_new(key, value, r, g, b, a);
	poi_set_set_visible(GOSM_POI_SET(poi_set), active);
	g_array_append_val(poi_manager -> poi_sets, poi_set);
	poi_manager_fill_poi_set(poi_manager, poi_set);
}

void poi_manager_fill_poi_set(PoiManager * poi_manager, StyledPoiSet * poi_set)
{
	char * key = named_poi_set_get_key(GOSM_NAMED_POI_SET(poi_set));
	char * value = named_poi_set_get_value(GOSM_NAMED_POI_SET(poi_set));
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

void poi_manager_toggle_poi_set(PoiManager * poi_manager, int index)
{
	StyledPoiSet * poi_set = poi_manager_get_poi_set(poi_manager, index);
	gboolean visible = poi_set_get_visible(GOSM_POI_SET(poi_set));
	poi_set_set_visible(GOSM_POI_SET(poi_set), !visible);
	g_signal_emit (poi_manager, poi_manager_signals[LAYER_TOGGLED], 0, index);
}

int poi_manager_get_number_of_poi_sets(PoiManager * poi_manager)
{
	return poi_manager -> poi_sets -> len;
}

StyledPoiSet * poi_manager_get_poi_set(PoiManager * poi_manager, int index)
{
	return g_array_index(poi_manager -> poi_sets, StyledPoiSet*, index);
}

int poi_manager_get_number_of_poi_sources(PoiManager * poi_manager)
{
	return poi_manager -> poi_sources -> len;
}

PoiSource * poi_manager_get_poi_source(PoiManager * poi_manager, int index)
{
	return g_array_index(poi_manager -> poi_sources, PoiSource*, index);
}

void poi_manager_activate_poi_source(PoiManager * poi_manager, int index)
{
	int old = poi_manager -> active_poi_source;
	poi_manager -> active_poi_source = index;
	if (old >= 0 && old != index){
		g_signal_emit (poi_manager, poi_manager_signals[SOURCE_DEACTIVATED], 0, old);
	}
	if (old != index){
		int num_poi_sets = poi_manager_get_number_of_poi_sets(poi_manager);
		osm_reader_clear(poi_manager -> osm_reader);
		int n;
		for (n = 0; n < num_poi_sets; n++){
			StyledPoiSet * poi_set = poi_manager_get_poi_set(poi_manager, n);
			poi_set_clear(GOSM_POI_SET(poi_set));
		}
		if (index >= 0){
			PoiSource * poi_source = poi_manager_get_poi_source(poi_manager, index);
			g_signal_emit (poi_manager, poi_manager_signals[FILE_PARSING_STARTED], 0, index);
			osm_reader_parse_file(poi_manager -> osm_reader, poi_source -> filename);
		}
	}
}

static gboolean poi_manager_osm_reader_finished_cb(OsmReader * osm_reader, int status, gpointer data)
{
	PoiManager * poi_manager = GOSM_POI_MANAGER(data);
	int num_poi_sets = poi_manager_get_number_of_poi_sets(poi_manager);
	int n;
	for (n = 0; n < num_poi_sets; n++){
		StyledPoiSet * poi_set = poi_manager_get_poi_set(poi_manager, n);
		poi_manager_fill_poi_set(poi_manager, poi_set);
	}
	if (poi_manager -> active_poi_source >= 0){
		g_signal_emit (poi_manager, poi_manager_signals[FILE_PARSING_ENDED], 0);
	}
	g_signal_emit (poi_manager, poi_manager_signals[SOURCE_ACTIVATED], 0, poi_manager -> active_poi_source);
	return FALSE;
}

void poi_manager_set_poi_set_colour(PoiManager * poi_manager, int index, double r, double g, double b, double a)
{
	StyledPoiSet * poi_set = g_array_index(poi_manager -> poi_sets, StyledPoiSet*, index);
	styled_poi_set_set_colour(poi_set, r, g, b, a);
	g_signal_emit (poi_manager, poi_manager_signals[COLOUR_CHANGED], 0, index);
}

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

gboolean poi_manager_layers_revert(PoiManager * poi_manager)
{
	printf("reverting poi layers\n");
	return TRUE;
}

gboolean poi_manager_sources_add(PoiManager * poi_manager, char * path)
{
	poi_manager_add_poi_source(poi_manager, path, FALSE);
}

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

void poi_manager_set_map_area(PoiManager * poi_manager, MapArea * map_area)
{
	poi_manager -> map_area = map_area;
}

void poi_manager_api_request(PoiManager * poi_manager)
{
	double min_lon, min_lat, max_lon, max_lat;
	map_area_get_visible_area(poi_manager -> map_area, &min_lon, &min_lat, &max_lon, &max_lat);
	char * buf = get_api_url_get(min_lon, min_lat, max_lon, max_lat);
	printf("getting osm-eata for bbox: %e %e %e %e\n", min_lon, min_lat, max_lon, max_lat);
	printf("%s\n", buf);
	poi_manager_activate_poi_source(poi_manager, -1);
	osm_reader_parse_api_url(poi_manager -> osm_reader, buf);
}
