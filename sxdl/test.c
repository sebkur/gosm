#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <libgen.h>
#include <locale.h>
#include <math.h>

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>

#include "sxdl_widget.h"

int main(int argc, char *argv[])
{
	if (argc < 2){
		printf("usage: test <filename>\n");
		exit(1);
	}
	char * filename = argv[1];
	/* init g_threads, gtk_threads, gtk */
	g_thread_init(NULL);
	gdk_threads_init();
	gtk_init(&argc, &argv);

	int width = 300;
	int height = 500;
	GtkWidget * main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(main_window), "Sxdl Renderer");
	gtk_window_set_default_size(GTK_WINDOW(main_window), width, height);
	g_signal_connect(G_OBJECT(main_window), "hide", G_CALLBACK(exit), NULL);

	SxdlWidget * sxdl = sxdl_widget_new();
        GtkWidget * scrolled = gtk_scrolled_window_new(NULL, NULL);
        gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled),
                                        GTK_POLICY_AUTOMATIC,
                                        GTK_POLICY_AUTOMATIC);
        gtk_container_add(GTK_CONTAINER(scrolled), sxdl);
	sxdl_widget_set_uri(sxdl, filename);
	gtk_container_add(GTK_CONTAINER(main_window), GTK_WIDGET(scrolled));

	gtk_widget_show_all(main_window);

	gdk_threads_enter();	
	gtk_main();
	gdk_threads_leave();
}
