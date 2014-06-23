/*
 * filter_definitions.h
 *
 *  Created on: 5 jun 2013
 *      Author: johan
 */

#ifndef FILTER_DEFINITIONS_H_
#define FILTER_DEFINITIONS_H_

#include <array>
#include "../image_signal/box_filtering_type_decl.h"

namespace CascadeCVSTemplate{

using _basic_image_signal::box_filter;
using _basic_image_signal::rotated_box_filter;
using _basic_image_signal::delta_box_filter;
using _basic_image_signal::down_sample;
using _basic_image_signal::down_shift;
using _basic_image_signal::is_downsample;

template <size_t value, size_t... sum_vals>
struct sum_struct{
	static constexpr size_t sum = value + sum_struct<sum_vals...>::sum;
};

template <size_t value>
struct sum_struct<value>{
	static constexpr size_t sum = value;
};


template < typename level, typename... Levels >
struct print_pack{
	static void print( std::ostream& os ){
		level::print( os );
		print_pack<Levels...>::print( os );
	}
};
template < typename level >
struct print_pack<level>{
	static void print( std::ostream& os ){
		level::print( os );
	}
};


namespace decimal_float_help{
typedef int64_t int_type;
typedef int32_t exp_type;

typedef long double real_type;
typedef long double unsigned_pow10_type;
template <std::make_unsigned<exp_type>::type power>
struct _unsigned_pow10{
	static constexpr unsigned_pow10_type value = 10 * _unsigned_pow10<power-1>::value;
};
template <>
struct _unsigned_pow10<0>{
	static constexpr unsigned_pow10_type value = 1;
};
template <exp_type power>
struct _pow10{
	static constexpr std::make_unsigned<exp_type>::type abs_power = power >= 0 ? power:-power;
	static constexpr real_type value_relative_size =  _unsigned_pow10< abs_power >::value ;
	static constexpr real_type value = power >= 0 ?value_relative_size : 1/value_relative_size;
};


inline real_type unsigned_pow10( std::make_unsigned<exp_type>::type power ){
	real_type value = 1;
	while( power-- > 0 )
		value *= 10;
	return value;
};
inline real_type pow10( exp_type power ){
	const std::make_unsigned<exp_type>::type abs_power = power >= 0 ? power:-power;
	const real_type value_relative_size =  unsigned_pow10( abs_power ) ;
	return power >= 0 ? value_relative_size : 1/value_relative_size;
};

struct decimal_float{
	int_type value;
	exp_type exponent;
};

inline std::ostream& operator<<( std::ostream& os, const decimal_float& dfv ){
	return ( os << ( dfv.value * pow10(dfv.exponent) ) );
}

template< int64_t value_part, int32_t exponent>
struct decimal_float_value{
	static constexpr double value = value_part * _pow10< exponent >::value;
};


};


template <
typename sigma,
typename norm_factor,
typename... Filters >
struct level_extract{
//	typedef decltype(sigma) sigma_type;
//	typedef decltype(norm_factor) norm_factor_type;

	static void print( std::ostream& os ){
		print_pack<Filters...>::print( os );
		os << "L| "<< sigma::value << ", " << norm_factor::value << "| \n";
	}
	static constexpr size_t count_downsamples(){
		return sum_struct< is_downsample<Filters>::count...>::sum;
	}
};



template < typename... Levels >
struct filter_config{
	static constexpr size_t count_downsamples(){
		return sum_struct< Levels::count_downsamples()...>::sum;
	}

	static constexpr size_t nof_levels = sizeof...(Levels);
	static constexpr size_t nof_downsamples = count_downsamples();
	static void print(std::ostream& os) {
		print_pack<Levels...>::print( os );
	}

};

}

#endif /* FILTER_DEFINITIONS_H_ */
