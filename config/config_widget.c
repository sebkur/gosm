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
#include "configuration.h"
#include "config_widget.h"
#include "color_box.h"
#include "../map_types.h"
#include "../customio.h"

G_DEFINE_TYPE (ConfigWidget, config_widget, GTK_TYPE_VBOX);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint config_widget_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, config_widget_signals[SIGNAL_NAME_n], 0);

GtkWidget * config_widget_new(Configuration * configuration)
{
	ConfigWidget * config_widget = g_object_new(GOSM_TYPE_CONFIG_WIDGET, NULL);
	config_widget -> configuration = configuration;
	config_widget_construct(config_widget, configuration);
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

// TODO: this function returns nothing???
gboolean ** config_widget_get_new_configuration(ConfigWidget * config_widget)
{
	Configuration * configuration = config_widget -> configuration;
	gboolean * changed = malloc(sizeof(gboolean) * configuration -> count);
	int i;
	for (i = 0; i < configuration -> count; i++){
		ConfEntry * ce = &(configuration -> entries[i]);
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
	for (i = 0; i < configuration -> count; i++){
		printf("%d\n", changed[i]);
	}
}

config_widget_construct(ConfigWidget * config_widget, Configuration * configuration)
{
	GtkWidget *box_v = GTK_WIDGET(config_widget); 

	int count = configuration -> count; 

	GtkWidget * labels[count];
	config_widget -> entries = malloc(sizeof(GtkWidget*) * count);
	GtkWidget ** entries = config_widget -> entries;

	int i; char buf[100];
	for (i = 0; i < count; i++){
		ConfEntry ce = configuration -> entries[i];
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
