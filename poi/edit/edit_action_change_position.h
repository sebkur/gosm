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

#ifndef _EDIT_ACTION_CHANGE_POSITION_H_
#define _EDIT_ACTION_CHANGE_POSITION_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "edit_action.h"

#define GOSM_TYPE_EDIT_ACTION_CHANGE_POSITION           (edit_action_change_position_get_type ())
#define GOSM_EDIT_ACTION_CHANGE_POSITION(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_EDIT_ACTION_CHANGE_POSITION, EditActionChangePosition))
#define GOSM_EDIT_ACTION_CHANGE_POSITION_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_EDIT_ACTION_CHANGE_POSITION, EditActionChangePositionClass))
#define GOSM_IS_EDIT_ACTION_CHANGE_POSITION(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_EDIT_ACTION_CHANGE_POSITION))
#define GOSM_IS_EDIT_ACTION_CHANGE_POSITION_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_EDIT_ACTION_CHANGE_POSITION))
#define GOSM_EDIT_ACTION_CHANGE_POSITION_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_EDIT_ACTION_CHANGE_POSITION, EditActionChangePositionClass))

typedef struct _EditActionChangePosition        EditActionChangePosition;
typedef struct _EditActionChangePositionClass   EditActionChangePositionClass;

struct _EditActionChangePosition
{
	EditAction parent;

	int node_id;
	double lon;
	double lat;
};

struct _EditActionChangePositionClass
{
	EditActionClass parent_class;

	//void (* function_name) (EditActionChangePosition *edit_action_change_position);
};

EditAction * edit_action_change_position_new(int node_id, double lon, double lat);

#endif /* _EDIT_ACTION_CHANGE_POSITION_H_ */
