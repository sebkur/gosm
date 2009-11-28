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
#include <string.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "sxdl_size_cache.h"

G_DEFINE_TYPE (SxdlSizeCache, sxdl_size_cache, G_TYPE_OBJECT);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint sxdl_size_cache_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, sxdl_size_cache_signals[SIGNAL_NAME_n], 0);

typedef struct{
	int w;
	int h;
}CacheRect; 

int sxdl_size_cache_int_compare(gconstpointer a, gconstpointer b)
{
	return *(int*) a - *(int*)b;
}

SxdlSizeCache * sxdl_size_cache_new()
{
	SxdlSizeCache * sxdl_size_cache = g_object_new(GOSM_TYPE_SXDL_SIZE_CACHE, NULL);
	sxdl_size_cache -> tree = g_tree_new_full(sxdl_size_cache_int_compare, NULL, free, free);
	return sxdl_size_cache;
}

void sxdl_size_cache_finalize(SxdlSizeCache * cache)
{
	printf("finalize\n");
	g_tree_destroy(cache -> tree);
}

static void sxdl_size_cache_class_init(SxdlSizeCacheClass *class)
{
	GObjectClass * object_class = G_OBJECT_CLASS(class);
	object_class -> finalize = sxdl_size_cache_finalize;
        /*sxdl_size_cache_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (SxdlSizeCacheClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void sxdl_size_cache_init(SxdlSizeCache *sxdl_size_cache)
{
}

void sxdl_size_cache_add(SxdlSizeCache * cache, int key, int width, int height)
{
	CacheRect * rect = malloc(sizeof(CacheRect));
	rect -> w = width;
	rect -> h = height;
	int * k = malloc(sizeof(int));
	*k = key;
	g_tree_insert(cache -> tree, k, rect);
}

gboolean sxdl_size_cache_get(SxdlSizeCache * cache, int key, int * width, int * height)
{
	CacheRect * rect = g_tree_lookup(cache -> tree, &key);
	if (rect == NULL) return FALSE;
	*width = rect -> w;
	*height = rect -> h;
	return TRUE;
}

