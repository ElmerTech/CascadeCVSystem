/*
 * CompileConfig.h
 *
 *  Created on: 3 jun 2013
 *      Author: johan
 */

#ifndef COMPILECONFIG_H_
#define COMPILECONFIG_H_

#include "filter_definitions.h"

namespace compile_cascade_config{
using namespace CascadeCVSTemplate;


template < size_t pos, char chr, typename... Args >
struct parse_error{};
template <size_t pos, char chr >
struct parse_error_2{};
template <size_t pos, char chr >
struct parse_error_3{};
template <size_t pos, char chr, long value >
struct parse_error_4{};

/*****
 * Utility for skipping white spaces
 */
template < const char* conf_str, size_t pos, char chr >
struct parse_consume_whitespace{
	static constexpr size_t stop_pos = pos;
};
template < const char* conf_str, size_t pos >
struct parse_consume_whitespace<conf_str, pos, ' ' >{
	typedef parse_consume_whitespace<conf_str, pos+1, conf_str[pos]> parse_child;
	static constexpr size_t stop_pos = parse_child::stop_pos;
};
template < const char* conf_str, size_t pos >
struct parse_consume_whitespace<conf_str, pos, '\t' >{
	typedef parse_consume_whitespace<conf_str, pos+1, conf_str[pos]> parse_child;
	static constexpr size_t stop_pos = parse_child::stop_pos;
};

/*****
 * Utility struct for parsing numbers
 */
namespace _parse_uint{
typedef long long value_type;
constexpr int decimal_digit( char chr ){ return chr - '0'; }
constexpr bool is_single_decimal_digit( int value ){ return value >= 0 && value <= 9; }
constexpr bool is_decimal_digit( char chr ){ return is_single_decimal_digit( decimal_digit( chr ) );}

enum class parse_uint_state{
	initial_state,
	reading_numbers
};

template < parse_uint_state state, const char* conf_str, size_t pos, char chr, value_type parent_value>
struct parse_uint{};


template < const char* conf_str, size_t pos, value_type parent_value >
struct parse_uint<parse_uint_state::initial_state, conf_str, pos,' ', parent_value>{
	typedef parse_uint<parse_uint_state::initial_state, conf_str, pos+1, conf_str[pos],0> parse_child;
	static constexpr value_type value = parse_child::value;
	static constexpr value_type chars_read = parse_child::chars_read + 1;
};
template < const char* conf_str, size_t pos, value_type parent_value >
struct parse_uint<parse_uint_state::initial_state, conf_str, pos,'\t', parent_value>{
	typedef parse_uint<parse_uint_state::initial_state, conf_str, pos+1, conf_str[pos],0> parse_child;
	static constexpr value_type value = parse_child::value;
	static constexpr value_type chars_read = parse_child::chars_read + 1;
};

#define PARSE_UINT_DIGIT_MACRO_TEMPLATE( digit ) \
	template < parse_uint_state state, const char* conf_str, size_t pos, value_type parent_value>\
	struct parse_uint< state, conf_str, pos, digit+'0' , parent_value>{\
	typedef parse_uint< parse_uint_state::reading_numbers, conf_str, pos+1, conf_str[pos],10*parent_value + (digit)> parse_child;\
	static constexpr value_type value      = parse_child::value;\
	static constexpr value_type chars_read = parse_child::chars_read + 1;\
};
PARSE_UINT_DIGIT_MACRO_TEMPLATE( 0 )
PARSE_UINT_DIGIT_MACRO_TEMPLATE( 1 )
PARSE_UINT_DIGIT_MACRO_TEMPLATE( 2 )
PARSE_UINT_DIGIT_MACRO_TEMPLATE( 3 )
PARSE_UINT_DIGIT_MACRO_TEMPLATE( 4 )
PARSE_UINT_DIGIT_MACRO_TEMPLATE( 5 )
PARSE_UINT_DIGIT_MACRO_TEMPLATE( 6 )
PARSE_UINT_DIGIT_MACRO_TEMPLATE( 7 )
PARSE_UINT_DIGIT_MACRO_TEMPLATE( 8 )
PARSE_UINT_DIGIT_MACRO_TEMPLATE( 9 )
#undef PARSE_UINT_DIGIT_MACRO_TEMPLATE

template < const char* conf_str, size_t pos, char chr, value_type parent_value>
struct parse_uint< parse_uint_state::reading_numbers, conf_str, pos, chr, parent_value>{
	static constexpr value_type value      = parent_value;
	static constexpr value_type chars_read = 0;
};

}
template < const char* conf_str, size_t pos, char chr>
struct parse_uint{
	typedef typename _parse_uint::parse_uint<_parse_uint::parse_uint_state::initial_state, conf_str, pos, chr, 0> parse_child;
	static constexpr _parse_uint::value_type value = parse_child::value;
	static constexpr _parse_uint::value_type chars_read = parse_child::chars_read;
};



namespace _parse_real{
typedef typename decimal_float_help::int_type value_type;
typedef typename decimal_float_help::exp_type exp_type;

template < const char* conf_str, size_t pos, char chr, value_type parent_value>
struct parse_uint{
	static constexpr value_type value = parent_value;
	static constexpr size_t stop_pos = pos;
};

#define PARSE_UINT_DIGIT_MACRO_TEMPLATE( digit ) \
	template < const char* conf_str, size_t pos, value_type parent_value>\
	struct parse_uint< conf_str, pos, '0'+(digit) , parent_value>{\
	typedef parse_uint< conf_str, pos+1, conf_str[pos],10*parent_value + (digit)> parse_child;\
	static constexpr value_type value = parse_child::value;\
	static constexpr size_t stop_pos = parse_child::stop_pos;\
};

PARSE_UINT_DIGIT_MACRO_TEMPLATE( 0 )
PARSE_UINT_DIGIT_MACRO_TEMPLATE( 1 )
PARSE_UINT_DIGIT_MACRO_TEMPLATE( 2 )
PARSE_UINT_DIGIT_MACRO_TEMPLATE( 3 )
PARSE_UINT_DIGIT_MACRO_TEMPLATE( 4 )
PARSE_UINT_DIGIT_MACRO_TEMPLATE( 5 )
PARSE_UINT_DIGIT_MACRO_TEMPLATE( 6 )
PARSE_UINT_DIGIT_MACRO_TEMPLATE( 7 )
PARSE_UINT_DIGIT_MACRO_TEMPLATE( 8 )
PARSE_UINT_DIGIT_MACRO_TEMPLATE( 9 )
#undef PARSE_UINT_DIGIT_MACRO_TEMPLATE



enum class parse_real_state{
	initial_state,
	sign_read,
	integer_part_read,
	period_read,
	fraction_part_read,
	init_reading_exponent,
	end_of_read
};

template <
parse_real_state state,
const char* conf_str,
size_t pos, char chr,
value_type parent_value,
int decimals>
struct parse_real{
	typedef parse_error<pos,chr> parse_err;
	typedef typename parse_err::r r;
};

/****
 * Consume white spaces
 */
template < const char* conf_str, size_t pos >
struct parse_real<parse_real_state::initial_state, conf_str, pos,' ', 0, 0 >{
	typedef parse_real<parse_real_state::initial_state, conf_str, pos+1, conf_str[pos],0,0> parse_child;
	static constexpr value_type value = parse_child::value;
	static constexpr exp_type decimal_places = parse_child::decimal_places;
	static constexpr size_t stop_pos = parse_child::stop_pos;
};
template < const char* conf_str, size_t pos >
struct parse_real<parse_real_state::initial_state, conf_str, pos,'\t', 0, 0>{
	typedef parse_real<parse_real_state::initial_state, conf_str, pos+1, conf_str[pos],0,0> parse_child;
	static constexpr value_type value = parse_child::value;
	static constexpr exp_type decimal_places = parse_child::decimal_places;
	static constexpr size_t stop_pos = parse_child::stop_pos;
};
/****
 * If char is not white space start to parse number
 */
template < const char* conf_str, size_t pos>
struct parse_real<parse_real_state::initial_state, conf_str, pos, '-', 0, 0 >{
	typedef parse_real<parse_real_state::sign_read, conf_str, pos+1, conf_str[pos],0,0> parse_child;
	static constexpr value_type value = -parse_child::value;
	static constexpr exp_type decimal_places = parse_child::decimal_places;
	static constexpr size_t stop_pos = parse_child::stop_pos;
};
template < const char* conf_str, size_t pos, char chr >
struct parse_real<parse_real_state::initial_state, conf_str, pos, chr, 0, 0 >{
	typedef parse_real<parse_real_state::sign_read, conf_str, pos, chr, 0, 0 > parse_child;
	static constexpr value_type value = parse_child::value;
	static constexpr exp_type decimal_places = parse_child::decimal_places;
	static constexpr size_t stop_pos = parse_child::stop_pos;
};

/****
 * Read integer part
 */
template < const char* conf_str, size_t pos, char chr >
struct parse_real<parse_real_state::sign_read, conf_str, pos, chr, 0, 0 >{
	typedef parse_uint< conf_str, pos, chr, 0 > parse_integer;
	static constexpr value_type integer_value = parse_integer::value;
	static constexpr size_t new_pos  = parse_integer::stop_pos;
	typedef parse_real< parse_real_state::integer_part_read, conf_str, new_pos, conf_str[new_pos-1], integer_value, 0> parse_child;
	static constexpr value_type value = parse_child::value;
	static constexpr exp_type decimal_places = parse_child::decimal_places;
	static constexpr size_t stop_pos = parse_child::stop_pos;
};

/****
 * After integer part comes either
 * - a fractional part ('.')
 * - an powers of ten part ('e' or 'E')
 * - some other character, end of number
 */
template < const char* conf_str, size_t pos, value_type parent_value >
struct parse_real<parse_real_state::integer_part_read, conf_str, pos,'.', parent_value, 0 >{
	typedef parse_real<parse_real_state::period_read, conf_str, pos+1, conf_str[pos],parent_value,0> parse_child;
	static constexpr value_type value = parse_child::value;
	static constexpr exp_type decimal_places = parse_child::decimal_places;
	static constexpr size_t stop_pos = parse_child::stop_pos;
};
template < const char* conf_str, size_t pos, value_type parent_value >
struct parse_real<parse_real_state::integer_part_read, conf_str, pos, 'e', parent_value, 0 >{
	typedef parse_real<parse_real_state::init_reading_exponent, conf_str, pos+1, conf_str[pos],parent_value, 0 > parse_child;
	static constexpr value_type value = parse_child::value;
	static constexpr exp_type decimal_places = parse_child::decimal_places;
	static constexpr size_t stop_pos = parse_child::stop_pos;
};
template < const char* conf_str, size_t pos, value_type parent_value >
struct parse_real<parse_real_state::integer_part_read, conf_str, pos, 'E', parent_value, 0 >{
	typedef parse_real<parse_real_state::init_reading_exponent, conf_str, pos+1, conf_str[pos],parent_value, 0 > parse_child;
	static constexpr value_type value = parse_child::value;
	static constexpr exp_type decimal_places = parse_child::decimal_places;
	static constexpr size_t stop_pos = parse_child::stop_pos;
};
template < const char* conf_str, size_t pos, char chr, value_type parent_value >
struct parse_real< parse_real_state::integer_part_read, conf_str, pos, chr, parent_value, 0 >{
	typedef parse_real< parse_real_state::end_of_read, conf_str, pos, chr, parent_value, 0 > parse_child;
	static constexpr value_type value = parse_child::value;
	static constexpr exp_type decimal_places = parse_child::decimal_places;
	static constexpr size_t stop_pos = parse_child::stop_pos;
};

/*****
 * Read fractional part
 */
template < const char* conf_str, size_t pos, char chr, value_type parent_value >
struct parse_real<parse_real_state::period_read, conf_str, pos, chr, parent_value, 0>{
	typedef parse_uint< conf_str, pos, chr, parent_value> parse_fraction;
	static constexpr value_type width = parse_fraction::value;
	static constexpr size_t new_pos  = parse_fraction::stop_pos;
	typedef parse_real<parse_real_state::fraction_part_read, conf_str, new_pos, conf_str[new_pos-1], width, static_cast<int>( pos - new_pos ) > parse_child;
	static constexpr value_type value = parse_child::value;
	static constexpr exp_type decimal_places = parse_child::decimal_places;
	static constexpr size_t stop_pos = parse_child::stop_pos;
};


/****
 * After fractional part comes either
 * - an powers of ten part ('e' or 'E')
 * - some other character, end of number
 */
template < const char* conf_str, size_t pos, value_type parent_value, exp_type decimals >
struct parse_real<parse_real_state::fraction_part_read, conf_str, pos, 'e', parent_value, decimals >{
	typedef parse_real<parse_real_state::init_reading_exponent, conf_str, pos+1, conf_str[pos],parent_value, decimals > parse_child;
	static constexpr value_type value = parse_child::value;
	static constexpr exp_type decimal_places = parse_child::decimal_places;
	static constexpr size_t stop_pos = parse_child::stop_pos;
};
template < const char* conf_str, size_t pos, value_type parent_value, exp_type decimals >
struct parse_real<parse_real_state::fraction_part_read, conf_str, pos, 'E', parent_value, decimals>{
	typedef parse_real<parse_real_state::init_reading_exponent, conf_str, pos+1, conf_str[pos],parent_value, decimals> parse_child;
	static constexpr value_type value = parse_child::value;
	static constexpr exp_type decimal_places = parse_child::decimal_places;
	static constexpr size_t stop_pos = parse_child::stop_pos;
};
template < const char* conf_str, size_t pos, char chr, value_type parent_value, exp_type decimals >
struct parse_real<parse_real_state::fraction_part_read, conf_str, pos, chr, parent_value, decimals>{
	typedef parse_real<parse_real_state::end_of_read, conf_str, pos, chr,parent_value, decimals> parse_child;
	static constexpr value_type value = parse_child::value;
	static constexpr exp_type decimal_places = parse_child::decimal_places;
	static constexpr size_t stop_pos = parse_child::stop_pos;
};

/****
 * Read powers of ten
 */
#define COMMA_MACRO_PROTECT ,
#define PARSE_REAL_EXPONENT_TEMPLATE_MACRO_FULL( chr, sgn, rel_pos, next_char, tmpl_arg ) \
		template < const char* conf_str, size_t pos, tmpl_arg value_type parent_value, exp_type decimals >\
		struct parse_real<parse_real_state::init_reading_exponent, conf_str, pos, (chr), parent_value, decimals>{\
			typedef parse_uint< conf_str, (rel_pos), next_char, 0> parse_exponent;\
			static constexpr value_type exponent = parse_exponent::value;\
			static constexpr size_t new_pos  = parse_exponent::stop_pos;\
			typedef parse_real<parse_real_state::end_of_read, conf_str, new_pos, conf_str[new_pos-1], parent_value, (decimals) sgn (exponent) > parse_child;\
			static constexpr value_type value = parse_child::value;\
			static constexpr exp_type decimal_places = parse_child::decimal_places;\
			static constexpr size_t stop_pos = parse_child::stop_pos;\
		};

#define PARSE_REAL_EXPONENT_TEMPLATE_MACRO( chr, sgn, rel_pos, next_char, tmpl_arg ) \
		PARSE_REAL_EXPONENT_TEMPLATE_MACRO_FULL( chr, sgn, rel_pos, next_char, )

PARSE_REAL_EXPONENT_TEMPLATE_MACRO( '+',+,pos+1,conf_str[pos], )
PARSE_REAL_EXPONENT_TEMPLATE_MACRO( '-',-,pos+1,conf_str[pos], )
PARSE_REAL_EXPONENT_TEMPLATE_MACRO_FULL( chr, +, pos, chr, char chr COMMA_MACRO_PROTECT )

#undef PARSE_REAL_EXPONENT_TEMPLATE_MACRO
#undef PARSE_REAL_EXPONENT_TEMPLATE_MACRO_FULL
#undef COMMA_MACRO_PROTECT

/***
 * Last level, return recursion
 */
template < const char* conf_str, size_t pos, char chr, value_type parent_value, exp_type decimals >
struct parse_real< parse_real_state::end_of_read, conf_str, pos, chr, parent_value, decimals >{
	static constexpr value_type value = parent_value;
	static constexpr exp_type decimal_places = decimals;
	static constexpr size_t stop_pos = pos;
};

}

typedef decimal_float_help::decimal_float real_type;

template < const char* conf_str, size_t pos, char chr>
struct parse_real{
	typedef typename _parse_real::parse_real<_parse_real::parse_real_state::initial_state, conf_str, pos, chr, 0, 0 > parse_child;
	static constexpr real_type value { parse_child::value , parse_child::decimal_places };
	static constexpr size_t stop_pos = parse_child::stop_pos;
};


/****
 * Parse configuration
 */

enum class parse_state{
	ground_state,
	read_straight_box,
	read_rotated_box,
	read_delta_box,
	read_level_info,
	one_shift_right,
	one_shift_down
};


/*****
 * Everything not allowed is disallowed
 */
template < parse_state state, const char* conf_str, size_t pos, char chr, typename... Args>
struct parse_config{
	typedef parse_error< pos, chr, Args...> conf_type;
	typedef typename conf_type::r best;
};


/*******
 * end of string specialization
 */
template < parse_state state, const char* conf_str, size_t pos, template <typename...> class complete_levels_collection, typename... complete_levels, typename... Args >
struct parse_config< state, conf_str,pos, 0, complete_levels_collection<complete_levels...>, Args...>{
	typedef complete_levels_collection<complete_levels...> conf_type;
};


/*****
 * For ground_state
 */

template < const char* conf_str, size_t pos, template <typename...> class complete_levels_collection, typename... complete_levels, typename... Args >
struct parse_config< parse_state::ground_state, conf_str,pos,' ',complete_levels_collection<complete_levels...>,Args...>{
	typedef typename parse_config< parse_state::ground_state, conf_str,pos+1,conf_str[pos],complete_levels_collection<complete_levels...>,Args...>::conf_type conf_type;
};
template < const char* conf_str, size_t pos, template <typename...> class complete_levels_collection, typename... complete_levels, typename... Args >
struct parse_config< parse_state::ground_state, conf_str,pos,'\t',complete_levels_collection<complete_levels...>,Args...>{
	typedef typename parse_config< parse_state::ground_state, conf_str,pos+1,conf_str[pos],complete_levels_collection<complete_levels...>,Args...>::conf_type conf_type;
};
template < const char* conf_str, size_t pos, template <typename...> class complete_levels_collection, typename... complete_levels, typename... Args >
struct parse_config< parse_state::ground_state, conf_str,pos,'-',complete_levels_collection<complete_levels...>,Args...>{
	typedef typename parse_config< parse_state::ground_state, conf_str,pos+1,conf_str[pos],complete_levels_collection<complete_levels...>,Args...>::conf_type conf_type;
};
template < const char* conf_str, size_t pos, template <typename...> class complete_levels_collection, typename... complete_levels, typename... Args >
struct parse_config< parse_state::ground_state, conf_str, pos,'[',complete_levels_collection<complete_levels...>,Args...>{
	typedef typename parse_config< parse_state::read_straight_box, conf_str,pos+1,conf_str[pos],complete_levels_collection<complete_levels...>,Args...>::conf_type conf_type;
};
template < const char* conf_str, size_t pos, template <typename...> class complete_levels_collection, typename... complete_levels, typename... Args >
struct parse_config< parse_state::ground_state, conf_str,pos,'<',complete_levels_collection<complete_levels...>,Args...>{
	typedef typename parse_config< parse_state::read_rotated_box, conf_str,pos+1,conf_str[pos],complete_levels_collection<complete_levels...>,Args...>::conf_type conf_type;
};
template < const char* conf_str, size_t pos, template <typename...> class complete_levels_collection, typename... complete_levels, typename... Args >
struct parse_config< parse_state::ground_state, conf_str,pos,'(',complete_levels_collection<complete_levels...>,Args...>{
	typedef typename parse_config< parse_state::read_delta_box, conf_str,pos+1,conf_str[pos],complete_levels_collection<complete_levels...>,Args...>::conf_type conf_type;
};
template < const char* conf_str, size_t pos, template <typename...> class complete_levels_collection, typename... complete_levels, typename... Args >
struct parse_config< parse_state::ground_state, conf_str,pos,'>',complete_levels_collection<complete_levels...>,Args...>{
	typedef typename parse_config< parse_state::one_shift_right, conf_str,pos+1,conf_str[pos],complete_levels_collection<complete_levels...>,Args...>::conf_type conf_type;
};
template < const char* conf_str, size_t pos, template <typename...> class complete_levels_collection, typename... complete_levels, typename... Args >
struct parse_config< parse_state::ground_state, conf_str,pos,'v',complete_levels_collection<complete_levels...>,Args...>{
	typedef typename parse_config< parse_state::one_shift_down, conf_str,pos+1,conf_str[pos],complete_levels_collection<complete_levels...>,Args...>::conf_type conf_type;
};
template < const char* conf_str, size_t pos, template <typename...> class complete_levels_collection, typename... complete_levels, typename... Args>
struct parse_config< parse_state::ground_state, conf_str,pos,'L',complete_levels_collection<complete_levels...>,Args...>{
	typedef typename parse_config< parse_state::read_level_info, conf_str,pos+1,conf_str[pos],complete_levels_collection<complete_levels...>,Args...>::conf_type conf_type;
};




/*****
 * Reading straight box filter
 */
template < const char* conf_str, size_t pos, char chr, template <typename...> class complete_levels_collection, typename... complete_levels, typename... Args >
struct parse_config< parse_state::read_straight_box, conf_str,pos,chr,complete_levels_collection<complete_levels...>,Args...>{
	static constexpr size_t init_white_space_pos  = parse_consume_whitespace<conf_str,pos,chr>::stop_pos;

