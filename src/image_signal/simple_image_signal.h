/*
 * simple_image_signal.h
 *
 *  Created on: 1 sep 2010
 *      Author: Johan
 */

#ifndef IMAGE_SIGNAL_DEFINITION_H_
#define IMAGE_SIGNAL_DEFINITION_H_

#ifndef BASICIMAGE_NO_CV
#include <opencv2/highgui/highgui_c.h>
#endif

#include "container/point.h"
#include "container/range.h"
#include "container/data_holder.h"
#include "utilities/type_utils.h"
#include "utilities/special_functions.h"
#include <iostream>
using std::cerr;

namespace _basic_image_signal {

template <typename T>
using point2D = mptl::point<T,2>;

struct image_area{
	typedef int size_type;
	typedef double position_type;
	typedef mptl::range<double> range_type;

	range_type range_x;
	range_type range_y;
};

inline std::ostream& operator << (std::ostream& os, image_area ia ){
	return (os << ia.range_x << ia.range_y );
}

template<class T>
class basic_image_signal;

template < typename T, bool is_const >
class point_reference_template{
//	basic_image_signal_point& operator=(const basic_image_signal_point&) = delete;
public:
	typedef T data_type;
	typedef typename mptl::conditional_const<T,is_const>::type reference_data_type;

	reference_data_type* point;
	const int width_step;

public:
	point_reference_template( const point_reference_template& original, int y = 0, int x = 0):
		point( original.point + y*original.width_step + x),
		width_step(original.width_step){}
	point_reference_template( reference_data_type* const & p, int w_s):point(p),width_step(w_s){}
	point_reference_template( basic_image_signal<data_type>& im, int y, int x );
	point_reference_template( const basic_image_signal<data_type>& im, int y, int x );

	reference_data_type* operator[](const int row){
		return point + row * width_step;}
	const reference_data_type* operator[](const int row) const {
		return point + row * width_step;}

	reference_data_type& operator()(const int y, const int x) {
		return point[ y * width_step + x]; }
	const reference_data_type& operator()(const int y, const int x) const {
		return point[ y * width_step + x]; }

	point_reference_template& inc_x() { point++; return *this; }
	point_reference_template& inc_y() { point += width_step; return *this; }

	point_reference_template& shift(const int y, const int x) {
		point += y * width_step + x;
		return point_reference_template(*this, y , x);
	}

//	point_reference_template shifted(const int y, const int x) {
//		return point_reference_template(*this, y , x); }
	point_reference_template shifted(const int y, const int x) const {
		return point_reference_template(*this, y , x); }

	operator point_reference_template<data_type,true> () const {
		return point_reference_template<data_type, true>( point, width_step ); }

};

template <typename T>
using point_reference = point_reference_template<T,false>;
template <typename T>
using const_point_reference = point_reference_template<T,true>;

class basic_image_signal_base{
public:
	typedef image_area::size_type size_type;
	typedef std::make_signed<size_type>::type change_size_type;
	typedef std::make_signed<size_type>::type shift_type;
	typedef point2D<size_type> image_size_type;
	typedef point2D<double> offset_type;
	typedef point2D<shift_type> raw_offset_type;
};

template<class T>
class raw_image_signal {
public:

};

template<class T>
class basic_image_signal {
public:
	typedef T data_type;
	typedef typename basic_image_signal_base::size_type size_type;
	typedef typename basic_image_signal_base::change_size_type change_size_type;
	typedef typename basic_image_signal_base::shift_type shift_type;
	typedef typename basic_image_signal_base::image_size_type image_size_type;
	typedef typename basic_image_signal_base::offset_type offset_type;
	typedef typename basic_image_signal_base::raw_offset_type raw_offset_type;

	basic_image_signal( size_type w = 0, size_type h = 0, shift_type wS = 0 );
	basic_image_signal( basic_image_signal&& original ):
		_offset(original._offset),
		_width(original._width),
		_height(original._height),
		sample_distance(original.sample_distance),
		_width_step(original._width_step),
		array( original.array )
		{
			original.array = nullptr;
		}
	virtual ~basic_image_signal() = 0;

