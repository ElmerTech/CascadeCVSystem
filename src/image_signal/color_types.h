/*
 * ColorTypes.h
 *
 *  Created on: 3 sep 2010
 *      Author: Johan
 */

#ifndef IMAGE_SIGNAL_COLOR_TYPES_H_
#define IMAGE_SIGNAL_COLOR_TYPES_H_

#include "color_types_rgb.h"

namespace _basic_image_signal {
typedef RGBSTRUCT<unsigned char> RGB8;
typedef BGRSTRUCT<unsigned char> BGR8;
typedef unsigned char MONO8;

typedef RGBREDSTRUCT<unsigned char> RGB_RED_8;
typedef RGBGREENSTRUCT<unsigned char> RGB_GREEN_8;
typedef RGBBLUESTRUCT<unsigned char> RGB_BLUE_8;

typedef BGRREDSTRUCT<unsigned char> BGR_RED_8;
typedef BGRGREENSTRUCT<unsigned char> BGR_GREEN_8;
typedef BGRBLUESTRUCT<unsigned char> BGR_BLUE_8;
typedef BGRGRAYSTRUCT<unsigned char> BGR_GRAY_8;
}

#endif /* IMAGE_SIGNAL_COLOR_TYPES_H_ */
