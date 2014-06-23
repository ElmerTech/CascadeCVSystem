/*
 * box_filtering_impl.h
 *
 *  Created on: 9 jul 2013
 *      Author: johan
 */

#ifndef SIMPLE_IMAGE_BOX_FILTERING_IMPL_H_
#define SIMPLE_IMAGE_BOX_FILTERING_IMPL_H_

#include "simple_image_signal.h"
#include "simple_image_signal_utility.h"
#include "box_filtering_type_decl.h"

namespace _basic_image_signal{

template <typename T>
using image_signal = basic_image_signal<T>;



template < typename T, T input_value, uint32_t pos, bool end >
struct log2_impl{
	static constexpr uint32_t value = log2_impl< T, ( input_value >> 1 ), pos+1, ( input_value >> 1 ) == 0 >::value;

};
template < typename T, T input_value, uint32_t pos>
struct log2_impl< T, input_value, pos, true >{
	static constexpr uint32_t value = pos-1;
};

template < typename T, T input_value >
struct log2ceil{
	static constexpr uint32_t value = ( input_value <= 1 ) ? 0 : 1 + log2_impl< T, input_value-1, 0, false >::value;
};

template < typename T, T input_value >
struct log2floor{
	static constexpr uint32_t value = log2_impl< T, input_value, 0, false >::value;
};



template< typename FilterType, typename DataType, DataType pre_filter_gain >
struct apply_filter{
	static constexpr DataType post_filter_gain = pre_filter_gain;
	template< typename... OutputDataTypes >
	static void filter( image_signal<DataType>& dst, image_signal<OutputDataTypes>... outputImages ){}
};

template< typename DataType, DataType pre_filter_gain >
struct apply_filter<down_sample,DataType, pre_filter_gain>{
	static constexpr DataType post_filter_gain = pre_filter_gain;
	template< typename... OutputDataTypes >
	static void filter( image_signal<DataType>& dst, image_signal<OutputDataTypes>... outputImages ){
		_basic_image_signal::subsample_image( dst );
	}
};

template< typename DataType, DataType pre_filter_gain >
struct apply_filter<down_shift,DataType, pre_filter_gain>{
	static constexpr uint32_t shift_size = log2ceil<DataType, pre_filter_gain>::value - (31 - 8) + 12;
	static constexpr DataType post_filter_gain = pre_filter_gain >> shift_size;
	template< typename... OutputDataTypes >
	static void filter( image_signal<DataType>& dst, image_signal<OutputDataTypes>... outputImages ){
		_basic_image_signal::downshift_image( dst , shift_size );
	}
};

enum class dimension { cols, rows };
enum class direction { forward, reverse };



template< size_t width >
struct box_filtering_data{
	static constexpr int filter_size = width;
	static constexpr int sum_delay = 1;
	static constexpr int sum_offset = -sum_delay;
	static constexpr double offset_shift{ (filter_size-1)/2.0 };
	static constexpr int data_shift{ -1 };

	static constexpr int nof_filtering_phases = 4;
};


template<>
struct box_filtering_data< 2 >{
	static constexpr int width = 2;
	static constexpr int filter_size = width;
	static constexpr int sum_delay = 0;
	static constexpr int sum_offset = 1 - sum_delay;
	static constexpr double offset_shift{ (filter_size-1)/2.0 };
	static constexpr int data_shift{ 0 };

	static constexpr int nof_filtering_phases = 1;
};



template< dimension dim, direction dir >
struct dir_dim_resolve_struct {
	static constexpr bool is_rows = ( dim == dimension::rows );
	static constexpr bool is_forward = ( dir == direction::forward );
	static inline int y_coord( const int i, const int k ){
		return ( is_rows ? k : ( is_forward ?  i: -i ) );
	}
	static inline int x_coord( const int i, const int k ){
		return ( is_rows ? ( is_forward ?  i: -i ) : k );
	}
};

template< typename DataType, dimension dim, direction dir >
struct image_data_resolve_struct {
	typedef dir_dim_resolve_struct< dim, dir > coord_trans;
	typedef typename point_reference< DataType >::reference_data_type data_type;

