/*
 * cvDebug.h
 *
 *  Created on: 8 nov 2010
 *      Author: Johan
 */

#ifndef CVDEBUG_H_
#define CVDEBUG_H_

#include <cstring>
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "image_signal/simple_image_signal_utility.h"


template<class T> void showDebugPlot(const T* const array, const int size, const unsigned char value = 255, bool wait_key = true){
	const char* win_name = "_my_cv_debug_";
	IplImage* debugim = cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,1);

	std::memset(debugim->imageData, 0, debugim->imageSize);
	using _basic_image_signal::wrapped_image_signal;
	using _basic_image_signal::MONO8;

	wrapped_image_signal<MONO8> wrapped_debug_im(debugim);
	drawPlot( array, size, dynamic_cast< _basic_image_signal::basic_image_signal<MONO8>& >(wrapped_debug_im), value );
	cvShowImage(win_name, debugim);
	if(wait_key)
		cvWaitKey(0);
	cvReleaseImage(&debugim);
}

template<class T> void showDebugPlot(const T* const index, const T* const array, const int size, const unsigned char value = 255){
	const char* win_name = "_my_cv_debug_index_";
	IplImage* debugim = cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,1);

	std::memset(debugim->imageData, 0, debugim->imageSize);
	using _basic_image_signal::wrapped_image_signal;
	using _basic_image_signal::MONO8;

	wrapped_image_signal<MONO8> wrapped_debug_im(debugim);
	drawPlot(index, array, size, wrapped_debug_im, value);
	cvShowImage(win_name, debugim);
	cvWaitKey(0);
	cvReleaseImage(&debugim);
}

#endif /* CVDEBUG_H_ */
