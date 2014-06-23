/*
 * CVSSelectImpl.h
 *
 *  Created on: 15 sep 2013
 *      Author: johan
 */

#ifndef CASCADECVSSYSTEMSELECTIMPL_H_
#define CASCADECVSSYSTEMSELECTIMPL_H_

//#include "CVSystem.h"
#include "debug/test_functions.h"
#include "../CascadeImageMatchingConfig.h"
#include "../CascadeCVSEvaluationInterface.h"
#include "../CascadeCVSDrawing.h"
#include "CVSTemplateUtilities.h"
#include "CascadeDataStructures.h"
#include "CVSystem.h"
//#include "CVSScalePyramid.h"

namespace CascadeCVSTemplate{


template <typename value_type, uint8_t neighborhood_side = 3>
class value_storage{
public:
	static constexpr int region_size = mptl::square( neighborhood_side );
	static constexpr int offset = ( (region_size + 1) >> 1 );
private:
	value_type* const array;
	value_type* current_point_data;
	value_storage( const value_storage& ) = delete;
	value_storage& operator= ( const value_storage& ) = delete;
public:
	value_storage( const size_t array_size ):
		array( new value_type[array_size + region_size] ),
		current_point_data(  array +  offset )
	{

	}
	~value_storage(){
		if(array){
			delete[] array;
			//array = nullptr;
		}
	}
	const value_type& current() const { return current_point_data[0]; }
	value_type& current(){ return current_point_data[0]; }
	const value_type& next() const { return current_point_data[1]; }
	value_type& next(){ return current_point_data[1]; }
	void to_next(){
		current_point_data++;
		if(current_point_data-array > 1000)
			assert(current_point_data-array < 1000);
	}
	int current_index() const {
		return current_point_data - ( array + offset );
	}
	void set_current_index( int index ) {
		current_point_data = array + offset + index;
		if(current_point_data-array > 1000)
			assert(current_point_data-array < 1000);
	}

	value_type* point_data(){ return current_point_data; }
	const value_type* point_data() const { return current_point_data; }

