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

#include <curl/curl.h>

#include <pthread.h>
#include <sched.h>
#include <glib.h>
#include <glib/gthread.h>
#include <unistd.h>

#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include <sys/time.h>

#include <string.h>

#include "tile_manager.h"

#define MATCHES(a,b)	(a & b) == b
#define TILESIZE	256

// OSM 1 == G 16
#define FORMAT_FILES	"/%d_%d_%d.png"

G_DEFINE_TYPE (TileManager, tile_manager, G_TYPE_OBJECT);

void function_load_from_netw(TileManager * tile_manager);

GObject * tile_manager_new()
{
	return g_object_new(GOSM_TYPE_TILE_MANAGER, NULL);
}

static void tile_manager_class_init(TileManagerClass *class)
{
}

static void tile_manager_init(TileManager *tile_manager)
{
	pthread_cond_init (&(tile_manager -> cond_wait_load_from_netw), NULL);
	pthread_mutex_init(&(tile_manager -> mutex_wait_load_from_netw), NULL);
	int x = 0; for (x = 0; x < 10; x++){
		pthread_t thread_netw;
		pthread_attr_t tattrn;
		pthread_attr_init(&tattrn);
		int p_netw = pthread_create(&thread_netw, &tattrn, (void *) function_load_from_netw, tile_manager);
	}
}

void function_load_from_netw(TileManager * tile_manager)
{
	printf("thread\n");
	CURL * easyhandle = curl_easy_init();
	pthread_cond_wait(&(tile_manager -> cond_wait_load_from_netw), &(tile_manager -> mutex_wait_load_from_netw));
}
