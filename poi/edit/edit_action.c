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

#include "edit_action.h"

G_DEFINE_TYPE (EditAction, edit_action, G_TYPE_OBJECT);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint edit_action_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, edit_action_signals[SIGNAL_NAME_n], 0);

EditAction * edit_action_new()
{
	EditAction * edit_action = g_object_new(GOSM_TYPE_EDIT_ACTION, NULL);
	return edit_action;
}

static void edit_action_class_init(EditActionClass *class)
{
        /*edit_action_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (EditActionClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void edit_action_init(EditAction *edit_action)
{
}

void edit_action_print(EditAction * edit_action)
{
	EditActionClass * class = GOSM_EDIT_ACTION_GET_CLASS(edit_action);
	//EditActionClass * class = (EditActionClass*)(edit_action);
	if (class -> print) class -> print(edit_action);
}
