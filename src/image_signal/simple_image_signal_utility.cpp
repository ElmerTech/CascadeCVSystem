/*
 * Utility.cpp
 *
 *  Created on: 1 sep 2010
 *      Author: Johan
 */

#include "simple_image_signal_utility.h"
#include "color_types.h"
#include <cmath>

namespace _basic_image_signal{
void drawPlot(const basic_image_signal<BGR8>* src, CvPoint p1, CvPoint p2, basic_image_signal<BGR8>* dst){
	const int width = dst->raw_width();
	const int height = dst->raw_height();

	const basic_image_signal<BGR8>& s_im = *src;
	basic_image_signal<BGR8>& d_im = *dst;

	clear_image( d_im );
	int noll_y = height * (1 - 127 / 256.f);

	for(int i = 0; i<width;i++){
		d_im[noll_y][i] = RGB8(255,255,255);
		const float rx = ( p1.x * (float)(width - (i + 1)) + p2.x * (float)i ) / (float)(width-1) ;
		const float ry = ( (p1.y * (float)(width - (i + 1))) + ( p2.y * (float)i ) ) / (float)(width-1) ;
		const int x = rx;
		const int y = ry;
		const float d1a = (1 - sqrt((rx-x)*(rx-x) 		+ (ry-y)*(ry-y)));
		const float d1 = d1a > 0 ? d1a : 0;
		const float d2a = (1 - sqrt((rx-(x+1))*(rx-(x+1)) + (ry-y)*(ry-y)));
		const float d2 = d2a > 0 ? d2a : 0;
		const float d3a = (1 - sqrt((rx-x)*(rx-x) 		+ (ry-(y+1))*(ry-(y+1))));
		const float d3 = d3a > 0 ? d3a : 0;
		const float d4a = (1 - sqrt((rx-(x+1))*(rx-(x+1)) + (ry-(y+1))*(ry-(y+1))));
		const float d4 = d4a > 0 ? d4a : 0;
		for(int c = 0;c<3; c++){
			float norm = (d1+d2+d3+d4);
			if(norm == 0)return;
			float value = (d1*(s_im[y][x].values[c]) + d2*(s_im[y][x+1].values[c]) + d3*(s_im[y+1][x].values[c]) + d4*(s_im[y+1][x+1].values[c]))/norm;
			int dst_y = height * (1 - value / 256.f);
			dst_y = dst_y > 0 ? dst_y : 0;
			dst_y = dst_y < height -1 ? dst_y : height-2;

			d_im[dst_y][i].values[c] = 255;
		}
	}
}



template<class T> void drawPlot(T* src, CvPoint p1, CvPoint p2, basic_image_signal<MONO8>* dst){
	const int width = dst->raw_width();
	const int height = dst->raw_height();

	basic_image_signal<BGR8>& d_im = *dst;

	memset(d_im.array, 0, sizeof(MONO8[d_im.raw_height()*d_im.width_step()]));
	int noll_y = height * (1 - 127 / 256.f);

	for(int i = 0; i<width;i++){
		d_im[noll_y][i] = MONO8(255);


	}
}

}