	data_type* operator[]( const shift_type row ){ return row_data(row); }
	const data_type* operator[]( const shift_type row ) const { return row_data( row ); }

	data_type& operator()( const shift_type y, const shift_type x ){
		return data_ref( y, x ); }
	const data_type& operator()( const shift_type y, const shift_type x) const {
		return data_ref( y, x ); }

	point_reference<T> point( const shift_type y, const shift_type x ) {
		return point_reference<T>( row_data(y) + x, width_step() );
	}
	const_point_reference<T> point( const shift_type y, const shift_type x ) const {
		return const_point_reference<T>( row_data(y) + x, width_step() );
	}

	size_type sample_factor() const { return sample_distance; }
	void sample_factor( size_type new_sample_factor ) {
		assert( new_sample_factor > 0 );
		sample_distance = new_sample_factor;
	}

	const offset_type& offset() const { return _offset; }
	void offset( const offset_type& new_offset ) { _offset = new_offset; }

	offset_type get_index_offset( offset_type point ) const {
		return  (point - _offset ) * ( 1.0 / sample_factor()) ;
	}
	raw_offset_type get_raw_offset( offset_type point ) const {
		return mptl::round<typename raw_offset_type::data_type>( get_index_offset( point ) ) ;
	}
	void apply_shift( offset_type shift ){
		_offset += shift * sample_distance;
	}
	bool valid_shift_data_offset( shift_type shift_y, shift_type shift_x ) const{
		return valid_shift_data_offset( raw_offset_type{ shift_x, shift_y } );
	}
	virtual bool valid_shift_data_offset( const raw_offset_type& shift ) const {
		return shift.x == 0 && shift.y == 0;
	}

	void shift_data_offset( shift_type shift_y, shift_type shift_x ){
		shift_data_offset( raw_offset_type{ shift_x, shift_y} );
	}
	void shift_data_offset( const raw_offset_type& shift ){
		array = row_data(shift.y) + shift.x;
//		_offset -= shift * sample_distance;
	}

	image_area::range_type range_x() const {
		return {_offset.x, _offset.x + width() - 1 };
	}
	image_area::range_type range_y() const {
		return {_offset.y, _offset.y + height() - 1 };
	}
	image_area definition_area() const { return { range_x() , range_y() }; }

	void set_raw_size( const size_type width, const size_type height){
		revalidate_storage( width, height );
	}
	void set_raw_size( const image_size_type& size ){
		set_raw_size( size.x, size.y );
	}
	image_size_type raw_size() const {
		return { _width, _height };
	}

	template <typename U>
	void copy_size( const basic_image_signal<U>& original ){
		this->set_raw_size(original.raw_size());
		this->_offset = original.offset();
		this->sample_distance = original.sample_factor();
	}

	constexpr const  size_type width() const { return raw_width() * sample_factor(); }
	constexpr const size_type height() const { return raw_height() * sample_factor(); }

	constexpr const  size_type raw_width() const { return _width; }
	constexpr const size_type raw_height() const { return _height; }

	void raw_width( const size_type& width ) { _width = width; }
	void raw_height( const size_type& height ) { _height = height; }

	void raw_width_diff( const change_size_type& width_diff ){ _width += width_diff; }
	void raw_height_diff( const change_size_type& height_diff ){ _height += height_diff; }

	constexpr const shift_type width_step() const { return _width_step; }
	constexpr const size_type total_size() const {
		return ( _height > 0 && _width > 0) ? ( _height - 1 ) * _width_step + _width : 0; }

	data_type* raw_data() { return array; }
	constexpr data_type* raw_data() const { return array; }
	char* char_raw_data() { return reinterpret_cast<char*>( array ); }
	const char* char_raw_data() const { return reinterpret_cast<const char*>(array); }
	size_type char_width_step() const { return width_step() * sizeof( data_type ); }

