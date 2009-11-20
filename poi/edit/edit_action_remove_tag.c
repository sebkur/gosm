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

#include "edit_action_remove_tag.h"

G_DEFINE_TYPE (EditActionRemoveTag, edit_action_remove_tag, GOSM_TYPE_EDIT_ACTION);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint edit_action_remove_tag_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, edit_action_remove_tag_signals[SIGNAL_NAME_n], 0);

EditAction * edit_action_remove_tag_new(int node_id, char * ikey, char * ivalue)
{
	EditActionRemoveTag * edit_action_remove_tag = g_object_new(GOSM_TYPE_EDIT_ACTION_REMOVE_TAG, NULL);
	GOSM_EDIT_ACTION(edit_action_remove_tag) -> node_id = node_id;
	int len_k = strlen(ikey) + 1;
	int len_v = strlen(ivalue) + 1;
	char * key = malloc(sizeof(char) * len_k);
	char * value = malloc(sizeof(char) * len_v);
	strncpy(key, ikey, len_k);
	strncpy(value, ivalue, len_v);
	edit_action_remove_tag -> key = key;
	edit_action_remove_tag -> value = value;
	return GOSM_EDIT_ACTION(edit_action_remove_tag);
}

static void edit_action_remove_tag_class_init(EditActionRemoveTagClass *class)
{
	EditActionClass * edit_action_class = GOSM_EDIT_ACTION_CLASS(class);
	edit_action_class -> print = edit_action_remove_tag_print;
	edit_action_class -> to_string = edit_action_remove_tag_to_string;
        /*edit_action_remove_tag_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (EditActionRemoveTagClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void edit_action_remove_tag_init(EditActionRemoveTag *edit_action_remove_tag)
{
}

void edit_action_remove_tag_print(EditAction * action)
{
	EditActionRemoveTag * a = GOSM_EDIT_ACTION_REMOVE_TAG(action);
	printf("remove tag %d %s %s\n", action -> node_id, a -> key, a -> value);
}

char * edit_action_remove_tag_to_string(EditAction * action)
{
	EditActionRemoveTag * a = GOSM_EDIT_ACTION_REMOVE_TAG(action);
	char * buf = malloc(sizeof(char) * 100);
	sprintf(buf, "remove tag %d %s %s", action -> node_id, a -> key, a -> value);
	return buf;
}

