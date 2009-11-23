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

#ifndef _OSM_DATA_SET_H_
#define _OSM_DATA_SET_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "tag_tree.h"
#include "poi_set.h"

#define GOSM_TYPE_OSM_DATA_SET           (osm_data_set_get_type ())
#define GOSM_OSM_DATA_SET(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_OSM_DATA_SET, OsmDataSet))
#define GOSM_OSM_DATA_SET_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_OSM_DATA_SET, OsmDataSetClass))
#define GOSM_IS_OSM_DATA_SET(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_OSM_DATA_SET))
#define GOSM_IS_OSM_DATA_SET_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_OSM_DATA_SET))
#define GOSM_OSM_DATA_SET_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_OSM_DATA_SET, OsmDataSetClass))

typedef struct _OsmDataSet        OsmDataSet;
typedef struct _OsmDataSetClass   OsmDataSetClass;

struct _OsmDataSet
{
	GObject parent;

	GTree * tree_ids;
	TagTree * tag_tree;
	PoiSet * all_pois;
	GArray * poi_sets;
	PoiSet * remaining_pois;
};

struct _OsmDataSetClass
{
	GObjectClass parent_class;

	//void (* function_name) (OsmDataSet *osm_data_set);
};

OsmDataSet * osm_data_set_new();

void osm_data_set_duplicate(OsmDataSet * original, OsmDataSet * copy);
void osm_data_set_duplicate_node(OsmDataSet * original, OsmDataSet * copy, int node_id);
void osm_data_set_change_node_id(OsmDataSet * ods, int id_old, int id_new);

#endif /* _OSM_DATA_SET_H_ */
