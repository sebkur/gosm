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

#ifndef _POI_SET_H_
#define _POI_SET_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "RTree/index.h"
#include "../map_types.h"

#define GOSM_TYPE_POI_SET           (poi_set_get_type ())
#define GOSM_POI_SET(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_POI_SET, PoiSet))
#define GOSM_POI_SET_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_POI_SET, PoiSetClass))
#define GOSM_IS_POI_SET(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_POI_SET))
#define GOSM_IS_POI_SET_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_POI_SET))
#define GOSM_POI_SET_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_POI_SET, PoiSetClass))

typedef struct _PoiSet        PoiSet;
typedef struct _PoiSetClass   PoiSetClass;

struct _PoiSet
{
	GObject parent;

	struct Node * root;
	GTree * points;
	gboolean visible;

	GArray * results;
	int result_index;
};

struct _PoiSetClass
{
	GObjectClass parent_class;

	//void (* function_name) (PoiSet *poi_set);
};

PoiSet * poi_set_new();
void poi_set_constructor(PoiSet * poi_set);
void poi_set_clear(PoiSet * poi_set);

void poi_set_add(PoiSet * poi_set, double lon, double lat, int id);
void poi_set_clear_area(PoiSet * poi_set, double min_lon, double min_lat, double max_lon, double max_lat);
void poi_set_remove_point(PoiSet * poi_set, double lon, double lat, int node_id);
GArray * poi_set_get(PoiSet * poi_set, double min_lon, double min_lat, double max_lon, double max_lat);
void poi_set_set_visible(PoiSet * poi_set, gboolean visible);
gboolean poi_set_get_visible(PoiSet * poi_set);

#endif /* _POI_SET_H_ */
