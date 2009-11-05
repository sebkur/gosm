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

#ifndef _R_R_TREE_H_
#define _R_R_TREE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "RTree/index.h"

#define GOSM_TYPE_R_R_TREE           (r_r_tree_get_type ())
#define GOSM_R_R_TREE(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_R_R_TREE, RRTree))
#define GOSM_R_R_TREE_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_R_R_TREE, RRTreeClass))
#define GOSM_IS_R_R_TREE(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_R_R_TREE))
#define GOSM_IS_R_R_TREE_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_R_R_TREE))
#define GOSM_R_R_TREE_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_R_R_TREE, RRTreeClass))

typedef struct _RRTree        RRTree;
typedef struct _RRTreeClass   RRTreeClass;

struct _RRTree
{
	GObject parent;

	struct Node * root_positive;
	struct Node * root_negative;
};

struct _RRTreeClass
{
	GObjectClass parent_class;

	//void (* function_name) (RRTree *r_r_tree);
};

RRTree * r_r_tree_new();

void r_r_tree_clear(RRTree * tree);
void r_r_tree_insert_rect(RRTree * tree, struct Rect* rect, int id);
int r_r_tree_delete_rect(RRTree * tree, struct Rect* rect, int id);
int r_r_tree_search(RRTree * tree, struct Rect* rect, SearchHitCallback cb, void* data);

#endif /* _R_R_TREE_H_ */
