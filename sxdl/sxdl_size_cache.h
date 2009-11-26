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

#ifndef _SXDL_SIZE_CACHE_H_
#define _SXDL_SIZE_CACHE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#define GOSM_TYPE_SXDL_SIZE_CACHE           (sxdl_size_cache_get_type ())
#define GOSM_SXDL_SIZE_CACHE(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_SXDL_SIZE_CACHE, SxdlSizeCache))
#define GOSM_SXDL_SIZE_CACHE_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_SXDL_SIZE_CACHE, SxdlSizeCacheClass))
#define GOSM_IS_SXDL_SIZE_CACHE(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_SXDL_SIZE_CACHE))
#define GOSM_IS_SXDL_SIZE_CACHE_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_SXDL_SIZE_CACHE))
#define GOSM_SXDL_SIZE_CACHE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_SXDL_SIZE_CACHE, SxdlSizeCacheClass))

typedef struct _SxdlSizeCache        SxdlSizeCache;
typedef struct _SxdlSizeCacheClass   SxdlSizeCacheClass;

struct _SxdlSizeCache
{
	GObject parent;

	GTree * tree;
};

struct _SxdlSizeCacheClass
{
	GObjectClass parent_class;

	//void (* function_name) (SxdlSizeCache *sxdl_size_cache);
};

SxdlSizeCache * sxdl_size_cache_new();

void sxdl_size_cache_add(SxdlSizeCache * cache, int key, int width, int height);
gboolean sxdl_size_cache_get(SxdlSizeCache * cache, int key, int * width, int * height);

#endif /* _SXDL_SIZE_CACHE_H_ */
