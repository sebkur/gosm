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

/****************************************************************************************************
* show a progress bar while a poi-source is beeing loaded from hard-disk
****************************************************************************************************/
G_DEFINE_TYPE (PoiSourceLoadProgress, poi_source_load_progress, G_TYPE_OBJECT);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint poi_source_load_progress_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, poi_source_load_progress_signals[SIGNAL_NAME_n], 0);

/****************************************************************************************************
* constructor
****************************************************************************************************/
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

/****************************************************************************************************
* method declaration
****************************************************************************************************/
static gboolean poi_source_load_progress_progress_cb(OsmReader * osm_reader, int percent, gpointer data);

/****************************************************************************************************
* show the window, borderless
****************************************************************************************************/
void poi_source_load_progress_show(
	PoiSourceLoadProgress * poi_source_load_progress, 
	GtkWindow * parent,
	OsmReader * osm_reader
)
{
	poi_source_load_progress -> dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	poi_source_load_progress -> osm_reader = osm_reader;
	GtkWidget * dialog = poi_source_load_progress -> dialog;
	gtk_window_set_decorated(GTK_WINDOW(dialog), FALSE);
	gtk_window_set_title(GTK_WINDOW(dialog), "Importing OSM file");
	gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER_ON_PARENT);
	gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(parent));
	gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
	gtk_widget_set_size_request(dialog, 300, 20);
	GtkWidget * vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(dialog), vbox);
	poi_source_load_progress -> progress_bar = gtk_progress_bar_new();
	gtk_box_pack_start(GTK_BOX(vbox), poi_source_load_progress -> progress_bar, TRUE, TRUE, 0);
	poi_source_load_progress -> h_id = g_signal_connect(
		G_OBJECT(osm_reader),"reading-progress",
		G_CALLBACK(poi_source_load_progress_progress_cb), (gpointer)poi_source_load_progress);
	gtk_widget_show_all(dialog);
}

/****************************************************************************************************
* destroy, disconnect handlers that were connected to OsmReader for progress
****************************************************************************************************/
void poi_source_load_progress_destroy(
	PoiSourceLoadProgress * poi_source_load_progress
)
{
	g_signal_handler_disconnect(G_OBJECT(poi_source_load_progress -> osm_reader), poi_source_load_progress -> h_id);
	gtk_widget_destroy(poi_source_load_progress -> dialog);
}

/****************************************************************************************************
* when progress has benn emitted by OsmReader
****************************************************************************************************/
static gboolean poi_source_load_progress_progress_cb(OsmReader * osm_reader, int percent, gpointer data)
{
	gdk_threads_enter();
	PoiSourceLoadProgress * pslp = GOSM_POI_SOURCE_LOAD_PROGRESS(data);
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(pslp -> progress_bar), ((double)percent) / 100);
	gdk_threads_leave();
}
