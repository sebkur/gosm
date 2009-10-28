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
#include <string.h>

#include <glib.h>

#include "ringbuffer.h"

/****************************************************************************************************
* this is a ringbuffer
* it is used by the TileManager to distinguish which tile to remove from the cache next
* currently cache management is FIFO
* TODO: implement Least-Recently-Used
****************************************************************************************************/

/****************************************************************************************************
* constructor
* TODO: refactor to be a glib-type
****************************************************************************************************/
RingBuffer * ringbuffer_new(guint length, guint element_size)
{
	RingBuffer * ring_buffer = malloc(sizeof(struct RingBuffer));
	ring_buffer -> len = length;
	ring_buffer -> element_size = element_size;
	ring_buffer -> start = 0;
	ring_buffer -> end = 0;
	ring_buffer -> count = 0;
	ring_buffer -> data = malloc(length * element_size);
	ring_buffer -> overwritten = malloc(element_size);
	return ring_buffer;
}

/****************************************************************************************************
* add something to the 'end' of the buffer, possibly overwriting an already present value
* return TRUE if a value has been overwritten, FALSE otherwise
****************************************************************************************************/
gboolean ringbuffer_append(RingBuffer * ring_buffer, gconstpointer data)
{
	gboolean overwrite = FALSE;
	if (ring_buffer -> end == ring_buffer -> start && ring_buffer -> count != 0){
		overwrite = TRUE;
		memcpy(ring_buffer->overwritten,
			ring_buffer->data + ring_buffer -> element_size * ring_buffer -> start,
			ring_buffer -> element_size);
	}
	memcpy((ring_buffer->data) + ring_buffer->element_size * ring_buffer->end,
		data, ring_buffer->element_size);
	if (ring_buffer -> end == ring_buffer -> start){
		// start and end are equal
		if (ring_buffer -> count == 0){
			// empty buffer
			ring_buffer -> end += 1;
			ring_buffer -> count += 1;
		}else{
			// full buffer
			ring_buffer -> start += 1;
			ring_buffer -> end += 1;
		}
	}else{
		// somehow filled buffer
		ring_buffer -> end += 1;
		ring_buffer -> count += 1;
	}
	if (ring_buffer->end == ring_buffer->len){
		ring_buffer->end = 0;
	}
	if (ring_buffer->start == ring_buffer->len){
		ring_buffer->start = 0;
	}
	return overwrite;
}

/****************************************************************************************************
* return the element at the index's position
****************************************************************************************************/
void * ringbuffer_index(RingBuffer * ring_buffer, guint index)
{
	return ring_buffer -> data + ring_buffer -> element_size * index;
}

/****************************************************************************************************
* TODO: is this function still necessary? seems to be used nowhere except the testing-code below
****************************************************************************************************/
gboolean ringbuffer_is_used(RingBuffer * ring_buffer, guint index)
{
	if (ring_buffer -> end == ring_buffer -> start){
		return !(ring_buffer -> count == 0);
	}
	if (ring_buffer -> end > ring_buffer -> start){
		return index >= ring_buffer -> start
			&& index < ring_buffer -> end;
	}
	return index >= ring_buffer -> start
		|| index < ring_buffer -> end;
}

/*
int main(int argc, char *argv[])
{
	int size = 10;

	RingBuffer * ring_buffer = ringbuffer_new(size, sizeof(int));
	//int x = 13;
	//int y = 27;
	//ringbuffer_append(ring_buffer, &x);
	//ringbuffer_append(ring_buffer, &y);

	int k;
	for (k = 1; k <= 20; k++){
		printf("----ROUND----\n");
		gboolean overwrite = ringbuffer_append(ring_buffer, &k);
		if (overwrite){
			printf("removed: %d\n", *((int *)(ring_buffer -> overwritten)));
		}
		int i;
		for (i = 0; i < size; i++){
			if (ringbuffer_is_used(ring_buffer, i)){
				int a = *((int *)(ringbuffer_index(ring_buffer, i)));
				printf("element %d: %d\n", i, a);
			}
		}
	}
}
*/