	typedef parse_uint<conf_str,init_white_space_pos,conf_str[init_white_space_pos-1]> parse_value;
	static constexpr unsigned long width = parse_value::value;
	static constexpr size_t new_pos  = pos + parse_value::chars_read;

	static constexpr size_t last_white_space_pos  = parse_consume_whitespace<conf_str,new_pos,conf_str[new_pos-1]>::stop_pos;
	static_assert(conf_str[last_white_space_pos-1] == ']', "Expected \']\' was not found");
	typedef typename parse_config< parse_state::ground_state, conf_str,last_white_space_pos+1,conf_str[last_white_space_pos],complete_levels_collection<complete_levels...>,Args...,box_filter<width>>::conf_type conf_type;
};



/******
 * Reading rotated box filter
 */
template < const char* conf_str, size_t pos, char chr, template <typename...> class complete_levels_collection, typename... complete_levels, typename... Args >
struct parse_config< parse_state::read_rotated_box, conf_str,pos,chr,complete_levels_collection<complete_levels...>, Args...>{
	static constexpr size_t init_white_space_pos  = parse_consume_whitespace<conf_str,pos,chr>::stop_pos;

	typedef parse_uint<conf_str,init_white_space_pos,conf_str[init_white_space_pos-1]> parse_value;
	static constexpr unsigned long shift = parse_value::value;
	static constexpr size_t new_pos  = pos + parse_value::chars_read;

