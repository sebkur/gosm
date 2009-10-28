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

#include "color_box.h"
#include "color_button.h"
#include "../map_types.h"
#include "../customio.h"

/****************************************************************************************************
* This is a combination of two widgets: an entry and a colour-button.
* the two widget are arranged horizontally in a box, first the entry, then the button.
* the two items are connected, so that changing the colour in on of them influences the
* representation of the other. the current value can be obtained from outside by the 
* function 'color_box_get_current_value'.
****************************************************************************************************/
G_DEFINE_TYPE (ColorBox, color_box, GTK_TYPE_HBOX);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint color_box_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, color_box_signals[SIGNAL_NAME_n], 0);

static gboolean color_box_entry_cb(GtkWidget * entry, ColorBox * color_box);
static gboolean color_box_button_cb(GtkWidget * button, GdkEventButton * event, ColorBox * color_box);

/****************************************************************************************************
* constructor
****************************************************************************************************/
GtkWidget * color_box_new()
{
	ColorBox * color_box = g_object_new(GOSM_TYPE_COLOR_BOX, NULL);
	color_box -> entry = gtk_entry_new();
	color_box -> button = color_button_new();
	gtk_widget_set_size_request(color_box -> button, 40, -1);
	gtk_box_pack_start(GTK_BOX(color_box), color_box -> entry, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(color_box), color_box -> button, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(color_box -> entry), "changed", G_CALLBACK(color_box_entry_cb), color_box);
	g_signal_connect(G_OBJECT(color_box -> button), "button_press_event", G_CALLBACK(color_box_button_cb), color_box);
	return GTK_WIDGET(color_box);
}

/****************************************************************************************************
* class init
****************************************************************************************************/
static void color_box_class_init(ColorBoxClass *class)
{
        /*color_box_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (ColorBoxClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void color_box_init(ColorBox *color_box)
{
}

/****************************************************************************************************
* return the currently selected value, e.g. '0.92,0.90,0.14,0.40'
* which is red, green, blue, alpha; each in range [0.0,1.0]
****************************************************************************************************/
const char * color_box_get_current_value(ColorBox * color_box)
{
	return gtk_entry_get_text(GTK_ENTRY(color_box -> entry));
}

/****************************************************************************************************
* set current value. parameter has to be in the form as obtained by the getter-function
****************************************************************************************************/
void color_box_set_current_value(ColorBox * color_box, char * value)
{
	gtk_entry_set_text(GTK_ENTRY(color_box -> entry), value);
	color_box -> last_correct_value = malloc(sizeof(char) * (strlen(value) + 1));
	strcpy(color_box -> last_correct_value, value);
}

/****************************************************************************************************
* callback for changes in the entry
* -> updates the appearance of the button
****************************************************************************************************/
static gboolean color_box_entry_cb(GtkWidget * entry, ColorBox * color_box)
{
	const char * value = gtk_entry_get_text(GTK_ENTRY(entry));
	/*if (color_box -> last_correct_value != NULL){
		printf("%s\n", color_box -> last_correct_value);
	}*/
	gchar ** split = g_strsplit(value, ",", 4);
	if (!(split[0] == NULL || split[1] == NULL || split[2] == NULL || split[3] == NULL)){
		// value is well-formed
		if (color_box -> last_correct_value != NULL) free(color_box -> last_correct_value);
		color_box -> last_correct_value = malloc(sizeof(char) * (strlen(value) + 1));
		strcpy(color_box -> last_correct_value, value);
		ColorQuadriple color_quad;
		color_quad.r = strtodouble(split[0]);
		color_quad.g = strtodouble(split[1]);
		color_quad.b = strtodouble(split[2]);
		color_quad.a = strtodouble(split[3]);
		color_button_set_color(GOSM_COLOR_BUTTON(color_box -> button), color_quad.r, color_quad.g, color_quad.b, color_quad.a);
	}
}

/****************************************************************************************************
* callback for changes of the button
* -> updates the contents of the entry
****************************************************************************************************/
static gboolean color_box_button_cb(GtkWidget * button, GdkEventButton * event, ColorBox * color_box)
{
	//printf("%s\n", gtk_entry_get_text(GTK_ENTRY(entry)));
	GtkWidget * dialog = gtk_color_selection_dialog_new("Select color");
	GtkColorSelection * sel = GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(dialog) -> colorsel);
	gtk_color_selection_set_has_opacity_control(sel, TRUE);

	gchar ** split = g_strsplit(color_box -> last_correct_value, ",", 4);
	if (!(split[0] == NULL || split[1] == NULL || split[2] == NULL || split[3] == NULL)){
		// value is well-formed
		double dr = strtodouble(split[0]);
		double dg = strtodouble(split[1]);
		double db = strtodouble(split[2]);
		double da = strtodouble(split[3]);
		GdkColor color;
		color.red = dr * 65535;
		color.green = dg * 65535;
		color.blue = db * 65535;
		guint16 a = da * 65535;
		gtk_color_selection_set_current_color(sel, &color);
		gtk_color_selection_set_current_alpha(sel, a);
	}

	gint response = gtk_dialog_run(GTK_DIALOG(dialog));
	if (response != GTK_RESPONSE_OK){
		gtk_widget_destroy(dialog);
		return;
	}
	GdkColor color;
	gtk_color_selection_get_current_color(GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(dialog) -> colorsel), &color);
	guint16 opacity = gtk_color_selection_get_current_alpha(GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(dialog) -> colorsel));
	gtk_widget_destroy(dialog);
	char buf[100];
	double red = ((double)color.red) / 65535;
	double green = ((double)color.green) / 65535;
	double blue = ((double)color.blue) / 65535;
	double alpha = ((double)opacity) / 65535;
	sprintdouble(buf, red, 2);
	sprintf(buf+strlen(buf), "%s", ",");
	sprintdouble(buf+strlen(buf), green, 2);
	sprintf(buf+strlen(buf), "%s", ",");
	sprintdouble(buf+strlen(buf), blue, 2);
	sprintf(buf+strlen(buf), "%s", ",");
	sprintdouble(buf+strlen(buf), alpha, 2);
	gtk_entry_set_text(GTK_ENTRY(color_box -> entry), buf);
}

