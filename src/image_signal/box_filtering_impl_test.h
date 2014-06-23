/*
 * box_filtering_impl.h
 *
 *  Created on: 9 jul 2013
 *      Author: johan
 */

#ifndef SIMPLE_IMAGE_BOX_FILTERING_TEST_IMPL_H_
#define SIMPLE_IMAGE_BOX_FILTERING_TEST_IMPL_H_

#include "simple_image_signal.h"
#include "box_filtering_type_decl.h"
#include "box_filtering_impl.h"
#include <utility>

namespace _basic_image_signal{

template <typename T>
using image_signal = basic_image_signal<T>;





template< class DataType, DataType total_gain, dimension dim, direction dir, size_t width, size_t... more_widths >
struct box_cascading_impl_struct{
	typedef typename basic_image_signal<DataType>::size_type size_type;
	typedef box_cascading_impl_struct< DataType, total_gain, dim, dir, more_widths... > remaining_filters;
	static constexpr int filter_size = width;

	template <size_t step_no>
	using filter_funcs = box_filtering_functions<width, step_no, DataType, total_gain, dim, dir> ;

	typedef image_data_resolve_struct< DataType, dim, dir > resolve_base;

	typedef box_filtering_primitives< width, DataType, total_gain, dim, dir > primitives;
	static constexpr auto sum_offset = primitives::sum_offset;
	static constexpr auto sum_delay = primitives::sum_delay;

	template < size_t array_size >
	static inline void box_filter_1D(
			point_reference<DataType>&& data,
			int i2,
			const size_type data_length
	){
		constexpr size_type rows_at_a_time = array_size;

		for( int k = 0; k < rows_at_a_time; k++ ){
			filter_funcs<0>::filter_step( resolve_base::shifted_point_ref( data, -1, k ) );
		}

		int i = 0;
		for(; i < sum_delay; i++ ){
			for( int k = 0; k < rows_at_a_time; k++ ){
				filter_funcs<1>::filter_step( resolve_base::shifted_point_ref( data, i, k ) );
			}
		}
		for(; i < data_length - filter_size; i++ ){
			for( int k = 0; k < rows_at_a_time; k++ ){
				filter_funcs<2>::filter_step( resolve_base::shifted_point_ref( data, i, k ) );
			}
		}
		for(; i < data_length - sum_offset; i++ ){
			for(int k = 0; k < rows_at_a_time; k++ ){
				filter_funcs<3>::filter_step( resolve_base::shifted_point_ref( data, i, k ) );
			}
		}
	}

};

template< size_t width, size_t phases_left, class DataType, DataType total_gain, dimension dim, direction dir >
struct box_filtering_loop_struct{
	typedef box_filtering_functions<width, box_filtering_data<width>::nof_filtering_phases - phases_left, DataType, total_gain, dim, dir> filter_funcs;
	typedef box_filtering_loop_struct<width, phases_left - 1, DataType, total_gain, dim, dir> next_phase_loop;
	typedef typename basic_image_signal<DataType>::size_type size_type;

	typedef image_data_resolve_struct< DataType, dim, dir > resolve_base;

	template < size_t array_size >
	static inline void filter_loop(
			point_reference<DataType>&& data,
			const size_type data_length
	){
		constexpr size_type rows_at_a_time = array_size;

		constexpr int sum_delay = 10;

		for(int i = 0; i < sum_delay; i++ ){
			for( int k = 0; k < rows_at_a_time; k++ ){
				filter_funcs::filter_step( resolve_base::shifted_point_ref( data, i, k ) );
			}
		}
		next_phase_loop::template filter_loop< array_size >(
				std::forward<point_reference<DataType>&&>(data),
				data_length );
	}
	static inline void filter_loop(
			point_reference<DataType>&& data,
			const size_type data_length,
			const size_type rows_at_a_time
	){
		typedef image_data_resolve_struct< DataType, dim, dir > resolve_base;
		int sum_delay = 10;

		for(int i = 0; i < sum_delay; i++ ){
			for( int k = 0; k < rows_at_a_time; k++ ){
				filter_funcs::filter_step( resolve_base::shifted_point_ref( data, i, k ) );
			}
		}
		next_phase_loop::filter_loop(
				std::forward<point_reference<DataType>&&>(data),
				data_length,
				rows_at_a_time );

	}
};

template< size_t width, class DataType, DataType total_gain, dimension dim, direction dir >
struct box_filtering_loop_struct< width, 0, DataType, total_gain, dim, dir >{
	typedef typename basic_image_signal<DataType>::size_type size_type;
	template < size_t array_size >
	static inline void filter_loop( point_reference<DataType>&& data, const size_type data_length ){ }
	static inline void filter_loop( point_reference<DataType>&& data, const size_type data_length, const size_type rows_at_a_time ){ }
};

template< size_t width, class DataType, DataType total_gain, dimension dim, direction dir >
struct box_filter_1D_init_struct{
	typedef box_filtering_loop_struct<width, box_filtering_data<width>::nof_filtering_phases, DataType, total_gain, dim, dir> initial_phase_loop;
	typedef typename basic_image_signal<DataType>::size_type size_type;