	static inline data_type& image_data_ref( point_reference< DataType >& data, const int i ){
		return data( coord_trans::y_coord( i, 0 ), coord_trans::x_coord( i, 0 ) );
	}
	static inline point_reference< DataType > shifted_point_ref(
			point_reference< DataType >& data, const int i, const int k
	){
		return data.shifted( coord_trans::y_coord( i, k ), coord_trans::x_coord( i, k ) );
	}
};


template< int size, class DataType, dimension dim, direction dir >
struct image_sum_struct{
	typedef image_data_resolve_struct< DataType, dim, dir > resolve_base;
	typedef image_sum_struct<size-1, DataType, dim, dir> sum_struct;

	static inline DataType sum( point_reference< DataType >& data, const int i ){
		return resolve_base::image_data_ref( data, i ) + sum_struct::sum( data, i + 1 );
	}
};
template< class DataType, dimension dim, direction dir >
struct image_sum_struct<1, DataType, dim, dir>{
	typedef image_data_resolve_struct< DataType, dim, dir > resolve_base;

	static inline DataType sum( point_reference< DataType >& data, const int i ){
		return resolve_base::image_data_ref( data, i );
	}
};





template< size_t width, class DataType, DataType total_gain, dimension dim, direction dir >
struct box_filtering_primitives {
	typedef image_data_resolve_struct< DataType, dim, dir > resolve_base;
	typedef image_sum_struct< width, DataType, dim, dir> sum_struct;

	typedef box_filtering_data< width > filtering_data;
	static constexpr decltype(filtering_data::sum_offset) sum_offset = filtering_data::sum_offset;

	static inline void init_step( point_reference< DataType >& data ){
		resolve_base::image_data_ref( data, 0 ) =
				-sum_struct::sum( data, 1 );
	}

	static inline void diff_step( point_reference< DataType >& data ){
		resolve_base::image_data_ref( data, 0 ) -=
				resolve_base::image_data_ref( data, width );
	}

	static inline void sum_step( point_reference< DataType >& data ){
		resolve_base::image_data_ref( data, sum_offset ) +=
				resolve_base::image_data_ref( data, sum_offset - 1 );
	}
};

template< class DataType, DataType total_gain, dimension dim, direction dir >
struct box_filtering_primitives< 2, DataType, total_gain, dim, dir >{
	static constexpr size_t width = 2;
	typedef image_data_resolve_struct< DataType, dim, dir > resolve_base;
	typedef box_filtering_data< width > filtering_data;
	static constexpr auto sum_offset = filtering_data::sum_offset;

