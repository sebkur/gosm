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
#include "page_math.h"
#include "../paths.h"

G_DEFINE_TYPE (AtlasTool, atlas_tool, GTK_TYPE_VBOX);

/****************************************************************************************************
* signals
****************************************************************************************************/
enum
{
	VALUES_CHANGED,
	LAST_SIGNAL
};

static guint atlas_tool_signals[LAST_SIGNAL] = { 0 };

/****************************************************************************************************
* constructor
****************************************************************************************************/
AtlasTool * atlas_tool_new()
{
	return g_object_new(GOSM_TYPE_ATLAS_TOOL, NULL);
}

/****************************************************************************************************
* class init
****************************************************************************************************/
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

/****************************************************************************************************
* method declarations
****************************************************************************************************/
void set_resize_visibility(AtlasTool * atlas_tool, gboolean state);
static gboolean mode_switched_cb(GtkWidget *widget, gpointer atlasp);
static gboolean widget_changed_cb(GtkWidget *widget, gpointer atlasp);
void atlas_tool_set_image_size_from_page_info(AtlasTool * atlas_tool);
void atlas_tool_set_page_info_from_image_size(AtlasTool * atlas_tool);
void atlas_tool_set_intersection(AtlasTool * atlas_tool, int intersect_x, int intersect_y);
void atlas_tool_set_page_info(AtlasTool * atlas_tool, PageInformation page_info);
void atlas_tool_set_image_dimension(AtlasTool * atlas_tool, ImageDimension image_dimension);
void atlas_tool_set_values(AtlasTool * atlas_tool, int zoom, gboolean visible);

