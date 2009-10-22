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

#include "../atlas/atlas.h"
#include "atlas_template_dialog.h"

G_DEFINE_TYPE (AtlasTemplateDialog, atlas_template_dialog, GTK_TYPE_DIALOG);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint atlas_template_dialog_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, atlas_template_dialog_signals[SIGNAL_NAME_n], 0);

char* page_format_names[] = {
	"A3",
        "A4",
        "A5",
	"Letter",
	"Custom"
};

PageDimension page_dimensions[] = {
	{297, 420},
	{210, 297},
	{148, 210},
	{216, 279}
};

static void set_dimension_entries(AtlasTemplateDialog * atd);

atlas_template_dialog_set(AtlasTemplateDialog * atd)
{
	PageSize page_size = atd -> page_info.page_size;
	gtk_combo_box_set_active(GTK_COMBO_BOX(atd -> combo_pagesize), page_size);
	if (atd -> page_info.page_orientation == ORIENTATION_PORTRAIT){
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(atd -> radio_orientation_portrait), TRUE);
	}else{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(atd -> radio_orientation_landscape), TRUE);
	}
	set_dimension_entries(atd);
	char b_t[10]; char b_b[10]; char b_l[10]; char b_r[10]; char res[10];
	sprintf(b_t, "%d", atd -> page_info.border_top);
	sprintf(b_b, "%d", atd -> page_info.border_bottom);
	sprintf(b_l, "%d", atd -> page_info.border_left);
	sprintf(b_r, "%d", atd -> page_info.border_right);
	sprintf(res, "%d", atd -> page_info.resolution);
	gtk_entry_set_text(GTK_ENTRY(atd -> entry_border_top), b_t);
	gtk_entry_set_text(GTK_ENTRY(atd -> entry_border_bottom), b_b);
	gtk_entry_set_text(GTK_ENTRY(atd -> entry_border_left), b_l);
	gtk_entry_set_text(GTK_ENTRY(atd -> entry_border_right), b_r);
	gtk_entry_set_text(GTK_ENTRY(atd -> entry_resolution), res);
}

static void set_dimension_entries(AtlasTemplateDialog * atd)
{
	int active = gtk_combo_box_get_active(GTK_COMBO_BOX(atd -> combo_pagesize));
	gboolean portrait = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(atd -> radio_orientation_portrait));
	char width[10]; char height[10];
	int w = 50;
	int h = 0;
	if (active == PAPERSIZE_CUSTOM){
		w = atd -> page_info.page_width;
		h = atd -> page_info.page_height;
	}else{	
		if (portrait){
			w = page_dimensions[active].width;
			h = page_dimensions[active].height;
		}else{
			w = page_dimensions[active].height;
			h = page_dimensions[active].width;
		}
	}
	sprintf(width, "%d", w);
	sprintf(height, "%d", h);
	gtk_entry_set_text(GTK_ENTRY(atd -> entry_width), width);
	gtk_entry_set_text(GTK_ENTRY(atd -> entry_height), height);
}

static gboolean papertype_changed_cb(GtkWidget *widget, gpointer dialogp)
{
	AtlasTemplateDialog * atd = (AtlasTemplateDialog*) dialogp;
	int active = gtk_combo_box_get_active(GTK_COMBO_BOX(atd -> combo_pagesize));
	if (active == PAPERSIZE_CUSTOM){
		gtk_widget_set_sensitive(atd -> entry_width, TRUE);
		gtk_widget_set_sensitive(atd -> entry_height, TRUE);
		gtk_widget_set_sensitive(atd -> radio_orientation_landscape, FALSE);
		gtk_widget_set_sensitive(atd -> radio_orientation_portrait, FALSE);
	}else{
		gtk_widget_set_sensitive(atd -> entry_width, FALSE);
		gtk_widget_set_sensitive(atd -> entry_height, FALSE);
		gtk_widget_set_sensitive(atd -> radio_orientation_landscape, TRUE);
		gtk_widget_set_sensitive(atd -> radio_orientation_portrait, TRUE);
		set_dimension_entries(atd);
	}
}

static gboolean orientation_changed_cb(GtkWidget *widget, gpointer dialogp)
{
	AtlasTemplateDialog * atd = (AtlasTemplateDialog*) dialogp;
	set_dimension_entries(atd);
}

static gboolean size_changed_cb(GtkWidget *widget, gpointer dialogp)
{
	AtlasTemplateDialog * atd = (AtlasTemplateDialog*) dialogp;
}

static gboolean border_changed_cb(GtkWidget *widget, gpointer dialogp)
{
	AtlasTemplateDialog * atd = (AtlasTemplateDialog*) dialogp;
}

