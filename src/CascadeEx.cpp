/*
 * CascadeEx.cpp
 *
 *  Created on: 1 sep 2010
 *      Author: Johan
 */

#include <opencv2/opencv.hpp>
#include <cstdio>
#include <iostream>
#include <cstring>

#include "CVSystem/CVSEvaluation.h"
#include "image_signal/simple_image_signal_utility.h"
#include "image_signal/simple_image_signal_draw.h"

using namespace std;

#include "CVSystem/CVSystem.h"
#include "CVSystem/filter_impl.h"

//#include "testsystem.h"
#include "cvDebug.h"

#include "RealTimePlot.h"
#include "DotPlot.h"


#include "utilities/type_utils.h"
using mptl::lref;


#include "CVSystem/CompileConfig.h"
using compile_cascade_config::make_config;


void DiscernMouseCallback(int event, int x, int y, int flags, void* param);


//static const int TEST_FILTER_SIZE = 21;


//static const int nof_colors = 12;
//static const BGR8 colors[nof_colors] = {BGR8(0,0,255), BGR8(0,127,255), BGR8(0,255,255), BGR8(0,255,127),
//								 BGR8(0,255,0), BGR8(127,255,0), BGR8(255,255,0), BGR8(255,127,0),
//								 BGR8(255,0,0), BGR8(255,0,127), BGR8(255,0,255), BGR8(127,0,255)};


//using CascadeCVSTemplate::scale_pyramid;
using CascadeCVSTemplate::apply_filter;
using CascadeCVSTemplate::box_filter;

using _basic_image_signal::wrapped_image_signal;
using _basic_image_signal::simple_image_signal;
using _basic_image_signal::BGR8;
using _basic_image_signal::MONO8;
using _basic_image_signal::BGR_GRAY_8;
using _basic_image_signal::apply_filter_test;



constexpr const char config_string_50[] =
		"-- [2] -- [2] --                             L| 1.0, 8e-2 |"
		"-- [2] -- [2] -- [2] -- [2] --               L| 1.4, 3e-4 |"
		"-- [2] -- [4] --                             L| 2.0, 5.0e-6 |"
		"-- [5] >> [5] --                             L| 2.8, 1.0e-8 |"
		"-- [2] >> [6] -- [7]--                       L| 4.0, 1.0 |"
		">> [9] vv [5] --                             L| 5.6, 1.0 |"
		">> [2] -- [6] >> [7] --                      L| 8.0, 1.0 |"
		"-- [7] v> [2] -- [2] -- [2] >> [3] -- [4] -- L|11.2, 1.0 |"
		"-- [2] >> [4] -- [6] >> [6]--                L|16.0, 1.0 |";

typedef typename make_config<config_string_50>::conf_type conf_type;
typedef typename CascadeCVSTemplate::CascadedCVSystem<conf_type> CascadedCVSystem50;
typedef typename CascadedCVSystem50::cvs_scale_pyramid cvs_scale_pyramid;
typedef typename CascadeCVSTemplate::CascadedCVSystem<conf_type>::CascadeCVSEval CascadeCVSEval;


#ifdef HAVE_NEOEX_SURF
#include "SURFSystem.h"
#include "SURFDrawing.h"
#include "SURFEvaluationInterface.h"
#include "SURFSystem/SURFEvaluation.h"

using _BasicImage::BasicImage;
using _BasicImage::WrappedImage;
using BIBGR8 = _BasicImage::BGR8;
using BIMONO8 = _BasicImage::MONO8;
using NexExSURFSystem::SURFSystem;
using NexExSURFSystem::DiscernMap;
using NexExSURFSystem::SURFEvaluationInterface;
#endif

