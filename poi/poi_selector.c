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

#include "poi_selector.h"
#include "../config/color_button.h"

G_DEFINE_TYPE (PoiSelector, poi_selector, GTK_TYPE_VBOX);

enum
{
	POI_SELECTOR_TOGGLED,
        LAST_SIGNAL
};

typedef struct {
	PoiSelector * poi_selector;
	int index;
	StyledPoiSet * poi_set;
	GtkWidget * label_key;
	GtkWidget * label_val;
	GtkWidget * check_visible;
	GtkWidget * color_button;
} PoiSelectorRow;

static guint poi_selector_signals[LAST_SIGNAL] = { 0 };

void poi_selector_add_pair(PoiSelector * poi_selector, StyledPoiSet * poi_set);
static gboolean poi_selector_check_cb(GtkWidget *widget, gpointer data);
static gboolean poi_selector_color_button_cb(GtkWidget *widget, GdkEventButton * button, gpointer data);
static gboolean poi_selector_colour_changed_cb(PoiManager * poi_manager, int index, gpointer data);

GtkWidget * poi_selector_new(PoiManager * poi_manager)
{
	PoiSelector * poi_selector = g_object_new(GOSM_TYPE_POI_SELECTOR, NULL);
	poi_selector -> poi_manager = poi_manager;
	poi_selector -> table = gtk_table_new(0, 4, FALSE);
	poi_selector -> number_of_entries = 0;
	poi_selector -> rows = g_array_new(FALSE, FALSE, sizeof(PoiSelectorRow*));
	int num_poi_sets = poi_manager_get_number_of_poi_sets(poi_manager);
	int poi;
	for (poi = 0; poi < num_poi_sets; poi++){
		StyledPoiSet * poi_set = poi_manager_get_poi_set(poi_manager, poi);
		poi_selector_add_pair(poi_selector, poi_set);
	}
	gtk_box_pack_start(GTK_BOX(poi_selector), poi_selector -> table, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(poi_manager),"colour-changed", G_CALLBACK(poi_selector_colour_changed_cb), (gpointer)poi_selector);
	return GTK_WIDGET(poi_selector);
}

static void poi_selector_class_init(PoiSelectorClass *class)
{
        poi_selector_signals[POI_SELECTOR_TOGGLED] = g_signal_new(
                "poi-selector-toggled",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PoiSelectorClass, poi_selector_toggled),
                NULL, NULL,
                g_cclosure_marshal_VOID__POINTER,
                G_TYPE_NONE, 1, G_TYPE_POINTER);
}

static void poi_selector_init(PoiSelector *poi_selector)
{
}

void poi_selector_add_pair(PoiSelector * poi_selector, StyledPoiSet * poi_set)
{
	PoiSelectorRow * row = malloc(sizeof(PoiSelectorRow));
	row -> poi_selector = poi_selector;
	row -> index = poi_selector -> number_of_entries;
	row -> poi_set = poi_set;
	/* insert into table */
	poi_selector -> number_of_entries ++;
	int pos = poi_selector -> number_of_entries;
	gtk_table_resize(GTK_TABLE(poi_selector -> table), poi_selector -> number_of_entries, 4);
	row -> label_key = gtk_label_new(named_poi_set_get_key(GOSM_NAMED_POI_SET(poi_set)));
	row -> label_val = gtk_label_new(named_poi_set_get_value(GOSM_NAMED_POI_SET(poi_set)));
	row -> check_visible = gtk_check_button_new();
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(row -> check_visible), poi_set_get_visible(GOSM_POI_SET(poi_set)));
	row -> color_button = color_button_new();
	double r, g, b, a;
	styled_poi_set_get_colour(poi_set, &r, &g, &b, &a);
	color_button_set_color(GOSM_COLOR_BUTTON(row -> color_button), r, g, b, a);
	gtk_widget_set_size_request(row -> color_button, 10, 10);
	g_array_append_val(poi_selector -> rows, row);
	g_signal_connect(G_OBJECT(row -> check_visible), "toggled", G_CALLBACK(poi_selector_check_cb), (gpointer)row);
	g_signal_connect(G_OBJECT(row -> color_button), "button-press-event",
		G_CALLBACK(poi_selector_color_button_cb), (gpointer)row);
	gtk_misc_set_alignment(GTK_MISC(row -> label_key), 0.0, 0.0);
	gtk_misc_set_alignment(GTK_MISC(row -> label_val), 0.0, 0.0);
	gtk_table_attach(GTK_TABLE(poi_selector -> table), row -> check_visible, 0, 1, pos-1, pos, 0, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(poi_selector -> table), row -> color_button, 1, 2, pos-1, pos, 
		GTK_EXPAND | GTK_FILL, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(poi_selector -> table), row -> label_key, 2, 3, pos-1, pos, GTK_EXPAND|GTK_FILL, 0, 2, 0);
	gtk_table_attach(GTK_TABLE(poi_selector -> table), row -> label_val, 3, 4, pos-1, pos, GTK_EXPAND|GTK_FILL, 0, 2, 0);
}

static gboolean poi_selector_check_cb(GtkWidget *widget, gpointer data)
{
	PoiSelectorRow * row = (PoiSelectorRow*) data;
	PoiSelector * poi_selector = row -> poi_selector;
	StyledPoiSet * poi_set = row -> poi_set;
	g_signal_emit (poi_selector, poi_selector_signals[POI_SELECTOR_TOGGLED], 0, (gpointer)poi_set);
	return FALSE;
}

static gboolean poi_selector_color_button_cb(GtkWidget *widget, GdkEventButton * button, gpointer data)
{
	PoiSelectorRow * row = (PoiSelectorRow*) data;
	PoiSelector * poi_selector = row -> poi_selector;
	StyledPoiSet * poi_set = row -> poi_set;

	GtkWidget * dialog = gtk_color_selection_dialog_new("Select color");
	GtkColorSelection * sel = GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(dialog) -> colorsel);
	gtk_color_selection_set_has_opacity_control(sel, TRUE);

	double r, g, b, a;
	styled_poi_set_get_colour(poi_set, &r, &g, &b, &a);
	GdkColor color;
	color.red = r * 65535;
	color.green = g * 65535;
	color.blue = b * 65535;
	guint16 alpha = a * 65535;

	gtk_color_selection_set_current_color(sel, &color);
	gtk_color_selection_set_current_alpha(sel, alpha);

	gint response = gtk_dialog_run(GTK_DIALOG(dialog));
	if (response != GTK_RESPONSE_OK){
		gtk_widget_destroy(dialog);
		return FALSE;
	}
	gtk_color_selection_get_current_color(GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(dialog) -> colorsel), &color);
	guint16 opacity = gtk_color_selection_get_current_alpha(GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(dialog) -> colorsel));
	gtk_widget_destroy(dialog);

	r = (double)color.red / 65535; 
	g = (double)color.green / 65535; 
	b = (double)color.blue / 65535; 
	a = (double)opacity / 65535; 

	poi_manager_set_poi_set_colour(poi_selector -> poi_manager, row -> index, r, g, b, a);
	return FALSE;
}

static gboolean poi_selector_colour_changed_cb(PoiManager * poi_manager, int index, gpointer data)
{
	PoiSelector * poi_selector = (PoiSelector*) data;
	PoiSelectorRow * row = g_array_index(poi_selector -> rows, PoiSelectorRow*, index);
	double r, g, b, a;
	styled_poi_set_get_colour(row -> poi_set, &r, &g, &b, &a);
	color_button_set_color(GOSM_COLOR_BUTTON(row -> color_button), r, g, b, a);
	return FALSE;
}