	static inline void add_step( point_reference< DataType >& data ){
		resolve_base::image_data_ref( data, sum_offset - 1 ) +=
				resolve_base::image_data_ref( data, sum_offset );
	}
};



template< size_t width, size_t step_no, class DataType, DataType total_gain, dimension dim, direction dir >
struct box_filtering_functions;



template< size_t width, class DataType, DataType total_gain, dimension dim, direction dir >
struct box_filtering_functions<width, 0, DataType, total_gain, dim, dir>{
	typedef box_filtering_primitives< width, DataType, total_gain, dim, dir > primitives;
	static inline void filter_step( point_reference< DataType >&& data ){
		primitives::init_step( data );
	}
};
template< size_t width, class DataType, DataType total_gain, dimension dim, direction dir >
struct box_filtering_functions<width, 1, DataType, total_gain, dim, dir>{
	typedef box_filtering_primitives< width, DataType, total_gain,  dim, dir > primitives;
	static inline void filter_step( point_reference< DataType >&& data ){
		primitives::diff_step( data );
	}
};
template< size_t width, class DataType, DataType total_gain, dimension dim, direction dir >
struct box_filtering_functions<width, 2, DataType, total_gain, dim, dir>{
	typedef box_filtering_primitives< width, DataType, total_gain,  dim, dir > primitives;
	static inline void filter_step( point_reference< DataType >&& data ){
		primitives::diff_step( data );
		primitives::sum_step( data );
	}
};
template< size_t width, class DataType, DataType total_gain, dimension dim, direction dir >
struct box_filtering_functions<width, 3, DataType, total_gain, dim, dir>{
	typedef box_filtering_primitives< width, DataType, total_gain, dim, dir > primitives;
	static inline void filter_step( point_reference< DataType >&& data ){
		primitives::sum_step( data );
	}
};



template< class DataType, DataType total_gain, dimension dim, direction dir >
struct box_filtering_functions< 2, 0, DataType, total_gain, dim, dir>{
	static constexpr size_t width = 2;
	typedef box_filtering_primitives< width, DataType, total_gain, dim, dir > primitives;
	static inline void filter_step( point_reference< DataType >&& data ){}
};
template< class DataType, DataType total_gain, dimension dim, direction dir >
struct box_filtering_functions< 2, 1, DataType, total_gain, dim, dir>{
	static constexpr size_t width = 2;
	typedef box_filtering_primitives< width, DataType, total_gain, dim, dir > primitives;
	static inline void filter_step( point_reference< DataType >&& data ){}
};
template< class DataType, DataType total_gain, dimension dim, direction dir >
struct box_filtering_functions< 2, 2, DataType, total_gain, dim, dir>{
	static constexpr size_t width = 2;
	typedef box_filtering_primitives< width, DataType, total_gain, dim, dir > primitives;
	static inline void filter_step( point_reference< DataType >&& data ){
		primitives::add_step( data );
	}
};
template< class DataType, DataType total_gain, dimension dim, direction dir >
struct box_filtering_functions< 2, 3, DataType, total_gain, dim, dir>{
	static constexpr size_t width = 2;
	typedef box_filtering_primitives< width, DataType, total_gain, dim, dir > primitives;
	static inline void filter_step( point_reference< DataType >&& data ){
		primitives::add_step( data );
	}
};







template< size_t width, class DataType, DataType pre_filter_gain >
struct apply_filter< box_filter<width>, DataType, pre_filter_gain >
: public box_filtering_data< width >{
	typedef box_filtering_data< width > filter_data;
	typedef typename basic_image_signal<DataType>::size_type size_type;
	template <size_t step_no, dimension dim, direction dir>
	using filter_funcs = box_filtering_functions<width, step_no, DataType, pre_filter_gain, dim, dir> ;
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
		box_filter_1D_rows( dst, outputImages... );
//		tcv.mark();
//		std::cerr << "horz took " << tcv.get_time()<< " s\n";
//		tcv.mark();
		box_filter_1D_cols( dst, outputImages... );
//		tcv.mark();
//		std::cerr << "vert took " << tcv.get_time()<< " s\n";
	}

	template < dimension dim, direction dir, size_t array_size >
	static inline void box_filter_1D(
			point_reference<DataType>&& data,
			const size_type data_length
	){
		typedef image_data_resolve_struct< DataType, dim, dir > resolve_base;
		constexpr size_type rows_at_a_time = array_size;

		for( int k = 0; k < rows_at_a_time; k++ ){
			filter_funcs<0,dim,dir>::filter_step( resolve_base::shifted_point_ref( data, -1, k) );
		}

		int i = 0;
		for(; i < sum_delay; i++ ){
			for( int k = 0; k < rows_at_a_time; k++ ){
				filter_funcs<1,dim,dir>::filter_step( resolve_base::shifted_point_ref( data, i, k ) );
			}
		}
		for(; i < data_length - filter_size; i++ ){
			for( int k = 0; k < rows_at_a_time; k++ ){
				filter_funcs<2,dim,dir>::filter_step( resolve_base::shifted_point_ref( data, i, k ) );
			}
		}
		for(; i < data_length - sum_offset; i++ ){
			for(int k = 0; k < rows_at_a_time; k++ ){
				filter_funcs<3,dim,dir>::filter_step( resolve_base::shifted_point_ref( data, i, k ) );
			}
		}
	}

	template < dimension dim, direction dir >
	static inline void box_filter_1D(
			point_reference<DataType>&& data,
			const size_type data_length,
			const size_type rows_at_a_time
	){
		typedef image_data_resolve_struct< DataType, dim, dir > resolve_base;

		for( int k = 0; k < rows_at_a_time; k++ ){
			filter_funcs<0,dim,dir>::filter_step( resolve_base::shifted_point_ref( data, -1, k ) );
		}

		int i=0;
		for(; i < sum_delay; i++ ){
			for( int k = 0; k < rows_at_a_time; k++ ){
				filter_funcs<1,dim,dir>::filter_step( resolve_base::shifted_point_ref( data, i, k ) );
			}
		}
		for(; i < data_length - filter_size; i++ ){
			for( int k = 0; k < rows_at_a_time; k++ ){
				filter_funcs<2,dim,dir>::filter_step( resolve_base::shifted_point_ref( data, i, k ) );
			}
		}
		for(; i < data_length - sum_offset; i++ ){
			for(int k = 0; k < rows_at_a_time; k++ ){
				filter_funcs<3,dim,dir>::filter_step( resolve_base::shifted_point_ref( data, i, k ) );
			}
		}
	}


