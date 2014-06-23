/*
 * simple_image_signalStructures.h
 *
 *  Created on: 15 okt 2010
 *      Author: Johan
 */

#ifndef IMAGE_SIGNAL_STRUCTURES_H_
#define IMAGE_SIGNAL_STRUCTURES_H_

namespace _basic_image_signal {
typedef float ImagePosition;
typedef float ImageLength;


typedef struct{
	ImagePosition x,y;
	ImageLength width, height;
} ImageArea;


class Range{
public:
	double min, max;
	Range(double mi, double ma):min(mi),max(ma){}
};

class BoundingBox{
public:
	double min_x, min_y, max_x, max_y;
	BoundingBox(double mi_x, double mi_y, double ma_x, double ma_y):min_x(mi_x),min_y(mi_y),max_x(ma_x),max_y(ma_y){}
};

}

#endif /* IMAGE_SIGNAL_STRUCTURES_H_ */