GtkDialog * atlas_template_dialog_new(PageInformation page_info)
{
	AtlasTemplateDialog * atlas_template_dialog = g_object_new(GOSM_TYPE_ATLAS_TEMPLATE_DIALOG, NULL);
	atlas_template_dialog -> page_info = page_info;
	atlas_template_dialog_set(atlas_template_dialog);
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

static void atlas_template_dialog_init(AtlasTemplateDialog *atlas_template_dialog)
{
	AtlasTemplateDialog * atd = atlas_template_dialog;
	gtk_dialog_set_has_separator(GTK_DIALOG(atd), FALSE);

	gtk_dialog_add_button(GTK_DIALOG(atd), "Apply", 0);
	gtk_dialog_add_button(GTK_DIALOG(atd), "Cancel", -1);

	atd -> combo_pagesize = gtk_combo_box_new_text();
	int i; char text[8];
	for (i = 0; i < PAPERSIZE_LAST; i++){
		gtk_combo_box_append_text(GTK_COMBO_BOX(atd -> combo_pagesize), page_format_names[i]);
	}

	atd -> entry_width = gtk_entry_new();
	atd -> entry_height = gtk_entry_new();

	atd -> radio_orientation_landscape	= gtk_radio_button_new(NULL);
	atd -> radio_orientation_portrait	= gtk_radio_button_new(gtk_radio_button_get_group(GTK_RADIO_BUTTON(atd -> radio_orientation_landscape)));
	GtkWidget * image_landscape 	= gtk_image_new_from_stock("gtk-orientation-landscape", GTK_ICON_SIZE_BUTTON);
	GtkWidget * image_portrait	= gtk_image_new_from_stock("gtk-orientation-portrait", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(atd -> radio_orientation_landscape), image_landscape);
	gtk_button_set_image(GTK_BUTTON(atd -> radio_orientation_portrait), image_portrait);
	GtkWidget * box_orientation = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_orientation), atd -> radio_orientation_portrait, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_orientation), atd -> radio_orientation_landscape, FALSE, FALSE, 0);

        atd -> entry_border_top    = gtk_entry_new();
        atd -> entry_border_left   = gtk_entry_new();
        atd -> entry_border_right  = gtk_entry_new();
        atd -> entry_border_bottom = gtk_entry_new();

        atd -> entry_resolution = gtk_entry_new();

	GtkWidget * label_paper_format	= gtk_label_new("Paper format");
	GtkWidget * label_format	= gtk_label_new("Format");
	GtkWidget * label_width		= gtk_label_new("Width");
	GtkWidget * label_height	= gtk_label_new("Height");
	GtkWidget * label_orientation	= gtk_label_new("Orientation");
	GtkWidget * label_margins	= gtk_label_new("Margins");
	GtkWidget * label_left		= gtk_label_new("Left");
	GtkWidget * label_right		= gtk_label_new("Right");
	GtkWidget * label_top		= gtk_label_new("Top");
	GtkWidget * label_bottom	= gtk_label_new("Bottom");
	GtkWidget * label_quality	= gtk_label_new("Quality");
	GtkWidget * label_resolution	= gtk_label_new("Resolution");

	GtkWidget * table = gtk_table_new(10, 2, FALSE);
        gtk_table_attach(GTK_TABLE(table), label_paper_format,		0, 1, 0,  1,  GTK_EXPAND | GTK_FILL, 0, 0, 0);
        gtk_table_attach(GTK_TABLE(table), label_format,		0, 1, 1,  2,  GTK_EXPAND | GTK_FILL, 0, 0, 0);
        gtk_table_attach(GTK_TABLE(table), atd -> combo_pagesize,	1, 2, 1,  2,  GTK_EXPAND | GTK_FILL, 0, 0, 0);
        gtk_table_attach(GTK_TABLE(table), label_width,			0, 1, 2,  3,  GTK_EXPAND | GTK_FILL, 0, 0, 0);
        gtk_table_attach(GTK_TABLE(table), atd -> entry_width,		1, 2, 2,  3,  GTK_EXPAND | GTK_FILL, 0, 0, 0);
        gtk_table_attach(GTK_TABLE(table), label_height,		0, 1, 3,  4,  GTK_EXPAND | GTK_FILL, 0, 0, 0);
        gtk_table_attach(GTK_TABLE(table), atd -> entry_height,		1, 2, 3,  4,  GTK_EXPAND | GTK_FILL, 0, 0, 0);
        gtk_table_attach(GTK_TABLE(table), label_orientation,		0, 1, 4,  5,  GTK_EXPAND | GTK_FILL, 0, 0, 0);
        gtk_table_attach(GTK_TABLE(table), box_orientation,		1, 2, 4,  5,  GTK_EXPAND | GTK_FILL, 0, 0, 0);
        gtk_table_attach(GTK_TABLE(table), label_margins,		0, 1, 5,  6,  GTK_EXPAND | GTK_FILL, 0, 0, 0);
        gtk_table_attach(GTK_TABLE(table), label_left,			0, 1, 6,  7,  GTK_EXPAND | GTK_FILL, 0, 0, 0);
        gtk_table_attach(GTK_TABLE(table), atd -> entry_border_left,	1, 2, 6,  7,  GTK_EXPAND | GTK_FILL, 0, 0, 0);
        gtk_table_attach(GTK_TABLE(table), label_right,			0, 1, 7,  8,  GTK_EXPAND | GTK_FILL, 0, 0, 0);
        gtk_table_attach(GTK_TABLE(table), atd -> entry_border_right,	1, 2, 7,  8,  GTK_EXPAND | GTK_FILL, 0, 0, 0);
        gtk_table_attach(GTK_TABLE(table), label_top,			0, 1, 8,  9,  GTK_EXPAND | GTK_FILL, 0, 0, 0);
        gtk_table_attach(GTK_TABLE(table), atd -> entry_border_top,	1, 2, 8,  9,  GTK_EXPAND | GTK_FILL, 0, 0, 0);
        gtk_table_attach(GTK_TABLE(table), label_bottom,		0, 1, 9,  10, GTK_EXPAND | GTK_FILL, 0, 0, 0);
        gtk_table_attach(GTK_TABLE(table), atd -> entry_border_bottom,	1, 2, 9,  10, GTK_EXPAND | GTK_FILL, 0, 0, 0);
        gtk_table_attach(GTK_TABLE(table), label_quality,		0, 1, 10, 11,  GTK_EXPAND | GTK_FILL, 0, 0, 0);
        gtk_table_attach(GTK_TABLE(table), label_resolution,		0, 1, 11, 12, GTK_EXPAND | GTK_FILL, 0, 0, 0);
        gtk_table_attach(GTK_TABLE(table), atd -> entry_resolution,	1, 2, 11, 12, GTK_EXPAND | GTK_FILL, 0, 0, 0);

	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(atd) -> vbox), table, FALSE, FALSE, 0);

	//gtk_widget_set_size_request(GTK_WIDGET(atd -> entry_height), 40, -1);
	gtk_widget_show_all(GTK_WIDGET(atd));

        g_signal_connect(G_OBJECT(atd -> combo_pagesize), "changed", G_CALLBACK(papertype_changed_cb), (gpointer)atd);
        g_signal_connect(G_OBJECT(atd -> radio_orientation_portrait), "toggled", G_CALLBACK(orientation_changed_cb), (gpointer)atd);
	g_signal_connect(G_OBJECT(atd -> entry_width), "changed", G_CALLBACK(size_changed_cb), (gpointer)atd);
	g_signal_connect(G_OBJECT(atd -> entry_height), "changed", G_CALLBACK(size_changed_cb), (gpointer)atd);
	g_signal_connect(G_OBJECT(atd -> entry_border_top), "changed", G_CALLBACK(border_changed_cb), (gpointer)atd);
	g_signal_connect(G_OBJECT(atd -> entry_border_bottom), "changed", G_CALLBACK(border_changed_cb), (gpointer)atd);
	g_signal_connect(G_OBJECT(atd -> entry_border_left), "changed", G_CALLBACK(border_changed_cb), (gpointer)atd);
	g_signal_connect(G_OBJECT(atd -> entry_border_right), "changed", G_CALLBACK(border_changed_cb), (gpointer)atd);
	//g_signal_connect(G_OBJECT(atd -> entry_resolution), "changed", G_CALLBACK(resolution_changed_cb), (gpointer)atd);
}