int main(){


//	std::cout << "\n" << (conf_type::nof_levels) << "\n";
	conf_type::print( std::cout );

	CascadedCVSystem50 image_matching(11);

	std::cout << image_matching.get_absolute_threshold()<< "\n";
	image_matching.set_absolute_threshold( 200 );


	CvCapture *capture = 0;
	IplImage  *frame = 0;
	IplImage  *gray = 0;
	IplImage  *monoIm2 = 0;
	IplImage  *monoFrame = 0;


	IplImage  *disIm = 0;

	static const bool show_gray = true;
	static const bool show_discern = true;


	int red_scale = 0;
	int green_scale = 1;
	int blue_scale = 2;



	capture = cvCaptureFromCAM( 0 );
	/* always check */
	if ( !capture ) {
		fprintf( stderr, "Cannot open initialize webcam!\n" );
		return 1;
	}

	frame = cvQueryFrame( capture );

	/* always check */
	if( !frame ){
		fprintf(stderr, "frame NULL ");
		cvReleaseCapture(&capture);
		return 0;
	}


	CvSize frameSize = cvSize(frame->width,frame->height);
	gray = cvCreateImage(frameSize,IPL_DEPTH_8U,3);
	monoIm2 = cvCreateImage(frameSize,IPL_DEPTH_8U,1);
	monoFrame = cvCreateImage(frameSize,IPL_DEPTH_8U,1);
	IplImage* fltIm = cvCreateImage(frameSize,IPL_DEPTH_32F,1);


	disIm = cvCreateImage(frameSize,IPL_DEPTH_8U,3);

	cvNamedWindow( "discern", CV_WINDOW_AUTOSIZE );

	const int nof_filters = image_matching.nof_filters( );
	cvCreateTrackbar("Red scale  ","discern", &red_scale, nof_filters - 1,0);
	cvCreateTrackbar("Green scale","discern", &green_scale, nof_filters - 1,0);
	cvCreateTrackbar("Blue scale ","discern", &blue_scale, nof_filters - 1,0);


	/*********
	 *
	 * SURF Evaluation variables
	 *
	 *
	 */

	wrapped_image_signal<BGR8> image_matching_debug_im( gray );
	image_matching.set_debug_image( &image_matching_debug_im );

	CascadeCVSEvaluationInterface* eval_object = image_matching.get_eval_object( );

	IplImage* time_plot_image = cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,3);
	RealTimePlot<BGR8> time_plots("time plot", time_plot_image);

	time_plots.add_plot( eval_object->get_time_series( CascadeCVSEval::TimeToCreateIntegralImage ), eval_object->get_series_length(), BGR8(255,255,255));

	time_plots.add_plot( eval_object->get_time_series( CascadeCVSEval::TimeToFilter ), eval_object->get_series_length(), BGR8(255,0,0));
	time_plots.add_plot( eval_object->get_time_series( CascadeCVSEval::TimeToSelectInterestPoints ), eval_object->get_series_length(), BGR8(0,255,0));
	time_plots.add_plot( eval_object->get_time_series( CascadeCVSEval::TimeToCreateDescriptors ), eval_object->get_series_length(), BGR8(0,0,255));
	time_plots.add_plot( eval_object->get_time_series( CascadeCVSEval::TimeToMatch ), eval_object->get_series_length(), BGR8(255,0,255));

	time_plots.add_plot( eval_object->get_time_series( CascadeCVSEval::TimeToCalculateOrientation), eval_object->get_series_length(), BGR8(127,127,255));
	time_plots.add_plot( eval_object->get_time_series( CascadeCVSEval::TimeToCalculateDescriptorsWithOrientation), eval_object->get_series_length(), BGR8(0,0,127));



	IplImage* dot_plot_image = cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,3);
	DotPlot<BGR8> dot_plots("dot plot", dot_plot_image, eval_object->get_nof_interest_point_series(), eval_object->get_series_length());

	dot_plots.add_plot( eval_object->get_time_series( CascadeCVSEval::TimeToFilter ), BGR8(255,0,0));
	dot_plots.add_plot( eval_object->get_time_series( CascadeCVSEval::TimeToSelectInterestPoints ), BGR8(0,255,0));
	dot_plots.add_plot( eval_object->get_time_series( CascadeCVSEval::TimeToCreateDescriptors ), BGR8(0,0,255));
	dot_plots.add_plot( eval_object->get_time_series( CascadeCVSEval::TimeToMatch ), BGR8(255,0,255));



	simple_image_signal<int> flip_image(frame->width,frame->height);

	cvNamedWindow("discern",0);
	cvNamedWindow("f2");
	cvMoveWindow("f2",0 ,0);
	cvNamedWindow("f3");
	cvMoveWindow("f3",0,480);

	if(false){
		cvNamedWindow("test filter",0);
	}
	simple_image_signal<int32_t> filt_test_im(768,768);
	_basic_image_signal::clear_image( filt_test_im );
	const int test_width = 640;
	const int test_height = 480;
	int total_gain = 1;
	filt_test_im.shift_data_offset(50,50);
	filt_test_im.offset( {0,0} );
	filt_test_im.set_raw_size(test_width,test_height);
	filt_test_im[test_height>>1][test_width>>1] = 255;
//	for(int i = -200;i<=200;i++){
//		for(int j=-1;j<=1;j++){
////			if(i*i + j*j < 100)
//				filt_test_im[j+(test_height>>1)][i+(test_width>>1)] = 2;
//		}
//	}
//	filt_test_im.sample_factor(10);

	IplImage* filter_out = cvCreateImage(cvSize(test_width,test_height),IPL_DEPTH_8U,1);
	wrapped_image_signal<unsigned char> filter_out_im( filter_out );

#ifdef HAVE_NEOEX_SURF
	BIU::Timer timer;

	IplImage*  gray2 = cvCreateImage(frameSize,IPL_DEPTH_8U,3);
	IplImage* disIm2 = cvCreateImage(frameSize,IPL_DEPTH_8U,3);

	SURFSystem im_ma(11);
	im_ma.set_hessian_threshold( 400e-0 );
	im_ma.set_interestpoint_limit(10000);
	im_ma.set_image( new WrappedImage<BIBGR8>(gray2) );
	SURFEvaluationInterface* im_ma_eval_object = im_ma.get_eval_object();
	typedef typename SURFTemplate::SURFEvaluation<4> SURFEvaluation;
