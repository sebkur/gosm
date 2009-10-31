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

#ifndef _TAG_TREE_H_
#define _TAG_TREE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#define GOSM_TYPE_TAG_TREE           (tag_tree_get_type ())
#define GOSM_TAG_TREE(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_TAG_TREE, TagTree))
#define GOSM_TAG_TREE_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_TAG_TREE, TagTreeClass))
#define GOSM_IS_TAG_TREE(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_TAG_TREE))
#define GOSM_IS_TAG_TREE_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_TAG_TREE))
#define GOSM_TAG_TREE_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_TAG_TREE, TagTreeClass))

typedef struct _TagTree        TagTree;
typedef struct _TagTreeClass   TagTreeClass;

struct _TagTree
{
	GObject parent;

	GTree * tree;
};

struct _TagTreeClass
{
	GObjectClass parent_class;

	//void (* function_name) (TagTree *tag_tree);
};

TagTree * tag_tree_new();

void tag_tree_add_node(
	TagTree * tag_tree, 
	int node_id, 
	GHashTable * tags);

void tag_tree_subtract_node(
	TagTree * tag_tree,
	int node_id,
	GHashTable * tags);

void tag_tree_add_tag_tree(
	TagTree * tag_tree,
	TagTree * tag_tree_add);

void tag_tree_subtract_tag_tree(
	TagTree * tag_tree,
	TagTree * tag_tree_subtract);

GSequence * tag_tree_get_nodes(
	TagTree * tag_tree,
	char * key,
	char * value);

void tag_tree_destroy(
	TagTree * tag_tree);

#endif /* _TAG_TREE_H_ */
