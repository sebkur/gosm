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

#ifndef _GOSM_POI_MANAGER_H_
#define _GOSM_POI_MANAGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

typedef struct _PoiManager        PoiManager;
typedef struct _PoiManagerClass   PoiManagerClass;

#include "poi_set.h"
#include "named_poi_set.h"
#include "styled_poi_set.h"
#include "osm_reader.h"
#include "../map_area.h"
#include "../map_types.h"
#include "tag_tree.h"

#define GOSM_TYPE_POI_MANAGER           (poi_manager_get_type ())
#define GOSM_POI_MANAGER(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_POI_MANAGER, PoiManager))
#define GOSM_POI_MANAGER_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_POI_MANAGER, PoiManagerClass))
#define GOSM_IS_POI_MANAGER(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_POI_MANAGER))
#define GOSM_IS_POI_MANAGER_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_POI_MANAGER))
#define GOSM_POI_MANAGER_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_POI_MANAGER, PoiManagerClass))

typedef struct {
	char * filename;
	char * basename;
	char * dirname;
	gboolean load_on_startup;
} PoiSource;

struct _PoiManager
{
	GObject parent;

	PoiSet * all_pois;
	GTree * tree_ids;
	TagTree * tag_tree;
	TagTree * tag_tree_new;
	TagTree * tag_tree_insertion;
	BoundingBox bbox_api_query;
	GArray * poi_sets;
	GArray * poi_sources;
	OsmReader * osm_reader;

	int active_poi_source;

	MapArea * map_area;
};

struct _PoiManagerClass
{
	GObjectClass parent_class;

	void (* layer_toggled) (PoiManager *poi_manager, int index);
	void (* layer_added) (PoiManager *poi_manager, int index);
	void (* layer_deleted) (PoiManager *poi_manager, int index);
	void (* colour_changed) (PoiManager *poi_manager, int index);
	void (* key_changed) (PoiManager *poi_manager, int index);
	void (* value_changed) (PoiManager *poi_manager, int index);
	void (* source_activated) (PoiManager *poi_manager, int index);
	void (* source_deactivated) (PoiManager *poi_manager, int index);
	void (* source_added) (PoiManager *poi_manager, int index);
	void (* source_deleted) (PoiManager *poi_manager, int index);
	void (* file_parsing_started) (PoiManager *poi_manager, int index);
	void (* file_parsing_ended) (PoiManager *poi_manager, int index);
	void (* api_request_started) (PoiManager *poi_manager);
	void (* api_request_ended) (PoiManager *poi_manager, int index);
};

PoiManager * poi_manager_new();

void poi_manager_add_poi_set(PoiManager * poi_manager, char * key, char * value, gboolean active,
	double r, double g, double b, double a);
void poi_manager_delete_poi_set(PoiManager * poi_manager, int index);

void poi_manager_toggle_poi_set(PoiManager * poi_manager, int index);

int poi_manager_get_number_of_poi_sets(PoiManager * poi_manager);
StyledPoiSet * poi_manager_get_poi_set(PoiManager * poi_manager, int index);

void poi_manager_set_poi_set_colour(PoiManager * poi_manager, int index, double r, double g, double b, double a);

gboolean poi_manager_layers_save(PoiManager * poi_manager);
gboolean poi_manager_layers_revert(PoiManager * poi_manager);
gboolean poi_manager_sources_add(PoiManager * poi_manager, char * path);
gboolean poi_manager_sources_delete(PoiManager * poi_manager, int index);
gboolean poi_manager_sources_save(PoiManager * poi_manager);

int poi_manager_get_number_of_poi_sources(PoiManager * poi_manager);
PoiSource * poi_manager_get_poi_source(PoiManager * poi_manager, int index);
void poi_manager_activate_poi_source(PoiManager * poi_manager, int index);

void poi_manager_api_request(PoiManager * poi_manager);

void poi_manager_set_map_area(PoiManager * poi_manager, MapArea * map_area);

void poi_manager_print_node_information(PoiManager * poi_manager, int node_id);

LonLatTags * poi_manager_get_node(PoiManager * poi_manager, int node_id);

void poi_manager_clear_pois(PoiManager * poi_manager);

#endif /* _GOSM_POI_MANAGER_H */
