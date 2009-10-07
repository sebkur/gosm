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

G_DEFINE_TYPE (PoiSelector, poi_selector, GTK_TYPE_VBOX);

enum
{
	POI_SELECTOR_TOGGLED,
        LAST_SIGNAL
};

static guint poi_selector_signals[LAST_SIGNAL] = { 0 };

void poi_selector_add_pair(PoiSelector * poi_selector, gboolean active, char * key, char * value);
static gboolean poi_selector_check_cb(GtkWidget *widget, gpointer num_p);

GtkWidget * poi_selector_new(PoiManager * poi_manager)
{
	PoiSelector * poi_selector = g_object_new(GOSM_TYPE_POI_SELECTOR, NULL);
	poi_selector -> poi_manager = poi_manager;
	poi_selector -> table = gtk_table_new(0, 3, FALSE);
	poi_selector -> number_of_entries = 0;
	poi_selector -> key_value_pairs = g_array_new(FALSE, FALSE, sizeof(KeyValueBoolean));
	poi_selector -> buttons = g_array_new(FALSE, FALSE, sizeof(GtkWidget*));
	int num_poi_sets = poi_manager_get_number_of_poi_sets(poi_manager);
	int poi;
	for (poi = 0; poi < num_poi_sets; poi++){
		KeyValueBooleanPoiSet * kvbps = poi_manager_get_poi_set(poi_manager, poi);
		poi_selector_add_pair(poi_selector, kvbps -> active, kvbps -> key, kvbps -> value);
	}
	gtk_box_pack_start(GTK_BOX(poi_selector), poi_selector -> table, FALSE, FALSE, 0);
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

void poi_selector_add_pair(PoiSelector * poi_selector, gboolean active, char * key, char * value)
{
	KeyValueBoolean node;
	node.key = malloc(sizeof(char) * (strlen(key) + 1));
	node.value = malloc(sizeof(char) * (strlen(value) + 1));
	strcpy(node.key, key);
	strcpy(node.value, value);
	node.active = active;
	g_array_append_val(poi_selector -> key_value_pairs, node);
	/* insert into table */
	poi_selector -> number_of_entries ++;
	int pos = poi_selector -> number_of_entries;
	gtk_table_resize(GTK_TABLE(poi_selector -> table), poi_selector -> number_of_entries, 3);
	GtkWidget * label_key = gtk_label_new(key);
	GtkWidget * label_val = gtk_label_new(value);
	GtkWidget * button = gtk_check_button_new();
	g_array_append_val(poi_selector -> buttons, button);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), active);
	int num = poi_selector -> buttons -> len - 1;
	PoiSelectorCheckSignalData * data = malloc(sizeof(PoiSelectorCheckSignalData));
	data -> poi_selector = poi_selector;
	data -> num = num;
	g_signal_connect(G_OBJECT(button), "toggled", G_CALLBACK(poi_selector_check_cb), data);
	gtk_misc_set_alignment(GTK_MISC(label_key), 0.0, 0.0);
	gtk_misc_set_alignment(GTK_MISC(label_val), 0.0, 0.0);
	gtk_table_attach(GTK_TABLE(poi_selector -> table), button, 0, 1, pos-1, pos, 0, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(poi_selector -> table), label_key, 1, 2, pos-1, pos, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	gtk_table_attach(GTK_TABLE(poi_selector -> table), label_val, 2, 3, pos-1, pos, GTK_EXPAND | GTK_FILL, 0, 0, 0);
}

static gboolean poi_selector_check_cb(GtkWidget *widget, gpointer signal_data)
{
	PoiSelectorCheckSignalData * data = (PoiSelectorCheckSignalData*) signal_data;
	PoiSelector * poi_selector = data -> poi_selector;
	KeyValueBoolean * kvb = &(((KeyValueBoolean*)(void*)(poi_selector -> key_value_pairs -> data))[data -> num]);
	gboolean active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
	kvb -> active = active;
//	printf("foo %d %s %s\n", data -> num, kvb -> key, kvb -> value);
//	int k;
//	for (k = 0; k < poi_selector -> number_of_entries; k++){
//		KeyValueBoolean kvb = g_array_index(poi_selector -> key_value_pairs, KeyValueBoolean, k);
//		printf("%d\n", kvb.active);
//	}
	g_signal_emit (poi_selector, poi_selector_signals[POI_SELECTOR_TOGGLED], 0, kvb);
}
