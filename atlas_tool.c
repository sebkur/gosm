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

#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include <pthread.h>
#include <glib.h>
#include <glib/gthread.h>
#include <unistd.h>

#include "atlas_tool.h"

G_DEFINE_TYPE (AtlasTool, atlas_tool, GTK_TYPE_VBOX);

enum
{
	VALUES_CHANGED,
	LAST_SIGNAL
};

static guint atlas_tool_signals[LAST_SIGNAL] = { 0 };

AtlasTool * atlas_tool_new()
{
	return g_object_new(GOSM_TYPE_ATLAS_TOOL, NULL);
}

static void atlas_tool_class_init(AtlasToolClass *class)
{
	atlas_tool_signals[VALUES_CHANGED] = g_signal_new(
		"values-changed",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET (AtlasToolClass, values_changed),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0);
}

static gboolean widget_changed_cb(GtkWidget *widget, gpointer atlasp)
{
	AtlasTool *atlas_tool = (AtlasTool*)atlasp;
	int n_slice_x = atoi(gtk_entry_get_text(GTK_ENTRY(atlas_tool -> entry_slice_x)));
	int n_slice_y = atoi(gtk_entry_get_text(GTK_ENTRY(atlas_tool -> entry_slice_y)));
	int n_slice_intersect_x = atoi(gtk_entry_get_text(GTK_ENTRY(atlas_tool -> entry_slice_intersect_x)));
	int n_slice_intersect_y = atoi(gtk_entry_get_text(GTK_ENTRY(atlas_tool -> entry_slice_intersect_y)));
	int n_zoom = gtk_combo_box_get_active(GTK_COMBO_BOX(atlas_tool -> combo)) + 1;
	gboolean n_visible = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(atlas_tool -> check_show));
	gboolean changed = FALSE;
	if (atlas_tool -> slice_x != n_slice_x && n_slice_x > 0){
		atlas_tool -> slice_x = n_slice_x;
		changed = TRUE;
	}
	if (atlas_tool -> slice_y != n_slice_y && n_slice_y > 0){
		atlas_tool -> slice_y = n_slice_y;
		changed = TRUE;
	}
	if (atlas_tool -> slice_intersect_x != n_slice_intersect_x && n_slice_intersect_x >= 0){
		atlas_tool -> slice_intersect_x = n_slice_intersect_x;
		changed = TRUE;
	}
	if (atlas_tool -> slice_intersect_y != n_slice_intersect_y && n_slice_intersect_y >= 0){
		atlas_tool -> slice_intersect_y = n_slice_intersect_y;
		changed = TRUE;
	}
	if (atlas_tool -> slice_zoom != n_zoom){
		atlas_tool -> slice_zoom = n_zoom;
		changed = TRUE;
	}
	if (atlas_tool -> visible != n_visible){
		atlas_tool -> visible = n_visible;
		changed = TRUE;
	}
	if(changed){
		g_signal_emit (atlas_tool, atlas_tool_signals[VALUES_CHANGED], 0);
	}
}