	template <int y, int x>
	value_type& get(){
		return current_point_data[ neighborhood_side*y + x ];
	}
	template <int y, int x>
	const value_type& get() const {
		return current_point_data[ neighborhood_side*y + x ];
	}
};
//template <typename value_type, uint8_t log_extra_length = 2>
//class value_storage{
//public:
//	static constexpr int extra_length = ( 1 << log_extra_length ) - 1;
//	static constexpr int neighborhood_side = 3;
//	static constexpr int region_size = mptl::square( neighborhood_side );
//private:
//	value_type array[ region_size + extra_length ];
//	int current_point_data;
//	int next_point_data;
//public:
//	value_storage():
//		current_point_data(  ( region_size >> 1 ) ),
//		next_point_data( ( region_size >> 1 ) + 1 )
//	{
//
//	}
////	value_type& operator()(int index){ return array[index]; }
////	const value_type& operator()(int index) const { return array[index]; }
////	value_type& operator[](int index){ return array[index]; }
////	const value_type& operator[](uint index) const { return array[index]; }
//	value_type& next(){ return array[next_point_data]; }
//	value_type& current(){ return array[current_point_data]; }
//	void to_next(){
//		current_point_data = next_point_data;
//		next_point_data++;
//		next_point_data &= extra_length;
//	}
//
//	value_type* point_data(){ return array + current_point_data; }
//	const value_type* point_data() const { return array + current_point_data; }
//
//	template <int y, int x>
//	value_type& get(){
//		return array[ current_point_data + neighborhood_side*y + x + ( current_point_data + neighborhood_side*y + x < 0 ? region_size + extra_length : 0 ) ];
//	}
//	template <int y, int x>
//	const value_type& get() const {
//		return array[ current_point_data + neighborhood_side*y + x + ( current_point_data + neighborhood_side*y + x < 0 ? region_size + extra_length : 0 )];
//	}
//};

enum class calc_inplane_opts {  U_ip, d_x_ip, d_y_ip, d_xx_ip, d_yy_ip, d_xy_ip };
template< calc_inplane_opts c > inline discern_type calc_inplane_function(const discern_type* const b);
template< calc_inplane_opts c > inline discern_type calc_function_weighted_2d( const discern_type* const values );

template< calc_inplane_opts c >
inline discern_type calc_function_weighted_2d( const discern_type* const values ){
	static constexpr discern_type normalizing_factors[6] = { 1, 8, 8, 6, 6, 4 };

	static constexpr int function_coefficients[6][9] = {
			{ 0, 0, 0, 0, 1, 0, 0, 0, 0},
			{-1, 0, 1,-2, 0, 2,-1, 0, 1},
			{-1,-2,-1, 0, 0, 0, 1, 2, 1},
			{ 1,-2, 1, 4,-8, 4, 1,-2, 1},
			{ 1, 4, 1,-2,-8,-2, 1, 4, 1},
			{ 1, 0,-1, 0, 0, 0,-1, 0, 1}};

	discern_type sum = 0;
	for( int i = 0; i < 9; i++ )
		sum += function_coefficients[static_cast<int>(c)][i] * values[i];
	return sum / normalizing_factors[static_cast<int>(c)];
}

inline bool interpolate_position_2d(const discern_type* const values, interest_point_position* p_i_p, const discern_type calc_step, const discern_type size_step );

inline bool interpolate_position_2d(const value_storage<discern_type>& value_storage, interest_point_position* p_i_p, const discern_type calc_step, const discern_type size_step ){
	return interpolate_position_2d( value_storage.point_data(), p_i_p, calc_step, size_step );
}

inline bool interpolate_position_2d(const discern_type* const values, interest_point_position* p_i_p, const discern_type calc_step, const discern_type size_step ){
	static const bool DEBUG_MESSAGES = false;
	static const double inverse_lower_eigenvalue_bound = .5;
	static const double threshold = 3*inverse_lower_eigenvalue_bound;
	static bool skip_interpolation = false;
	static const float maxdistance = 2;

	/* Calculate estimated second order derivatives and test for definiteness
	 * Since the interest point is a maximum, D2H should be negative-definite, product of 3 negative eigenvalues is negative
	 */
	const discern_type dxx = calc_function_weighted_2d<calc_inplane_opts::d_xx_ip>(values);
	if( dxx >= 0 ){ //Test Definiteness
		return false;
	}
	const discern_type dyy = calc_function_weighted_2d<calc_inplane_opts::d_yy_ip>(values);
	if( dyy >= 0 ){ //Test Definiteness
		return false;
	}

	// Calculate determinant of Hessian matrix
	const discern_type dxy = calc_function_weighted_2d<calc_inplane_opts::d_xy_ip>(values);
	const discern_type detD2H = dxx * dyy - mptl::square(dxy);
	if( detD2H <= 0 ){ // Test definiteness again, this time based on the last leading principal minor, the determinant of the matrix itself
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




	const discern_type adjointD2H[2][2] = {
			{  dyy, -dxy },
			{ -dxy,  dxx }};

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


	if( true || (adjointD2H[0][0] + adjointD2H[1][1] ) < threshold*(-detD2H)){

		//Calculate estimated the derivatives
		const discern_type dx  = calc_function_weighted_2d<calc_inplane_opts::d_x_ip>(values);
		const discern_type dy  = calc_function_weighted_2d<calc_inplane_opts::d_y_ip>(values);

		if(DEBUG_MESSAGES)
			cerr<<"{ " << p_i_p->pos.x << " , " << p_i_p->pos.y << " , " << p_i_p->scale << " }";


		const interest_point_position::position_type diff_x = ( adjointD2H[0][0]*dx + adjointD2H[0][1]*dy ) / detD2H;
		const interest_point_position::position_type diff_y = ( adjointD2H[1][0]*dx + adjointD2H[1][1]*dy ) / detD2H;

		if(abs(diff_x) < maxdistance && abs(diff_y) < maxdistance ){
			if(skip_interpolation)
				return true;

			p_i_p->pos.x -= diff_x * calc_step;
			p_i_p->pos.y -= diff_y * calc_step;
			if(DEBUG_MESSAGES)
				cerr<<" => { " << p_i_p->pos.x << " , " << p_i_p->pos.y << " , " << p_i_p->scale << " } " << detD2H <<" \n";
		}else{
			if(DEBUG_MESSAGES)
				cerr<<" unstable { " << diff_x << " , " << diff_y <<  " } " << detD2H <<" \n";
			return false;
		}
	}else{
		if(DEBUG_MESSAGES)
			cerr<< " Error: D2H not invertible: detD2H = " << detD2H << "\n";
		return false;
	}
	return true;
}



void select_interest_points_state_machine_impl(
		const image_signal<pyramid_data_type>& discern_image,
		internal_interest_point_position_set& intrest_point_positions,
		const float sigma,
		discern_type threshold,
		CascadeCVSEvaluationInterface* eval_object );
void select_interest_points_impl(
		const image_signal<pyramid_data_type>& discern_image,
		internal_interest_point_position_set& intrest_point_positions,
		const float sigma,
		discern_type threshold,
		CascadeCVSEvaluationInterface* eval_object );

template < typename cvs_scale_pyramid, size_t level_no, typename current_level, typename... more_levels  >
struct select_recursion;

template < typename cvs_scale_pyramid, size_t level_no, template <typename...> class current_level, typename sigma, typename norm_factor, typename... filters, typename... more_levels  >
struct select_recursion<cvs_scale_pyramid, level_no, current_level<sigma,norm_factor,filters...>,more_levels...>{
	typedef select_recursion<cvs_scale_pyramid, level_no+1, more_levels...> next_select_recursion;
	static void select_interest_points(
			const cvs_scale_pyramid& scale_pyramid,
			image_signal<discern_type>& discern_image,
			internal_interest_point_position_set& interest_point_positions,
			discern_type cap,
			CascadeCVSEvaluationInterface* eval_object
	){
		constexpr auto norm = 1 / mptl::quad( sigma::value );
		calculate_response_image( scale_pyramid.maps[level_no], discern_image);
		select_interest_points_impl(
				discern_image,
				interest_point_positions,
				sigma::value,
				cap * mptl::quad<double>( scale_pyramid.maps[level_no].sample_factor() ) * norm,
				eval_object );
//		debug_test_state_machine_approach(scale_pyramid, level_no, sigma::value, cap, norm, eval_object);
		next_select_recursion::select_interest_points(
				scale_pyramid,
				discern_image,
				interest_point_positions,
				cap,
				eval_object );
//		cout << "Level " << level_no << ", Sigma: " << sigma::value << ", threshold: "<< (cap * mptl::quad<double>( scale_pyramid.maps[level_no].sample_factor() ) * norm) <<"\n";
	}
};

template < typename cvs_scale_pyramid, size_t level_no, template <typename...> class current_level, typename sigma, typename norm_factor, typename... filters>
struct select_recursion<cvs_scale_pyramid, level_no, current_level<sigma,norm_factor,filters...>>{
	static void select_interest_points(
			const cvs_scale_pyramid& scale_pyramid,
			image_signal<discern_type>& discern_image,
			internal_interest_point_position_set& interest_point_positions,
			discern_type cap,
			CascadeCVSEvaluationInterface* eval_object
	){
		constexpr auto norm = 1 / mptl::quad( sigma::value );
		calculate_response_image( scale_pyramid.maps[level_no], discern_image);
		select_interest_points_impl(
				discern_image,
				interest_point_positions,
				sigma::value,
				cap * mptl::quad<double>( scale_pyramid.maps[level_no].sample_factor() ) * norm,
				eval_object );
//		debug_test_state_machine_approach(scale_pyramid, level_no, sigma::value, cap, norm, eval_object);
//		cout << "Level " << level_no << ", Sigma: " << sigma::value << ", threshold: "<< (cap * mptl::quad<double>( scale_pyramid.maps[level_no].sample_factor() ) * norm) <<"\n";
	}
};

template < typename CVSystemConfig >
struct select_config_unpack_struct;

template < template <typename...>  class CVSystemConfig, typename... levels >
struct select_config_unpack_struct< CVSystemConfig<levels...> >{
	typedef typename CascadedCVSystem<CVSystemConfig<levels...>>::cvs_scale_pyramid cvs_scale_pyramid;
	static void select_pyramid_interest_points(
			const cvs_scale_pyramid& scale_pyramid,
			internal_interest_point_position_set& interest_point_positions,
			discern_type cap,
			CascadeCVSEvaluationInterface* eval_object
	){
		simple_image_signal<discern_type> discern_image(scale_pyramid.width, scale_pyramid.height);
		select_recursion<cvs_scale_pyramid, 0, levels...>::select_interest_points(
				scale_pyramid,
				discern_image,
				interest_point_positions,
				cap,
				eval_object );
	}
};




template <typename CVSystemConfig>
void CascadedCVSystem<CVSystemConfig>::SelectInterestPoints(
		const cvs_scale_pyramid& scale_pyramid,
		internal_interest_point_position_set& intrest_point_positions,
		int cap,
		CascadeCVSEvaluationInterface* eval_object
){
	_basic_image_signal::Timer tcv;
	tcv.mark();
	select_config_unpack_struct<CVSystemConfig>::select_pyramid_interest_points(
			scale_pyramid,
			intrest_point_positions,
			cap,
			eval_object );
	tcv.mark();
	std::cerr << "Select interest points took " << tcv.get_time() << " s\n";
}

//struct evaluation_parameters{
////	discern_type pos_sum;
////	int nof_pos;
//	evaluation_parameters():p_count(0){
//		for(int i =0;i<9;i++){
//			passed[i]=0;
//		}
//	}
//	int p_count;
//	int passed[9];
//};



template<typename T>
inline discern_type getResponse( const const_image_signal_point<T>& image_point ){
	return ( image_point(0,0) );
}

inline void skip_level_part (
		const const_image_signal_point<discern_type>& cur_line,
		int& next_m,
		const int stop_x,
		value_storage<discern_type>& values
) {
	do{
		values.to_next();
		values.next() = getResponse( cur_line.shifted( 0, next_m ) );
	} while( next_m++ < stop_x + 1 && values.current() == values.next() );
}


enum class extrema{ maximum, minimum };

template < extrema extrema_type >
struct find_extrema_config;

template <>
struct find_extrema_config<extrema::maximum>{
	static bool is_extrema( const discern_type& current, const discern_type& next ){ return current > next; }
	static bool is_strict_trend( const discern_type& current, const discern_type& next ){ return current < next; }
	static void save_extrema(
			internal_interest_point_position_set& intrest_point_positions,
			const interest_point_position& position
	){
		intrest_point_positions.maximum_positions.push_back( position );
	}
	static void save_rejected_extrema(
			internal_interest_point_position_set& intrest_point_positions,
			const interest_point_position& position
	){
		intrest_point_positions.rejected_maximum_positions.push_back( position );
	}
};
template <>
struct find_extrema_config<extrema::minimum>{
	static bool is_extrema( const discern_type& current, const discern_type& next ){ return current < next; }
	static bool is_strict_trend( const discern_type& current, const discern_type& next ){ return current > next; }
	static void save_extrema(
			internal_interest_point_position_set& intrest_point_positions,
			const interest_point_position& position
	){
		intrest_point_positions.minimum_positions.push_back( position );
	}
	static void save_rejected_extrema(
			internal_interest_point_position_set& intrest_point_positions,
			const interest_point_position& position
	){
		intrest_point_positions.rejected_minimum_positions.push_back( position );
	}
};




//template < extrema extrema_type >
//inline void find_line_extrema (
//		const const_image_signal_point<pyramid_data_type>& cur_line,
//		int& next_m,
//		const int stop_x,
//		const discern_type& level_threshold,
//		value_storage<discern_type>& values
//) {
//	typedef find_extrema_config<extrema_type> config;
//
//	do{
//		values.to_next();
//		values.next() = getResponse( cur_line.shifted( 0, next_m ) );
//	} while( next_m++ < stop_x + 1 && config::is_strict_trend( values.current() , values.next() ) );
//
//}

template < extrema extrema_type >
static inline void find_line_extrema (
		const const_image_signal_point<discern_type>& cur_line,
		int& next_m,
		const int stop_x,
		discern_type& a,
		discern_type& b
) {
	typedef find_extrema_config<extrema_type> config;
	switch( ( stop_x - next_m ) & 0x1 ){
	case 1: do{
		b = getResponse( cur_line.shifted( 0, ++next_m ) );
		if( !config::is_strict_trend( a, b ))
			break;
	case 0:
		a = getResponse( cur_line.shifted( 0, ++next_m ) );
		if( !config::is_strict_trend( b, a ))
			break;
	}while(next_m < stop_x);
	}
}

template <extrema extrema_type, int offset_y, int offset_x >
static inline bool test_extrema(
		const const_image_signal_point<discern_type>& cur_p,
		value_storage<discern_type>& packed_values
){
	typedef find_extrema_config<extrema_type> config;
	auto&& c_value = packed_values.current();

	const discern_type comp =  getResponse( cur_p.shifted(offset_y,offset_x) );
	if( !config::is_extrema( c_value , comp ) )
		return false;
	packed_values.get<offset_y,offset_x>() = comp;
	return true;

}


template <extrema extrema_type>
static inline bool test_line_extrema(
		const const_image_signal_point<discern_type>& cur_p,
		value_storage<discern_type>& values
){
	if(	!test_extrema<extrema_type, 1, 0>( cur_p, values) )
		return false;
	if(	!test_extrema<extrema_type,-1, 0>( cur_p, values) )
		return false;
			if(		!test_extrema<extrema_type, 1,-1>( cur_p, values) ||
			!test_extrema<extrema_type,-1, 1>( cur_p, values) ||
			!test_extrema<extrema_type, 1, 1>( cur_p, values) ||
			!test_extrema<extrema_type,-1,-1>( cur_p, values)
	)
		return false;
	return true;
}


template < extrema extrema_type, bool DEBUG_DRAW_REJECTED, bool SKIP_INTERPOLATION >
static void save_extrema(
		const image_signal<discern_type>& cur_im,
		const int n,
		const int next_m,
		value_storage<discern_type>& values,
		internal_interest_point_position_set& intrest_point_positions,
		const float sigma,
		CascadeCVSEvaluationInterface* eval_object = nullptr
){
	typedef find_extrema_config<extrema_type> config;
	auto offset = cur_im.offset();
	auto step_size = cur_im.sample_factor();
	interest_point_position interest_point = {
			interest_point.pos = {
					static_cast<interest_point_position::position_type>( offset.x + values.current_index() * step_size ),
					static_cast<interest_point_position::position_type>( offset.y + n * step_size )
			},
			interest_point.scale = sigma,
			interest_point.orientation = 0.0f,
			interest_point.magnitude = values.current() };

	if(SKIP_INTERPOLATION){
		config::save_extrema( intrest_point_positions, interest_point );
	}else{
		if( interpolate_position_2d( values, &interest_point, step_size, step_size*1.2f/3))
			config::save_extrema( intrest_point_positions, interest_point );
		else{
			config::save_rejected_extrema( intrest_point_positions, interest_point );
			if( DEBUG_DRAW_REJECTED && eval_object ){
				image_signal<BGR8>* debugIm = eval_object->get_image( );
				if(debugIm)
					drawInterest( *debugIm, interest_point,BGR8(255,0,0) );
			}
		}
	}
}

template < extrema extrema_type, bool DEBUG_DRAW_REJECTED, bool SKIP_INTERPOLATION >
static inline bool find_extrema(
		const image_signal<discern_type>& cur_im,
		const const_image_signal_point<discern_type>& cur_line,
		const int& n,
		int& next_m,
		const int stop_x,
		value_storage<discern_type>& values,
		discern_type& a,
		discern_type& b,
		const discern_type& level_threshold,
		internal_interest_point_position_set& intrest_point_positions,
		const float sigma,
		CascadeCVSEvaluationInterface* eval_object = nullptr
){
	typedef find_extrema_config<extrema_type> config;

	find_line_extrema< extrema_type >(
			cur_line,
			next_m,
			stop_x,
			a,
			b );


	if( next_m >= stop_x )
		return true;

	values.set_current_index( next_m - 1 );
	switch( ( stop_x - next_m ) & 0x1 ){
	case 0:
		values.current() = a;
		values.next() = b;
		break;
	case 1:
		values.current() = b;
		values.next() = a;
		break;
	}

	if(
			!config::is_extrema( values.current(), level_threshold) ||
			!config::is_extrema( values.current(), values.next())
	)
		return false;
	const bool is_extrema = test_line_extrema< extrema_type >(
			cur_line.shifted(0, values.current_index()),
			values
	);

//	discern_type laplace = 0;
//	const discern_type c_p = getResponse_with_laplacian( cur_line.shifted(0, values.current_index()), &laplace );
//	if(laplace*laplace > 9.f*c_p)
//		return false;

	if( is_extrema ){
		save_extrema< extrema_type, DEBUG_DRAW_REJECTED, SKIP_INTERPOLATION >(
				cur_im,
				n,
				next_m,
				values,
				intrest_point_positions,
				sigma,
				eval_object );
	}
	return false;
}


void select_interest_points_state_machine_impl(
		const image_signal<discern_type>& cur_im,
		internal_interest_point_position_set& intrest_point_positions,
		const float sigma,
		discern_type threshold,
		CascadeCVSEvaluationInterface* eval_object = nullptr
);
void select_interest_points_impl(
		const image_signal<discern_type>& cur_im,
		internal_interest_point_position_set& intrest_point_positions,
		const float sigma,
		discern_type threshold,
		CascadeCVSEvaluationInterface* eval_object = nullptr
);

}
#endif /* CASCADECVSSYSTEMSELECTIMPL_H_ */
