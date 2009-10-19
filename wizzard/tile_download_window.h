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
#include "../map_types.h"

#define GOSM_TYPE_TILE_DOWNLOAD_WINDOW		(tile_download_window_get_type ())
#define GOSM_TILE_DOWNLOAD_WINDOW(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_TILE_DOWNLOAD_WINDOW, TileDownloadWindow))
#define GOSM_TILE_DOWNLOAD_WINDOW_CLASS(obj)	(G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TILE_DOWNLOAD_WINDOW, TileDownloadWindowClass))
#define GOSM_IS_TILE_DOWNLOAD_WINDOW(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_TILE_DOWNLOAD_WINDOW))
#define GOSM_IS_TILE_DOWNLOAD_WINDOW_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_TILE_DOWNLOAD_WINDOW))
#define GOSM_TILE_DOWNLOAD_WINDOW_GET_CLASS	(G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_TILE_DOWNLOAD_WINDOW, TileDownloadWindowClass))

#ifndef _TILE_DOWNLOAD_WINDOW_H_
#define _TILE_DOWNLOAD_WINDOW_H_

#define TILE_DOWNLOAD_WINDOW_PAUSE_BUTTON_PAUSED	0
#define TILE_DOWNLOAD_WINDOW_PAUSE_BUTTON_RUNNING	1

typedef struct _TileDownloadWindow		TileDownloadWindow;
typedef struct _TileDownloadWindowClass	TileDownloadWindowClass;

struct _TileDownloadWindow
{
	GtkWindow parent;

	GtkWidget * bar;
	GtkWidget * label_ready_s;
	GtkWidget * label_failed_s;
	GtkWidget * button_pause;
	GtkWidget * button_cancel;
};

struct _TileDownloadWindowClass
{
	GtkWindowClass parent_class;
};

GtkWidget * tile_download_window_new(Selection * s, gboolean *todo);

void tile_download_window_set_pause_button_state(TileDownloadWindow * tile_download_window, int state);

#endif