/****************************************************************************************************
* object init
****************************************************************************************************/
static void atlas_tool_init(AtlasTool *atlas_tool)
{
	GtkWidget *label_size = gtk_label_new("Size:");
	GtkWidget *label_intersect = gtk_label_new("Intersect:");
	
	atlas_tool -> entry_slice_x = gtk_entry_new();
	atlas_tool -> entry_slice_y = gtk_entry_new();
	atlas_tool -> entry_slice_intersect_x = gtk_entry_new();
	atlas_tool -> entry_slice_intersect_y = gtk_entry_new();

	GtkWidget *icon1 = gtk_image_new_from_file(GOSM_ICON_DIR "object-flip-horizontal.png");
	GtkWidget *icon2 = gtk_image_new_from_file(GOSM_ICON_DIR "object-flip-vertical.png");

	// VISIBLE
	atlas_tool -> check_show = gtk_check_button_new_with_label("visible");
	GtkWidget *box_check_show = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_check_show), atlas_tool -> check_show, FALSE, FALSE, 0);

	// COMBO
	atlas_tool -> combo = gtk_combo_box_new_text();
        int i; char text[5];
        for (i = 0; i < 18; i++){
                sprintf(text, "%d", i+1);
                gtk_combo_box_append_text(GTK_COMBO_BOX(atlas_tool -> combo), text);
        }

	atlas_tool -> radio_conf_page	= gtk_radio_button_new(NULL);
	atlas_tool -> radio_conf_pixel	= gtk_radio_button_new(gtk_radio_button_get_group(GTK_RADIO_BUTTON(atlas_tool -> radio_conf_page)));
	gtk_button_set_label(GTK_BUTTON(atlas_tool -> radio_conf_page), "by Pagesize");
	gtk_button_set_label(GTK_BUTTON(atlas_tool -> radio_conf_pixel), "by Imagesize");

	GtkWidget * icon_download = gtk_image_new_from_file(GOSM_ICON_DIR "document-save.png");
	GtkWidget * icon_export = gtk_image_new_from_file(GOSM_ICON_DIR "stock_insert_image.png");
	GtkWidget * icon_export_pdf = gtk_image_new_from_file(GOSM_ICON_DIR "stock_save-pdf.png");
	atlas_tool -> button_action		= gtk_button_new();
	atlas_tool -> button_export		= gtk_button_new();
	atlas_tool -> button_export_pdf		= gtk_button_new();
	gtk_button_set_image(GTK_BUTTON(atlas_tool -> button_action), icon_download);
	gtk_button_set_image(GTK_BUTTON(atlas_tool -> button_export), icon_export);
	gtk_button_set_image(GTK_BUTTON(atlas_tool -> button_export_pdf), icon_export_pdf);
	GtkWidget * box_buttons = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_buttons), atlas_tool -> combo, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_buttons), atlas_tool -> button_action, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_buttons), atlas_tool -> button_export, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_buttons), atlas_tool -> button_export_pdf, FALSE, FALSE, 0);
	gtk_widget_set_tooltip_text(atlas_tool -> button_action, "load to disk");
	gtk_widget_set_tooltip_text(atlas_tool -> button_export, "export image sequence");
	gtk_widget_set_tooltip_text(atlas_tool -> button_export_pdf, "export to pdf");

	GtkWidget * table = gtk_table_new(5, 3, FALSE);
	gtk_table_attach(GTK_TABLE(table), label_size,				1, 2, 0, 1, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), label_intersect,			2, 3, 0, 1, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), icon1,				0, 1, 1, 2, GTK_SHRINK, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), icon2,				0, 1, 2, 3, GTK_SHRINK, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), atlas_tool->entry_slice_x,		1, 2, 1, 2, GTK_FILL | GTK_EXPAND, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), atlas_tool->entry_slice_y,		1, 2, 2, 3, GTK_FILL | GTK_EXPAND, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), atlas_tool->entry_slice_intersect_x,	2, 3, 1, 2, GTK_FILL | GTK_EXPAND, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), atlas_tool->entry_slice_intersect_y,	2, 3, 2, 3, GTK_FILL | GTK_EXPAND, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), box_check_show,			0, 3, 3, 4, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(table), box_buttons,				0, 3, 4, 5, GTK_EXPAND | GTK_FILL, 0, 0, 0);

	gtk_widget_set_size_request(table, 200, -1);
	gtk_widget_set_size_request(atlas_tool -> entry_slice_x, 0 , -1);
	gtk_widget_set_size_request(atlas_tool -> entry_slice_y, 0 , -1);
	gtk_widget_set_size_request(atlas_tool -> entry_slice_intersect_x, 0 , -1);
	gtk_widget_set_size_request(atlas_tool -> entry_slice_intersect_y, 0 , -1);

	atlas_tool -> button_conf_page = gtk_button_new_with_label("Setup");

	GtkWidget * hbox_page = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox_page), atlas_tool -> radio_conf_page, FALSE, FALSE, 0);
	gtk_box_pack_end(GTK_BOX(hbox_page), atlas_tool -> button_conf_page, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(atlas_tool), hbox_page, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(atlas_tool), atlas_tool -> radio_conf_pixel, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(atlas_tool), table, FALSE, FALSE, 0);

	g_signal_connect(
		G_OBJECT(atlas_tool -> radio_conf_page), "toggled", 
		G_CALLBACK(mode_switched_cb), (gpointer)atlas_tool);
	g_signal_connect(
		G_OBJECT(atlas_tool -> combo), "changed", 
		G_CALLBACK(widget_changed_cb), (gpointer)atlas_tool);
	g_signal_connect(
		G_OBJECT(atlas_tool -> check_show), "toggled", 
		G_CALLBACK(widget_changed_cb), (gpointer)atlas_tool);
	g_signal_connect(
		G_OBJECT(atlas_tool -> entry_slice_x), "changed", 
		G_CALLBACK(widget_changed_cb), (gpointer)atlas_tool);
	g_signal_connect(
		G_OBJECT(atlas_tool -> entry_slice_y), "changed", 
		G_CALLBACK(widget_changed_cb), (gpointer)atlas_tool);
	g_signal_connect(
		G_OBJECT(atlas_tool -> entry_slice_intersect_x), "changed", 
		G_CALLBACK(widget_changed_cb), (gpointer)atlas_tool);
	g_signal_connect(
		G_OBJECT(atlas_tool -> entry_slice_intersect_y), "changed", 
		G_CALLBACK(widget_changed_cb), (gpointer)atlas_tool);

	set_resize_visibility(atlas_tool, FALSE);
}

/****************************************************************************************************
* when radio is changed between "by page" and "by imagesize"
****************************************************************************************************/
static gboolean mode_switched_cb(GtkWidget *widget, gpointer atlasp)
{
	AtlasTool *atlas_tool = (AtlasTool*)atlasp;
	gboolean by_page = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(atlas_tool -> radio_conf_page));
	if(by_page){
		atlas_tool -> mode = MODE_BY_PAGESIZE;
		atlas_tool_set_image_size_from_page_info(atlas_tool);
	}else{
		atlas_tool -> mode = MODE_BY_IMAGESIZE;
	}
	if(atlas_tool -> mode == MODE_BY_PAGESIZE){
		set_resize_visibility(atlas_tool, FALSE);
		atlas_tool -> page_info = atlas_tool -> page_info_stored;
	}
	if(atlas_tool -> mode == MODE_BY_IMAGESIZE){
		set_resize_visibility(atlas_tool, TRUE);
		atlas_tool_set_page_info_from_image_size(atlas_tool);
	}
}

