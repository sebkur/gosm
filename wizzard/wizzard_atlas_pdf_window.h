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

#ifndef _WIZZARD_ATLAS_PDF_WINDOW_H_
#define _WIZZARD_ATLAS_PDF_WINDOW_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#define GOSM_TYPE_WIZZARD_ATLAS_PDF_WINDOW           (wizzard_atlas_pdf_window_get_type ())
#define GOSM_WIZZARD_ATLAS_PDF_WINDOW(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_WIZZARD_ATLAS_PDF_WINDOW, WizzardAtlasPdfWindow))
#define GOSM_WIZZARD_ATLAS_PDF_WINDOW_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_WIZZARD_ATLAS_PDF_WINDOW, WizzardAtlasPdfWindowClass))
#define GOSM_IS_WIZZARD_ATLAS_PDF_WINDOW(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_WIZZARD_ATLAS_PDF_WINDOW))
#define GOSM_IS_WIZZARD_ATLAS_PDF_WINDOW_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_WIZZARD_ATLAS_PDF_WINDOW))
#define GOSM_WIZZARD_ATLAS_PDF_WINDOW_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_WIZZARD_ATLAS_PDF_WINDOW, WizzardAtlasPdfWindowClass))

typedef struct _WizzardAtlasPdfWindow        WizzardAtlasPdfWindow;
typedef struct _WizzardAtlasPdfWindowClass   WizzardAtlasPdfWindowClass;

struct _WizzardAtlasPdfWindow
{
	GtkWindow parent;

	GtkWidget *button_export;
	GtkWidget *button_cancel;

	GtkWidget *entry_filename;
	GtkWidget *button_filename;
	GtkWidget *progress_bar_images;
	GtkWidget *progress_bar_pdf;
};

struct _WizzardAtlasPdfWindowClass
{
	GtkWindowClass parent_class;

	//void (* function_name) (WizzardAtlasPdfWindow *wizzard_atlas_pdf_window);
};

WizzardAtlasPdfWindow * wizzard_atlas_pdf_window_new();

void wizzard_atlas_pdf_window_set_inactive(WizzardAtlasPdfWindow * wapw);

#endif
