#ifndef _ATLAS_H_
#define _ATLAS_H_

typedef enum {
	ORIENTATION_LANDSCAPE,
	ORIENTATION_PORTRAIT
} PageOrientation;

typedef enum {
	PAPERSIZE_A3,
	PAPERSIZE_A4,
	PAPERSIZE_A5,
	PAPERSIZE_LETTER,
	PAPERSIZE_CUSTOM,
	PAPERSIZE_LAST
} PageSize;

typedef struct {
	int width;
	int height;
} PageDimension;

typedef struct {
	int width;
	int height;
} ImageDimension;

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
