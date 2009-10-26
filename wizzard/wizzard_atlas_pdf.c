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

#include "wizzard_atlas_pdf.h"
#include "wizzard_atlas_pdf_window.h"
#include "../imageglue/image_glue.h"
#include "../imageglue/pdf_generator.h"
#include "../map_types.h"
#include "../atlas/atlas.h"
#include "../config/config.h"

G_DEFINE_TYPE (WizzardAtlasPdf, wizzard_atlas_pdf, G_TYPE_OBJECT);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint wizzard_atlas_pdf_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, wizzard_atlas_pdf_signals[SIGNAL_NAME_n], 0);

extern Config * config;

static gboolean wizzard_atlas_pdf_export_cb(GtkWidget * button, WizzardAtlasPdf * wap);
static gboolean wizzard_atlas_pdf_cancel_cb(GtkWidget * button, WizzardAtlasPdf * wap);

WizzardAtlasPdf * wizzard_atlas_pdf_new(char * cache_dir, int zoom, Selection selection,
	PageInformation page_info, ImageDimension image_dimension, int intersect_x, int intersect_y)
{
	WizzardAtlasPdf * wap = g_object_new(GOSM_TYPE_WIZZARD_ATLAS_PDF, NULL);
	wap -> cache_dir = cache_dir;
	wap -> zoom = zoom;
	wap -> selection = selection;
	wap -> page_info = page_info;
	wap -> image_dimension = image_dimension;
	wap -> intersect_x = intersect_x;
	wap -> intersect_y = intersect_y;
	return wap;
}

static void wizzard_atlas_pdf_class_init(WizzardAtlasPdfClass *class)
{
        /*wizzard_atlas_pdf_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (WizzardAtlasPdfClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void wizzard_atlas_pdf_init(WizzardAtlasPdf *wizzard_atlas_pdf)
{
}

void wizzard_atlas_pdf_show(WizzardAtlasPdf * wap, GtkWindow * parent)
{
	wap -> wapw = wizzard_atlas_pdf_window_new();
	gtk_window_set_transient_for(GTK_WINDOW(wap -> wapw), GTK_WINDOW(parent));
	gtk_window_set_title(GTK_WINDOW(wap -> wapw), "Export Pdf");
	gtk_window_set_position(GTK_WINDOW(wap -> wapw), GTK_WIN_POS_CENTER_ON_PARENT);
	gtk_window_set_modal(GTK_WINDOW(wap -> wapw), TRUE);
	gtk_widget_show_all(GTK_WIDGET(wap -> wapw));

	g_signal_connect(G_OBJECT(wap -> wapw -> button_cancel), "clicked", G_CALLBACK(wizzard_atlas_pdf_cancel_cb), wap);
	g_signal_connect(G_OBJECT(wap -> wapw -> button_export), "clicked", G_CALLBACK(wizzard_atlas_pdf_export_cb), wap);
}

static void page_cb(PdfGenerator * pdf_generator, int n, WizzardAtlasPdf * wap)
{
		wap -> ready_parts += 1;
		gdk_threads_enter();
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(wap -> wapw -> progress_bar_pdf), ((double) wap -> ready_parts) / wap -> total_parts);
		gdk_threads_leave();
}

static void tile_cb(ImageGlue * image_glue, int n, WizzardAtlasPdf * wap)
{
	if (n == 1){
		wap -> ready_tiles += 1;
		gdk_threads_enter();
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(wap -> wapw -> progress_bar_images), ((double) wap -> ready_tiles) / wap -> total_tiles);
		gdk_threads_leave();
	}
	if (n == 2 && wap -> ready_tiles == wap -> total_tiles){
		PdfGenerator * pdf_generator = pdf_generator_new();
		g_signal_connect(G_OBJECT(pdf_generator), "page-progress", G_CALLBACK(page_cb), wap);
		char * java_binary = config_get_entry_data(config, "java_binary");
		pdf_generator_set_java_binary(pdf_generator, java_binary);
		pdf_generator_setup(pdf_generator, wap -> page_info, wap -> filename_prefix, wap -> parts_x, wap -> parts_y);
		pdf_generator_process(pdf_generator);
	}
}

static gboolean wizzard_atlas_pdf_export_cb(GtkWidget * button, WizzardAtlasPdf * wap)
{
	wizzard_atlas_pdf_window_set_inactive(wap -> wapw);
	const char * filename = gtk_entry_get_text(GTK_ENTRY(wap -> wapw -> entry_filename));
	wap -> filename_prefix = malloc(sizeof(char) * (1 + strlen(filename)));
	strcpy(wap -> filename_prefix, filename);

	ImageGlue * image_glue = image_glue_new();
	g_signal_connect(G_OBJECT(image_glue), "tile-completed", G_CALLBACK(tile_cb), wap);
	wap -> total_tiles = image_glue_sequence_get_number_of_tiles(wap -> zoom, wap -> selection, wap -> image_dimension, wap -> intersect_x, wap -> intersect_y);
	image_glue_sequence_get_number_of_parts(&(wap -> parts_x), &(wap -> parts_y),
		wap -> zoom, wap -> selection, wap -> image_dimension, wap -> intersect_x, wap -> intersect_y);
	wap -> total_parts = wap -> parts_x * wap -> parts_y;

	image_glue_sequence_setup(image_glue, wap -> filename_prefix, wap -> cache_dir, wap -> zoom, wap -> selection,
				  wap -> image_dimension, wap -> intersect_x, wap -> intersect_y);
	image_glue_sequence_process(image_glue);
}

static gboolean wizzard_atlas_pdf_cancel_cb(GtkWidget * button, WizzardAtlasPdf * wap)
{
	gtk_widget_hide_all(GTK_WIDGET(wap -> wapw));
}
