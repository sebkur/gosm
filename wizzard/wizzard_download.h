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
#include "select_use_window.h"
#include "tile_download_window.h"
#include "../map_types.h"
#include "../tiles/tile_loader.h"

#define GOSM_TYPE_WIZZARD_DOWNLOAD		(wizzard_download_get_type ())
#define GOSM_WIZZARD_DOWNLOAD(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_WIZZARD_DOWNLOAD, WizzardDownload))
#define GOSM_WIZZARD_DOWNLOAD_CLASS(obj)	(G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_WIZZARD_DOWNLOAD, WizzardDownloadClass))
#define GOSM_IS_WIZZARD_DOWNLOAD(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_WIZZARD_DOWNLOAD))
#define GOSM_IS_WIZZARD_DOWNLOAD_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_WIZZARD_DOWNLOAD))
#define GOSM_WIZZARD_DOWNLOAD_GET_CLASS	(G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_WIZZARD_DOWNLOAD, WizzardDownloadClass))

typedef struct _WizzardDownload		WizzardDownload;
typedef struct _WizzardDownloadClass	WizzardDownloadClass;

struct _WizzardDownload
{
	GObject parent;

	int total;
	int ready;

	char * format_url;
	char * download_dir;

	GtkWindow * parent_window;
	SelectUseWindow * select_use_window;
	TileDownloadWindow * tile_download_window;
	TileLoader * tile_loader;

	Selection selection;
	gboolean to_load[18];
	gulong handler_id_tile_loaded;
};

struct _WizzardDownloadClass
{
	GObjectClass parent_class;
};

WizzardDownload * wizzard_download_new(GtkWindow * parent_window, char * format_url, char * download_dir, Selection selection);

void wizzard_download_show(WizzardDownload * wizzard);

void wizzard_download_set_active(WizzardDownload * wizzard, int zoomlevel, gboolean state);
