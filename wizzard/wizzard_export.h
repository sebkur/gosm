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
#include "select_export_window.h"
#include "../map_types.h"

#define GOSM_TYPE_WIZZARD_EXPORT		(wizzard_export_get_type ())
#define GOSM_WIZZARD_EXPORT(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_WIZZARD_EXPORT, WizzardExport))
#define GOSM_WIZZARD_EXPORT_CLASS(obj)		(G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_WIZZARD_EXPORT, WizzardExportClass))
#define GOSM_IS_WIZZARD_EXPORT(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_WIZZARD_EXPORT))
#define GOSM_IS_WIZZARD_EXPORT_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_WIZZARD_EXPORT))
#define GOSM_WIZZARD_EXPORT_GET_CLASS		(G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_WIZZARD_EXPORT, WizzardExportClass))

typedef struct _WizzardExport		WizzardExport;
typedef struct _WizzardExportClass	WizzardExportClass;

struct _WizzardExport
{
	GObject parent;

	int total;
	int ready;

	GtkWindow * parent_window;
	SelectExportWindow * select_export_window;
	//TileExportWindow * tile_export_window;

	Selection selection;
	gint zoom;
	//gboolean to_load[18];

	char * cache_dir;

};

struct _WizzardExportClass
{
	GObjectClass parent_class;
};

WizzardExport * wizzard_export_new(GtkWindow * parent_window, char * cache_dir, Selection selection, gint zoom);

void wizzard_export_show(WizzardExport * wizzard);
