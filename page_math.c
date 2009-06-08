#include <stdio.h>
#include <stdlib.h>

#include "page_math.h"
#include "atlas.h"

#define MM_TO_INCH(x)	((double)x)/25.4

ImageDimension get_image_dimension(PageInformation page_info)
{
	ImageDimension id;
	id.width = MM_TO_INCH(page_info.page_width - page_info.border_left - page_info.border_right) * page_info.resolution;
	id.height = MM_TO_INCH(page_info.page_height - page_info.border_top - page_info.border_bottom) * page_info.resolution;
	return id;
}
