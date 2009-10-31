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
#include <string.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "tag_tree.h"

GSequence * tag_tree_get_nodes_create(
	TagTree * tag_tree,
	char * key,
	char * value);

G_DEFINE_TYPE (TagTree, tag_tree, G_TYPE_OBJECT);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint tag_tree_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, tag_tree_signals[SIGNAL_NAME_n], 0);

gint tag_tree_compare_strings(gconstpointer a, gconstpointer b, gpointer user_data)
{
        return strcmp(a, b);
}
gint tag_tree_compare_ints(gconstpointer a, gconstpointer b, gpointer user_data)
{
        return *(int*)a - *(int*)b;
}
void tag_tree_destroy_just_free(gpointer data)
{
	free(data);
}
/****************************************************************************************************
* free a GTree
****************************************************************************************************/
void tag_tree_destroy_value_trees(gpointer data)
{
	//printf("DESTROY value_tree\n");
	g_tree_destroy((GTree*)data);
}
/****************************************************************************************************
* free a GSequence
****************************************************************************************************/
void tag_tree_destroy_element_sequences(gpointer data)
{	//printf("DESTROY element array\n");
	g_sequence_free((GSequence*)data);
}

TagTree * tag_tree_new()
{
	TagTree * tag_tree = g_object_new(GOSM_TYPE_TAG_TREE, NULL);
	tag_tree -> tree = g_tree_new_full(
		tag_tree_compare_strings, NULL, 
		tag_tree_destroy_just_free, 
		tag_tree_destroy_value_trees);
	return tag_tree;
}

static void tag_tree_class_init(TagTreeClass *class)
{
}

static void tag_tree_init(TagTree *tag_tree)
{
}

void tag_tree_destroy(
	TagTree * tag_tree)
{
	g_tree_destroy(tag_tree -> tree);
}

void tag_tree_add_node(
	TagTree * tag_tree, 
	int node_id, 
	GHashTable * tags)
{
	GHashTableIter iter;
	g_hash_table_iter_init(&iter, tags);
	gpointer key, val;
	while(g_hash_table_iter_next(&iter, &key, &val)){
		GTree * tree1 = (GTree*)g_tree_lookup(tag_tree -> tree, key);
		if (tree1 == NULL){
			/* key not found on first level */
			tree1 = g_tree_new_full(
				tag_tree_compare_strings, NULL, 
				tag_tree_destroy_just_free, tag_tree_destroy_element_sequences);
			int len_k = strlen(key) + 1;
			char * key_insert = malloc(sizeof(char) * len_k);
			strcpy(key_insert, key);
			g_tree_insert(tag_tree -> tree, key_insert, tree1);
		}
		/* now tree1 exists */
		GSequence * elements = (GSequence*)g_tree_lookup(tree1, val);
		if (elements == NULL){
			/* val not found on second level */
			elements = g_sequence_new(tag_tree_destroy_just_free);
			int len_v = strlen(val) + 1;
			char * val_insert = malloc(sizeof(char) * len_v);
			strcpy(val_insert, val);
			g_tree_insert(tree1, val_insert, elements);
		}
		/* now elements exists */
		int * id_p = malloc(sizeof(int));
		*id_p = node_id;
		g_sequence_insert_sorted(elements, id_p, tag_tree_compare_ints, NULL);
	}
}

void tag_tree_subtract_node(
	TagTree * tag_tree,
	int node_id,
	GHashTable * tags)
{
	GHashTableIter iter;
	g_hash_table_iter_init(&iter, tags);
	gpointer key, val;
	while(g_hash_table_iter_next(&iter, &key, &val)){
		GTree * tree1 = (GTree*)g_tree_lookup(tag_tree -> tree, key);
		if (tree1 == NULL){
			/* key not found on first level */
			continue;
		}
		GSequence * elements = (GSequence*)g_tree_lookup(tree1, val);
		if (elements == NULL){
			/* val not found on second level */
			continue;
		}
		GSequenceIter * seq_iter = g_sequence_search(elements, &node_id, tag_tree_compare_ints, NULL);
		seq_iter = g_sequence_iter_prev(seq_iter);
		if (node_id == *(int*)g_sequence_get(seq_iter)){
			/* node_id found in elements */
			g_sequence_remove(seq_iter);
		}
		if (g_sequence_get_length(elements) == 0){
			g_sequence_free(elements);
			g_tree_remove(tree1, val);
			if (g_tree_height(tree1) == 0){
				g_tree_destroy(tree1);
				g_tree_remove(tag_tree -> tree, tree1);
			}
		}
	}
}

gboolean tag_tree_add_tag_tree__iter_keys(gpointer k, gpointer v, gpointer data);
gboolean tag_tree_add_tag_tree__iter_vals(gpointer k, gpointer v, gpointer data);

void tag_tree_add_tag_tree(
	TagTree * tag_tree,
	TagTree * tag_tree_add)
{
	g_tree_foreach(tag_tree_add -> tree, tag_tree_add_tag_tree__iter_keys, tag_tree);
}

gboolean tag_tree_add_tag_tree__iter_keys(gpointer key_p, gpointer tree_vals_p, gpointer data)
{
	char * key = (char*) key_p;
	GTree * tree_vals = (GTree*) tree_vals_p;
	gpointer array[2] = {data, key_p};
	g_tree_foreach(tree_vals, tag_tree_add_tag_tree__iter_vals, array);
	return FALSE;
}

