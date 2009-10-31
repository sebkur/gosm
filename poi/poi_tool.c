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
#include <string.h>

#include "poi_tool.h"
#include "../config/color_button_auto.h"

/****************************************************************************************************
* PoiTool acts as a view for the PoiManager
* it is a frontend for editing all aspects of PoiSources and PoiLayers
****************************************************************************************************/
G_DEFINE_TYPE (PoiTool, poi_tool, GTK_TYPE_VBOX);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint poi_tool_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, poi_tool_signals[SIGNAL_NAME_n], 0);

/****************************************************************************************************
* method declarations
****************************************************************************************************/
static gboolean poi_tool_button_layers_add_cb(GtkWidget * button, gpointer data);
static gboolean poi_tool_button_layers_delete_cb(GtkWidget * button, gpointer data);
static gboolean poi_tool_button_layers_save_cb(GtkWidget * button, gpointer data);
static gboolean poi_tool_button_layers_revert_cb(GtkWidget * button, gpointer data);
static gboolean poi_tool_button_sources_add_cb(GtkWidget * button, gpointer data);
static gboolean poi_tool_button_sources_delete_cb(GtkWidget * button, gpointer data);
static gboolean poi_tool_button_sources_save_cb(GtkWidget * button, gpointer data);
static gboolean poi_tool_button_api_request_cb(GtkWidget * button, gpointer data);
static gboolean poi_tool_api_start_cb(PoiManager * poi_manager, gpointer data);
static gboolean poi_tool_api_end_cb(PoiManager * poi_manager, int index, gpointer data);
static gboolean poi_tool_button_clear_pois_cb(GtkWidget * button, gpointer data);

