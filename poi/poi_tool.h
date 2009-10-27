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

#ifndef _POI_TOOL_H_
#define _POI_TOOL_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "poi_manager.h"
#include "poi_selector.h"
#include "poi_source_selector.h"

#define GOSM_TYPE_POI_TOOL           (poi_tool_get_type ())
#define GOSM_POI_TOOL(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_POI_TOOL, PoiTool))
#define GOSM_POI_TOOL_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_POI_TOOL, PoiToolClass))
#define GOSM_IS_POI_TOOL(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_POI_TOOL))
#define GOSM_IS_POI_TOOL_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_POI_TOOL))
#define GOSM_POI_TOOL_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_POI_TOOL, PoiToolClass))

typedef struct _PoiTool        PoiTool;
typedef struct _PoiToolClass   PoiToolClass;

struct _PoiTool
{
	GtkVBox parent;

	PoiManager * poi_manager;
	PoiSelector * poi_selector;
	PoiSourceSelector * poi_source_selector;

	GtkWidget * button_api;
};

struct _PoiToolClass
{
	GtkVBoxClass parent_class;

	//void (* function_name) (PoiTool *poi_tool);
};

GtkWidget * poi_tool_new(PoiManager * poi_manager);

#endif /* _POI_TOOL_H_ */
