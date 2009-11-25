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

#ifndef _SXDL_IMAGE_H_
#define _SXDL_IMAGE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "sxdl_base.h"

#define GOSM_TYPE_SXDL_IMAGE           (sxdl_image_get_type ())
#define GOSM_SXDL_IMAGE(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_SXDL_IMAGE, SxdlImage))
#define GOSM_SXDL_IMAGE_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_SXDL_IMAGE, SxdlImageClass))
#define GOSM_IS_SXDL_IMAGE(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_SXDL_IMAGE))
#define GOSM_IS_SXDL_IMAGE_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_SXDL_IMAGE))
#define GOSM_SXDL_IMAGE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_SXDL_IMAGE, SxdlImageClass))

typedef struct _SxdlImage        SxdlImage;
typedef struct _SxdlImageClass   SxdlImageClass;

struct _SxdlImage
{
	SxdlBase parent;

	char * src;
	int width;
	int height;
};

struct _SxdlImageClass
{
	SxdlBaseClass parent_class;

	//void (* function_name) (SxdlImage *sxdl_image);
};

SxdlImage * sxdl_image_new(char * src);

#endif /* _SXDL_IMAGE_H_ */
