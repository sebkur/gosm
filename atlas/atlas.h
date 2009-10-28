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

#ifndef _ATLAS_H_
#define _ATLAS_H_

/****************************************************************************************************
* used to discriminate between landscape and portrait orientation of paper-layouts
****************************************************************************************************/
typedef enum {
	ORIENTATION_LANDSCAPE,
	ORIENTATION_PORTRAIT
} PageOrientation;

/****************************************************************************************************
* defines some papersizes
****************************************************************************************************/
typedef enum {
	PAPERSIZE_A3,
	PAPERSIZE_A4,
	PAPERSIZE_A5,
	PAPERSIZE_LETTER,
	PAPERSIZE_CUSTOM,
	PAPERSIZE_LAST
} PageSize;

/****************************************************************************************************
* a paper has width and height
****************************************************************************************************/
typedef struct {
	int width;
	int height;
} PageDimension;

/****************************************************************************************************
* an image has width and height
****************************************************************************************************/
typedef struct {
	int width;
	int height;
} ImageDimension;

/****************************************************************************************************
* a page consist of pagesize, orientation, border and resolution
* TODO: there seems to be some redundancy here (width, height are also in pagesize?)
****************************************************************************************************/
typedef struct {
	PageSize page_size;
	PageOrientation page_orientation;
	int page_width;
	int page_height;
	int border_top;
	int border_bottom;
	int border_left;
	int border_right;
	int resolution;
} PageInformation;

#endif
