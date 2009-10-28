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

#include "gtk_custom_frame.h"
#include "gtk_rollup_button.h"

/****************************************************************************************************
* GtkCustomFrame is somehow like a GtkFrame.
* it add the functionality to show/hide the child-widget.
* TODO: although it works like this, this should propably implement the GtkContainer interface
****************************************************************************************************/
G_DEFINE_TYPE (GtkCustomFrame, gtk_custom_frame, GTK_TYPE_VBOX);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint gtk_custom_frame_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, gtk_custom_frame_signals[SIGNAL_NAME_n], 0);

/****************************************************************************************************
* method declarations
****************************************************************************************************/
static gboolean button_toggled_cb(GtkRollupButton * button, gpointer data);

/****************************************************************************************************
* constructor
****************************************************************************************************/
GtkWidget * gtk_custom_frame_new(char * title)
{
	GtkCustomFrame * gtk_custom_frame = g_object_new(GOSM_TYPE_GTK_CUSTOM_FRAME, NULL);
	/* we have two frames without labels for title and child */
	GtkWidget * frame_title = gtk_frame_new("");
	gtk_frame_set_label_widget(GTK_FRAME(frame_title), NULL);
	gtk_custom_frame -> frame_child = gtk_frame_new("");
	gtk_frame_set_label_widget(GTK_FRAME(gtk_custom_frame -> frame_child), NULL);
	/* put them into the widget */
	gtk_box_pack_start(GTK_BOX(gtk_custom_frame), frame_title, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(gtk_custom_frame), gtk_custom_frame -> frame_child, TRUE, TRUE, 0);
	/* an event-box for background of title */
	GtkWidget * box = gtk_event_box_new();
	gtk_container_add(GTK_CONTAINER(frame_title), box);
//	GdkColor color;
//	color.red = 0.4 * 65535;
//	color.green = 0.4 * 65535;
//	color.blue = 0.4 * 65535;
//	gtk_widget_modify_bg(GTK_WIDGET(box), GTK_STATE_NORMAL, &color);
	/* a hbox for title-label and button */
	GtkWidget * hbox = gtk_hbox_new(FALSE, 0);
	/* into the event-box */
	gtk_container_add(GTK_CONTAINER(box), hbox);
	/* the title's elements */
	GtkWidget * label_title = gtk_label_new(title);
	gtk_custom_frame -> button_rollup = gtk_rollup_button_new();
	/* the button into another vbox, so that it won't be higher than needed */
	GtkWidget * box_button = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_button), gtk_custom_frame -> button_rollup, FALSE, FALSE, 5);
	/* put label and button into hbox */
	gtk_box_pack_start(GTK_BOX(hbox), label_title, FALSE, FALSE, 5);
	gtk_box_pack_end(GTK_BOX(hbox), box_button, FALSE, FALSE, 5);
	/* initialize state of the rollup button */
	gtk_rollup_button_set_active(GOSM_GTK_ROLLUP_BUTTON(gtk_custom_frame -> button_rollup), TRUE);
	g_signal_connect(
		G_OBJECT(gtk_custom_frame -> button_rollup), "toggled",
		G_CALLBACK(button_toggled_cb), (gpointer)gtk_custom_frame);
	gtk_custom_frame_set_child_visible(gtk_custom_frame, FALSE);
	return GTK_WIDGET(gtk_custom_frame);
}

static void gtk_custom_frame_class_init(GtkCustomFrameClass *class)
{
        /*gtk_custom_frame_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (GtkCustomFrameClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void gtk_custom_frame_init(GtkCustomFrame *gtk_custom_frame)
{
}

/****************************************************************************************************
* add the child widget
* TODO: invalid usage of the container is not catched
****************************************************************************************************/
void gtk_custom_frame_add(GtkCustomFrame * gtk_custom_frame, GtkWidget * widget)
{
	gtk_custom_frame -> child = widget;
	gtk_container_add(GTK_CONTAINER(gtk_custom_frame -> frame_child), widget);
	gtk_widget_show_all(widget);
}

/****************************************************************************************************
* set/get wheter the child-widget is visible
****************************************************************************************************/
gboolean gtk_custom_frame_get_child_visible(GtkCustomFrame * gtk_custom_frame)
{
	return gtk_custom_frame -> child_visible;
}

void gtk_custom_frame_set_child_visible(GtkCustomFrame * gtk_custom_frame, gboolean visible)
{
	gtk_widget_set_no_show_all(gtk_custom_frame -> frame_child, !visible);
	gtk_custom_frame -> child_visible = visible;
	gtk_rollup_button_set_active(GOSM_GTK_ROLLUP_BUTTON(gtk_custom_frame -> button_rollup), visible);
	if (visible){
		gtk_widget_show(gtk_custom_frame -> frame_child);
	}else{
		gtk_widget_hide(gtk_custom_frame -> frame_child);
	}
}

/****************************************************************************************************
* when the rollup-button has been clicked, toggle visibility of the child
****************************************************************************************************/
static gboolean button_toggled_cb(GtkRollupButton * button, gpointer data)
{
	GtkCustomFrame * frame = GOSM_GTK_CUSTOM_FRAME(data);
	if (frame -> child != NULL){
		if (gtk_rollup_button_get_active(button)){
			gtk_widget_set_no_show_all(frame -> frame_child, FALSE);
			gtk_widget_show(frame -> frame_child);
		}else{
			gtk_widget_hide(frame -> frame_child);
			gtk_widget_set_no_show_all(frame -> frame_child, TRUE);
		}
	}
	return FALSE;
}
