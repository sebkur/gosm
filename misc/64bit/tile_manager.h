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

#include <pthread.h>
#include <glib.h>
#include <glib/gthread.h>
#include <unistd.h>

#include <curl/curl.h>

#ifndef _TILE_MANAGER_H_
#define _TILE_MANAGER_H_

#define GOSM_TYPE_TILE_MANAGER		(tile_manager_get_type ())
#define GOSM_TILE_MANAGER(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_TILE_MANAGER, TileManager))
#define GOSM_TILE_MANAGER_CLASS(obj)	(G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TILE_MANAGER, TileManagerClass))
#define GOSM_IS_TILE_MANAGER(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_TILE_MANAGER))
#define GOSM_IS_TILE_MANAGER_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_TILE_MANAGER))
#define GOSM_TILE_MANAGER_GET_CLASS	(G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_TILE_MANAGER, TileManagerClass))

typedef struct _TileManager		TileManager;
typedef struct _TileManagerClass	TileManagerClass;

struct _TileManager
{
	GObject parent;

	pthread_cond_t  cond_wait_load_from_netw;
	pthread_mutex_t mutex_wait_load_from_netw;
};

struct _TileManagerClass
{
	GObjectClass parent_class;
};

GObject * tile_manager_new();

#endif
