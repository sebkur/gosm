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

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "wizzard_atlas_sequence.h"
#include "wizzard_atlas_sequence_window.h"

G_DEFINE_TYPE (WizzardAtlasSequence, wizzard_atlas_sequence, G_TYPE_OBJECT);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint wizzard_atlas_sequence_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, wizzard_atlas_sequence_signals[SIGNAL_NAME_n], 0);

static gboolean wizzard_atlas_sequence_export_cb(GtkWidget * button, WizzardAtlasSequence * was);
static gboolean wizzard_atlas_sequence_cancel_cb(GtkWidget * button, WizzardAtlasSequence * was);

WizzardAtlasSequence * wizzard_atlas_sequence_new(char * cache_dir, int zoom, Selection selection, ImageDimension image_dimension, int intersect_x, int intersect_y)
{
	WizzardAtlasSequence * was = g_object_new(GOSM_TYPE_WIZZARD_ATLAS_SEQUENCE, NULL);
	was -> cache_dir = cache_dir;
	was -> zoom = zoom;
	was -> selection = selection;
	was -> image_dimension = image_dimension;
	was -> intersect_x = intersect_x;
	was -> intersect_y = intersect_y;
	return was;
}

static void wizzard_atlas_sequence_class_init(WizzardAtlasSequenceClass *class)
{
        /*wizzard_atlas_sequence_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (WizzardAtlasSequenceClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void wizzard_atlas_sequence_init(WizzardAtlasSequence *wizzard_atlas_sequence)
{
}

void wizzard_atlas_sequence_show(WizzardAtlasSequence * was, GtkWindow * parent)
{
	was -> wasw = wizzard_atlas_sequence_window_new();
	gtk_window_set_transient_for(GTK_WINDOW(was -> wasw), GTK_WINDOW(parent));
	gtk_window_set_title(GTK_WINDOW(was -> wasw), "Export Sequence");
	gtk_window_set_position(GTK_WINDOW(was -> wasw), GTK_WIN_POS_CENTER_ON_PARENT);
	gtk_window_set_modal(GTK_WINDOW(was -> wasw), TRUE);
	gtk_widget_show_all(GTK_WIDGET(was -> wasw));

	g_signal_connect(G_OBJECT(was -> wasw -> button_cancel), "clicked", G_CALLBACK(wizzard_atlas_sequence_cancel_cb), was);
	g_signal_connect(G_OBJECT(was -> wasw -> button_export), "clicked", G_CALLBACK(wizzard_atlas_sequence_export_cb), was);
}

static void tile_cb(ImageGlue * image_glue, int n, WizzardAtlasSequence * was)
{
	if (n == 1){
		was -> ready += 1;
		gdk_threads_enter();
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(was -> wasw -> progress_bar), ((double) was -> ready) / was -> total);
		gdk_threads_leave();
	}
}

static gboolean wizzard_atlas_sequence_export_cb(GtkWidget * button, WizzardAtlasSequence * was)
{
	wizzard_atlas_sequence_window_set_inactive(was -> wasw);
	const char * filename = gtk_entry_get_text(GTK_ENTRY(was -> wasw -> entry_filename));
	char * filen = malloc(sizeof(char) * (1 + strlen(filename)));
	strcpy(filen, filename);
	ImageGlue * image_glue = image_glue_new();
	g_signal_connect(G_OBJECT(image_glue), "tile-completed", G_CALLBACK(tile_cb), was);
	was -> total = image_glue_sequence_get_number_of_tiles(was -> zoom, was -> selection, was -> image_dimension, was -> intersect_x, was -> intersect_y);
	image_glue_sequence_setup(image_glue, filen, was -> cache_dir, was -> zoom, was -> selection,
				  was -> image_dimension, was -> intersect_x, was -> intersect_y);
	image_glue_sequence_process(image_glue);
}

static gboolean wizzard_atlas_sequence_cancel_cb(GtkWidget * button, WizzardAtlasSequence * was)
{
	gtk_widget_hide_all(GTK_WIDGET(was -> wasw));
}