	static constexpr size_t last_white_space_pos  = parse_consume_whitespace<conf_str,new_pos,conf_str[new_pos-1]>::stop_pos;
	static_assert(conf_str[last_white_space_pos-1] == '>', "Expected \'>\' was not found");
	typedef typename parse_config< parse_state::ground_state, conf_str,last_white_space_pos+1,conf_str[last_white_space_pos],complete_levels_collection<complete_levels...>,Args...,rotated_box_filter<shift> >::conf_type conf_type;
};



/*****
 * Reading delta box filter
 */
template < const char* conf_str, size_t pos, char chr, template <typename...> class complete_levels_collection, typename... complete_levels, typename... Args >
struct parse_config< parse_state::read_delta_box, conf_str,pos,chr,complete_levels_collection<complete_levels...>,Args...>{
	static constexpr size_t init_white_space_pos  = parse_consume_whitespace<conf_str,pos,chr>::stop_pos;

	typedef parse_uint<conf_str,init_white_space_pos,conf_str[init_white_space_pos-1]> parse_value;
	static constexpr unsigned long shift = parse_value::value;
	static constexpr size_t new_pos  = pos + parse_value::chars_read;

	static constexpr size_t last_white_space_pos  = parse_consume_whitespace<conf_str,new_pos,conf_str[new_pos-1]>::stop_pos;
	static_assert(conf_str[last_white_space_pos-1] == ')', "Expected \')\' was not found when reading delta_box_filter");
	typedef typename parse_config< parse_state::ground_state, conf_str,last_white_space_pos+1,conf_str[last_white_space_pos],complete_levels_collection<complete_levels...>, Args..., delta_box_filter<shift> >::conf_type conf_type;
};



/*****
 * Read down-shift and down-sample modifiers
 */
template < const char* conf_str, size_t pos, template <typename...> class complete_levels_collection, typename... complete_levels, typename... Args >
struct parse_config< parse_state::one_shift_right, conf_str,pos,'>',complete_levels_collection<complete_levels...>,Args...>{
	typedef typename parse_config< parse_state::ground_state, conf_str,pos+1,conf_str[pos],complete_levels_collection<complete_levels...>, Args..., down_shift >::conf_type conf_type;
};
template < const char* conf_str, size_t pos, template <typename...> class complete_levels_collection, typename... complete_levels, typename... Args >
struct parse_config< parse_state::one_shift_down, conf_str,pos,'>',complete_levels_collection<complete_levels...>,Args...>{
	typedef typename parse_config< parse_state::ground_state, conf_str,pos+1,conf_str[pos],complete_levels_collection<complete_levels...>, Args..., down_sample, down_shift >::conf_type conf_type;
};
template < const char* conf_str, size_t pos, template <typename...> class complete_levels_collection, typename... complete_levels, typename... Args>
struct parse_config< parse_state::one_shift_down, conf_str,pos,'v',complete_levels_collection<complete_levels...>,Args...>{
	typedef typename parse_config< parse_state::ground_state, conf_str,pos+1,conf_str[pos],complete_levels_collection<complete_levels...>, Args..., down_sample>::conf_type conf_type;
};


/*****
 * Read level info
 */
template < const char* conf_str, size_t pos, template <typename...> class complete_levels_collection, typename... complete_levels, typename... Args >
struct parse_config< parse_state::read_level_info, conf_str,pos,'|', complete_levels_collection<complete_levels...>, Args...>{
	static constexpr size_t sigma_value_pos  = parse_consume_whitespace<conf_str,pos+1,conf_str[pos]>::stop_pos;

