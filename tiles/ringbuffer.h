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

#include <glib.h>

#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

typedef struct RingBuffer {
	gchar *data;
	gchar *overwritten;
	guint len;
	guint start;
	guint end;
	guint count;
	guint element_size;
} RingBuffer;

RingBuffer * ringbuffer_new(guint length, guint element_size);

gboolean ringbuffer_append(RingBuffer * ring_buffer, gconstpointer data);

void * ringbuffer_index(RingBuffer * ring_buffer, guint index);

gboolean ringbuffer_is_used(RingBuffer * ring_buffer, guint index);

#endif
