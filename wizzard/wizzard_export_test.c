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

#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include <pthread.h>
#include <glib.h>
#include <glib/gthread.h>
#include <unistd.h>

//#include "wizzard_export.h"
#include "wizzard_atlas_sequence.h"

int main(int argc, char *argv[])
{
	int width = 600, height = 500;
	gtk_init(&argc, &argv);
	GtkWidget *widget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title((GtkWindow*)widget, "GOsmView");
	gtk_window_set_default_size((GtkWindow*)widget, width, height);
	g_signal_connect(G_OBJECT(widget), "hide", G_CALLBACK(gtk_main_quit), NULL);

	GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
	GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start((GtkBox*)hbox, vbox,   TRUE, TRUE, 0);

	gtk_container_add((GtkContainer*)widget, hbox);
	gtk_widget_show_all(widget);

	/*Selection s;
	s.lon1 = 13.336029;
	s.lon2 = 13.492584;
	s.lat1 = 52.557151;
	s.lat2 = 52.485289;
	WizzardExport * wizzard = wizzard_export_new(GTK_WINDOW(widget), s, 11);
	wizzard_export_show(wizzard);*/

	WizzardAtlasSequence * wizzard = wizzard_atlas_sequence_new(GTK_WINDOW(widget));
	wizzard_atlas_sequence_show(wizzard);

	gtk_main();
}
