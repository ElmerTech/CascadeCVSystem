/*
 * CascadeSURFDescriptorImpl.h
 *
 *  Created on: 1 jun 2014
 *      Author: johan
 */

#ifndef CASCADESURFDESCRIPTORIMPL_H_
#define CASCADESURFDESCRIPTORIMPL_H_

#include <CascadeSURFDescriptor.h>
#include "GaussianKernel.h"
#include "CVSTemplateUtilities.h"

namespace CascadeCVSTemplate{


template<int Size>
template <typename scale_pyramid_type>
CascadeSURFDescriptor<Size>::CascadeSURFDescriptor(
		const scale_pyramid_type& scale_pyramid,
		const interest_point_position& position,
		image_signal<BGR8>* debug
){
	calculateDescriptor( scale_pyramid.maps[0], position, debug );
}


template<int Size>
void CascadeSURFDescriptor<Size>::calculateDescriptor(
		const image_signal<pyramid_data_type>& scale_image,
		const interest_point_position& position,
		image_signal<BGR8>* debug
){
	using _basic_image_signal::f2i;
	using _basic_image_signal::f2i_p;
	using _basic_image_signal::BGR_BLUE_8;
	using _basic_image_signal::BGR_RED_8;
	static const bool DEBUG_DRAW_SAMPLE_POINTS = false;
	static const bool DEBUG_DRAW_ORIENTATION = false;

	static const int WINDOW_SIZE_IN_S = 20;
	static const int SUB_WINDOW_DIVISIONS = 5;
	static const int TOTAL_DIVISIONS = Size * SUB_WINDOW_DIVISIONS;
	static const int NUMBER_OF_STEPS = (TOTAL_DIVISIONS - 1);

	static const int Dij_Dnm = SUB_WINDOW_DIVISIONS;
	static const float step_size_in_s = WINDOW_SIZE_IN_S / (float)TOTAL_DIVISIONS;
	static const float start_offset_in_s = NUMBER_OF_STEPS * step_size_in_s / 2;

	static const float sigma = 3.3;
	static const GaussianKernel<TOTAL_DIVISIONS> gauss(sigma);



	/*****
	 * If interest point is too close to the edge, do not create descriptor
	 */
	if(!valid_interest_point_position(position, scale_image)){
		throw 0;
	}

	static const bool SKIP_DESCRIPTORS = true;
	if(SKIP_DESCRIPTORS){
		for(int i=0;i<VECTOR_SIZE;i++)
			vector[i] = 0;
		return;
	}

	const float s = position.scale;

	const float p_x = position.pos.x;
	const float p_y = position.pos.y;

	const float orientation  = position.orientation;
	const float cos_o = cos(orientation);
	const float sin_o = sin(orientation);


	if(DEBUG_DRAW_ORIENTATION){
		const int debug_x = f2i_p(p_x + 10*s*(cos_o));
		const int debug_y = f2i_p(p_y + 10*s*(sin_o));
		drawLine( *debug, p_x, p_y, debug_x, debug_y, static_cast<BGR8>( BGR_BLUE_8(255)));
	}

	const float step_cos = step_size_in_s * s * cos_o;
	const float step_sin = step_size_in_s * s * sin_o;

	const float curr_x = p_x - start_offset_in_s * s * (cos_o - sin_o);
	const float curr_y = p_y - start_offset_in_s * s * (cos_o + sin_o);

	const float Dx_Dn = -step_sin;			// Should be read as:
	const float Dy_Dn =  step_cos; 			//	 d(x)
	const float Dx_Dm =  step_cos; 			//  ------- = Dx_Dn, and so on...
	const float Dy_Dm =  step_sin; 			//	 d(n)

	const float Dx_Di = Dij_Dnm * Dx_Dn;
	const float Dy_Di = Dij_Dnm * Dy_Dn;
	const float Dx_Dj = Dij_Dnm * Dx_Dm;
	const float Dy_Dj = Dij_Dnm * Dy_Dm;

	const int half_haar_lobe_size = f2i(s);
	if(half_haar_lobe_size < 1){
		//cerr<< "Scale too small, half_lobe_size < 1 , s: " << s << "\n";
		throw 0;
	}


	int current_index = 0;
	for(int i = 0; i < Size; i++){
		for(int j = 0; j < Size; j++){
			float sub_region_start_x = curr_x + Dx_Di * i + Dx_Dj * j;
			float sub_region_start_y = curr_y + Dy_Di * i + Dy_Dj * j;

			/***
			 * 	Calculate sub-region haar_sums
			 *
			 */
			float S_di = 0, S_dj = 0, S_abs_di = 0, S_abs_dj = 0;
			for(int n = 0; n < SUB_WINDOW_DIVISIONS; n++){
				for(int m = 0; m < SUB_WINDOW_DIVISIONS; m++){
					const float weight = 6*sigma*sigma*gauss[Dij_Dnm * i + n][Dij_Dnm * j + m];
					const int x = f2i(sub_region_start_x + (Dx_Dn * n) + (Dx_Dm * m));
					const int y = f2i(sub_region_start_y + (Dy_Dn * n) + (Dy_Dm * m));

					if(DEBUG_DRAW_SAMPLE_POINTS){
						BGR8& p = (*debug)[y][x];
						if(i==0&&j==0)
							p = BGR_BLUE_8(255);
						else
							p = BGR_RED_8(weight*255);
					}

					const float haar_x = weight * calc_deriv_x(scale_image, y, x, half_haar_lobe_size);
					const float haar_y = weight * calc_deriv_y(scale_image, y, x, half_haar_lobe_size);

					const float haar_i = cos_o * haar_y - sin_o * haar_x;
					const float haar_j = cos_o * haar_x + sin_o * haar_y;
					S_di += haar_i;
					S_abs_di += abs(haar_i);
					S_dj += haar_j;
					S_abs_dj += abs(haar_j);
				}
			}
			vector[4*current_index+0] = S_di;
			vector[4*current_index+1] = S_dj;
			vector[4*current_index+2] = S_abs_di;
			vector[4*current_index+3] = S_abs_dj;
			current_index++;
		}
	}
	float norm_squared =0;
	for(int i=0; i<VECTOR_SIZE;i++)
		norm_squared += vector[i]*vector[i];

	const float normalizing_factor = 1.f/sqrtf(norm_squared);
	for(int i=0; i<VECTOR_SIZE;i++)
		vector[i] *= normalizing_factor;

}

} // namespace CascadeCVSTemplate

#endif /* CASCADESURFDESCRIPTORIMPL_H_ */
