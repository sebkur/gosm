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

#include "../atlas.h"

#define GOSM_TYPE_ATLAS_TEMPLATE_DIALOG           (atlas_template_dialog_get_type ())
#define GOSM_ATLAS_TEMPLATE_DIALOG(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_ATLAS_TEMPLATE_DIALOG, AtlasTemplateDialog))
#define GOSM_ATLAS_TEMPLATE_DIALOG_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_ATLAS_TEMPLATE_DIALOG, AtlasTemplateDialogClass))
#define GOSM_IS_ATLAS_TEMPLATE_DIALOG(obj)        (G_TYPE_CHECK_INSTANCE ((obj), GOSM_TYPE_ATLAS_TEMPLATE_DIALOG))
#define GOSM_IS_ATLAS_TEMPLATE_DIALOG_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_ATLAS_TEMPLATE_DIALOG))
#define GOSM_ATLAS_TEMPLATE_DIALOG_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_ATLAS_TEMPLATE_DIALOG, AtlasTemplateDialogClass))

typedef struct _AtlasTemplateDialog        AtlasTemplateDialog;
typedef struct _AtlasTemplateDialogClass   AtlasTemplateDialogClass;

struct _AtlasTemplateDialog
{
	GtkDialog parent;

	GtkWidget * radio_orientation_landscape;
	GtkWidget * radio_orientation_portrait;
	GtkWidget * radio_size_a4;
	GtkWidget * radio_size_a5;
	GtkWidget * radio_size_letter;
	GtkWidget * entry_resolution;
	GtkWidget * entry_border;
	GtkWidget * entry_intersect ;
};

struct _AtlasTemplateDialogClass
{
	GtkDialogClass parent_class;

	//void (* function_name) (AtlasTemplateDialog *atlas_template_dialog);
};

GtkDialog * atlas_template_dialog_new();

AtlasImageDimension atlas_template_dialog_get_info(AtlasTemplateDialog * atd);
