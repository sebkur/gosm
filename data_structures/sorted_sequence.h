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

#ifndef _SORTED_SEQUENCE_H_
#define _SORTED_SEQUENCE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#define GOSM_TYPE_SORTED_SEQUENCE           (sorted_sequence_get_type ())
#define GOSM_SORTED_SEQUENCE(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_SORTED_SEQUENCE, SortedSequence))
#define GOSM_SORTED_SEQUENCE_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_SORTED_SEQUENCE, SortedSequenceClass))
#define GOSM_IS_SORTED_SEQUENCE(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_SORTED_SEQUENCE))
#define GOSM_IS_SORTED_SEQUENCE_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_SORTED_SEQUENCE))
#define GOSM_SORTED_SEQUENCE_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_SORTED_SEQUENCE, SortedSequenceClass))

typedef struct _SortedSequence        SortedSequence;
typedef struct _SortedSequenceClass   SortedSequenceClass;

struct _SortedSequence
{
	GObject parent;

	GSequence * seq;
	GCompareDataFunc cmp_func;
};

struct _SortedSequenceClass
{
	GObjectClass parent_class;

	//void (* function_name) (SortedSequence *sorted_sequence);
};

SortedSequence * sorted_sequence_new(GDestroyNotify data_destroy, GCompareDataFunc cmp_func);

void sorted_sequence_insert(SortedSequence * sequence, gpointer data);
gboolean sorted_sequence_contains(SortedSequence * sequence, gpointer data);
void sorted_sequence_remove(SortedSequence * sequence, gpointer data);
gpointer sorted_sequence_get(SortedSequence * sequence, int index);
int sorted_sequence_get_length(SortedSequence * sequence);

#endif /* _SORTED_SEQUENCE_H_ */
