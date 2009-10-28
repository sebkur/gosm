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

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <webkit/webkit.h>
#include <JavaScriptCore/JavaScript.h>

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "manual.h"
#include "../tool.h"
#include "../paths.h"

G_DEFINE_TYPE (ManualDialog, manual_dialog, GTK_TYPE_WINDOW);

static void manual_dialog_class_init(ManualDialogClass *class)
{
}

static void manual_dialog_init(ManualDialog *manual_dialog)
{
}

GtkWidget * manual_widget_new(ManualDialog *manual_dialog);

/****************************************************************************************************
* constructor, create the manual dialog
****************************************************************************************************/
GtkWidget * manual_dialog_new(GtkWindow * parent_window)
{
	ManualDialog * win = g_object_new(GOSM_TYPE_MANUAL_DIALOG, NULL);

        gtk_window_set_title(GTK_WINDOW(win), "Manual GOSM");
        gtk_window_set_transient_for(GTK_WINDOW(win), GTK_WINDOW(parent_window));
        gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER_ON_PARENT);
        gtk_window_set_modal(GTK_WINDOW(win), TRUE);

	GtkWidget * manual_wid = manual_widget_new(win);
	gtk_container_add(GTK_CONTAINER(win), manual_wid);

	return GTK_WIDGET(win);
}

/****************************************************************************************************
* create the webkit view showing the manual pages
****************************************************************************************************/
GtkWidget * manual_widget_new(ManualDialog * manual_dialog)
{
	char * uri = get_abs_uri(GOSM_MANUAL_DIR "index.html");

	GtkWidget * web = webkit_web_view_new();
	webkit_web_view_open(WEBKIT_WEB_VIEW(web), uri);
	free(uri);
	GtkWidget * scrolled = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolled), web);
        gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled),
                                        GTK_POLICY_AUTOMATIC,
                                        GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(scrolled, 600, 400);
	return scrolled;
}
