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

#include "r_r_tree.h"

/****************************************************************************************************
* This is a wrapper class for RTree. Since RTree does not support insertion of nodes with negative
* ids, this class just has two RTrees. One for negative and another for non-negative values.
* All (used) RTree methods are wrapped in methods that operate on both RTrees.
****************************************************************************************************/
G_DEFINE_TYPE (RRTree, r_r_tree, G_TYPE_OBJECT);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint r_r_tree_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, r_r_tree_signals[SIGNAL_NAME_n], 0);

static void r_r_tree_class_init(RRTreeClass *class)
{
        /*r_r_tree_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (RRTreeClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void r_r_tree_init(RRTree *r_r_tree)
{
}

RRTree * r_r_tree_new()
{
	RRTree * r_r_tree = g_object_new(GOSM_TYPE_R_R_TREE, NULL);
	r_r_tree -> root_positive = RTreeNewIndex();
	r_r_tree -> root_negative = RTreeNewIndex();
	return r_r_tree;
}

void r_r_tree_clear(RRTree * tree)
{
	RTreeClear(tree -> root_positive);
	RTreeClear(tree -> root_negative);
}

void r_r_tree_insert_rect(RRTree * tree, struct Rect* rect, int id)
{
	struct Node ** root = id < 0 ? &(tree -> root_negative) : &(tree -> root_positive);
	if (id < 0) id = -id;
	RTreeInsertRect(rect, id, root, 0);
}

int r_r_tree_delete_rect(RRTree * tree, struct Rect* rect, int id)
{
	struct Node ** root = id < 0 ? &(tree -> root_negative) : &(tree -> root_positive);
	if (id < 0) id = -id;
	return RTreeDeleteRect(rect, id, root);
}

int r_r_tree_search(RRTree * tree, struct Rect* rect, SearchHitCallback cb, void* data)
{
	int n = RTreeSearch(tree -> root_positive, rect, cb, data);
	n += RTreeSearch2(tree -> root_negative, rect, cb, data);
	return n;
}

