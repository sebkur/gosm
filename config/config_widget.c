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
#include <errno.h>
#include <string.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "config.h"
#include "config_widget.h"
#include "color_box.h"
#include "../map_types.h"
#include "../customio.h"
#include "../tool.h"

/****************************************************************************************************
* this is a widget that displays the configuration and let's the user edit the values
****************************************************************************************************/
G_DEFINE_TYPE (ConfigWidget, config_widget, GTK_TYPE_VBOX);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint config_widget_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, config_widget_signals[SIGNAL_NAME_n], 0);

/****************************************************************************************************
* constructor
****************************************************************************************************/
GtkWidget * config_widget_new(Config * config)
{
	ConfigWidget * config_widget = g_object_new(GOSM_TYPE_CONFIG_WIDGET, NULL);
	config_widget -> config = config;
	config_widget_construct(config_widget, config);
	return GTK_WIDGET(config_widget);
}

static void config_widget_class_init(ConfigWidgetClass *class)
{
        /*config_widget_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (ConfigWidgetClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void config_widget_init(ConfigWidget *config_widget)
{
}

/****************************************************************************************************
* when the user clicked "apply"
****************************************************************************************************/
static gboolean preferences_confirm_cb(GtkWidget * widget, ConfigWidget * conf_widget)
{
	gboolean * changed = config_widget_get_new_configuration(conf_widget);
	free(changed);
	//TODO: inspect if something actually changed
	GtkWidget * window = find_containing_gtk_window(GTK_WIDGET(conf_widget));
	gtk_widget_destroy(window);
	config_save_config_file(conf_widget -> config);
}

/****************************************************************************************************
* when the user cancelled
****************************************************************************************************/
static gboolean preferences_cancel_cb(GtkWidget * widget, GtkWindow *window)
{
	gtk_widget_destroy(GTK_WIDGET(window));
}

/****************************************************************************************************
* present this widget in a window
****************************************************************************************************/
void config_widget_show_in_window(ConfigWidget * config_widget, GtkWindow * parent)
{
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Preferences");
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER_ON_PARENT);
	gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(parent));
	gtk_window_set_modal(GTK_WINDOW(window), TRUE);
	//TODO: reconnect this signal
	//g_signal_connect(G_OBJECT(window), "hide", G_CALLBACK(close_cb), NULL);
	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(config_widget));
	gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
	g_signal_connect(
		G_OBJECT(config_widget -> button_cancel), "clicked", 
		G_CALLBACK(preferences_cancel_cb), window);
        g_signal_connect(
		G_OBJECT(config_widget -> button_confirm), "clicked", 
		G_CALLBACK(preferences_confirm_cb), config_widget);
        gtk_widget_show_all(window);
}

/****************************************************************************************************
* apply the configuration edited by the user to the configuration-array
* return an array of gbooleans indicating whether each of the entries has changed
****************************************************************************************************/
gboolean * config_widget_get_new_configuration(ConfigWidget * config_widget)
{
	Config * config = config_widget -> config;
	gboolean * changed = malloc(sizeof(gboolean) * config -> num_entries);
	int i;
	for (i = 0; i < config -> num_entries; i++){
		ConfEntry * ce = &(config -> entries[i]);
		switch(ce -> type){
		case TYPE_BOOLEAN:{
			if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(config_widget -> entries[i]))){
				changed[i] = config_set_entry_data(ce, "TRUE");
			}else{
				changed[i] = config_set_entry_data(ce, "FALSE");
			}
			break;
		}
		case TYPE_COLOR:{
			const char * text = color_box_get_current_value(GOSM_COLOR_BOX(config_widget -> entries[i]));
			int size = strlen(text) + 1;
	                char * buf = malloc(sizeof(char) * size);
	                strncpy(buf, text, size);
			changed[i] = config_set_entry_data(ce, buf);
			free(buf);
			break;
		}
		default:{
			const char * text = gtk_entry_get_text(GTK_ENTRY(config_widget -> entries[i]));
			int size = strlen(text) + 1;
	                char * buf = malloc(sizeof(char) * size);
	                strncpy(buf, text, size);
			changed[i] = config_set_entry_data(ce, buf);
			free(buf);
		}
		}
	}
