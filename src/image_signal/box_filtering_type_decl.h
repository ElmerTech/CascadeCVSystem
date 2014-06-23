/*
 * box_filtering_type_decl.h
 *
 *  Created on: 9 jul 2013
 *      Author: johan
 */

#ifndef SIMPLE_IMAGE_BOX_FILTERING_TYPE_DECL_H_
#define SIMPLE_IMAGE_BOX_FILTERING_TYPE_DECL_H_

namespace _basic_image_signal {


template <size_t size>
struct box_filter{
	static constexpr int filter_size = size;
	static constexpr int total_gain = filter_size * filter_size;

	static void print( std::ostream& os ){
		os << "[ "<< filter_size << " ]  ";
	}
};

template<>
struct box_filter<2>{
	static constexpr int filter_size = 2;
	static constexpr int total_gain = filter_size * filter_size;

	static void print( std::ostream& os ){
		os << "[ "<< filter_size << " ]  ";
	}
};


template <size_t size>
struct rotated_box_filter{
	static constexpr int filter_size = size;
	static constexpr int total_gain = filter_size * filter_size;

	static void print( std::ostream& os ){
		os << "< "<< size << " >  ";
	}
};
template <size_t size>
struct delta_box_filter{
	static void print( std::ostream& os ){
		os << "( "<< size << " )  ";
	}
};


struct down_shift{
	static void print( std::ostream& os ){
		os << ">>  ";
	}
};
struct down_sample{
	static void print( std::ostream& os ){
		os << "vv  ";
	}
};


template <typename T>
struct is_downsample{
	static constexpr bool value = false;
	static constexpr size_t count = 0;
};
template <>
struct is_downsample< down_sample >{
	static constexpr bool value = true;
	static constexpr size_t count = 1;
};

}

#endif /* SIMPLE_IMAGE_BOX_FILTERING_TYPE_DECL_H_ */