	template < size_t array_size >
	static inline void filter(
			point_reference<DataType>&& data,
			const size_type data_length
	){
		initial_phase_loop::template filter_loop< array_size >(
				std::forward<point_reference<DataType>&&>(data),
				data_length );
	}
	static inline void filter(
			point_reference<DataType>&& data,
			const size_type data_length,
			const size_type array_size
	){
		initial_phase_loop::template filter_loop(
				std::forward<point_reference<DataType>&&>(data),
				data_length,
				array_size );
	}
};



template< typename FilterType, typename DataType, DataType pre_filter_gain >
struct apply_filter_test{
	static constexpr DataType post_filter_gain = pre_filter_gain;
	template< typename... OutputDataTypes >
	static void filter( image_signal<DataType>& dst, image_signal<OutputDataTypes>... outputImages ){}
};

template< size_t width, class DataType, DataType pre_filter_gain >
struct apply_filter_test< box_filter<width>, DataType, pre_filter_gain >:
public box_filtering_data< width >{
	typedef box_filtering_data< width > filter_data;
	typedef typename basic_image_signal<DataType>::size_type size_type;

	template< dimension dim, direction dir >
	using box_filter_1D = box_filter_1D_init_struct<width,DataType,pre_filter_gain,dim,dir>;

	using filter_data::filter_size;
	using filter_data::sum_offset;
	using filter_data::sum_delay;
	using filter_data::offset_shift;
	using filter_data::data_shift;

	static constexpr DataType post_filter_gain = pre_filter_gain * mptl::square( width );

	template< typename... OutputDataTypes >
	static void filter( image_signal<DataType>& dst, image_signal<OutputDataTypes>&... outputImages  ){
		dst.apply_shift( point2D<double>{ 1.0 * offset_shift } );
//		_basic_image_signal::Timer tcv;
//		tcv.mark();
		box_filter_1D_rows_init( dst, outputImages... );
//		tcv.mark();
//		std::cerr << "horz took " << tcv.get_time()<< " s\n";
//		tcv.mark();
		box_filter_1D_cols_init( dst, outputImages... );
//		tcv.mark();
//		std::cerr << "vert took " << tcv.get_time()<< " s\n";
	}


	template< direction dir, typename... OutputDataTypes >
	static void box_filter_1D_rows( basic_image_signal<DataType>& image, image_signal<OutputDataTypes>&... outputImages ){
		if( image.raw_width() < filter_size ){
			image.raw_width( 0 );
			return;
		}

		static constexpr int rows_at_a_time = 1;

		const size_type data_length = image.raw_width();
		size_type multi_row_height = image.raw_height() - rows_at_a_time;

		constexpr bool is_forward = dir == direction::forward ;
		const int x_offset = is_forward ? 0 : image.width()-1;

		size_type n = 0;
		for(; n < multi_row_height; n+=rows_at_a_time ){
			box_filter_1D< dimension::rows, dir >::template filter<rows_at_a_time>(
					image.point(n,x_offset),
					data_length );
		}

		const size_type rows_left = image.raw_height() - n;
		if( rows_left > 0 ){
			box_filter_1D< dimension::rows, dir >::filter(
					image.point(n,x_offset),
					data_length,
					rows_left );
			//				cout << rows_left << "\n";
		}

		image.raw_width_diff( -( filter_size - 1 ) );
		image.shift_data_offset( 0, is_forward ? data_shift : -data_shift );
	};

	template< direction dir, typename... OutputDataTypes >
	static void box_filter_1D_cols( basic_image_signal<DataType>& image, image_signal<OutputDataTypes>&... outputImages ){
		if(image.raw_height() < filter_size){
			image.raw_height( 0 ) ;
			return;
		}

		static constexpr int cols_at_a_time = 32;

		const size_type data_length = image.raw_height();
		size_type multi_col_width = image.raw_width() - cols_at_a_time;

		constexpr bool is_forward = dir == direction::forward ;
		const int y_offset = is_forward ? 0 : image.height()-1;

		size_type n = 0;
		for( ; n < multi_col_width; n+=cols_at_a_time ){
			box_filter_1D< dimension::cols, dir >::template filter<cols_at_a_time>(
					image.point(y_offset,n),
					data_length );
		}

		const size_type cols_left = image.raw_width() - n;
		if( cols_left > 0 ){
			box_filter_1D< dimension::cols, dir>::filter(
					image.point(y_offset,n),
					data_length,
					cols_left );
		}
		image.raw_height_diff( -( filter_size - 1 ) );
		image.shift_data_offset( is_forward ? data_shift : - data_shift, 0 );
	}

	template< typename... OutputDataTypes >
	static void box_filter_1D_rows_init(
			basic_image_signal<DataType>& image,
			image_signal<OutputDataTypes>&... outputImages
	){
		if( image.raw_width() < filter_size ){
			image.raw_width( 0 );
			return;
		}

		if(image.valid_shift_data_offset(0,data_shift)){
			box_filter_1D_rows< direction::forward>(image, outputImages...);
		}else if( image.valid_shift_data_offset(0,-data_shift )){
			box_filter_1D_rows< direction::reverse>(image, outputImages...);
		}else{
			throw 0;
		}
	}

	template< typename... OutputDataTypes >
	static void box_filter_1D_cols_init(
			basic_image_signal<DataType>& image,
			image_signal<OutputDataTypes>&... outputImages
	){
		if(image.raw_height() < filter_size){
			image.raw_height( 0 ) ;
			return;
		}

		if(image.valid_shift_data_offset(data_shift,0)){
			box_filter_1D_cols< direction::forward >( image, outputImages... );
		}else if( image.valid_shift_data_offset(-data_shift,0) ){
			box_filter_1D_cols< direction::reverse >( image, outputImages... );
		}else{
			throw 0;
		}
	}
};



}


#endif /* SIMPLE_IMAGE_BOX_FILTERING_TEST_IMPL_H_ */
