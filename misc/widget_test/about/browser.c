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
#include <webkit/webkit.h>

#include <limits.h>
#include <stdlib.h>
#include <string.h>

char * get_abs_uri(char * filename)
{
	if (filename == NULL) return NULL;
	char * abs = realpath(filename, NULL);
	if (abs == NULL) return NULL;
	char * uri_prefix = "file://";
	char * uri = malloc(sizeof(char) * (strlen(uri_prefix) + strlen(abs) + 1));
	uri[0] = '\0';
	strcat(uri, uri_prefix);
	strcat(uri, abs);
	free(abs);
	return uri;
}

GtkWidget * browser_widget_new()
{

	GtkWidget *notebook = gtk_notebook_new();

	char * uri_cr = get_abs_uri("copyright.html");
	char * uri_gpl = get_abs_uri("gpl.html");
	
	GtkWidget * web1 = webkit_web_view_new();
	webkit_web_view_open(WEBKIT_WEB_VIEW(web1), uri_cr);
	free(uri_cr);
	GtkWidget * scrolled1 = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolled1), web1);
        gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled1),
                                        GTK_POLICY_AUTOMATIC,
                                        GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(scrolled1, 400, 300);

	GtkWidget * web2 = webkit_web_view_new();
	webkit_web_view_open(WEBKIT_WEB_VIEW(web2), uri_gpl);
	free(uri_gpl);
	GtkWidget * scrolled2 = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolled2), web2);
        gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled2),
                                        GTK_POLICY_AUTOMATIC,
                                        GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(scrolled2, 400, 300);

	GtkWidget *label_copyright= gtk_label_new("Copyright");
	GtkWidget *label_license = gtk_label_new("License");
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scrolled1, label_copyright);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scrolled2, label_license);
	return notebook;
}