static void atlas_tool_init(AtlasTool *atlas_tool)
{
	GtkWidget *label_size = gtk_label_new("Size:");
	GtkWidget *label_intersect = gtk_label_new("Intersect:");
	
	atlas_tool -> entry_slice_x = gtk_entry_new();
	atlas_tool -> entry_slice_y = gtk_entry_new();
	atlas_tool -> entry_slice_intersect_x = gtk_entry_new();
	atlas_tool -> entry_slice_intersect_y = gtk_entry_new();

	GtkWidget *icon1 = gtk_image_new_from_file("icons/object-flip-horizontal.png");
	GtkWidget *icon2 = gtk_image_new_from_file("icons/object-flip-vertical.png");

	// VISIBLE
	atlas_tool -> check_show = gtk_check_button_new_with_label("visible");
	//gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(atlas_tool->check_show), TRUE);
	GtkWidget *box_check_show = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_check_show), atlas_tool -> check_show, FALSE, FALSE, 0);

	// COMBO
	atlas_tool -> combo = gtk_combo_box_new_text();
        int i; char text[5];
        for (i = 0; i < 18; i++){
                sprintf(text, "%d", i+1);
                gtk_combo_box_append_text(GTK_COMBO_BOX(atlas_tool -> combo), text);
        }

	GtkWidget * icon_template = gtk_image_new_from_file("icons/template.png");
	GtkWidget * icon_download = gtk_image_new_from_file("icons/document-save.png");
	GtkWidget * icon_export = gtk_image_new_from_file("icons/stock_insert_image.png");
	atlas_tool -> button_template = gtk_button_new();
	atlas_tool -> button_action   = gtk_button_new();
	atlas_tool -> button_export   = gtk_button_new();
	gtk_button_set_image(GTK_BUTTON(atlas_tool -> button_template), icon_template);
	gtk_button_set_image(GTK_BUTTON(atlas_tool -> button_action), icon_download);
	gtk_button_set_image(GTK_BUTTON(atlas_tool -> button_export), icon_export);
	GtkWidget * box_buttons = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_buttons), atlas_tool -> combo, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_buttons), atlas_tool -> button_template, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_buttons), atlas_tool -> button_action, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_buttons), atlas_tool -> button_export, FALSE, FALSE, 0);
	gtk_widget_set_tooltip_text(atlas_tool -> button_template, "template values");
	gtk_widget_set_tooltip_text(atlas_tool -> button_action, "load to disk");
	gtk_widget_set_tooltip_text(atlas_tool -> button_export, "export atlas");

	GtkWidget * table = gtk_table_new(5, 3, FALSE);
	gtk_table_attach(GTK_TABLE(table), label_size,					1, 2, 0, 1, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), label_intersect,				2, 3, 0, 1, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), icon1,					0, 1, 1, 2, GTK_SHRINK, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), icon2,					0, 1, 2, 3, GTK_SHRINK, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), atlas_tool->entry_slice_x,			1, 2, 1, 2, GTK_FILL | GTK_EXPAND, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), atlas_tool->entry_slice_y,			1, 2, 2, 3, GTK_FILL | GTK_EXPAND, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), atlas_tool->entry_slice_intersect_x,		2, 3, 1, 2, GTK_FILL | GTK_EXPAND, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), atlas_tool->entry_slice_intersect_y,		2, 3, 2, 3, GTK_FILL | GTK_EXPAND, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), box_check_show,				0, 3, 3, 4, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), box_buttons,					0, 3, 4, 5, GTK_EXPAND | GTK_FILL, 0, 0, 0);

	gtk_widget_set_size_request(table, 200, -1);
	gtk_widget_set_size_request(atlas_tool -> entry_slice_x, 0 , -1);
	gtk_widget_set_size_request(atlas_tool -> entry_slice_y, 0 , -1);
	gtk_widget_set_size_request(atlas_tool -> entry_slice_intersect_x, 0 , -1);
	gtk_widget_set_size_request(atlas_tool -> entry_slice_intersect_y, 0 , -1);

	gtk_box_pack_start(GTK_BOX(atlas_tool), table, FALSE, FALSE, 0);

	g_signal_connect(G_OBJECT(atlas_tool -> combo), "changed", G_CALLBACK(widget_changed_cb), (gpointer)atlas_tool);
	g_signal_connect(G_OBJECT(atlas_tool -> check_show), "toggled", G_CALLBACK(widget_changed_cb), (gpointer)atlas_tool);
	g_signal_connect(G_OBJECT(atlas_tool -> entry_slice_x), "changed", G_CALLBACK(widget_changed_cb), (gpointer)atlas_tool);
	g_signal_connect(G_OBJECT(atlas_tool -> entry_slice_y), "changed", G_CALLBACK(widget_changed_cb), (gpointer)atlas_tool);
	g_signal_connect(G_OBJECT(atlas_tool -> entry_slice_intersect_x), "changed", G_CALLBACK(widget_changed_cb), (gpointer)atlas_tool);
	g_signal_connect(G_OBJECT(atlas_tool -> entry_slice_intersect_y), "changed", G_CALLBACK(widget_changed_cb), (gpointer)atlas_tool);
}

//TODO: the fuction with lesser parameters should be called by the other one...
void atlas_tool_set_dimensions(AtlasTool * atlas_tool, int slice_x, int slice_y, int slice_intersect_x, int slice_intersect_y)
{
	atlas_tool_set_values(atlas_tool, slice_x, slice_y, slice_intersect_x, slice_intersect_y, atlas_tool -> slice_zoom, atlas_tool -> visible);
}

void atlas_tool_set_values(AtlasTool * atlas_tool, int slice_x, int slice_y, int slice_intersect_x, int slice_intersect_y, int zoom, gboolean visible)
{
	atlas_tool -> slice_x = slice_x,
	atlas_tool -> slice_y = slice_y,
	atlas_tool -> slice_intersect_x = slice_intersect_x,
	atlas_tool -> slice_intersect_y = slice_intersect_y;
	atlas_tool -> slice_zoom = zoom;
	atlas_tool -> visible = visible;
	int vals[4] = {slice_x, slice_y, slice_intersect_x, slice_intersect_y};
	GtkWidget* widgets[4] = {
		atlas_tool -> entry_slice_x,
		atlas_tool -> entry_slice_y,
		atlas_tool -> entry_slice_intersect_x,
		atlas_tool -> entry_slice_intersect_y};
	char buf[20]; int i;
	for (i = 0; i < 4; i++){
		sprintf(buf, "%d", vals[i]);
		gtk_entry_set_text(GTK_ENTRY(widgets[i]), buf);
	}
        gtk_combo_box_set_active(GTK_COMBO_BOX(atlas_tool -> combo), zoom - 1);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(atlas_tool -> check_show), visible);
}
