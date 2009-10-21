#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>

#include "foo_widget.h"

void * thread_fun(gpointer data);
static gboolean close_cb(GtkWidget * widget);

GtkWidget * main_window;
pthread_cond_t  cond;
pthread_mutex_t mutex;
pthread_mutex_t mutex_curl;

int main(int argc, char * argv[])
{
	curl_global_init(CURL_GLOBAL_ALL);
	g_thread_init(NULL);
	gdk_threads_init();
	gdk_threads_enter();
	gtk_init(&argc, &argv);

	volatile GType dummy = GOSM_TYPE_FOO_WIDGET;

//	printf("gpointer has size %ld\n", sizeof(gpointer));
//	printf("void* has size %ld\n", sizeof(void*));

	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(main_window), "Foo");
	gtk_window_set_default_size(GTK_WINDOW(main_window), 500, 300);
	g_signal_connect(G_OBJECT(main_window), "hide", G_CALLBACK(close_cb), NULL);
	gtk_widget_show_all(main_window);

	int ic = pthread_cond_init (&cond, NULL);
	int im = pthread_mutex_init(&mutex, NULL);
	int icurl = pthread_mutex_init(&mutex_curl, NULL);
	if (ic != 0) printf("error initializing condition variable\n");
	if (im != 0) printf("error initializing mutex variable\n");
	int x;
	for (x = 0; x < 8; x++){
		pthread_t thread;
		int p_netw = pthread_create(&thread, NULL, thread_fun, GINT_TO_POINTER(x));
	}

	GtkWidget * foo_wid = foo_widget_new();
	FooWidget * foo = GOSM_FOO_WIDGET(foo_wid);
	gtk_container_add(GTK_CONTAINER(main_window), foo_wid);

	gtk_main();
	gdk_threads_leave();
	return 0;
}

static gboolean close_cb(GtkWidget * widget)
{
	exit(0);
}


void * thread_fun(gpointer data)
{
	int i = GPOINTER_TO_INT(data);
	pthread_mutex_lock(&mutex_curl);
	CURL * easyhandle = curl_easy_init();
	pthread_cond_wait(&cond, &mutex);
	pthread_mutex_unlock(&mutex_curl);
	return (NULL);
}
