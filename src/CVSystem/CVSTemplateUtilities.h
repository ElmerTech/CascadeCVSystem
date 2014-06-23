/*
 * SURFUtilities.h
 *
 *  Created on: 9 sep 2010
 *      Author: Johan
 */

#ifndef CASCADESURFUTILITIES_H_
#define CASCADESURFUTILITIES_H_

#include "../CascadeImageMatchingConfig.h"
#include "CVSScalePyramid.h"

//template<class T> inline CascadeDiscern calcResponse(T* image_point, int& width_step);
//#include "Octaves.h"


namespace CascadeCVSTemplate{

//template< class T>
//inline discern_type calcResponse( const const_image_signal_point<T>& image_point );




/******
 * Test if a descriptor can be created for the specified interest point position
 */
template <typename T>
inline bool valid_interest_point_position(interest_point_position const & position, const image_signal<T>& image){
	const int margin = 15*position.scale;
	return !(position.pos.x < margin || position.pos.y < margin || image.raw_width() - position.pos.x < margin || image.raw_height() - position.pos.y < margin);
}

inline bool valid_interest_point_position( interest_point_position const & position, const scale_pyramid_base& pyramid){
	const int margin = 15*position.scale;
	return !(position.pos.x < margin || position.pos.y < margin || pyramid.width - position.pos.x < margin || pyramid.height - position.pos.y < margin);
}


/****
 *	Calculates deriv response in x and y directions respectively
 */

inline integral_type calc_deriv_x( image_signal< pyramid_data_type > const& image, const int y, const int x, const int half_size){
	const int s = half_size;
	return  (image[y][x-s] - image[y][x+s]);
}

inline integral_type calc_deriv_y( image_signal< pyramid_data_type > const& image, const int y, const int x, const int half_size){
	const int s = half_size;
	return  (image[y-s][x] - image[y+s][x]);
}

//inline integral_type
//calc_deriv_x( image_signal< std::enable_if< !std::is_same<discern_type,pyramid_data_type>::value,pyramid_data_type>::type > const& image, const int y, const int x, const int half_size){
//	const int s = half_size;
//	return  (image[y][x-s] - image[y][x+s]);
//}
//
//inline std::enable_if< !std::is_same<discern_type,pyramid_data_type>::value, integral_type >::type
//calc_deriv_y( image_signal< pyramid_data_type > const& image, const int y, const int x, const int half_size){
//	const int s = half_size;
//	return  (image[y-s][x] - image[y+s][x]);
//}








template<class T>
inline discern_type calcDxy( const const_image_signal_point<T>& image_point){
	static constexpr discern_type DxyFactor = 1/4.0;
	return 	DxyFactor *
			(- image_point[-1][-1] + image_point[-1][ 1]
	       	 + image_point[ 1][-1] - image_point[ 1][ 1]);
}


template<class T>
inline discern_type calcDxx( const const_image_signal_point<T>& image_point){
	return 	image_point[0][-1] - (2*image_point[0][0]) + image_point[0][1];
}

template<class T>
inline discern_type calcDyy( const const_image_signal_point<T>& image_point){
	return 	image_point[-1][0] - (2*image_point[0][0]) + image_point[1][0];
}



template< class T>
inline discern_type calcResponse( const const_image_signal_point<T>& image_point){
	const discern_type Dxy = calcDxy(image_point);
	const discern_type Dxx = calcDxx(image_point);
	const discern_type Dyy = calcDyy(image_point);

	return static_cast<int>((Dxx * Dyy  - Dxy * Dxy)>>32);
}



template< class T>
inline discern_type calcResponse_with_laplacian( const const_image_signal_point<T>& image_point, discern_type* laplace){
	const discern_type Dxy = calcDxy(image_point);
	const discern_type Dxx = calcDxx(image_point);
	const discern_type Dyy = calcDyy(image_point);

	if(laplace)
		*laplace = Dxx + Dyy;

	return ( Dxx * Dyy - Dxy * Dxy);
}




//template<int CURRENT_OCTAVE, int FILT_PER_OCT, int CURRENT_FILTER> struct calc_struct{
//	template<class D, class T> static void calcResponsesRecursively(D** const discern_point, const T* const& image_point, const int& width_step){
//		 *(discern_point[0]) = calcResponse<CascadeOctave<CURRENT_OCTAVE,FILT_PER_OCT>::first_filter_size + CascadeOctave<CURRENT_OCTAVE,FILT_PER_OCT>::filter_size_step * CURRENT_FILTER>(image_point, width_step);
//		calc_struct<CURRENT_OCTAVE, FILT_PER_OCT, CURRENT_FILTER+1>::calcResponsesRecursively(discern_point+1, image_point, width_step);
//	}
//
//};
//
//template<int CURRENT_OCTAVE, int FILT_PER_OCT> struct calc_struct<CURRENT_OCTAVE, FILT_PER_OCT, FILT_PER_OCT>{
//	template<class D, class T> static void calcResponsesRecursively(D** const discern_point, const T* const& image_point, const int& width_step){
////		*(discern_point[0]) = calcResponse<CascadeOctave<CURRENT_OCTAVE,FILT_PER_OCTAVE>::first_filter_size + CascadeOctave<CURRENT_OCTAVE,FILT_PER_OCTAVE>::filter_size_step * (FILT_PER_OCTAVE-1)>(image_point, width_step);
//	}
//
//};



void calculate_response_image(
		const image_signal< pyramid_data_type>& source,
		image_signal<discern_type>& result
);



enum Calculate_Options            	{  U, d_x, d_y, d_s, d_xx, d_yy, d_ss, d_xy, d_xs, d_ys};
enum Calculate_Inplane_Options		{  U_ip, d_x_ip, d_y_ip, d_xx_ip, d_yy_ip, d_xy_ip};
//template<Calculate_Options c> inline CascadeDiscern calc_function(const float* const b);
template<Calculate_Options c> inline discern_type calc_function_weighted(const discern_type* const b);
template<Calculate_Inplane_Options c> inline discern_type calc_inplane_function_weighted(const discern_type* const b);

/***
 * Interpolates position of interest point
 *
 * assumes b of form:
 * b: [H(-1,-1,-1), H(0,-1,-1), H(1,-1,-1), H(-1,0,-1), H(0,0,-1), H(1,0,-1), H(-1,1,-1), H(0,1,-1), H(1,1,-1), H(-1,-1,0), H(0,-1,0), H(1,-1,0), H(-1,0,0), H(0,0,0), H(1,0,0), H(-1,1,0), H(0,1,0), H(1,1,0), H(-1,-1,1), H(0,-1,1), H(1,-1,1), H(-1,0,1), H(0,0,1), H(1,0,1), H(-1,1,1), H(0,1,1), H(1,1,1)]
 * 	where H(delta_x,delta_y,delta_s): discern response in position {x0+delta_x, y0 + delta_y, z0 + delta_z}
 * 	and {x0,y0,z0} denotes the initially found interest point
 */

bool interpolate_position(const discern_type* const b, interest_point_position* p_i_p, const discern_type calc_step, const discern_type size_step);



template<Calculate_Options c> inline discern_type calc_function_weighted(const discern_type* const b){
	static const discern_type normalizing_factor[10] = { 27, 52, 52, 52, 108, 108, 108, 28, 28, 28 };

	static const discern_type Function_Coefficients[10][27] = {
			{-2,  1, -2,  1,  4,  1, -2,  1, -2,  1,  4,  1,  4,  7,  4,  1,  4,  1, -2,  1, -2,  1,  4,  1, -2,  1, -2},
			//The line above does not belong to this solution
			{-2,   0,   2,  -3,   0,   3,  -2,   0,   2,  -3,   0,   3,  -6,   0,   6,  -3,   0,   3,  -2,   0,   2,  -3,   0,   3,  -2,   0,   2},
			{-2,  -3,  -2,   0,   0,   0,   2,   3,   2,  -3,  -6,  -3,   0,   0,   0,   3,   6,   3,  -2,  -3,  -2,   0,   0,   0,   2,   3,   2},
			{-2,  -3,  -2,  -3,  -6,  -3,  -2,  -3,  -2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,   3,   2,   3,   6,   3,   2,   3,   2},
			{ 4, -14,   4,  13, -14,  13,   4, -14,   4,  13, -14,  13,  40,-104,  40,  13, -14,  13,   4, -14,   4,  13, -14,  13,   4, -14,   4},
			{ 4,  13,   4, -14, -14, -14,   4,  13,   4,  13,  40,  13, -14,-104, -14,  13,  40,  13,   4,  13,   4, -14, -14, -14,   4,  13,   4},
			{ 4,  13,   4,  13,  40,  13,   4,  13,   4, -14, -14, -14, -14,-104, -14, -14, -14, -14,   4,  13,   4,  13,  40,  13,   4,  13,   4},
			{ 2,   0,  -2,   0,   0,   0,  -2,   0,   2,   3,   0,  -3,   0,   0,   0,  -3,   0,   3,   2,   0,  -2,   0,   0,   0,  -2,   0,   2},
			{ 2,   0,  -2,   3,   0,  -3,   2,   0,  -2,   0,   0,   0,   0,   0,   0,   0,   0,   0,  -2,   0,   2,  -3,   0,   3,  -2,   0,   2},
			{ 2,   3,   2,   0,   0,   0,  -2,  -3,  -2,   0,   0,   0,   0,   0,   0,   0,   0,   0,  -2,  -3,  -2,   0,   0,   0,   2,   3,   2}};
	discern_type sum = 0;
	for(int i=0; i<27; i++)
		sum += Function_Coefficients[c][i]*b[i];
	return sum / normalizing_factor[c];
}

template<Calculate_Inplane_Options c> inline discern_type calc_inplane_function_weighted(const discern_type* const b){
	static const discern_type weights[6] = { 1/1.0, 1/8.0, 1/8.0, 1/6.0, 1/6.0, 1/4.0 };

	static const discern_type Function_Coefficients[6][9] = {
			{ 0, 0, 0, 0, 1, 0, 0, 0, 0},
			{-1, 0, 1,-2, 0, 2,-1, 0, 1},
			{-1,-2,-1, 0, 0, 0, 1, 2, 1},
			{ 1,-2, 1, 4,-8, 4, 1,-2, 1},
			{ 1, 4, 1,-2,-8,-2, 1, 4, 1},
			{ 1, 0,-1, 0, 0, 0,-1, 0, 1}};
	const discern_type* const values = b+9;

	discern_type sum = 0;
	for(int i=0; i<9; i++)
		sum += Function_Coefficients[c][i]*values[i];
	return sum * weights[c];
}



inline void pack_values(discern_type* const b, const discern_type* const cur_p, const discern_type* const lo_p, const discern_type* const up_p, const int cur_ss, const int lo_ss, const int up_ss, const int cur_rs, const int lo_rs, const int up_rs){
	for(int y = -1; y <= 1; y++){
		for(int x = -1; x <= 1; x++){
			const int current_index = 3*(y + 1) + (x + 1);
			b[current_index]      =  lo_p[  lo_rs*y +  lo_ss*x];
			b[current_index + 9]  = cur_p[ cur_rs*y + cur_ss*x];
			b[current_index + 18] =  up_p[  up_rs*y +  up_ss*x];
		}
	}
}

}

#endif /* CASCADESURFUTILITIES_H_ */
