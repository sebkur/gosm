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

#ifndef _NODE_ADD_TAG_DIALOG_H_
#define _NODE_ADD_TAG_DIALOG_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#define GOSM_TYPE_NODE_ADD_TAG_DIALOG           (node_add_tag_dialog_get_type ())
#define GOSM_NODE_ADD_TAG_DIALOG(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_NODE_ADD_TAG_DIALOG, NodeAddTagDialog))
#define GOSM_NODE_ADD_TAG_DIALOG_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_NODE_ADD_TAG_DIALOG, NodeAddTagDialogClass))
#define GOSM_IS_NODE_ADD_TAG_DIALOG(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_NODE_ADD_TAG_DIALOG))
#define GOSM_IS_NODE_ADD_TAG_DIALOG_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_NODE_ADD_TAG_DIALOG))
#define GOSM_NODE_ADD_TAG_DIALOG_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_NODE_ADD_TAG_DIALOG, NodeAddTagDialogClass))

typedef struct _NodeAddTagDialog        NodeAddTagDialog;
typedef struct _NodeAddTagDialogClass   NodeAddTagDialogClass;

struct _NodeAddTagDialog
{
	GObject parent;

	char * key;
	char * value;
};

struct _NodeAddTagDialogClass
{
	GObjectClass parent_class;

	//void (* function_name) (NodeAddTagDialog *node_add_tag_dialog);
};

NodeAddTagDialog * node_add_tag_dialog_new();

NodeAddTagDialog * node_add_tag_dialog_new_with_preset(char * key, char * value);

char * node_add_tag_dialog_get_key(NodeAddTagDialog * node_add_tag_dialog);
char * node_add_tag_dialog_get_value(NodeAddTagDialog * node_add_tag_dialog);

int node_add_tag_dialog_run(NodeAddTagDialog * node_add_tag_dialog);

#endif /* _NODE_ADD_TAG_DIALOG_H_ */
