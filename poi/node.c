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

#include "node.h"

G_DEFINE_TYPE (Node, node, G_TYPE_OBJECT);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint node_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, node_signals[SIGNAL_NAME_n], 0);

Node * node_new(int id, int version, double lon, double lat)
{
	Node * node = g_object_new(GOSM_TYPE_NODE, NULL);
	node -> id = id;
	node -> version = version;
	node -> lon = lon;
	node -> lat = lat;
	node -> tags = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
	node -> refs = 0;
	return node;
}

void node_free(Node * node)
{
	g_hash_table_destroy(node -> tags);
	//g_slice_free1(node);
}

static void node_class_init(NodeClass *class)
{
        /*node_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (NodeClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void node_init(Node *node)
{
}

void node_add_tag(Node * node, char * key, char * value)
{
	g_hash_table_insert(node -> tags, g_strdup(key), g_strdup(value));
}

char * node_get_value(Node * node, char * key)
{
	return g_hash_table_lookup(node -> tags, key);
}

Node * node_copy(Node * node)
{
	Node * copy = node_new(node -> id, node -> version, node -> lon, node -> lat);
	copy -> refs = node -> refs;
	GHashTableIter iter;
	g_hash_table_iter_init(&iter, node -> tags);
	gpointer hash_key, hash_val;
	while(g_hash_table_iter_next(&iter, &hash_key, &hash_val)){
		char * hash_k = (char*) hash_key;
		char * hash_v = (char*) hash_val;
		node_add_tag(copy, hash_k, hash_v);
	}
	return copy;
}

int node_get_number_of_tags(Node * node)
{
	return g_hash_table_size(node -> tags);
}
