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

#include "poi_set.h"
#include "named_poi_set.h"
#include "styled_poi_set.h"
#include "osm_reader.h"

#define GOSM_TYPE_POI_MANAGER           (poi_manager_get_type ())
#define GOSM_POI_MANAGER(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_POI_MANAGER, PoiManager))
#define GOSM_POI_MANAGER_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_POI_MANAGER, PoiManagerClass))
#define GOSM_IS_POI_MANAGER(obj)        (G_TYPE_CHECK_INSTANCE ((obj), GOSM_TYPE_POI_MANAGER))
#define GOSM_IS_POI_MANAGER_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_POI_MANAGER))
#define GOSM_POI_MANAGER_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_POI_MANAGER, PoiManagerClass))

typedef struct _PoiManager        PoiManager;
typedef struct _PoiManagerClass   PoiManagerClass;

//typedef struct {
//	char * key;
//	char * value;
//	gboolean active;
//	PoiSet * poi_set;
//} KeyValueBooleanPoiSet;

struct _PoiManager
{
	GObject parent;

	GArray * poi_sets;
	OsmReader * osm_reader;
};

struct _PoiManagerClass
{
	GObjectClass parent_class;

	void (* function_name) (PoiManager *poi_manager);
};

PoiManager * poi_manager_new();

void poi_manager_add_poi_set(PoiManager * poi_manager, char * key, char * value, gboolean active,
	double r, double g, double b, double a);

void poi_manager_toggle_poi_set(PoiManager * poi_manager, char * key, char * value, gboolean active);

int poi_manager_get_number_of_poi_sets(PoiManager * poi_manager);

StyledPoiSet * poi_manager_get_poi_set(PoiManager * poi_manager, int index);

#endif /* _GOSM_POI_MANAGER_H */
