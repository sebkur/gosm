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

#ifndef _NODE_H_
#define _NODE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#define GOSM_TYPE_NODE           (node_get_type ())
#define GOSM_NODE(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_NODE, Node))
#define GOSM_NODE_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_NODE, NodeClass))
#define GOSM_IS_NODE(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_NODE))
#define GOSM_IS_NODE_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_NODE))
#define GOSM_NODE_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_NODE, NodeClass))

typedef struct _Node        Node;
typedef struct _NodeClass   NodeClass;

struct _Node
{
	GObject parent;

	int id;
	double lon;
	double lat;
	GHashTable * tags;

	int refs;
};

struct _NodeClass
{
	GObjectClass parent_class;

	//void (* function_name) (Node *node);
};

Node * node_new(int id, double lon, double lat);
void node_free(Node * node);
void node_add_tag(Node * node, char * key, char * value);
char * node_get_value(Node * node, char * key);
Node * node_copy(Node * node);

#endif /* _NODE_H_ */
