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

#include "about.h"
#include "../tool.h"
#include "../map_area.h"
#include "../paths.h"

// this is bad; VERY BAD
static MapArea * map_area;

G_DEFINE_TYPE (AboutDialog, about_dialog, GTK_TYPE_WINDOW);

static void about_dialog_class_init(AboutDialogClass *class)
{
	GtkWidgetClass *widget_class;
	widget_class = GTK_WIDGET_CLASS(class);
}

static void about_dialog_init(AboutDialog *about_dialog)
{
}

GtkWidget * about_widget_new(AboutDialog *about_dialog);

/****************************************************************************************************
* constructor
****************************************************************************************************/
GtkWidget * about_dialog_new(GtkWindow * parent_window, MapArea * m_a)
{
	AboutDialog * win = g_object_new(GOSM_TYPE_ABOUT_DIALOG, NULL);

        gtk_window_set_title(GTK_WINDOW(win), "About GOSM");
        gtk_window_set_transient_for(GTK_WINDOW(win), GTK_WINDOW(parent_window));
        gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER_ON_PARENT);
        gtk_window_set_modal(GTK_WINDOW(win), TRUE);

	GtkWidget * about_wid = about_widget_new(win);
	gtk_container_add(GTK_CONTAINER(win), about_wid);

	// UGLY, problem is, that the JavaScript-Callback has no custom reference-passing mechanism
	map_area = m_a;

	return GTK_WIDGET(win);
}

/****************************************************************************************************
* get the notebook
****************************************************************************************************/
GtkNotebook * about_dialog_get_notebook(AboutDialog * about_dialog)
{
	return GTK_NOTEBOOK(about_dialog -> notebook);
}

/****************************************************************************************************
* this function gets called, when the user clicks a link that 
* calls the function 'showpos' from javascript
* it will then position the map to the position given by the javascript functions' parameters
****************************************************************************************************/
JSValueRef show_pos_cb(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception){
        //printf("show_pos function called from javascript\n");
	double values[3];
	int valcount = 0;
	int i;
	for (i = 0; i < argumentCount; i++){
		if (JSValueIsNumber(ctx, arguments[i])){
			values[i] = JSValueToNumber(ctx, arguments[i], NULL);
			valcount ++;
		}
	}
	if (valcount == 3){
		int zoom = (int)values[0];
		printf("goto: zoom: %d, lon: %f, lat: %f\n", zoom, values[1], values[2]);
	}
	gdk_threads_init(); //TODO: only once??
	gdk_threads_enter();
	map_area_goto_lon_lat_zoom(map_area, values[1], values[2], (int)values[0]);
	map_area_repaint(map_area);
	//gtk_widget_queue_draw(GTK_WIDGET(map_area));
	gdk_threads_leave();
}

/****************************************************************************************************
* create the widget
****************************************************************************************************/
GtkWidget * about_widget_new(AboutDialog * about_dialog)
{

	about_dialog -> notebook = gtk_notebook_new();

	char * uri_cr = get_abs_uri(GOSM_ABOUT_DIR "copyright.html");
	char * uri_osm = get_abs_uri(GOSM_ABOUT_DIR "aboutosm.html");
	char * uri_nf = get_abs_uri(GOSM_ABOUT_DIR "namefinder.html");
	char * uri_gpl = get_abs_uri(GOSM_ABOUT_DIR "gpl.html");
	
	GtkWidget * web1 = webkit_web_view_new();
	webkit_web_view_open(WEBKIT_WEB_VIEW(web1), uri_cr);
	free(uri_cr);
	GtkWidget * scrolled1 = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolled1), web1);
        gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled1),
                                        GTK_POLICY_AUTOMATIC,
                                        GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(scrolled1, 400, 300);
	// Javascript
	WebKitWebFrame * frame = webkit_web_view_get_main_frame(WEBKIT_WEB_VIEW(web1));
	JSGlobalContextRef context = webkit_web_frame_get_global_context(frame);
	JSObjectRef global = JSContextGetGlobalObject(context);
	JSStringRef name = JSStringCreateWithUTF8CString("showpos");
	JSObjectRef obref = JSObjectMakeFunctionWithCallback(context, name, show_pos_cb);
	JSObjectSetProperty(context, global, name, obref,
                kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly, NULL);
	// Javascript

	GtkWidget * web2 = webkit_web_view_new();
	webkit_web_view_open(WEBKIT_WEB_VIEW(web2), uri_osm);
	free(uri_osm);
	GtkWidget * scrolled2 = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolled2), web2);
        gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled2),
                                        GTK_POLICY_AUTOMATIC,
                                        GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(scrolled2, 400, 300);

	GtkWidget * web4 = webkit_web_view_new();
	webkit_web_view_open(WEBKIT_WEB_VIEW(web4), uri_nf);
	free(uri_nf);
	GtkWidget * scrolled4 = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolled4), web4);
        gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled4),
                                        GTK_POLICY_AUTOMATIC,
                                        GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(scrolled4, 400, 300);

	GtkWidget * web3 = webkit_web_view_new();
	webkit_web_view_open(WEBKIT_WEB_VIEW(web3), uri_gpl);
	free(uri_gpl);
	GtkWidget * scrolled3 = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolled3), web3);
        gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled3),
                                        GTK_POLICY_AUTOMATIC,
                                        GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(scrolled3, 400, 300);

	GtkWidget *label_copyright= gtk_label_new("GOSM");
	GtkWidget *label_aboutosm = gtk_label_new("OpenStreetMap");
	GtkWidget *label_namefinder = gtk_label_new("Namefinder");
	GtkWidget *label_license = gtk_label_new("License");
	gtk_notebook_append_page(GTK_NOTEBOOK(about_dialog -> notebook), scrolled1, label_copyright);
	gtk_notebook_append_page(GTK_NOTEBOOK(about_dialog -> notebook), scrolled2, label_aboutosm);
	gtk_notebook_append_page(GTK_NOTEBOOK(about_dialog -> notebook), scrolled4, label_namefinder);
	gtk_notebook_append_page(GTK_NOTEBOOK(about_dialog -> notebook), scrolled3, label_license);
	return about_dialog -> notebook;
}
