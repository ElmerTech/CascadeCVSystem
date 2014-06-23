/*
 * Utility.h
 *
 *  Created on: 30 sep 2010
 *      Author: Johan
 */

#ifndef GENERAL_SIGNAL_UTILITY_H_
#define GENERAL_SIGNAL_UTILITY_H_

#include <cmath>
#include <iostream>
using std::cerr;


namespace _basic_image_signal{
//
//template<class T> inline T max(const T& a, const T& b){
//	return a > b ? a : b;
//}
//
//template<class T> inline T min(const T& a, const T& b){
//	return a < b ? a : b;
//}

template<class T> inline T array_max(const T* array, const int size){
	T current_max = array[0];
	for(int i = 1; i< size; i++){
		if(current_max < array[i])
			current_max = array[i];
	}
	return current_max;
}


template<class T> inline T array_min(const T* array, const int size){
	T current_min = array[0];
	for(int i = 1; i< size; i++){
		if(array[i] < current_min)
			current_min = array[i];
	}
	return current_min;
}


template<class T> inline void array_max_min(T* max, T* min, const T* array, const int size){
	T current_max = array[0];
	T current_min = array[0];
	for(int i = 1; i< size; i++){
		if(array[i] < current_min){
			current_min = array[i];
		}else if(current_max < array[i]){
			current_max = array[i];
		}
	}
	*max = current_max;
	*min = current_min;
}



/****
 * 	Converts float to int, round to nearest.
 */
inline int f2i(const float value){
	return value >=0 ? value + .5f : value - .5f;
}
inline int f2i_p(const float value){
	return value + .5f;
}

inline double euclidean_remainder(const double numerator, const double denominator){
	const double remainder = std::fmod(numerator, denominator);
	return remainder >= 0 ? remainder : remainder + denominator;
}


/********
 * Sorting algorithms
 *
 *
 */


template<class T> void modified_merge_sort(T* const array, const int size){
	if(size <= 1) //Array length 1, already sorted
		return;
	int index = 1;
	int nofTrends = 0;
	bool increasing_trend = *array[0] < *array[1];
	bool increase = false;

	while(index < size){
		if( (increase = (*array[index-1] < *array[index])) != increasing_trend ){
			increasing_trend = increase;
			nofTrends++;
		}
		index++;
	}
	//cerr << "Trends: " << nofTrends << ", ";
}

template<class T> inline void swap(T& a, T& b){
	T temp = a;
	a = b;
	b = temp;
}

template<class T, bool compare(T const & a, T const & b)> inline void merge_two_sorted_arrays(T* dst, const int dst_size, const T* a, const T* const a_end, const T* b, const T* const b_end){
	T* const dst_end = dst + dst_size;
	if(((b_end-b) + (a_end - a) != dst_size ) || dst_size<=0)
		throw 0;


	bool done = false;
	while(!done){
		if( compare(*b,*a)){
			if(dst>=dst_end)
				throw 0;
			*dst++ = *b++;
			if(b >= b_end){
				while(a < a_end)
					*dst++ = *a++;
				done = true;
			}
		}else{
			if(dst>=dst_end)
				throw 0;
			*dst++ = *a++;
			if(a >= a_end){
				while(b < b_end){
					if(dst>=dst_end)
						throw 0;
					*dst++ = *b++;
				}
				done = true;
			}
		}
	}
}


template<class T, bool compare(T const & a, T const & b)> void merge_sort(T* const array, const int size){
	if(size <= 1) //Array length 1, already sorted
		return;

	T* working_array = new T[2*size];
	T* arrays[2] = {working_array, working_array + size};
	int sort_size = 1;

	const T* const src = array;
	T* const dst = arrays[0];

	int i = 0;
	for(i = 0; i+1 < size; i+=2){
		const T* const src = array;
		T* const dst = arrays[0];

		if(compare(src[i+1],src[i])){
			dst[i] = src[i+1];
			dst[i+1] = src[i];
		}else{
			dst[i] = src[i];
			dst[i+1] = src[i+1];
		}
	}
	const int left = size - i;
	if(left == 1)
		dst[i] = src[i];
	else if( left > 1)
		throw 0;

	while((sort_size *= 2) < size){
		const int step_size = 2 * sort_size;
		swap(arrays[0], arrays[1]);
		const T* const src = arrays[1];
		T* const dst = arrays[0];

		int i = 0;
		for(i = 0; i + step_size < size; i += step_size){
			const T* const a = src + i;
			const T* const a_end = a + sort_size;
			const T* const b = a_end;
			const T* const b_end = b + sort_size;
			merge_two_sorted_arrays<T,compare>(dst + i, step_size, a, a_end, b, b_end);
		}
		const int left = size - i;
		if(left > sort_size){
			const T* const a = src + i;
			const T* const a_end = a + sort_size;
			const T* const b = a_end;
			const T* const b_end = src + size;
			merge_two_sorted_arrays<T,compare>(dst + i, left, a, a_end, b, b_end);
		}else //if(left>0)
			memcpy(dst + i, src + i, sizeof(T[left]));
	}
	memcpy(array, arrays[0], sizeof(T[size]));
	delete[] working_array;
}





template<class T> inline void merge_two_sorted_arrays(T* dst, const int dst_size, const T* a, const T* const a_end, const T* b, const T* const b_end){
	T* const dst_end = dst + dst_size;
	if(((b_end-b) + (a_end - a) != dst_size ) || dst_size<=0)
		throw 0;


	bool done = false;
	while(!done){
		if( *b > *a){
			if(dst>=dst_end)
				throw 0;
			*dst++ = *b++;
			if(b >= b_end){
				while(a < a_end)
					*dst++ = *a++;
				done = true;
			}
		}else{
			if(dst>=dst_end)
				throw 0;
			*dst++ = *a++;
			if(a >= a_end){
				while(b < b_end){
					if(dst>=dst_end)
						throw 0;
					*dst++ = *b++;
				}
				done = true;
			}
		}
	}
}



template<class T> void merge_sort(T* const array, const int size){
	if(size <= 1) //Array length 1, already sorted
		return;

	T* working_array = new T[2*size];
	T* arrays[2] = {working_array, working_array + size};
	int sort_size = 1;

	const T* const src = array;
	T* const dst = arrays[0];

	int i = 0;
	for(i = 0; i+1 < size; i+=2){
		const T* const src = array;
		T* const dst = arrays[0];

		if(src[i+1] > src[i]){
			dst[i] = src[i+1];
			dst[i+1] = src[i];
		}else{
			dst[i] = src[i];
			dst[i+1] = src[i+1];
		}
	}
	const int left = size - i;
	if(left == 1)
		dst[i] = src[i];
	else if( left > 1)
		throw 0;

	while((sort_size *= 2) < size){
		const int step_size = 2 * sort_size;
		swap(arrays[0], arrays[1]);
		const T* const src = arrays[1];
		T* const dst = arrays[0];

		int i = 0;
		for(i = 0; i + step_size < size; i += step_size){
			const T* const a = src + i;
			const T* const a_end = a + sort_size;
			const T* const b = a_end;
			const T* const b_end = b + sort_size;
			merge_two_sorted_arrays(dst + i, step_size, a, a_end, b, b_end);
		}
		const int left = size - i;
		if(left > sort_size){
			const T* const a = src + i;
			const T* const a_end = a + sort_size;
			const T* const b = a_end;
			const T* const b_end = src + size;
			merge_two_sorted_arrays(dst + i, left, a, a_end, b, b_end);
		}else //if(left>0)
			memcpy(dst + i, src + i, sizeof(T[left]));
	}
	memcpy(array, arrays[0], sizeof(T[size]));
	delete[] working_array;
}


class Timer{
	static const long long ticks_per_time;
	long long startTime;
	long long stopTime;
	long long get_current_time();
public:
	Timer();
	void mark();
	long long get_ticks();
	double get_time();
	float get_timef();
	double get_meantime();
	float get_meantimef();
};

}

#endif /* GENERAL_SIGNAL_UTILITY_H_ */
