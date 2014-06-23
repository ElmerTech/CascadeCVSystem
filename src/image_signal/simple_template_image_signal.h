/*
 * simple_template_image_signal.h
 *
 *  Created on: 13 dec 2010
 *      Author: Johan
 */

#ifndef TEMPLATE_IMAGE_SIGNAL_DEFINITION_H_
#define TEMPLATE_IMAGE_SIGNAL_DEFINITION_H_

#include "simple_image_signal.h"

namespace _basic_image_signal {

template<int widthstep, class T> class basic_template_image_signal{
public:
	basic_template_image_signal(int w, int h);
	basic_template_image_signal(const basic_image_signal<T>& original);
	virtual ~basic_template_image_signal();
	static const int width_step = widthstep;

	int width, height;
	T* array;

	T* operator[](const int row){return array + row * widthstep;}
	const T* operator[](const int row) const {return array + row * widthstep;}
};

template<int widthstep, class T>
basic_template_image_signal<widthstep, T>::basic_template_image_signal(
		int w,
		int h):
		width(w),
		height(h)
{
	array = 0;
}

template<int widthstep, class T>
basic_template_image_signal<widthstep, T>::basic_template_image_signal(
		const basic_image_signal<T>& original):
		width(original.width),
		height(original.height){
	if( original.width_step != widthstep )
		throw 0;
	array = original.array;
}

template<int widthstep, class T>
basic_template_image_signal<widthstep, T>::~basic_template_image_signal(){}



template<int widthstep, class T>
class basic_template_image_signalPoint{
	basic_template_image_signalPoint& operator=(const basic_template_image_signalPoint&);
	static const int width_step;
	T* point;

public:

	T* operator[](const int row){return point + row * widthstep;}
	const T* operator[](const int row) const {return point + row * widthstep;}
	basic_template_image_signalPoint(const basic_template_image_signalPoint<widthstep,T>& original):point(original.point){}
	basic_template_image_signalPoint(T* const & p):point(p){}
	basic_template_image_signalPoint(basic_template_image_signal<widthstep,T>& im, int y, int x)
	:point(im[y] + x)
	{}
};


template<int widthstep, class T> class simple_template_image_signal: public basic_template_image_signal<widthstep, T>{
	simple_template_image_signal& operator=(const simple_template_image_signal&);
	simple_template_image_signal(const simple_template_image_signal&);
public:
	simple_template_image_signal(int w, int h,  int horizontal_margin = 0, int vertical_margin = 0);
	virtual ~simple_template_image_signal();
private:
	T* internalArray;
};

template<int widthstep, class T>
simple_template_image_signal<widthstep, T>::simple_template_image_signal(int w, int h, int horizontal_margin, int vertical_margin)
:basic_template_image_signal<widthstep, T>::basic_template_image_signal(w, h)
{
	if(widthstep < w + horizontal_margin )
		throw 0;
	int size = widthstep * ( basic_template_image_signal<widthstep,T>::height + 2*vertical_margin);
	internalArray = new T[size];
	basic_template_image_signal<widthstep,T>::array = internalArray + widthstep * vertical_margin + horizontal_margin;
}

template<int widthstep, class T>
simple_template_image_signal<widthstep, T>::~simple_template_image_signal(){
	if(internalArray)
		delete[] internalArray;
}

}


#endif /* TEMPLATE_IMAGE_SIGNAL_DEFINITION_H_ */