/****************************************************************************************************
* constructor
****************************************************************************************************/
GtkWidget * poi_tool_new(PoiManager * poi_manager)
{
	PoiTool * poi_tool = g_object_new(GOSM_TYPE_POI_TOOL, NULL);
	poi_tool -> poi_manager = poi_manager;
	/* boxes */
	GtkWidget * box_layers = gtk_vbox_new(FALSE, 0);
	GtkWidget * box_source= gtk_vbox_new(FALSE, 0);
	/* BOX layers */
	/* layers, poi_selector widget */
	poi_tool -> poi_selector = GOSM_POI_SELECTOR(poi_selector_new(poi_manager));
	/* layers, tool bar */
	GtkWidget * tool_bar_layers = gtk_hbox_new(FALSE, 0);
	GtkWidget * button_layers_add = gtk_button_new();
	GtkWidget * button_layers_delete = gtk_button_new();
	GtkWidget * button_layers_save = gtk_button_new();
	GtkWidget * button_layers_revert = gtk_button_new();
	GtkWidget * icon_add = gtk_image_new_from_stock("gtk-add", GTK_ICON_SIZE_MENU);
	GtkWidget * icon_delete = gtk_image_new_from_stock("gtk-remove", GTK_ICON_SIZE_MENU);
	GtkWidget * icon_save = gtk_image_new_from_stock("gtk-save", GTK_ICON_SIZE_MENU);
	GtkWidget * icon_revert = gtk_image_new_from_stock("gtk-revert-to-saved", GTK_ICON_SIZE_MENU);
	gtk_button_set_image(GTK_BUTTON(button_layers_add), icon_add);
	gtk_button_set_image(GTK_BUTTON(button_layers_delete), icon_delete);
	gtk_button_set_image(GTK_BUTTON(button_layers_save), icon_save);
	gtk_button_set_image(GTK_BUTTON(button_layers_revert), icon_revert);
	gtk_box_pack_start(GTK_BOX(tool_bar_layers), button_layers_add, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(tool_bar_layers), button_layers_delete, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(tool_bar_layers), button_layers_save, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(tool_bar_layers), button_layers_revert, FALSE, FALSE, 0);
	gtk_widget_set_tooltip_text(button_layers_add, "add layer");
	gtk_widget_set_tooltip_text(button_layers_delete, "remove layer");
	gtk_widget_set_tooltip_text(button_layers_save, "save layers");
	gtk_widget_set_tooltip_text(button_layers_revert, "revert to saved");
	/* layers, layout */
	gtk_box_pack_start(GTK_BOX(box_layers), tool_bar_layers, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_layers), GTK_WIDGET(poi_tool -> poi_selector), TRUE, TRUE, 0);
	/* layers, callbacks */
	g_signal_connect(
		G_OBJECT(button_layers_add), "clicked",
		G_CALLBACK(poi_tool_button_layers_add_cb), (gpointer)poi_tool);
	g_signal_connect(
		G_OBJECT(button_layers_delete), "clicked",
		G_CALLBACK(poi_tool_button_layers_delete_cb), (gpointer)poi_tool);
	g_signal_connect(
		G_OBJECT(button_layers_save), "clicked",
		G_CALLBACK(poi_tool_button_layers_save_cb), (gpointer)poi_tool);
	g_signal_connect(
		G_OBJECT(button_layers_revert), "clicked",
		G_CALLBACK(poi_tool_button_layers_revert_cb), (gpointer)poi_tool);
	g_signal_connect(
		G_OBJECT(poi_manager),"api-request-started",
		G_CALLBACK(poi_tool_api_start_cb), (gpointer)poi_tool);
	g_signal_connect(
		G_OBJECT(poi_manager),"api-request-ended", 
		G_CALLBACK(poi_tool_api_end_cb), (gpointer)poi_tool);
	/* BOX sources */
	/* sources, source_selector widget */
	poi_tool -> poi_source_selector = poi_source_selector_new(poi_manager);
	/* sources, tool bar */
	GtkWidget * tool_bar_sources = gtk_hbox_new(FALSE, 0);
	GtkWidget * button_sources_add = gtk_button_new();
	GtkWidget * button_sources_delete = gtk_button_new();
	GtkWidget * button_sources_save = gtk_button_new();
	GtkWidget * button_sources_repos = gtk_button_new();
	poi_tool -> button_api = gtk_button_new_with_label("API");
	GtkWidget * button_clear_pois = gtk_button_new();
	GtkWidget * icon_add2 = gtk_image_new_from_stock("gtk-add", GTK_ICON_SIZE_MENU);
	GtkWidget * icon_delete2 = gtk_image_new_from_stock("gtk-remove", GTK_ICON_SIZE_MENU);
	GtkWidget * icon_save2 = gtk_image_new_from_stock("gtk-save", GTK_ICON_SIZE_MENU);
	GtkWidget * icon_repos = gtk_image_new_from_stock("gtk-network", GTK_ICON_SIZE_MENU);
	GtkWidget * icon_clear = gtk_image_new_from_stock("gtk-clear", GTK_ICON_SIZE_MENU);
	gtk_button_set_image(GTK_BUTTON(button_sources_add), icon_add2);
	gtk_button_set_image(GTK_BUTTON(button_sources_delete), icon_delete2);
	gtk_button_set_image(GTK_BUTTON(button_sources_save), icon_save2);
	gtk_button_set_image(GTK_BUTTON(button_sources_repos), icon_repos);
	gtk_button_set_image(GTK_BUTTON(button_clear_pois), icon_clear);
	gtk_box_pack_start(GTK_BOX(tool_bar_sources), button_sources_add, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(tool_bar_sources), button_sources_delete, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(tool_bar_sources), button_sources_save, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(tool_bar_sources), button_sources_repos, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(tool_bar_sources), poi_tool -> button_api, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(tool_bar_sources), button_clear_pois, FALSE, FALSE, 0);
	gtk_widget_set_tooltip_text(button_sources_add, "add source file");
	gtk_widget_set_tooltip_text(button_sources_delete, "remove selected file");
	gtk_widget_set_tooltip_text(button_sources_save, "save list");
	gtk_widget_set_tooltip_text(button_sources_repos, "sync to repository (not implemented)");
	gtk_widget_set_tooltip_text(poi_tool -> button_api, "request api for pois");
	gtk_widget_set_tooltip_text(button_clear_pois, "remove all visible pois");
	/* sources, layout */
	gtk_box_pack_start(GTK_BOX(box_source), tool_bar_sources, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_source), GTK_WIDGET(poi_tool -> poi_source_selector), TRUE, TRUE, 0);
	/* sources, callbacks */
	g_signal_connect(
		G_OBJECT(button_sources_add), "clicked",
		G_CALLBACK(poi_tool_button_sources_add_cb), (gpointer)poi_tool);
	g_signal_connect(
		G_OBJECT(button_sources_delete), "clicked",
		G_CALLBACK(poi_tool_button_sources_delete_cb), (gpointer)poi_tool);
	g_signal_connect(
		G_OBJECT(button_sources_save), "clicked",
		G_CALLBACK(poi_tool_button_sources_save_cb), (gpointer)poi_tool);
	g_signal_connect(
		G_OBJECT(poi_tool -> button_api), "clicked",
		G_CALLBACK(poi_tool_button_api_request_cb), (gpointer)poi_tool);
	g_signal_connect(
		G_OBJECT(button_clear_pois), "clicked",
		G_CALLBACK(poi_tool_button_clear_pois_cb), (gpointer)poi_tool);

	/* notebook */
	GtkWidget * notebook = gtk_notebook_new();
	GtkWidget * label_layers = gtk_label_new("Layers");
	GtkWidget * label_source = gtk_label_new("Sources");
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), box_layers, label_layers);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), box_source, label_source);
	/* layout */
	gtk_box_pack_start(GTK_BOX(poi_tool), notebook, TRUE, TRUE, 0);
	return GTK_WIDGET(poi_tool);
}

