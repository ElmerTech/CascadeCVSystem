/*
 * Utility.cpp
 *
 *  Created on: 19 okt 2010
 *      Author: Johan
 */

#include "Utility.h"



#ifdef _WIN32

#include <windows.h>

namespace _basic_image_signal {

long long Get_tick_per_time_from_system(){
	LARGE_INTEGER tmp;
	QueryPerformanceFrequency(&tmp);
	return tmp.QuadPart;
}

inline long long Timer::get_current_time(){
	LARGE_INTEGER tmp;
	QueryPerformanceCounter(&tmp);
	return tmp.QuadPart;
}

} // namespace _basic_image_signal


#else //NOT DEFINED _WIN32

#include <time.h>

namespace _basic_image_signal {

static const long long NSEC_PER_SEC = 1e9;

static inline long long timespec_to_ns(const struct timespec *ts)
{
	return ((long long) ts->tv_sec * NSEC_PER_SEC) + ts->tv_nsec;
}

static const long long TICKS_PER_S = NSEC_PER_SEC;

long long Get_tick_per_time_from_system(){
	return TICKS_PER_S;
}

long long Timer::get_current_time(){
	timespec tmp;
	if(clock_gettime(CLOCK_MONOTONIC, &tmp)) cerr << "cp\n";
	return timespec_to_ns(&tmp);
}


#endif


const long long Timer::ticks_per_time = Get_tick_per_time_from_system();


Timer::Timer(){
	mark();
	startTime = stopTime;
}

void Timer::mark(){
	startTime = stopTime;

	stopTime = get_current_time();
}


long long Timer::get_ticks(){
	return (stopTime>startTime) ? stopTime - startTime: -1;
}

double Timer::get_time(){
	return (double)get_ticks() / ticks_per_time;
}

float Timer::get_timef(){
	return (float)get_ticks() / ticks_per_time;
}

double Timer::get_meantime(){
	const double diff = get_current_time() - startTime;

	return ( diff > 0) ? diff / ticks_per_time : -1;
}
float Timer::get_meantimef(){
	const float diff = get_current_time() - startTime;

	return ( diff > 0) ? diff / ticks_per_time : -1;

}
}
