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

#include <unistd.h>
#include <wait.h>

#include "tile_manager.h"

static gboolean close_cb(GtkWidget * widget);

GtkWidget * main_window;

int main(int argc, char * argv[])
{
	g_thread_init(NULL);
	gdk_threads_init();
	gtk_init(&argc, &argv);

	/***************************************************************************
	 * GTK WIDGETS AND LAYOUT, CALLBACKS
	 ***************************************************************************/
	/* main window */
	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(main_window), "GOsmView");
	gtk_window_set_default_size(GTK_WINDOW(main_window), 500, 300);
	g_signal_connect(G_OBJECT(main_window), "hide", G_CALLBACK(close_cb), NULL);

	gtk_widget_show_all(main_window);

	TileManager * tile_manager = tile_manager_new();
	GtkWidget * foo_wid = foo_widget_new();
	gtk_container_add(GTK_CONTAINER(main_window), foo_wid);

	gdk_threads_enter();	
	gtk_main();
	gdk_threads_leave();
}

static gboolean close_cb(GtkWidget * widget)
{
	exit(0);
}