PageInformation atlas_template_dialog_get_page_info(AtlasTemplateDialog * atd)
{
	int active = gtk_combo_box_get_active(GTK_COMBO_BOX(atd -> combo_pagesize));
	atd -> page_info.page_size = active;
	atd -> page_info.page_width = atoi(gtk_entry_get_text(GTK_ENTRY(atd -> entry_width)));
	atd -> page_info.page_height = atoi(gtk_entry_get_text(GTK_ENTRY(atd -> entry_height)));
	gboolean portrait = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(atd -> radio_orientation_portrait));
	if (portrait){
		atd -> page_info.page_orientation = ORIENTATION_PORTRAIT;
	}else{
		atd -> page_info.page_orientation = ORIENTATION_LANDSCAPE;
	}
	atd -> page_info.border_top = atoi(gtk_entry_get_text(GTK_ENTRY(atd -> entry_border_top)));
	atd -> page_info.border_bottom = atoi(gtk_entry_get_text(GTK_ENTRY(atd -> entry_border_bottom)));
	atd -> page_info.border_left = atoi(gtk_entry_get_text(GTK_ENTRY(atd -> entry_border_left)));
	atd -> page_info.border_right = atoi(gtk_entry_get_text(GTK_ENTRY(atd -> entry_border_right)));
	atd -> page_info.resolution = atoi(gtk_entry_get_text(GTK_ENTRY(atd -> entry_resolution)));
	return atd -> page_info;
}
