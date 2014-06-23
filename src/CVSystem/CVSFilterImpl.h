/*
 * CVSFilterImpl.h
 *
 *  Created on: 22 aug 2013
 *      Author: johan
 */

#ifndef CASCADECVSSYSTEMFILTERIMPL_H_
#define CASCADECVSSYSTEMFILTERIMPL_H_

#include "filter_definitions.h"
#include "filter_impl.h"
#include "../image_signal/simple_image_signal_utility.h"


namespace CascadeCVSTemplate{
typedef filtering_data_type gain_type;

using _basic_image_signal::log2ceil;

inline uint ceil_log2(uint x){
	int count = 0;
	while(x >>= 1) ++count;
	return count+1;
}

template < gain_type initial_gain, typename filter, typename... rest >
struct level_filter_recursion_struct{
	typedef apply_filter<filter, filtering_data_type, initial_gain> filter_struct;
	static constexpr gain_type post_filter_gain = filter_struct::post_filter_gain;
	typedef level_filter_recursion_struct< post_filter_gain, rest... > next_struct;
	static constexpr gain_type post_level_gain = next_struct::post_level_gain;

	static void apply_filters(
			image_signal< filtering_data_type >& calc_image,
			image_signal< pyramid_data_type >& level_image
	){
//		cout 	<< "log2ceil( " << std::hex << std::setw(8) << std::right << initial_gain << " ) = "
//				<< log2ceil< decltype(initial_gain), initial_gain>::value << " "
//				<< log2ceil< decltype(initial_gain), initial_gain+1>::value << " "
//				<< ceil_log2(initial_gain) << "\n";
		filter_struct::filter(calc_image);
		next_struct::apply_filters(calc_image, level_image);
	}
};

template < gain_type initial_gain, typename filter >
struct level_filter_recursion_struct<initial_gain, filter>{
	typedef apply_filter<filter, filtering_data_type, initial_gain> filter_struct;
	static constexpr gain_type post_filter_gain = filter_struct::post_filter_gain;
	static constexpr gain_type post_level_gain = post_filter_gain;
	static void apply_filters(
			image_signal< filtering_data_type >& calc_image,
			image_signal< pyramid_data_type >& level_image
	){
//		cout 	<< "log2ceil( " << std::hex << std::setw(8) << std::right << initial_gain << " ) = "
//				<< log2ceil< decltype(initial_gain), initial_gain>::value << " "
//				<< log2ceil< decltype(initial_gain), initial_gain+1>::value << " "
//				<< ceil_log2(initial_gain) << "\n";
		filter_struct::filter( calc_image, level_image );
	}
};

template < typename CVSystemConfig, gain_type initial_gain, size_t level_no, typename... rest>
struct recursive_calc;

template < typename CVSystemConfig, gain_type initial_gain, size_t level_no, typename sigma, typename norm_factor, typename... level_filters, typename... rest>
struct recursive_calc<CVSystemConfig, initial_gain, level_no, level_extract<sigma,norm_factor,level_filters...>, rest...>{
	typedef level_filter_recursion_struct<initial_gain, level_filters...> level_filtering_struct;
	typedef recursive_calc< CVSystemConfig, level_filtering_struct::post_level_gain, level_no+1, rest...> next_level_struct;
	static void filter_layer(
			image_signal< filtering_data_type >& calc_image,
			typename CascadedCVSystem<CVSystemConfig>::cvs_scale_pyramid& result
	){
		level_filtering_struct::apply_filters( calc_image, result.maps[level_no] );
		_basic_image_signal::Timer tcv;
		tcv.mark();
//		_basic_image_signal::copy_scale_image( calc_image, result.maps[level_no], 1/static_cast<double>(level_filtering_struct::post_level_gain) );
		_basic_image_signal::copy_image( calc_image, result.maps[level_no] );
		tcv.mark();
		std::cerr << "copy_scale_image took " << tcv.get_time()<< " s\n";
		result.info[level_no].sigma = sigma::value;
		result.info[level_no].total_gain = level_filtering_struct::post_level_gain;
		next_level_struct::filter_layer( calc_image, result );
	}
};
template < typename CVSystemConfig, gain_type initial_gain, size_t level_no>
struct recursive_calc<CVSystemConfig, initial_gain, level_no>{
	static void filter_layer(
			image_signal< filtering_data_type >& calc_image,
			typename CascadedCVSystem<CVSystemConfig>::cvs_scale_pyramid& result
	){}
};

template < typename source_type, gain_type initial_gain, typename CVSystemConfig, typename... levels >
struct first_level_struct{
	typedef recursive_calc<CVSystemConfig,initial_gain,0,levels...> recursive_eval;
	static void initialize_create_pyramid(
			const image_signal<source_type>& image,
			image_signal<filtering_data_type>& integral_image,
			typename CascadedCVSystem<CVSystemConfig>::cvs_scale_pyramid& result
	){
		recursive_eval::filter_layer(integral_image, result);
	}
};


template < typename CVSystemConfig, typename source_type >
struct config_unpack_struct;

template < template <typename...>  class CVSystemConfig, typename... levels, typename source_type >
struct config_unpack_struct< CVSystemConfig<levels...>, source_type >{
	typedef first_level_struct<source_type,1,CVSystemConfig<levels...>,levels...> level_struct;
	static void initialize_create_pyramid(
			const image_signal<source_type>& image,
			image_signal<filtering_data_type>& integral_image,
			typename CascadedCVSystem<CVSystemConfig<levels...> >::cvs_scale_pyramid& result
	){
		level_struct::initialize_create_pyramid( image, integral_image, result );
	}
};

template <typename CVSystemConfig>
template <typename source_type>
void CascadedCVSystem<CVSystemConfig>::CreateDiscernMap(
		const image_signal<source_type>& image,
		image_signal<filtering_data_type>& integral_image,
		cvs_scale_pyramid& result
){
	std::cerr << image.definition_area() << "\n";
	_basic_image_signal::Timer tcv;
	tcv.mark();
	_basic_image_signal::copy_image( image, integral_image );
	config_unpack_struct<CVSystemConfig,source_type>::initialize_create_pyramid( image, integral_image, result );
	tcv.mark();
	std::cerr << "CreateDiscernMap took " << tcv.get_time()<< " s\n";
}




}

#endif /* CASCADECVSSYSTEMFILTERIMPL_H_ */