static void poi_tool_class_init(PoiToolClass *class)
{
        /*poi_tool_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PoiToolClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void poi_tool_init(PoiTool *poi_tool)
{
}

/****************************************************************************************************
* add a poi-layer
****************************************************************************************************/
static gboolean poi_tool_button_layers_add_cb(GtkWidget * button, gpointer data)
{
	PoiTool * poi_tool = GOSM_POI_TOOL(data);
	GtkDialog * dialog = GTK_DIALOG(gtk_dialog_new_with_buttons(
		"Enter Key and Value",
		NULL,
		GTK_DIALOG_MODAL,
		GTK_STOCK_OK,
		GTK_RESPONSE_ACCEPT,
		GTK_STOCK_CANCEL,
		GTK_RESPONSE_REJECT,
		NULL));
	GtkWidget * entry_key = gtk_entry_new();
	GtkWidget * entry_val = gtk_entry_new();
	GtkWidget * colour_button = color_button_auto_new();
	gtk_widget_set_size_request(colour_button, -1, 20);
	GtkBox * box = GTK_BOX(dialog -> vbox);
	gtk_box_pack_start(box, entry_key, FALSE, FALSE, 0);
	gtk_box_pack_start(box, entry_val, FALSE, FALSE, 0);
	gtk_box_pack_start(box, colour_button, FALSE, FALSE, 0);
	gtk_widget_show_all(GTK_WIDGET(dialog));
	int response = gtk_dialog_run(dialog);
	if (response == GTK_RESPONSE_ACCEPT){
		double r, g, b, a;
		color_button_auto_get_current_value(GOSM_COLOR_BUTTON_AUTO(colour_button), &r, &g, &b, &a);
		const char * ckey = gtk_entry_get_text(GTK_ENTRY(entry_key));
		const char * cval = gtk_entry_get_text(GTK_ENTRY(entry_val));
		char * key = malloc(sizeof(char) * (strlen(ckey) + 1));
		strcpy(key, ckey);
		char * val = malloc(sizeof(char) * (strlen(cval) + 1));
		strcpy(val, cval);
		gtk_widget_destroy(GTK_WIDGET(dialog));
		poi_manager_add_poi_set(
			poi_tool -> poi_manager,
			key, val,
			TRUE, r, g, b, a);
		free(key);
		free(val);
	}else{
		gtk_widget_destroy(GTK_WIDGET(dialog));
	}
	return FALSE;
}

/****************************************************************************************************
* remove a poi-layer
****************************************************************************************************/
static gboolean poi_tool_button_layers_delete_cb(GtkWidget * button, gpointer data)
{
	PoiTool * poi_tool = GOSM_POI_TOOL(data);
	PoiSelector * poi_selector = poi_tool -> poi_selector;
	int index = poi_selector_get_active_item_index(poi_selector);
	if (index >= 0){
		poi_manager_delete_poi_set(poi_tool -> poi_manager, index);
	}
	return FALSE;
}

/****************************************************************************************************
* save poi-layers
****************************************************************************************************/
static gboolean poi_tool_button_layers_save_cb(GtkWidget * button, gpointer data)
{
	PoiTool * poi_tool = GOSM_POI_TOOL(data);
	//TODO: use boolean return value of save-function
	poi_manager_layers_save(poi_tool -> poi_manager);
	return FALSE;
}

/****************************************************************************************************
* revert layers to saved state
****************************************************************************************************/
static gboolean poi_tool_button_layers_revert_cb(GtkWidget * button, gpointer data)
{
	PoiTool * poi_tool = GOSM_POI_TOOL(data);
	poi_manager_layers_revert(poi_tool -> poi_manager);
	return FALSE;
}

/****************************************************************************************************
* add a poi-source
****************************************************************************************************/
static gboolean poi_tool_button_sources_add_cb(GtkWidget * button, gpointer data)
{
	GtkWidget * chooser = gtk_file_chooser_dialog_new("Select Osm-File to add",
					NULL,
					GTK_FILE_CHOOSER_ACTION_OPEN,
					GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
					NULL);
	gtk_window_set_modal(GTK_WINDOW(chooser), TRUE);
	int result = gtk_dialog_run(GTK_DIALOG(chooser));
	//TODO: some other dialogs don't trigger the delete-event
	if (result == GTK_RESPONSE_CANCEL || result == GTK_RESPONSE_DELETE_EVENT){
		gtk_widget_destroy(chooser);
	}
	if (result == GTK_RESPONSE_ACCEPT){
		char * filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(chooser));
		char * fn = malloc(sizeof(char) * (strlen(filename) + 1));
		strcpy(fn, filename);
		gtk_widget_destroy(chooser);
		PoiTool * poi_tool = GOSM_POI_TOOL(data);
		poi_manager_sources_add(poi_tool -> poi_manager, fn);
		free(fn);
	}
	return FALSE;
}

