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

#include <gtk/gtk.h>
#include <gdk/gdk.h>

char * names[] = {
	"gtk-dialog-authentication",
	"gtk-dialog-info",
	"gtk-dialog-warning",
	"gtk-dialog-error",
	"gtk-dialog-question",
	"gtk-dnd",
	"gtk-dnd-multiple",
	"gtk-about",
	"gtk-add",
	"gtk-apply",
	"gtk-bold",
	"gtk-cancel",
	"gtk-cdrom",
	"gtk-clear",
	"gtk-close",
	"gtk-color-picker",
	"gtk-convert",
	"gtk-connect",
	"gtk-copy",
	"gtk-cut",
	"gtk-delete",
	"gtk-directory",
	"gtk-discard",
	"gtk-disconnect",
	"gtk-edit",
	"gtk-execute",
	"gtk-file",
	"gtk-find",
	"gtk-find-and-replace",
	"gtk-floppy",
	"gtk-fullscreen",
	"gtk-goto-bottom",
	"gtk-goto-first",
	"gtk-goto-last",
	"gtk-goto-top",
	"gtk-go-back",
	"gtk-go-down",
	"gtk-go-forward",
	"gtk-go-up",
	"gtk-harddisk",
	"gtk-help",
	"gtk-home",
	"gtk-index",
	"gtk-indent",
	"gtk-info",
	"gtk-unindent",
	"gtk-italic",
	"gtk-jump-to",
	"gtk-justify-center",
	"gtk-justify-fill",
	"gtk-justify-left",
	"gtk-justify-right",
	"gtk-leave-fullscreen",
	"gtk-missing-image",
	"gtk-media-forward",
	"gtk-media-next",
	"gtk-media-pause",
	"gtk-media-play",
	"gtk-media-previous",
	"gtk-media-record",
	"gtk-media-rewind",
	"gtk-media-stop",
	"gtk-network",
	"gtk-new",
	"gtk-no",
	"gtk-ok",
	"gtk-open",
	"gtk-orientation-portrait",
	"gtk-orientation-landscape",
	"gtk-orientation-reverse-landscape",
	"gtk-orientation-reverse-portrait",
	"gtk-page-setup",
	"gtk-paste",
	"gtk-preferences",
	"gtk-print",
	"gtk-print-error",
	"gtk-print-paused",
	"gtk-print-preview",
	"gtk-print-report",
	"gtk-print-warning",
	"gtk-properties",
	"gtk-quit",
	"gtk-redo",
	"gtk-refresh",
	"gtk-remove",
	"gtk-revert-to-saved",
	"gtk-save",
	"gtk-save-as",
	"gtk-select-all",
	"gtk-select-color",
	"gtk-select-font",
	"gtk-sort-ascending",
	"gtk-sort-descending",
	"gtk-spell-check",
	"gtk-stop",
	"gtk-strikethrough",
	"gtk-undelete",
	"gtk-underline",
	"gtk-undo",
	"gtk-yes",
	"gtk-zoom-100",
	"gtk-zoom-fit",
	"gtk-zoom-in",
	"gtk-zoom-out",
	"gtk-stock-insert-note"
};

int main(int argc, char *argv[])
{
	int width = 1000, height = 700;

	gtk_init(&argc, &argv);
	GtkWidget *widget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title((GtkWindow*)widget, "GOsmView");
	gtk_window_set_default_size((GtkWindow*)widget, width, height);
	g_signal_connect(G_OBJECT(widget), "hide", G_CALLBACK(gtk_main_quit), NULL);

	GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
	GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start((GtkBox*)hbox, vbox,   TRUE, TRUE, 0);

	//GtkWidget * vertical = gtk_vbox_new(FALSE, 0);
	GtkWidget * table = gtk_table_new(4, 3, FALSE);

	int cols = 5;
	int i;
	for (i = 0; i < sizeof(names); i++){	
		char * name = names[i];
		GtkWidget * box = gtk_hbox_new(FALSE, 0);
		GtkWidget * icon = gtk_image_new_from_stock(name, GTK_ICON_SIZE_BUTTON);
		GtkWidget * label = gtk_label_new(name);
		gtk_box_pack_start(GTK_BOX(box), icon, FALSE, FALSE, 3);
		gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 3);
		//gtk_box_pack_start(GTK_BOX(vertical), box, FALSE, FALSE, 0);
		gtk_table_attach((GtkTable*)table, box,	i%cols, i%cols + 1, i/cols, i/cols + 1, GTK_FILL, 0, 3, 3);
	}

	GtkWidget * scrolled = gtk_scrolled_window_new(NULL, NULL);
        //gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled), vertical);	
        gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled), table);	
	gtk_box_pack_start((GtkBox*)vbox, scrolled, TRUE, TRUE, 0);

	gtk_container_add((GtkContainer*)widget, hbox);
	gtk_widget_show_all(widget);
	gtk_main();
}
