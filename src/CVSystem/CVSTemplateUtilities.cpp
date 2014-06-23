/*
 * SURFUtilities.cpp
 *
 *  Created on: 31 jan 2011
 *      Author: Johan
 */

#include "CVSTemplateUtilities.h"

namespace CascadeCVSTemplate{




void calculate_response_image(
		const image_signal< pyramid_data_type >& source,
		image_signal<discern_type>& result
){
	const int start_y = 1;
	const int stop_y = source.raw_height()-1;

	const int start_x = 1;
	const int stop_x = source.raw_width()-1;

	result.copy_size( source );
	for(int i = start_y; i < stop_y; i++ ){
		image_signal_point<discern_type> result_line(result,i,0);
		const_image_signal_point<pyramid_data_type> source_line(source,i,0);
		for(int j = start_x; j < stop_x; j++ ){
			result_line( 0, j ) = calcResponse( source_line.shifted( 0, j ));
		}
	}
	result.apply_shift({1,1});
	result.shift_data_offset(1,1);
	result.raw_width_diff(-2);
	result.raw_height_diff(-2);
}


bool interpolate_position(const discern_type* const b, interest_point_position* p_i_p, const discern_type calc_step, const discern_type size_step){
	static const bool DEBUG_MESSAGES = false;
	static const double inverse_lower_eigenvalue_bound = .5;
	static const double threshold = 3*inverse_lower_eigenvalue_bound;
	static bool skip_interpolation = false;
	static const float maxdistance = 2;


	/* Calculate estimated second order derivatives and test for definiteness
	 * Since the interest point is a maximum, D2H should be negative-definite, product of 3 negative eigenvalues is negative
	 */
	const discern_type dxx = calc_function_weighted<d_xx>(b);
	if(dxx >=0){ //Test Definiteness
		return false;
	}
	const discern_type dyy = calc_function_weighted<d_yy>(b);
	if(dyy >=0){ //Test Definiteness
		return false;
	}
	const discern_type dxy = calc_function_weighted<d_xy>(b);
	if(dxx*dyy-dxy*dxy <= 0){ //Test for negative-definiteness by means of leading principal minors (Sylvester's criterion)
		return false;
	}

	const discern_type dss = calc_function_weighted<d_ss>(b);
	if(dss >=0){ //Test Definiteness
		return false;
	}
	const discern_type dxs = calc_function_weighted<d_xs>(b);
	const discern_type dys = calc_function_weighted<d_ys>(b);

	// Calculate determinant of Hessian matrix
	const discern_type detD2H = dss*dxx*dyy - dxx*dys*dys - dyy*dxs*dxs - dss*dxy*dxy + 2*dxs*dxy*dys;
	if(detD2H >= 0){ // Test definiteness again, this time based on the last leading principal minor, the determinant of the matrix itself
		return false;
	}

	//	// Calculate in plane second order derivatives for localization
	//	const CascadeDiscern ip_dxx = calc_inplane_function_weighted<d_xx_ip>(b);
	//	const CascadeDiscern ip_dyy = calc_inplane_function_weighted<d_yy_ip>(b);
	//	const CascadeDiscern ip_dxy = calc_inplane_function_weighted<d_xy_ip>(b);
	//	const CascadeDiscern ip_determinant = ip_dxx * ip_dyy - ip_dxy*ip_dxy;
	//	const CascadeDiscern ip_trace = ip_dxx + ip_dyy;
	//	const CascadeDiscern s = p_i_p->s;
	//	if(ip_determinant<0 || (( ip_trace*(-1)*b[13] > 2*ip_determinant*s ))){
	//		p_i_p->s = .5;
	//		return false;
	//	}




	const discern_type adjointD2H[3][3] = {
			{ dss*dyy-dys*dys, dxs*dys-dss*dxy, dxy*dys-dxs*dyy},
			{ dxs*dys-dss*dxy, dss*dxx-dxs*dxs, dxs*dxy-dxx*dys},
			{ dxy*dys-dxs*dyy, dxs*dxy-dxx*dys, dxx*dyy-dxy*dxy}};

	// Calculate estimated function value in the un-interpolated identified maximum
	// const CascadeDiscern fv  = calc_function<U>(b);


	/*
	 * The following criteria is to is purely ad-hoc.
	 * Should infer some limit on the smallest eigenvalue of D2H to ensure a well defined maximum
	 */
//	if((adjointD2H[0][0] + adjointD2H[1][1] + adjointD2H[2][2]) >= threshold*(-detD2H)){
//		cerr << (adjointD2H[0][0] + adjointD2H[1][1] + adjointD2H[2][2]) << " >= " << threshold*(-detD2H) << "\n";
//		p_i_p->s = .5;
//		return true; //should return false
//	}


	if(true || (adjointD2H[0][0] + adjointD2H[1][1] + adjointD2H[2][2]) < threshold*(-detD2H)){

		//Calculate estimated the derivatives
		const discern_type dx  = calc_function_weighted<d_x>(b);
		const discern_type dy  = calc_function_weighted<d_y>(b);
		const discern_type ds  = calc_function_weighted<d_s>(b);

		if(DEBUG_MESSAGES)
			cerr<<"{ " << p_i_p->pos.x << " , " << p_i_p->pos.y << " , " << p_i_p->scale << " }";


		const interest_point_position::position_type diff_x = ( adjointD2H[0][0]*dx + adjointD2H[0][1]*dy + adjointD2H[0][2]*ds ) / detD2H;
		const interest_point_position::position_type diff_y = ( adjointD2H[1][0]*dx + adjointD2H[1][1]*dy + adjointD2H[1][2]*ds ) / detD2H;
		const interest_point_position::position_type diff_s = ( adjointD2H[2][0]*dx + adjointD2H[2][1]*dy + adjointD2H[2][2]*ds ) / detD2H;

		if(abs(diff_x) < maxdistance && abs(diff_y) < maxdistance && abs(diff_s) < maxdistance){
			if(skip_interpolation)
				return true;

			p_i_p->pos.x -= diff_x * calc_step;
			p_i_p->pos.y -= diff_y * calc_step;
			p_i_p->scale -= diff_s * size_step;
			if(DEBUG_MESSAGES)
				cerr<<" => { " << p_i_p->pos.x << " , " << p_i_p->pos.y << " , " << p_i_p->scale << " } " << detD2H <<" \n";
		}else{
			if(DEBUG_MESSAGES)
				cerr<<" unstable { " << diff_x << " , " << diff_y << " , " << diff_s << " } " << detD2H <<" \n";
			return false;
		}
	}else{
		if(DEBUG_MESSAGES)
			cerr<< " Error: D2H not invertible: detD2H = " << detD2H << "\n";
		return false;
	}
	return true;
}

}