	constexpr data_type& data_ref( const shift_type row, const shift_type col ) const {
		return array[ row * _width_step + col ];
	}
	data_type& data_ref( const shift_type row, const shift_type col ) {
		return array[ row * _width_step + col ];
	}

	constexpr data_type* row_data( const shift_type row )const {
		return array + row * _width_step;
	}
	data_type* row_data( const shift_type row ){
		return array + row * _width_step;
	}
protected:
	bool revalidate_storage( const size_t width, const size_t height ){
		/****
		 * Data is lost when calling this function
		 */
		const size_t fixed_width = mptl::ceil_binary_power<4>(width);

		return
				revalidate_storage_size( fixed_width, height ) &&
				revalidate_storage_position( width, height );
	}
	virtual bool revalidate_storage_size( const size_type width_step, const size_type height ) = 0;
	virtual bool revalidate_storage_position( const size_type width, const size_type height ) = 0;

protected:
	offset_type _offset;
	size_type _width, _height;
	size_type sample_distance;
	shift_type _width_step;
	data_type* array;

};

template<class T>
basic_image_signal<T>::basic_image_signal(size_type w, size_type h, shift_type wS):
		_offset{0.0,0.0},
		_width(w),
		_height(h),
		sample_distance( 1 ),
		_width_step(wS),
		array(nullptr)
{ }
template<class T>
basic_image_signal<T>::~basic_image_signal(){}

template <typename T, bool is_const> inline
point_reference_template<T,is_const>::point_reference_template( basic_image_signal<data_type>& im, int y, int x )
:point(im[y] + x),
 width_step(im.width_step())
{}
template <typename T, bool is_const> inline
point_reference_template<T,is_const>::point_reference_template( const basic_image_signal<data_type>& im, int y, int x )
:point(im[y] + x),
 width_step(im.width_step())
{}


class default_wrapped_image_delegation{
public:
	typedef typename basic_image_signal_base::size_type size_type;
	typedef typename basic_image_signal_base::offset_type offset_type;
	typedef typename basic_image_signal_base::raw_offset_type raw_offset_type;

	virtual bool revalidate_storage_size( const size_type width_step, const size_type height ) {
		cerr << "default_wrapped_image_delegation never returns true in " << __func__ << "\n";
		return false; }
	virtual bool revalidate_storage_position( const size_t width, const size_t height ) {
		cerr << "default_wrapped_image_delegation never returns true in " << __func__ << "\n";
		return false; }
	virtual bool valid_shift_data_offset( const raw_offset_type& shift ) const {
		cerr << "default_wrapped_image_delegation never returns true in " << __func__ << "\n";
		return false; }
	virtual ~default_wrapped_image_delegation() {}
};


template < typename T, typename U = default_wrapped_image_delegation >
class wrapped_image_signal : public basic_image_signal<T>{
public:
	using typename basic_image_signal<T>::data_type;
	using typename basic_image_signal<T>::size_type;
	using typename basic_image_signal<T>::shift_type;
	using typename basic_image_signal<T>::offset_type;
	using typename basic_image_signal<T>::raw_offset_type;

#ifndef BASICIMAGE_NO_CV
	wrapped_image_signal(IplImage* im, U del_obj = U());
#endif
	wrapped_image_signal(basic_image_signal<T>* im, U del_obj = U());
	wrapped_image_signal(T* image_data, int width, int height, int width_step, U del_obj = U());
	basic_image_signal<T>& as_is(){ return *this; }

	virtual ~wrapped_image_signal() override { };

