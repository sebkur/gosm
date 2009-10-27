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

#ifndef _POI_SOURCE_LOAD_PROGRESS_H_
#define _POI_SOURCE_LOAD_PROGRESS_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "osm_reader.h"

#define GOSM_TYPE_POI_SOURCE_LOAD_PROGRESS           (poi_source_load_progress_get_type ())
#define GOSM_POI_SOURCE_LOAD_PROGRESS(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_POI_SOURCE_LOAD_PROGRESS, PoiSourceLoadProgress))
#define GOSM_POI_SOURCE_LOAD_PROGRESS_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_POI_SOURCE_LOAD_PROGRESS, PoiSourceLoadProgressClass))
#define GOSM_IS_POI_SOURCE_LOAD_PROGRESS(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_POI_SOURCE_LOAD_PROGRESS))
#define GOSM_IS_POI_SOURCE_LOAD_PROGRESS_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_POI_SOURCE_LOAD_PROGRESS))
#define GOSM_POI_SOURCE_LOAD_PROGRESS_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_POI_SOURCE_LOAD_PROGRESS, PoiSourceLoadProgressClass))

typedef struct _PoiSourceLoadProgress        PoiSourceLoadProgress;
typedef struct _PoiSourceLoadProgressClass   PoiSourceLoadProgressClass;

struct _PoiSourceLoadProgress
{
	GObject parent;

	GtkWidget * dialog;	
	GtkWidget * progress_bar;
	OsmReader * osm_reader;
	gulong h_id;
};

struct _PoiSourceLoadProgressClass
{
	GObjectClass parent_class;

	//void (* function_name) (PoiSourceLoadProgress *poi_source_load_progress);
};

PoiSourceLoadProgress * poi_source_load_progress_new();

void poi_source_load_progress_show(
	PoiSourceLoadProgress * poi_source_load_progress, 
	GtkWindow * parent,
	OsmReader * osm_reader
);

void poi_source_load_progress_destroy(
	PoiSourceLoadProgress * poi_source_load_progress
);

#endif /* _POI_SOURCE_LOAD_PROGRESS_H_ */
