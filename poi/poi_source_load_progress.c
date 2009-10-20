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

#include "poi_source_load_progress.h"

G_DEFINE_TYPE (PoiSourceLoadProgress, poi_source_load_progress, G_TYPE_OBJECT);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint poi_source_load_progress_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, poi_source_load_progress_signals[SIGNAL_NAME_n], 0);

PoiSourceLoadProgress * poi_source_load_progress_new()
{
	PoiSourceLoadProgress * poi_source_load_progress = g_object_new(GOSM_TYPE_POI_SOURCE_LOAD_PROGRESS, NULL);
	return poi_source_load_progress;
}

static void poi_source_load_progress_class_init(PoiSourceLoadProgressClass *class)
{
        /*poi_source_load_progress_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PoiSourceLoadProgressClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void poi_source_load_progress_init(PoiSourceLoadProgress *poi_source_load_progress)
{
}

void poi_source_load_progress_show(
	PoiSourceLoadProgress * poi_source_load_progress, 
	GtkWindow * parent,
	OsmReader * osm_reader
)
{

}

void poi_source_load_progress_destroy(
	PoiSourceLoadProgress * poi_source_load_progress
)
{

}
