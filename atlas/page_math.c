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

#include "page_math.h"
#include "atlas.h"

#define MM_TO_INCH(x)	((double)x)/25.4

/****************************************************************************************************
* calculates the visible area of a page in pixels by subtracting margins and applying resolution
****************************************************************************************************/
ImageDimension get_image_dimension(PageInformation page_info)
{
	ImageDimension id;
	id.width = MM_TO_INCH(page_info.page_width - page_info.border_left - page_info.border_right) * page_info.resolution;
	id.height = MM_TO_INCH(page_info.page_height - page_info.border_top - page_info.border_bottom) * page_info.resolution;
	return id;
}
