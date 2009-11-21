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

#include "sorted_sequence.h"

G_DEFINE_TYPE (SortedSequence, sorted_sequence, G_TYPE_OBJECT);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint sorted_sequence_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, sorted_sequence_signals[SIGNAL_NAME_n], 0);

SortedSequence * sorted_sequence_new(GDestroyNotify data_destroy, GCompareDataFunc cmp_func)
{
	SortedSequence * sorted_sequence = g_object_new(GOSM_TYPE_SORTED_SEQUENCE, NULL);
	sorted_sequence -> seq = g_sequence_new(data_destroy);
	sorted_sequence -> cmp_func = cmp_func;
	return sorted_sequence;
}

static void sorted_sequence_class_init(SortedSequenceClass *class)
{
        /*sorted_sequence_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (SortedSequenceClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void sorted_sequence_init(SortedSequence *sorted_sequence)
{
}

void sorted_sequence_insert(SortedSequence * sequence, gpointer data)
{
	g_sequence_insert_sorted(sequence -> seq, data, sequence -> cmp_func, NULL);
}

gboolean sorted_sequence_contains(SortedSequence * sequence, gpointer data)
{
	GSequenceIter * iter = g_sequence_search(sequence -> seq, data, sequence -> cmp_func, NULL);
	GSequenceIter * prev = g_sequence_iter_prev(iter);
	if (g_sequence_iter_is_end(prev)) return FALSE;
	gpointer element = g_sequence_get(prev);
	return sequence -> cmp_func(data, element, NULL) == 0;
}

void sorted_sequence_remove(SortedSequence * sequence, gpointer data)
{
	GSequenceIter * iter = g_sequence_search(sequence -> seq, data, sequence -> cmp_func, NULL);
	GSequenceIter * prev = g_sequence_iter_prev(iter);
	gpointer element = g_sequence_get(prev);
	if(sequence -> cmp_func(data, element, NULL) == 0){
		g_sequence_remove(prev);
	}
}

gpointer sorted_sequence_get(SortedSequence * sequence, int index)
{
	GSequenceIter * iter = g_sequence_get_iter_at_pos(sequence -> seq, index);
	return g_sequence_get(iter);
}

int sorted_sequence_get_length(SortedSequence * sequence)
{
	return g_sequence_get_length(sequence -> seq);
}
