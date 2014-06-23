/*
 * simple_image_signalDraw.h
 *
 *  Created on: 14 sep 2010
 *      Author: Johan
 */

#ifndef IMAGE_SIGNAL_DRAW_FUNCTIONS_H_
#define IMAGE_SIGNAL_DRAW_FUNCTIONS_H_

#include "color_types.h"
#include "simple_image_signal.h"
#include <cmath>

namespace _basic_image_signal {


template<class CT>
inline void drawHorizontalLine(
		basic_image_signal<CT>& image,
		const int y,
		const int x1,
		const int x2,
		const CT color
){
	CT* const row = image[y];
	const bool swap = (x2 < x1);

	int x = swap ? x2 : x1;
	const int x_end = swap ? x1 : x2;
	while(x <= x_end)
		row[x++] = color;
}

template<class CT>
inline void drawVerticalLine(
		basic_image_signal<CT>& image,
		const int x,
		const int y1,
		const int y2,
		const CT color
){
	const bool swap = y2 < y1;
	int y = swap ? y2 : y1;
	const int y_end = swap ? y1 : y2;


	while(y <= y_end)
		image[y++][x] = color;
}


template<class CT>
inline void drawLine(
		basic_image_signal<CT>& im,
		const int x1,
		const int y1,
		const int x2,
		const int y2,
		const CT color
){
	const int diff_x = x2 - x1;
	const int diff_y = y2 - y1;

	if(diff_x==0&&diff_y==0){
		im[y1][x1] = color;
		return;
	}

	const int abs_d_x = std::abs(diff_x);
	const int abs_d_y = std::abs(diff_y);

	if(std::abs(diff_x)>std::abs(diff_y)){
		const bool swap = diff_x < 0;
		const int x_s = swap ? x2 : x1;
		const int x_e = swap ? x1 : x2;
		const int dy = ( (diff_y>0) ^ swap ) ? 1 : -1;

		int x = x_s;
		int y = swap ? y2 : y1;
		int steps_y = abs_d_y;

		int n_x = abs_d_x >> 1;
		int x_i = (0<steps_y--) ? x_s + n_x / abs_d_y : x_e;
		while(x < x_e){
			while(x<=x_i)
				im[y][x++] = color;
			n_x += abs_d_x;
			x_i = (0<steps_y--) ? x_s + n_x / abs_d_y : x_e;
			y+=dy;
		}
	}else{// (abs(dx)<abs(dy))
		const bool swap = diff_y < 0;
		const int y_s = swap ? y2 : y1;
		const int y_e = swap ? y1 : y2;
		const int dx = ( (diff_x > 0) ^ swap ) ? 1 : -1;

		int y = y_s;
		int x = swap ? x2 : x1;
		int steps_x = abs_d_x;

		int n_y = abs_d_y >> 1;
		int y_i = (0<steps_x--) ? y_s + n_y / abs_d_x : y_e;
		while(y < y_e){
			while(y<=y_i)
				im[y++][x] = color;
			n_y += abs_d_y;
			y_i = (0<steps_x--) ? y_s + n_y / abs_d_x : y_e;
			x+=dx;
		}
	}
}

template<class CT>
inline void draw2SymetricDots(
		basic_image_signal<CT>& image,
		const int x,
		const int y,
		const int dx,
		const int dy,
		const CT color
){
	image[y+dy][x+dx] = color;
	image[y-dy][x-dx] = color;
}

template<class CT>
inline void draw4SymetricDots(
		basic_image_signal<CT>& image,
		const int x,
		const int y,
		const int dx,
		const int dy,
		const CT color
){
	draw2SymetricDots(image,x,y,dx,dy,color);
	draw2SymetricDots(image,x,y,dy,dx,color);
}

template<class CT>
inline void drawCircle(
		basic_image_signal<CT>& image,
		const int x,
		const int y,
		const int r,
		const CT color
){
	/***
	 * error(x,y) = 4*( (x-.5)(x-.5) + y*y -r*r ) = 4*x*x -4*x + 1 + 4*y*y - 4*r*r
	 * Initially,
	 * 	dx = r, dy = 0
	 * 	and
	 * error(dx,dy) = error(r,0) = 1 - 4*r;
	 * error(dx-1,dy) - error(dx,dy) = -8*(x-1)
	 * error(dx,dy+1) - error(dx,dy) = 8*y +4
	 */
	int dx = r;
	int dy = 0;
	int error =  1 - 4*dx;

	do{
		draw4SymetricDots(image,x,y,dx,dy,color);
		draw4SymetricDots(image,x,y,dx,-dy,color);
		error += 8*(dy++) + 4;
		if(error > 0)
			error -= 8*(--dx);
	}while(dx >= dy);
}

template<class CT>
inline void drawCircle(
		basic_image_signal<CT>& image,
		const int x,
		const int y,
		const int r,
		const CT color,
		const int thickness
){
	/***
	 *                {x,y}+.5
	 *                  / /
	 * error(x,y) = 3 * | | (2x)^2 + (2y)^2 - (2r)^2 dx dy = 12 * ( x^2 + y^2 - r^2 ) + 2
	 *                  / /
	 *                {x,y}-.5
	 *
	 * error(dx-1,dy) - error(dx,dy) = -24x + 12
	 * error(dx,dy+1) - error(dx,dy) =  24y + 12
	 *
	 * Initially,
	 * 	dx = r, dy = 0
	 * 	and
	 * error(dx,dy) = error(r,0) = 2 + 12*x*x - 3*d*d;
	 */
	const int diameter = r << 1;
	const int diameter_i = diameter - thickness;
	const int diameter_o = diameter + thickness;
	const int r_i = (diameter_i)>>1;
	const int r_o = (diameter_o)>>1;

	int dy = 0;
	int dx_i = r_i;
	int dx_o = r_o;
	int error_i =  2 + 12 * dx_i * dx_i - 3 * diameter_i * diameter_i;
	int error_o =  2 + 12 * dx_o * dx_o - 3 * diameter_o * diameter_o;

	while(error_i > 0 && dx_i>=0)
		error_i -= 24*(dx_i--) - 12;

	while(error_o > 0 && dx_o>=0)
		error_o -= 24*(dx_o--) - 12;

	const int x_i = dx_i + 1;
	drawHorizontalLine(image, y, x + x_i, x + dx_o, color);
	drawHorizontalLine(image, y, x - dx_o, x - x_i, color);

//	while(dy < dx_i){
//		const int derr_dy = 12*(++dy) + 6;
//		error_i += derr_dy;
//		if(error_i > 0)
//			error_i -= 12*(--dx_i) + 6;
//
//		error_o += derr_dy;
//		if(error_o > 0)
//			error_o -= 12*(dx_o--) + 6;
//
//		drawHorizontalLine(image, y + dy, x + dx_i, x + dx_o, color);
//		drawHorizontalLine(image, y + dy, x - dx_o, x - dx_i, color);
//		drawHorizontalLine(image, y - dy, x + dx_i, x + dx_o, color);
//		drawHorizontalLine(image, y - dy, x - dx_o, x - dx_i, color);
//	}
	while(dy < r_i){
		const int derr_dy = 24*(dy++) + 12;
		error_i += derr_dy;
		while(error_i > 0 && dx_i>=-1)
			error_i -= 24*(dx_i--) - 12;

		error_o += derr_dy;
		while(error_o > 0)
			error_o -= 24*(dx_o--) - 12;

		if(dx_i>=0){
			const int x_i = dx_i + 1;
			drawHorizontalLine(image, y + dy, x + x_i, x + dx_o, color);
			drawHorizontalLine(image, y + dy, x - dx_o, x - x_i, color);
			drawHorizontalLine(image, y - dy, x + x_i, x + dx_o, color);
			drawHorizontalLine(image, y - dy, x - dx_o, x - x_i, color);
		}else{
			drawHorizontalLine(image, y + dy, x - dx_o, x + dx_o, color);
			drawHorizontalLine(image, y - dy, x - dx_o, x + dx_o, color);
		}
	}
	while(dy <= r_o){
		const int derr_dy = 24*(dy++) + 12;
		error_o += derr_dy;
		while(error_o > 0){
			if(dx_o>=0)
				error_o -= 24*(dx_o--) - 12;
			else
				return;
		}

		drawHorizontalLine(image, y + dy, x - dx_o, x + dx_o, color);
		drawHorizontalLine(image, y - dy, x - dx_o, x + dx_o, color);
	}

}



template<class CT>
inline void drawSquare(
		basic_image_signal<CT>& image,
		const int x,
		const int y,
		const int r,
		const CT color
){
	for(int n=1-r; n < r; n++){
		image[ y - r ][ x + n ] = color;
		image[ y + r ][ x + n ] = color;
	}
	for(int m=1-r; m < r; m++){
		image[ y + m ][x - r ] = color;
		image[ y + m ][x + r ] = color;
	}
}

template<class CT>
inline void drawCross(
		basic_image_signal<CT>& image,
		const int x,
		const int y,
		const int r,
		const CT color
){
	for( int n = 1-r; n < r; n++){
		image[ y ][ x + n ] = color;
	}
	for(int m=1-r; m < r; m++){
		image[ y + m ][ x ] = color;
	}
}

}

#endif /* IMAGE_SIGNAL_DRAW_FUNCTIONS_H_ */