//	for (i = 0; i < config -> num_entries; i++){
//		printf("%d\n", changed[i]);
//	}
	return changed;
}

/****************************************************************************************************
* construct the widget
****************************************************************************************************/
config_widget_construct(ConfigWidget * config_widget, Config * config)
{
	GtkWidget *box_v = GTK_WIDGET(config_widget); 

	int count = config -> num_entries; 

	GtkWidget * labels[count];
	config_widget -> entries = malloc(sizeof(GtkWidget*) * count);
	GtkWidget ** entries = config_widget -> entries;

	int i; char buf[100];
	for (i = 0; i < count; i++){
		ConfEntry ce = config -> entries[i];
		labels[i] = gtk_label_new(ce.name);
		switch (ce.type){
		case TYPE_DIR:
		case TYPE_IP:{
			sprintf(buf, "%s", (char*)ce.data);
			entries[i] = gtk_entry_new();
			gtk_entry_set_text(GTK_ENTRY(entries[i]), buf) ;
			break;
		}
		case TYPE_COLOR:{
			sprintdouble(buf, ((ColorQuadriple*)ce.data) -> r, 2);
			sprintf(buf+strlen(buf), "%s", ",");
			sprintdouble(buf+strlen(buf), ((ColorQuadriple*)ce.data) -> g, 2);
			sprintf(buf+strlen(buf), "%s", ",");
			sprintdouble(buf+strlen(buf), ((ColorQuadriple*)ce.data) -> b, 2);
			sprintf(buf+strlen(buf), "%s", ",");
			sprintdouble(buf+strlen(buf), ((ColorQuadriple*)ce.data) -> a, 2);
			entries[i] = color_box_new();
			color_box_set_current_value(GOSM_COLOR_BOX(entries[i]), buf);
			break;
		}
		case TYPE_INT:{
			sprintf(buf, "%d", *(int*)ce.data);
			entries[i] = gtk_entry_new();
			gtk_entry_set_text(GTK_ENTRY(entries[i]), buf);
			break;
		}
		case TYPE_DOUBLE:{
			sprintdouble(buf, *(double*)ce.data, 4);
			entries[i] = gtk_entry_new();
			gtk_entry_set_text(GTK_ENTRY(entries[i]), buf);
			break;
		}
		case TYPE_BOOLEAN:{ 
			entries[i] = gtk_check_button_new();
			gboolean active = *(gboolean*)ce.data;
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(entries[i]), active);
			break;
		}
		default:{
			printf("default\n");
			entries[i] = gtk_entry_new();
			gtk_entry_set_text(GTK_ENTRY(entries[i]), ce.data_str);
		}
		}
	}

	GtkWidget * table = gtk_table_new(2, 9, FALSE);
	for (i = 0; i < count; i++){
		GtkWidget * box = gtk_hbox_new(FALSE, 0);
		gtk_box_pack_start(GTK_BOX(box), labels[i], FALSE, FALSE, 0);
		gtk_table_attach(GTK_TABLE(table), box,  0, 1, i, i+1, GTK_EXPAND | GTK_FILL, 0, 0, 0);
		gtk_table_attach(GTK_TABLE(table), entries[i], 1, 2, i, i+1, GTK_EXPAND | GTK_FILL, 0, 0, 0);
	}

	GtkWidget * scrolled = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled), table);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled), 
					GTK_POLICY_AUTOMATIC, 
					GTK_POLICY_AUTOMATIC);

	GtkWidget * box_buttons = gtk_hbox_new(FALSE, 0);
	config_widget -> button_confirm = gtk_button_new_from_stock("gtk-apply");
	config_widget -> button_cancel  = gtk_button_new_from_stock("gtk-cancel");
	gtk_box_pack_start(GTK_BOX(box_buttons), config_widget -> button_confirm, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_buttons), config_widget -> button_cancel , FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(box_v), scrolled, TRUE, TRUE, 0);
	gtk_box_pack_end  (GTK_BOX(box_v), box_buttons, FALSE, FALSE, 0);
}
