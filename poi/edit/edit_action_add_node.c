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

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "edit_action_add_node.h"

G_DEFINE_TYPE (EditActionAddNode, edit_action_add_node, GOSM_TYPE_EDIT_ACTION);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint edit_action_add_node_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, edit_action_add_node_signals[SIGNAL_NAME_n], 0);

EditAction * edit_action_add_node_new(int node_id, double lon, double lat)
{
	EditActionAddNode * edit_action_add_node = g_object_new(GOSM_TYPE_EDIT_ACTION_ADD_NODE, NULL);
	GOSM_EDIT_ACTION(edit_action_add_node) -> node_id = node_id;
	edit_action_add_node -> lon = lon;
	edit_action_add_node -> lat = lat;
	return GOSM_EDIT_ACTION(edit_action_add_node);
}

static void edit_action_add_node_class_init(EditActionAddNodeClass *class)
{
	EditActionClass * edit_action_class = GOSM_EDIT_ACTION_CLASS(class);
	edit_action_class -> print = edit_action_add_node_print;
	edit_action_class -> to_string = edit_action_add_node_to_string;
        /*edit_action_add_node_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (EditActionAddNodeClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void edit_action_add_node_init(EditActionAddNode *edit_action_add_node)
{
}

void edit_action_add_node_print(EditAction * action)
{
	EditActionAddNode * add = GOSM_EDIT_ACTION_ADD_NODE(action);
	printf("add node %d %f %f\n", action -> node_id, add -> lon, add -> lat);
}

char * edit_action_add_node_to_string(EditAction * action)
{
	EditActionAddNode * add = GOSM_EDIT_ACTION_ADD_NODE(action);
	char * buf = malloc(sizeof(char) * 100);
	sprintf(buf, "add node %d %f %f", action -> node_id, add -> lon, add -> lat);
	return buf;
}