	virtual bool revalidate_storage_size( const size_type width_step, const size_type height ) override {
		return delegation_object.revalidate_storage_size( width_step, height ); }
	virtual bool revalidate_storage_position( const size_type width, const size_type height ) override {
		return delegation_object.revalidate_storage_position( width, height ); }
	virtual bool valid_shift_data_offset( const raw_offset_type& shift ) const override {
		return delegation_object.valid_shift_data_offset( shift );
	}


private:
	U delegation_object;
};

#ifndef BASICIMAGE_NO_CV
template < typename T, typename U >
wrapped_image_signal<T,U>::wrapped_image_signal(IplImage* im, U del_obj):
	basic_image_signal<T>(im->width, im->height, im->widthStep / sizeof(T))
{
	basic_image_signal<T>::array = reinterpret_cast<T*>(im->imageData);
}
#endif

template < typename T, typename U >
wrapped_image_signal<T,U>::wrapped_image_signal(basic_image_signal<T>* im, U del_obj):
basic_image_signal<T>(im->width, im->height, im->width_step)
{
	basic_image_signal<T>::array = im->array;
}

template < typename T, typename U >
wrapped_image_signal<T,U>::wrapped_image_signal(T* image_data, int width, int height, int width_step, U del_obj):
basic_image_signal<T>(width, height, width_step)
{
	basic_image_signal<T>::array = image_data;
}





template < typename T >
class simple_image_signal: public basic_image_signal<T>{
//	simple_image_signal& operator=(const simple_image_signal&) = delete;
//	simple_image_signal( const simple_image_signal& ) = delete;
public:
	using typename basic_image_signal<T>::data_type;
	using typename basic_image_signal<T>::size_type;
	using typename basic_image_signal<T>::shift_type;
	using typename basic_image_signal<T>::offset_type;
	using typename basic_image_signal<T>::raw_offset_type;

	simple_image_signal():
		basic_image_signal<T>(),
		internal_array()
		{}
	simple_image_signal(
			size_type w,
			size_type h,
			size_type extra_width = 0
	):
	basic_image_signal<T>( w, h, w + extra_width ),
	internal_array( basic_image_signal<T>::_width_step * basic_image_signal<T>::_height )
	{
		basic_image_signal<T>::array = internal_array.data();
	}
	simple_image_signal( simple_image_signal&& original ) noexcept:
		basic_image_signal<T>( std::move(original) ),
		internal_array( std::move( original.internal_array ) )
	{ }
	simple_image_signal& operator=( simple_image_signal&& original )  = default;

	virtual bool revalidate_storage_size( const size_type width_step, const size_type height ) override {
		const size_t needed_capacity = width_step * height;
		if( internal_array.capacity() < needed_capacity ){
			cerr << " internal_array.capacity() : " << internal_array.capacity() << "  relative to needed capacity : " << needed_capacity << "\n";
			internal_array.reserve( needed_capacity );
		}
		basic_image_signal<T>::_width_step = width_step;
		return true;
	}

	virtual bool revalidate_storage_position( const size_type width, const size_type height ) override {
		basic_image_signal<T>::_width = width;
		basic_image_signal<T>::_height = height;
		basic_image_signal<T>::array = internal_array.end() - basic_image_signal<T>::total_size();
		return true;
	}

	virtual bool valid_shift_data_offset( const raw_offset_type& shift ) const override {
		const int shift_size = shift.y * basic_image_signal<T>::_width_step + shift.x;
		const data_type* const test_data_pos = basic_image_signal<T>::array + shift_size;
		auto storage_range = internal_array.storage_range();
//		cerr << "storage_range.in_closed_open( test_data_pos  ) : " << storage_range.in_closed_open( test_data_pos  )
//				<< "storage_range.in_closed( test_data_pos + total_size() ) : " << storage_range.in_closed( test_data_pos + basic_image_signal<T>::total_size() )<<"\n";
//		cerr << storage_range << " " << test_data_pos << " + " << basic_image_signal<T>::total_size() << " = " << test_data_pos + basic_image_signal<T>::total_size() << "\n";
		return
				storage_range.in_closed_open( test_data_pos  ) &&
				storage_range.in_closed( test_data_pos + basic_image_signal<T>::total_size() );
	}

	virtual ~simple_image_signal() override { };
	private:
	mptl::no_copy_data_holder<T> internal_array;
};




}

#endif /* IMAGE_SIGNAL_DEFINITION_H_ */
