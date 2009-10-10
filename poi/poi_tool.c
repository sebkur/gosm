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

#include "poi_tool.h"

G_DEFINE_TYPE (PoiTool, poi_tool, GTK_TYPE_VBOX);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint poi_tool_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, poi_tool_signals[SIGNAL_NAME_n], 0);

static gboolean poi_tool_button_save_cb(GtkWidget * button, gpointer data);
static gboolean poi_tool_button_revert_cb(GtkWidget * button, gpointer data);

GtkWidget * poi_tool_new(PoiManager * poi_manager)
{
	PoiTool * poi_tool = g_object_new(GOSM_TYPE_POI_TOOL, NULL);
	poi_tool -> poi_manager = poi_manager;
	/* boxes */
	GtkWidget * box_layers = gtk_vbox_new(FALSE, 0);
	GtkWidget * box_source= gtk_vbox_new(FALSE, 0);
	/* BOX layers */
	/* the poi_selector widget */
	poi_tool -> poi_selector = GOSM_POI_SELECTOR(poi_selector_new(poi_manager));
	GtkWidget * poi_selector_scrolled = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(poi_selector_scrolled), 
		GTK_WIDGET(poi_tool -> poi_selector));
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (poi_selector_scrolled),
	                                GTK_POLICY_AUTOMATIC,
	                                GTK_POLICY_AUTOMATIC);
	/* tool bar */
	GtkWidget * tool_bar = gtk_hbox_new(FALSE, 0);
	GtkWidget * button_save = gtk_button_new();
	GtkWidget * button_revert = gtk_button_new();
	GtkWidget * icon_save = gtk_image_new_from_stock("gtk-save", GTK_ICON_SIZE_BUTTON);
	GtkWidget * icon_revert = gtk_image_new_from_stock("gtk-revert-to-saved", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(button_save), icon_save);
	gtk_button_set_image(GTK_BUTTON(button_revert), icon_revert);
	gtk_box_pack_start(GTK_BOX(tool_bar), button_save, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(tool_bar), button_revert, FALSE, FALSE, 0);
	/* layers layout */
	gtk_box_pack_start(GTK_BOX(box_layers), tool_bar, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_layers), poi_selector_scrolled, TRUE, TRUE, 0);
	/* callbacks */
	g_signal_connect(G_OBJECT(button_save), "clicked", G_CALLBACK(poi_tool_button_save_cb), (gpointer)poi_tool);
	g_signal_connect(G_OBJECT(button_revert), "clicked", G_CALLBACK(poi_tool_button_revert_cb), (gpointer)poi_tool);
	/* BOX source */
	poi_tool -> poi_source_selector = poi_source_selector_new(poi_manager);
	gtk_box_pack_start(GTK_BOX(box_source), GTK_WIDGET(poi_tool -> poi_source_selector), TRUE, TRUE, 0);
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

static gboolean poi_tool_button_save_cb(GtkWidget * button, gpointer data)
{
	PoiTool * poi_tool = GOSM_POI_TOOL(data);
	poi_manager_save(poi_tool -> poi_manager);
	return FALSE;
}

static gboolean poi_tool_button_revert_cb(GtkWidget * button, gpointer data)
{
	PoiTool * poi_tool = GOSM_POI_TOOL(data);
	poi_manager_revert(poi_tool -> poi_manager);
	return FALSE;
}
