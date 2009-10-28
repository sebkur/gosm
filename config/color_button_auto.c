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

#include "color_button_auto.h"
#include "color_button.h"
#include "../map_types.h"
#include "../customio.h"

/****************************************************************************************************
* this is a coloured button that can be used to let the user select a colour.
* the buttons appears in the preselected colour. when the user clicks the button, a colour-chooser-
* dialog will appear.
****************************************************************************************************/
G_DEFINE_TYPE (ColorButtonAuto, color_button_auto, GTK_TYPE_HBOX);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint color_button_auto_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, color_button_auto_signals[SIGNAL_NAME_n], 0);

static gboolean color_button_auto_entry_cb(GtkWidget * entry, ColorButtonAuto * color_button_auto);
static gboolean color_button_auto_button_cb(GtkWidget * button, GdkEventButton * event, ColorButtonAuto * color_button_auto);

/****************************************************************************************************
* constructor
****************************************************************************************************/
GtkWidget * color_button_auto_new()
{
	ColorButtonAuto * color_button_auto = g_object_new(GOSM_TYPE_COLOR_BUTTON_AUTO, NULL);
	color_button_auto -> button = color_button_new();
	gtk_box_pack_start(GTK_BOX(color_button_auto), color_button_auto -> button, TRUE, TRUE, 0);
	g_signal_connect(
		G_OBJECT(color_button_auto -> button), "button_press_event", 
		G_CALLBACK(color_button_auto_button_cb), color_button_auto);
	return GTK_WIDGET(color_button_auto);
}

static void color_button_auto_class_init(ColorButtonAutoClass *class)
{
        /*color_button_auto_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (ColorButtonAutoClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void color_button_auto_init(ColorButtonAuto *color_button_auto)
{
}

/****************************************************************************************************
* get the current value
****************************************************************************************************/
void color_button_auto_get_current_value(ColorButtonAuto * color_button_auto, double *r, double *g, double *b, double *a)
{
	*r = GOSM_COLOR_BUTTON(color_button_auto -> button) -> r;
	*g = GOSM_COLOR_BUTTON(color_button_auto -> button) -> g;
	*b = GOSM_COLOR_BUTTON(color_button_auto -> button) -> b;
	*a = GOSM_COLOR_BUTTON(color_button_auto -> button) -> a;
}

/****************************************************************************************************
* set the current value externaly
****************************************************************************************************/
void color_button_auto_set_current_value(ColorButtonAuto * color_button_auto, double r, double g, double b, double a)
{
	color_button_set_color(
		GOSM_COLOR_BUTTON(color_button_auto -> button), r, g, b, a);
}

/****************************************************************************************************
* show the dialog to select a colour for the button
****************************************************************************************************/
static gboolean color_button_auto_button_cb(GtkWidget * button, GdkEventButton * event, ColorButtonAuto * color_button_auto)
{
	GtkWidget * dialog = gtk_color_selection_dialog_new("Select color");
	GtkColorSelection * sel = GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(dialog) -> colorsel);
	gtk_color_selection_set_has_opacity_control(sel, TRUE);

	GdkColor color;
	color.red = GOSM_COLOR_BUTTON(color_button_auto -> button) -> r * 65535;
	color.green = GOSM_COLOR_BUTTON(color_button_auto -> button) -> g * 65535;
	color.blue = GOSM_COLOR_BUTTON(color_button_auto -> button) -> b * 65535;
	guint16 a = GOSM_COLOR_BUTTON(color_button_auto -> button) -> a * 65535;
	gtk_color_selection_set_current_color(sel, &color);
	gtk_color_selection_set_current_alpha(sel, a);

	gint response = gtk_dialog_run(GTK_DIALOG(dialog));
	if (response != GTK_RESPONSE_OK){
		gtk_widget_destroy(dialog);
		return FALSE;
	}
	gtk_color_selection_get_current_color(GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(dialog) -> colorsel), &color);
	guint16 opacity = gtk_color_selection_get_current_alpha(GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(dialog) -> colorsel));
	gtk_widget_destroy(dialog);
	double red = ((double)color.red) / 65535;
	double green = ((double)color.green) / 65535;
	double blue = ((double)color.blue) / 65535;
	double alpha = ((double)opacity) / 65535;
	color_button_set_color (GOSM_COLOR_BUTTON(color_button_auto -> button), red, green, blue, alpha);
	return FALSE;
}

