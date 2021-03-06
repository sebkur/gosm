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

#ifndef _NODE_TOOL_H_
#define _NODE_TOOL_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "../map_types.h"
#include "poi_manager.h"

#define GOSM_TYPE_NODE_TOOL           (node_tool_get_type ())
#define GOSM_NODE_TOOL(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_NODE_TOOL, NodeTool))
#define GOSM_NODE_TOOL_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_NODE_TOOL, NodeToolClass))
#define GOSM_IS_NODE_TOOL(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_NODE_TOOL))
#define GOSM_IS_NODE_TOOL_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_NODE_TOOL))
#define GOSM_NODE_TOOL_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_NODE_TOOL, NodeToolClass))

typedef struct _NodeTool        NodeTool;
typedef struct _NodeToolClass   NodeToolClass;

struct _NodeTool
{
	GtkVBox parent;

	PoiManager * poi_manager;
	GtkWidget * view;
	GtkTreeModel * model;
	GtkTreeModel * model_sort;
	int node_id;
};

struct _NodeToolClass
{
	GtkVBoxClass parent_class;

	//void (* function_name) (NodeTool *node_tool);
};

GtkWidget * node_tool_new(PoiManager * poi_manager);

void node_tool_set_tags(NodeTool * node_tool, int node_id);

#endif /* _NODE_TOOL_H_ */
