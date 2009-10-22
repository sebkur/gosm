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

#include "../atlas/atlas.h"
#include "../map_types.h"

#define GOSM_TYPE_PDF_GENERATOR           (pdf_generator_get_type ())
#define GOSM_PDF_GENERATOR(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_PDF_GENERATOR, PdfGenerator))
#define GOSM_PDF_GENERATOR_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_PDF_GENERATOR, PdfGeneratorClass))
#define GOSM_IS_PDF_GENERATOR(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_PDF_GENERATOR))
#define GOSM_IS_PDF_GENERATOR_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_PDF_GENERATOR))
#define GOSM_PDF_GENERATOR_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_PDF_GENERATOR, PdfGeneratorClass))

typedef struct _PdfGenerator        PdfGenerator;
typedef struct _PdfGeneratorClass   PdfGeneratorClass;

struct _PdfGenerator
{
	GObject parent;

	char * java_binary;

	PageInformation page_info;
	char * file_prefix;
	int parts_x;
	int parts_y;
};

struct _PdfGeneratorClass
{
	GObjectClass parent_class;

	void (* page_progress) (PdfGenerator *pdf_generator);
};

PdfGenerator * pdf_generator_new();

void pdf_generator_set_java_binary(PdfGenerator * pdf_generator, char * path);

void pdf_generator_setup(PdfGenerator * pdf_generator, PageInformation page_info, char * file_prefix, int parts_x, int parts_y);

void pdf_generator_process(PdfGenerator * pdf_generator);


//void pdf_generator_generate(PdfGenerator * pdf_generator, PageInformation page_info, char * file_prefix, int parts_x, int parts_y);

//void generate_pdf(PdfGenerator * pdf_gen, char * cache_dir, Selection selection, PageInformation page_info,
//	ImageDimension image_dimension, int intersect_x, int intersect_y, int zoom);
