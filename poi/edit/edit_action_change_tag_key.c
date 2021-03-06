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

#include "edit_action_change_tag_key.h"

G_DEFINE_TYPE (EditActionChangeTagKey, edit_action_change_tag_key, GOSM_TYPE_EDIT_ACTION);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint edit_action_change_tag_key_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, edit_action_change_tag_key_signals[SIGNAL_NAME_n], 0);

EditAction * edit_action_change_tag_key_new(int node_id, char * old_key, char * new_key)
{
	EditActionChangeTagKey * edit_action_change_tag_key = g_object_new(GOSM_TYPE_EDIT_ACTION_CHANGE_TAG_KEY, NULL);
	GOSM_EDIT_ACTION(edit_action_change_tag_key) -> node_id = node_id;
	edit_action_change_tag_key -> old_key = old_key;
	edit_action_change_tag_key -> new_key = new_key;
	return GOSM_EDIT_ACTION(edit_action_change_tag_key);
}

static void edit_action_change_tag_key_class_init(EditActionChangeTagKeyClass *class)
{
	EditActionClass * edit_action_class = GOSM_EDIT_ACTION_CLASS(class);
	edit_action_class -> print = edit_action_change_tag_key_print;
	edit_action_class -> to_string = edit_action_change_tag_key_to_string;
        /*edit_action_change_tag_key_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (EditActionChangeTagKeyClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void edit_action_change_tag_key_init(EditActionChangeTagKey *edit_action_change_tag_key)
{
}

void edit_action_change_tag_key_print(EditAction * action)
{
	EditActionChangeTagKey * a = GOSM_EDIT_ACTION_CHANGE_TAG_KEY(action);
	printf("change tag-key %d %s %s\n", action -> node_id, a -> old_key, a -> new_key);
}

char * edit_action_change_tag_key_to_string(EditAction * action)
{
	EditActionChangeTagKey * a = GOSM_EDIT_ACTION_CHANGE_TAG_KEY(action);
	char * buf = malloc(sizeof(char) * 100);
	sprintf(buf, "change tag-key %d %s %s", action -> node_id, a -> old_key, a -> new_key);
	return buf;
}

