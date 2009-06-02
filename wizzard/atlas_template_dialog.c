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

#include "../atlas.h"
#include "atlas_template_dialog.h"

#define WIDTH_A4	210.0
#define HEIGHT_A4	297.0
#define WIDTH_A5	148.0
#define HEIGHT_A5	210.0
#define WIDTH_LETTER	215.9
#define HEIGHT_LETTER	279.4

#define MM_TO_INCH(x)	((double)x)/26


G_DEFINE_TYPE (AtlasTemplateDialog, atlas_template_dialog, GTK_TYPE_DIALOG);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint atlas_template_dialog_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, atlas_template_dialog_signals[SIGNAL_NAME_n], 0);

GtkDialog * atlas_template_dialog_new()
{
	AtlasTemplateDialog * atlas_template_dialog = g_object_new(GOSM_TYPE_ATLAS_TEMPLATE_DIALOG, NULL);
	return GTK_DIALOG(atlas_template_dialog);
}

static void atlas_template_dialog_class_init(AtlasTemplateDialogClass *class)
{
        /*atlas_template_dialog_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (AtlasTemplateDialogClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

AtlasImageDimension atlas_template_dialog_get_info(AtlasTemplateDialog * atd)
{
	AtlasImageDimension im;
	gboolean landscape 	= gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(atd -> radio_orientation_landscape));
	gboolean a4 		= gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(atd -> radio_size_a4));
	gboolean a5 		= gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(atd -> radio_size_a5));
	gboolean letter 	= gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(atd -> radio_size_letter));
	int dpi 		= atoi(gtk_entry_get_text(GTK_ENTRY(atd -> entry_resolution)));
	int border 		= atoi(gtk_entry_get_text(GTK_ENTRY(atd -> entry_border)));
	int intersect		= atoi(gtk_entry_get_text(GTK_ENTRY(atd -> entry_intersect)));

	double width  = WIDTH_A4;
	double height = HEIGHT_A4;
	width  = a5 ? WIDTH_A5  : width;
	height = a5 ? HEIGHT_A5 : height;
	width  = letter ? WIDTH_LETTER  : width;
	height = letter ? HEIGHT_LETTER : height;

	if (landscape){
		double tmp = height;
		height = width;
		width = tmp;
	}

	width  -= 2 * border;
	height -= 2 * border;
	int x = MM_TO_INCH(width) * dpi; 
	int y = MM_TO_INCH(height) * dpi; 

	im.width = x;
	im.height = y;
	im.intersect_x = intersect;
	im.intersect_y = intersect;

	return im;
}

static void atlas_template_dialog_init(AtlasTemplateDialog *atlas_template_dialog)
{
	AtlasTemplateDialog * atd = atlas_template_dialog;
	gtk_dialog_set_has_separator(GTK_DIALOG(atd), FALSE);

	gtk_dialog_add_button(GTK_DIALOG(atd), "Apply", 0);
	gtk_dialog_add_button(GTK_DIALOG(atd), "Cancel", -1);

	GtkWidget * box_orientation = gtk_hbox_new(FALSE, 0);
	atd -> radio_orientation_landscape	= gtk_radio_button_new(NULL);
	atd -> radio_orientation_portrait	= gtk_radio_button_new(gtk_radio_button_get_group(GTK_RADIO_BUTTON(atd -> radio_orientation_landscape)));
	GtkWidget * image_landscape 	= gtk_image_new_from_stock("gtk-orientation-landscape", GTK_ICON_SIZE_BUTTON);
	GtkWidget * image_portrait	= gtk_image_new_from_stock("gtk-orientation-portrait", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(atd -> radio_orientation_landscape), image_landscape);
	gtk_button_set_image(GTK_BUTTON(atd -> radio_orientation_portrait), image_portrait);
	gtk_box_pack_start(GTK_BOX(box_orientation), atd -> radio_orientation_landscape, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_orientation), atd -> radio_orientation_portrait, FALSE, FALSE, 0);

	GtkWidget * box_size = gtk_hbox_new(FALSE, 0);
	atd -> radio_size_a4		= gtk_radio_button_new(NULL);
	atd -> radio_size_a5		= gtk_radio_button_new(gtk_radio_button_get_group(GTK_RADIO_BUTTON(atd -> radio_size_a4)));
	atd -> radio_size_letter	= gtk_radio_button_new(gtk_radio_button_get_group(GTK_RADIO_BUTTON(atd -> radio_size_a4)));
	gtk_button_set_label(GTK_BUTTON(atd -> radio_size_a4), "A4");
	gtk_button_set_label(GTK_BUTTON(atd -> radio_size_a5), "A5");
	gtk_button_set_label(GTK_BUTTON(atd -> radio_size_letter), "Letter");
	gtk_box_pack_start(GTK_BOX(box_size), atd -> radio_size_a4, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_size), atd -> radio_size_a5, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_size), atd -> radio_size_letter, FALSE, FALSE, 0);

	GtkWidget * label_orientation	= gtk_label_new("Orientation");
	GtkWidget * label_size 		= gtk_label_new("Size");

	GtkWidget * label_resolution = gtk_label_new("Resolution(dpi)");
	atd -> entry_resolution = gtk_entry_new();

	GtkWidget * label_border = gtk_label_new("Border(mm)");
	atd -> entry_border = gtk_entry_new();

	GtkWidget * label_intersect = gtk_label_new("Intersection(px)");
	atd -> entry_intersect = gtk_entry_new();

	gtk_entry_set_text(GTK_ENTRY(atd -> entry_resolution), "150");
	gtk_entry_set_text(GTK_ENTRY(atd -> entry_border), "15");
	gtk_entry_set_text(GTK_ENTRY(atd -> entry_intersect), "150");

	GtkWidget * table = gtk_table_new(2, 5, FALSE);
	gtk_table_attach(GTK_TABLE(table), label_orientation,		0, 1, 0, 1, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), label_size,			0, 1, 1, 2, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), label_resolution,		0, 1, 2, 3, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), label_border,		0, 1, 3, 4, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), label_intersect,		0, 1, 4, 5, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), box_orientation,		1, 2, 0, 1, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), box_size,			1, 2, 1, 2, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), atd -> entry_resolution,	1, 2, 2, 3, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), atd -> entry_border,		1, 2, 3, 4, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), atd -> entry_intersect,	1, 2, 4, 5, GTK_EXPAND | GTK_FILL, 0, 0, 0);

	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(atd)->vbox), table,	FALSE, FALSE, 0);
	gtk_widget_show_all(GTK_WIDGET(atd));
}