gboolean tag_tree_add_tag_tree__iter_vals(gpointer val_p, gpointer elements_to_add_p, gpointer data_p)
{
	gpointer * data = (gpointer*) data_p;
	char * key = (char*)(data[1]);
	char * val = (char*) val_p;
	TagTree * tag_tree = GOSM_TAG_TREE(data[0]);
	GSequence * elements = tag_tree_get_nodes_create(tag_tree, key, val);
	GSequence * elements_to_add = (GSequence*) elements_to_add_p;
	/* iterate the elements to add */
	GSequenceIter * iter = g_sequence_get_begin_iter(elements_to_add);
	while(!g_sequence_iter_is_end(iter)){
		int * node_id = (int*)g_sequence_get(iter);
		GSequenceIter * iter_search = g_sequence_search(elements, node_id, tag_tree_compare_ints, NULL);
		GSequenceIter * iter_find = g_sequence_iter_prev(iter_search);
		if (g_sequence_iter_is_begin(iter_search) || *node_id != *(int*)g_sequence_get(iter_find)){
			/* node_id not found in elements */
			int * node_id_insert = malloc(sizeof(int));
			*node_id_insert = *node_id;
			//TODO: not search again, maybe use g_sequence_insert_before
			g_sequence_insert_sorted(elements, node_id_insert, tag_tree_compare_ints, NULL);
		}
		/* continue with next one */
		iter = g_sequence_iter_next(iter);
	}
	return FALSE;
}

gboolean tag_tree_subtract_tag_tree__iter_keys(gpointer k, gpointer v, gpointer data);
gboolean tag_tree_subtract_tag_tree__iter_vals(gpointer k, gpointer v, gpointer data);

void tag_tree_subtract_tag_tree(
	TagTree * tag_tree,
	TagTree * tag_tree_subtract)
{
	g_tree_foreach(tag_tree_subtract -> tree, tag_tree_subtract_tag_tree__iter_keys, tag_tree);
}

gboolean tag_tree_subtract_tag_tree__iter_keys(gpointer key_p, gpointer tree_vals_p, gpointer data)
{
	char * key = (char*) key_p;
	GTree * tree_vals = (GTree*) tree_vals_p;
	gpointer array[2] = {data, key_p};
	g_tree_foreach(tree_vals, tag_tree_subtract_tag_tree__iter_vals, array);
	return FALSE;
}

gboolean tag_tree_subtract_tag_tree__iter_vals(gpointer val_p, gpointer elements_to_subtract_p, gpointer data_p)
{
	gpointer * data = (gpointer*) data_p;
	char * key = (char*)(data[1]);
	char * val = (char*) val_p;
	TagTree * tag_tree = GOSM_TAG_TREE(data[0]);
	GSequence * elements = tag_tree_get_nodes(tag_tree, key, val);
	if (elements == NULL){
		return FALSE;
	}
	GSequence * elements_to_subtract = (GSequence*) elements_to_subtract_p;
	/* iterate the elements to subtract */
	GSequenceIter * iter = g_sequence_get_begin_iter(elements_to_subtract);
	while(!g_sequence_iter_is_end(iter)){
		int * node_id = (int*)g_sequence_get(iter);
		GSequenceIter * iter_find = g_sequence_search(elements, &node_id, tag_tree_compare_ints, NULL);
		iter_find = g_sequence_iter_prev(iter_find);
		if (*node_id == *(int*)g_sequence_get(iter_find)){
			/* node_id found in elements */
			g_sequence_remove(iter_find);
		}
		/* continue with next one */
		iter = g_sequence_iter_next(iter);
	}
	return FALSE;
}

GSequence * tag_tree_get_nodes(
	TagTree * tag_tree,
	char * key,
	char * value)
{
	GTree * tree1 = (GTree*)g_tree_lookup(tag_tree -> tree, key);
	if (tree1 == NULL){
		/* key not found on first level */
		return NULL;
	}
	GSequence * elements = (GSequence*)g_tree_lookup(tree1, value);
	return elements;
}

/****************************************************************************************************
* return the GSequence used for storing node_ids for the given tag.
* creates the sequence, if it is not already present in the structure.
****************************************************************************************************/
GSequence * tag_tree_get_nodes_create(
	TagTree * tag_tree,
	char * key,
	char * value)
{
	GTree * tree1 = (GTree*)g_tree_lookup(tag_tree -> tree, key);
	if (tree1 == NULL){
		/* key not found on first level */
		tree1 = g_tree_new_full(
				tag_tree_compare_strings, NULL, 
				tag_tree_destroy_just_free, tag_tree_destroy_element_sequences);
		int len_k = strlen(key) + 1;
		char * key_insert = malloc(sizeof(char) * len_k);
		strcpy(key_insert, key);
		g_tree_insert(tag_tree -> tree, key_insert, tree1);
	}
	/* now tree1 exists */
	GSequence * elements = (GSequence*)g_tree_lookup(tree1, value);
	if (elements == NULL){
		/* val not found on second level */
		elements = g_sequence_new(tag_tree_destroy_just_free);
		int len_v = strlen(value) + 1;
		char * val_insert = malloc(sizeof(char) * len_v);
		strcpy(val_insert, value);
		g_tree_insert(tree1, val_insert, elements);
	}
	return elements;
}