	template< typename... OutputDataTypes >
	static void box_filter_1D_rows( basic_image_signal<DataType>& image, image_signal<OutputDataTypes>&... outputImages ){
		if( image.raw_width() < filter_size ){
			image.raw_width( 0 );
			return;
		}

		static const int rows_at_a_time = 1;

		const size_type data_length = image.raw_width();
		size_type multi_row_height = image.raw_height() - rows_at_a_time;


		if(image.valid_shift_data_offset(0,data_shift)){
			size_type n = 0;
			for(; n < multi_row_height; n+=rows_at_a_time ){
				box_filter_1D< dimension::rows, direction::forward, rows_at_a_time >(
						image.point(n,0),
						data_length );
			}

			const size_type rows_left = image.raw_height() - n;
			if( rows_left > 0 ){
				box_filter_1D< dimension::rows, direction::forward >(
						image.point(n,0),
						data_length,
						rows_left );
//				cout << rows_left << "\n";
			}

			image.raw_width_diff( -( filter_size - 1 ) );
			image.shift_data_offset( 0, data_shift );
		}else if( image.valid_shift_data_offset(0,-data_shift )){
			size_type n = 0;
			for(; n < multi_row_height; n+=rows_at_a_time ){
				box_filter_1D< dimension::rows, direction::reverse, rows_at_a_time >(
						image.point(n,image.width()-1),
						data_length );
			}

			const size_type rows_left = image.raw_height() - n;
			if( rows_left > 0 ){
				box_filter_1D< dimension::rows, direction::reverse >(
						image.point(n,image.width()-1),
						data_length,
						rows_left );
//				cout << rows_left << "\n";
			}

			image.raw_width_diff( -( filter_size - 1 ) );
			image.shift_data_offset( 0, -data_shift );
		}else{
			throw 0;
		}
	}

	template< typename... OutputDataTypes >
	static void box_filter_1D_cols( basic_image_signal<DataType>& image, image_signal<OutputDataTypes>&... outputImages ){
		if(image.raw_height() < filter_size){
			image.raw_height( 0 ) ;
			return;
		}

		static const int cols_at_a_time = 32;

		const size_type data_length = image.raw_height();
		size_type multi_col_width = image.raw_width() - cols_at_a_time;

		if(image.valid_shift_data_offset(data_shift,0)){
			size_type n = 0;
			for( ; n < multi_col_width; n+=cols_at_a_time ){
				box_filter_1D< dimension::cols, direction::forward, cols_at_a_time >(
						image.point(0,n),
						data_length );
			}

			const size_type cols_left = image.raw_width() - n;
			if( cols_left > 0 ){
				box_filter_1D< dimension::cols, direction::forward >(
						image.point(0,n),
						data_length,
						cols_left );
			}
			image.raw_height_diff( -( filter_size - 1 ) );
			image.shift_data_offset( data_shift, 0 );
		}else if( image.valid_shift_data_offset(-data_shift,0) ){
			size_type n = 0;
			for( ; n < multi_col_width; n+=cols_at_a_time ){
				box_filter_1D< dimension::cols, direction::reverse, cols_at_a_time >(
						image.point(image.height()-1,n),
						data_length );
			}

			const size_type cols_left = image.raw_width() - n;
			if( cols_left > 0 ){
				box_filter_1D< dimension::cols, direction::reverse >(
						image.point(image.height()-1,n),
						data_length,
						cols_left );
			}

			image.raw_height_diff( -( filter_size - 1 ) );
			image.shift_data_offset( -data_shift, 0 );
		}else{
			throw 0;
		}
	}
};




}


#endif /* SIMPLE_IMAGE_BOX_FILTERING_IMPL_H_ */