	typedef parse_real<conf_str,sigma_value_pos,conf_str[sigma_value_pos-1]> read_sigma_value;
	static constexpr real_type sigma = read_sigma_value::value;
	static constexpr size_t sigma_end_pos  = read_sigma_value::stop_pos;

	static constexpr size_t comma_pos  = parse_consume_whitespace<conf_str,sigma_end_pos,conf_str[sigma_end_pos-1]>::stop_pos;
	static_assert( conf_str[comma_pos-1] == ',', "Expected a comma after sigma_value in reading scale level info");
	static constexpr size_t norm_const_pos  = parse_consume_whitespace<conf_str,comma_pos+1,conf_str[comma_pos]>::stop_pos;

	typedef parse_real<conf_str,norm_const_pos,conf_str[norm_const_pos-1]> read_norm_value;
	static constexpr real_type norm_value = read_norm_value::value;
	static constexpr size_t norm_value_end_pos  = read_norm_value::stop_pos;

	static constexpr size_t level_info_stop_pos  = parse_consume_whitespace<conf_str,norm_value_end_pos,conf_str[norm_value_end_pos-1]>::stop_pos;
	static_assert( conf_str[level_info_stop_pos-1] == '|', "Expected a '|' after norm value in reading scale level info");

	typedef typename parse_config< parse_state::ground_state, conf_str,level_info_stop_pos+1,conf_str[level_info_stop_pos], complete_levels_collection<complete_levels..., level_extract< decimal_float_help::decimal_float_value< sigma.value, sigma.exponent >, decimal_float_help::decimal_float_value< norm_value.value, norm_value.exponent > ,Args...>> >::conf_type conf_type;
};



template <const char* conf_str>
struct make_config{
	typedef typename parse_config< parse_state::ground_state, conf_str,1,conf_str[0], filter_config<> >::conf_type conf_type;
};

}

#endif /* COMPILECONFIG_H_ */
