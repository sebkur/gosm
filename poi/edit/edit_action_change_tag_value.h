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

#ifndef _EDIT_ACTION_CHANGE_TAG_VALUE_H_
#define _EDIT_ACTION_CHANGE_TAG_VALUE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "edit_action.h"

#define GOSM_TYPE_EDIT_ACTION_CHANGE_TAG_VALUE           (edit_action_change_tag_value_get_type ())
#define GOSM_EDIT_ACTION_CHANGE_TAG_VALUE(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_EDIT_ACTION_CHANGE_TAG_VALUE, EditActionChangeTagValue))
#define GOSM_EDIT_ACTION_CHANGE_TAG_VALUE_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_EDIT_ACTION_CHANGE_TAG_VALUE, EditActionChangeTagValueClass))
#define GOSM_IS_EDIT_ACTION_CHANGE_TAG_VALUE(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_EDIT_ACTION_CHANGE_TAG_VALUE))
#define GOSM_IS_EDIT_ACTION_CHANGE_TAG_VALUE_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_EDIT_ACTION_CHANGE_TAG_VALUE))
#define GOSM_EDIT_ACTION_CHANGE_TAG_VALUE_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_EDIT_ACTION_CHANGE_TAG_VALUE, EditActionChangeTagValueClass))

typedef struct _EditActionChangeTagValue        EditActionChangeTagValue;
typedef struct _EditActionChangeTagValueClass   EditActionChangeTagValueClass;

struct _EditActionChangeTagValue
{
	EditAction parent;

	int node_id;
	char * key;
	char * value;
};

struct _EditActionChangeTagValueClass
{
	EditActionClass parent_class;

	//void (* function_name) (EditActionChangeTagValue *edit_action_change_tag_value);
};

EditAction * edit_action_change_tag_value_new(int node_id, char * key, char * value);

#endif /* _EDIT_ACTION_CHANGE_TAG_VALUE_H_ */