/****************************************************************************************************
* set, whether user can edit image size (only possible in "by imagesize" mode)
****************************************************************************************************/
void set_resize_visibility(AtlasTool * atlas_tool, gboolean state)
{
	gtk_widget_set_sensitive(atlas_tool -> entry_slice_x, state);
	gtk_widget_set_sensitive(atlas_tool -> entry_slice_y, state);
}

/****************************************************************************************************
* one of the configurable items has changed
****************************************************************************************************/
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
	if (atlas_tool -> image_dimension.width != n_slice_x && n_slice_x > 0){
		atlas_tool -> image_dimension.width = n_slice_x;
		changed = TRUE;
	}
	if (atlas_tool -> image_dimension.height != n_slice_y && n_slice_y > 0){
		atlas_tool -> image_dimension.height = n_slice_y;
		changed = TRUE;
	}
	if (atlas_tool -> intersect_x != n_slice_intersect_x && n_slice_intersect_x >= 0){
		atlas_tool -> intersect_x = n_slice_intersect_x;
		changed = TRUE;
	}
	if (atlas_tool -> intersect_y != n_slice_intersect_y && n_slice_intersect_y >= 0){
		atlas_tool -> intersect_y = n_slice_intersect_y;
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

/****************************************************************************************************
* set the page information
****************************************************************************************************/
void atlas_tool_set_page_info(AtlasTool * atlas_tool, PageInformation page_info)
{
	atlas_tool -> page_info_stored = page_info;
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(atlas_tool -> radio_conf_page))){
		atlas_tool -> page_info = page_info;
		atlas_tool_set_image_size_from_page_info(atlas_tool);
	}
}

/****************************************************************************************************
* adjust the imagesize to fit the selected page-layout
****************************************************************************************************/
void atlas_tool_set_image_size_from_page_info(AtlasTool * atlas_tool)
{
	ImageDimension image_dim = get_image_dimension(atlas_tool -> page_info);
	atlas_tool -> image_dimension = image_dim;
	char buf1[20]; char buf2[20];
	sprintf(buf1, "%d", atlas_tool -> image_dimension.width);
	sprintf(buf2, "%d", atlas_tool -> image_dimension.height);
	gtk_entry_set_text(GTK_ENTRY(atlas_tool -> entry_slice_x), buf1);
	gtk_entry_set_text(GTK_ENTRY(atlas_tool -> entry_slice_y), buf2);
	g_signal_emit (atlas_tool, atlas_tool_signals[VALUES_CHANGED], 0);
}

/****************************************************************************************************
* adjust the pageinfo to be exactly fitting the imagesize
****************************************************************************************************/
void atlas_tool_set_page_info_from_image_size(AtlasTool * atlas_tool)
{
	atlas_tool -> page_info.border_top = 0;
	atlas_tool -> page_info.border_bottom = 0;
	atlas_tool -> page_info.border_left = 0;
	atlas_tool -> page_info.border_right = 0;
	atlas_tool -> page_info.resolution = 72;
	atlas_tool -> page_info.page_width = atlas_tool -> image_dimension.width * 25.4 / atlas_tool -> page_info.resolution;
	atlas_tool -> page_info.page_height = atlas_tool -> image_dimension.height * 25.4 / atlas_tool -> page_info.resolution;
}

/****************************************************************************************************
* set intersection values
****************************************************************************************************/
void atlas_tool_set_intersection(AtlasTool * atlas_tool, int intersect_x, int intersect_y)
{
	atlas_tool -> intersect_x = intersect_x,
	atlas_tool -> intersect_y = intersect_y;
	char buf1[20]; char buf2[20];
	sprintf(buf1, "%d", atlas_tool -> intersect_x);
	sprintf(buf2, "%d", atlas_tool -> intersect_y);
	gtk_entry_set_text(GTK_ENTRY(atlas_tool -> entry_slice_intersect_x), buf1);
	gtk_entry_set_text(GTK_ENTRY(atlas_tool -> entry_slice_intersect_y), buf2);
}

/****************************************************************************************************
* set image size
****************************************************************************************************/
void atlas_tool_set_image_dimension(AtlasTool * atlas_tool, ImageDimension image_dimension)
{
	atlas_tool -> image_dimension = image_dimension;
}

/****************************************************************************************************
* set the other values
****************************************************************************************************/
void atlas_tool_set_values(AtlasTool * atlas_tool, int zoom, gboolean visible)
{
	atlas_tool -> slice_zoom = zoom;
	atlas_tool -> visible = visible;
        gtk_combo_box_set_active(GTK_COMBO_BOX(atlas_tool -> combo), zoom - 1);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(atlas_tool -> check_show), visible);
}