#endif


	int key = 0;
	bool pause = true;


	while( (0xff & key) != 'q' ) {

		frame = cvQueryFrame( capture );

		/* always check */
		if( !frame ){
			fprintf(stderr, "frame NULL ");
			break;
		}


		if(show_gray){
			copy_image( wrapped_image_signal<BGR8>(frame), wrapped_image_signal<BGR_GRAY_8>(gray).as_is() );
		}

		copy_image( wrapped_image_signal<BGR8>(frame), wrapped_image_signal<MONO8>(monoFrame).as_is());

		int image_no = image_matching.addImage( wrapped_image_signal<MONO8>(monoFrame) );

#ifdef HAVE_NEOEX_SURF
		if(show_gray){
			copy_image( wrapped_image_signal<BGR8>(frame), wrapped_image_signal<BGR_GRAY_8>(gray2).as_is() );
		}

		int image_no_org = im_ma.addImage( new WrappedImage<BIMONO8>(monoFrame));

		cout << "NeoEx SURF filter:    " << im_ma_eval_object->get_time_meassurement(SURFEvaluation::TimeToFilter) << " s\n";
		cout << "NeoEx SURF select:    " << im_ma_eval_object->get_time_meassurement(SURFEvaluation::TimeToSelectInterestPoints) << " s\n";
#endif

		_basic_image_signal::Timer tcv;


		/****
		 *   Test apply box filter struct
		 *
		 */

		if (false) {
			_basic_image_signal::Timer tcv;
			cout << filt_test_im.offset() << " " << filt_test_im.raw_width() << " " << filt_test_im.raw_height() << "\n";
			double max_val = image_signal_max( filt_test_im );
			_basic_image_signal::clear_to<unsigned char>(filter_out_im,255);
			_basic_image_signal::copy_scale_image( filt_test_im, filter_out_im,255/max_val );
			cvShowImage("test filter", filter_out );
			cout << total_gain << " / " << max_val << " = " << ( total_gain / max_val ) << "\n";

			constexpr int bfw1 = 3;
			total_gain *= box_filter<bfw1>::total_gain;
			if( total_gain > (1<<(30-8)) ){
				_basic_image_signal::downshift_image( filt_test_im, 8);
				total_gain >>= 8;
			}
			tcv.mark();
			apply_filter_test<box_filter<bfw1>,int32_t,0>::filter( filt_test_im );
			tcv.mark();
			std::cerr << "apply_filter<box_filter<" << bfw1 << ">,false,int32_t> took " << tcv.get_time()<< " s\n";
		}



		/***
		 *
		 *
		 */


		copy_scale_image( wrapped_image_signal<float>(fltIm), wrapped_image_signal<MONO8>(monoFrame).as_is(),1.0);


		if(show_discern){
			const cvs_scale_pyramid& sp = image_matching.get_pyramid( image_no );
			drawDiscernImage( wrapped_image_signal<BGR8>(disIm).as_is(), sp, red_scale, green_scale, blue_scale);
			cvShowImage("discern", disIm);
		}
		cvShowImage("f2", gray);


#ifdef HAVE_NEOEX_SURF
		if(show_discern){
			const DiscernMap* dm = im_ma.get_discern_map( image_no_org );
			drawDiscernImage( new WrappedImage<BIBGR8>(disIm2), dm, red_scale, green_scale, blue_scale);
			cvShowImage("discern2", disIm2);
		}
		cvShowImage("f3", gray2);
#endif


		key = cvWaitKey( pause ? 0 : 1 );
		if( ( 0xff & key ) == 'p')
			pause = !pause;

	}


	/* free memory */
    cvDestroyAllWindows();
    cvReleaseCapture( &capture );


	return 0;
}


void DiscernMouseCallback(int event, int x, int y, int flags, void* param){
	enum {LeftMouseButton = 0, RightMouseButton, MiddleMouseButton };
	CvPoint* const pos = reinterpret_cast<CvPoint*>(param);

	static int MouseTrack =0;

	if(MouseTrack){
		if(event == CV_EVENT_MOUSEMOVE){
			if(MouseTrack & (1 << LeftMouseButton)){
				pos[0].x = x; pos[0].y = y;
			}
			if(MouseTrack & (1 << RightMouseButton)){
				pos[1].x = x; pos[1].y = y;
			}
		}
	}
	if(event == CV_EVENT_LBUTTONDOWN){
		MouseTrack |= (1 << LeftMouseButton);
		pos[0].x = x; pos[0].y = y;

	}else if(event == CV_EVENT_RBUTTONDOWN){
		MouseTrack |= (1 << RightMouseButton);
		pos[1].x = x; pos[1].y = y;

	}else if(event == CV_EVENT_MBUTTONDOWN){
		MouseTrack |= (1 << MiddleMouseButton);

	}else if(event == CV_EVENT_LBUTTONUP){
		MouseTrack &= ~(1 << LeftMouseButton);
		pos[0].x = x; pos[0].y = y;

	}else if(event == CV_EVENT_RBUTTONUP){
		MouseTrack &= ~(1 << RightMouseButton);
		pos[1].x = x; pos[1].y = y;

	}else if(event == CV_EVENT_MBUTTONUP){
		MouseTrack &= ~(1 << MiddleMouseButton);

	}

}