/****************************************************************************************************
* remove a poi-source
****************************************************************************************************/
static gboolean poi_tool_button_sources_delete_cb(GtkWidget * button, gpointer data)
{
	PoiTool * poi_tool = GOSM_POI_TOOL(data);
	PoiSourceSelector * poi_source_selector = poi_tool -> poi_source_selector;
	int index = poi_source_selector_get_active_item_index(poi_source_selector);
	if (index >= 0){
		poi_manager_sources_delete(poi_tool -> poi_manager, index);
	}
	return FALSE;
}

/****************************************************************************************************
* save the list of poi-sources
****************************************************************************************************/
static gboolean poi_tool_button_sources_save_cb(GtkWidget * button, gpointer data)
{
	PoiTool * poi_tool = GOSM_POI_TOOL(data);
	poi_manager_sources_save(poi_tool -> poi_manager);
	return FALSE;
}

/****************************************************************************************************
* request the api for currently visible area
****************************************************************************************************/
static gboolean poi_tool_button_api_request_cb(GtkWidget * button, gpointer data)
{
	PoiTool * poi_tool = GOSM_POI_TOOL(data);
	poi_manager_api_request(poi_tool -> poi_manager);
	return FALSE;
}

/****************************************************************************************************
* callback; when api-request started, deactivate the api-button, so that there is always maximally
* one api-request running
****************************************************************************************************/
static gboolean poi_tool_api_start_cb(PoiManager * poi_manager, gpointer data)
{
	PoiTool * poi_tool = GOSM_POI_TOOL(data);
	gtk_widget_set_sensitive(poi_tool -> button_api, FALSE);
}

/****************************************************************************************************
* callback; when api-request ended, acitvate the api-button, so that the next api-request can be 
* stated
****************************************************************************************************/
static gboolean poi_tool_api_end_cb(PoiManager * poi_manager, int index, gpointer data)
{
	PoiTool * poi_tool = GOSM_POI_TOOL(data);
	gdk_threads_enter();
	gtk_widget_set_sensitive(poi_tool -> button_api, TRUE);
	gdk_threads_leave();
}

/****************************************************************************************************
* remove all pois from poi_manager
****************************************************************************************************/
static gboolean poi_tool_button_clear_pois_cb(GtkWidget * button, gpointer data)
{
	PoiTool * poi_tool = GOSM_POI_TOOL(data);
	poi_manager_clear_pois(poi_tool -> poi_manager);
}
