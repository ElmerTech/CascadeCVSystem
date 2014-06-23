/*
 * RGBColors.h
 *
 *  Created on: 15 okt 2010
 *      Author: Johan
 */

#ifndef IMAGE_SIGNAL_RGB_COLOR_TYPES_H_
#define IMAGE_SIGNAL_RGB_COLOR_TYPES_H_

#include <cmath>

namespace _basic_image_signal {

template<class T> struct RGBSTRUCT;
template<class T> struct BGRSTRUCT;

template<class T>
struct RGBSTRUCT{
	T values[3];
	RGBSTRUCT(){}
	RGBSTRUCT( T r, T g, T b ){ values[0] = r; values[1] = g; values[2] = b;}
	operator BGRSTRUCT<T>(){ return BGRSTRUCT<T>( values[2], values[1], values[0]);}
	operator unsigned char(){ return sqrt((values[0]*(double)values[0] + values[1]*(double)values[1] + values[2]*(double)values[2])/3);}
};

template<class T>
struct BGRSTRUCT{
	T values[3];
	BGRSTRUCT(){}
	BGRSTRUCT(T b, T g, T r){values[0] = b; values[1] = g; values[2] = r;}
	BGRSTRUCT(T gray){values[0] = gray; values[1] = gray; values[2] = gray;}
	operator RGBSTRUCT<T>() const { return RGBSTRUCT<T>(values[2],values[1],values[0]);}
	operator unsigned char() const { return std::sqrt((values[0]*(double)values[0] + values[1]*(double)values[1] + values[2]*(double)values[2])/3);}
	template<class S>
	BGRSTRUCT<T>& operator =(const BGRSTRUCT<S>& operand){
		values[0] = operand.values[0];
		values[1] = operand.values[1];
		values[2] = operand.values[2];
		return *this;
	}
	template<class S>
	BGRSTRUCT<T>& operator +=(const BGRSTRUCT<S>& operand){
		values[0] += operand.values[0];
		values[1] += operand.values[1];
		values[2] += operand.values[2];
		return *this;
	}
	template<class S>
	BGRSTRUCT<T>& operator *=(const S& operand){
		values[0] *= operand;
		values[1] *= operand;
		values[2] *= operand;
		return *this;
	}
};

template<class T, class S>
BGRSTRUCT<T> operator -(const BGRSTRUCT<T>& value, const BGRSTRUCT<S>& operand)  {
	return BGRSTRUCT<T>(
			value.values[0] - operand.values[0],
			value.values[1] - operand.values[1],
			value.values[2] - operand.values[2] );
}
template<class T, class S>
BGRSTRUCT<T> operator +(const BGRSTRUCT<T>& value, const BGRSTRUCT<S>& operand)  {
	return BGRSTRUCT<T>(
			value.values[0] - operand.values[0],
			value.values[1] - operand.values[1],
			value.values[2] - operand.values[2] );
}
template<class T>
BGRSTRUCT<T> operator *(double d, const BGRSTRUCT<T>& operand)  {
	return BGRSTRUCT<T>(
			d * operand.values[0],
			d * operand.values[1],
			d * operand.values[2] );
}


template<class T>
struct RGBREDSTRUCT: public RGBSTRUCT<T>{
	RGBREDSTRUCT(T r){
		RGBSTRUCT<T>::values[0] = r;
		RGBSTRUCT<T>::values[1] = 0;
		RGBSTRUCT<T>::values[2] = 0;
	}
	RGBREDSTRUCT& operator=(const T& r){
		RGBSTRUCT<T>::values[0] = r;
		return *this;
	}
};
template<class T>
struct RGBGREENSTRUCT: public RGBSTRUCT<T>{
	RGBGREENSTRUCT(T g){
		RGBSTRUCT<T>::values[0] = 0;
		RGBSTRUCT<T>::values[1] = g;
		RGBSTRUCT<T>::values[2] = 0;
	}
	RGBGREENSTRUCT& operator=(const T& g){
		RGBSTRUCT<T>::values[1] = g;
		return *this;
	}
};
template<class T>
struct RGBBLUESTRUCT: public RGBSTRUCT<T>{
	RGBBLUESTRUCT(T b){RGBSTRUCT<T>::values[0] = 0; RGBSTRUCT<T>::values[1] = 0; RGBSTRUCT<T>::values[2] = b;}
	RGBBLUESTRUCT& operator=(const T& b){RGBSTRUCT<T>::values[2] = b;return *this;}
};



template<class T>
struct BGRREDSTRUCT: public BGRSTRUCT<T>{
	BGRREDSTRUCT(T r){
		BGRSTRUCT<T>::values[0] = 0;
		BGRSTRUCT<T>::values[1] = 0;
		BGRSTRUCT<T>::values[2] = r;
	}
	BGRREDSTRUCT& operator=(const T& r){
		BGRSTRUCT<T>::values[2] = r;
		return *this;
	}
};
template<class T>
struct BGRGREENSTRUCT: public BGRSTRUCT<T>{
	BGRGREENSTRUCT(T g){
		BGRSTRUCT<T>::values[0] = 0;
		BGRSTRUCT<T>::values[1] = g;
		BGRSTRUCT<T>::values[2] = 0;
	}
	BGRGREENSTRUCT& operator=(const T& g){
		BGRSTRUCT<T>::values[1] = g;
		return *this;
	}
};
template<class T>
struct BGRBLUESTRUCT: public BGRSTRUCT<T>{
	BGRBLUESTRUCT(T b){
		BGRSTRUCT<T>::values[0] = b;
		BGRSTRUCT<T>::values[1] = 0;
		BGRSTRUCT<T>::values[2] = 0;
	}
	BGRBLUESTRUCT& operator=(const T& b){
		BGRSTRUCT<T>::values[0] = b;
		return *this;
	}
};


template<class T>
struct BGRGRAYSTRUCT: public BGRSTRUCT<T>{
	BGRGRAYSTRUCT(T g){
		BGRSTRUCT<T>::values[0] = g;
		BGRSTRUCT<T>::values[1] = g;
		BGRSTRUCT<T>::values[2] = g;
	}
	BGRGRAYSTRUCT& operator=(const T& g){
		BGRSTRUCT<T>::values[0] = g;
		BGRSTRUCT<T>::values[1] = g;
		BGRSTRUCT<T>::values[2] = g;
		return *this;
	}
};

template<class T>
BGRSTRUCT<T>& operator<<(BGRSTRUCT<T>& target, BGRBLUESTRUCT<T>& source){
	target.values[0] = source[0];
	return target;
}

template<class T>
BGRSTRUCT<T>& operator<<(BGRSTRUCT<T>& target, BGRGREENSTRUCT<T>& source){
	target.values[1] = source[1];
	return target;
}

template<class T>
BGRSTRUCT<T>& operator<<(BGRSTRUCT<T>& target, BGRREDSTRUCT<T>& source){
	target.values[2] = source[2];
	return target;
}

}

#endif /* IMAGE_SIGNAL_RGB_COLOR_TYPES_H_ */
