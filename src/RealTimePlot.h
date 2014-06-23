/*
 * RealTimePlot.h
 *
 *  Created on: 13 dec 2010
 *      Author: Johan
 */

#ifndef REALTIMEPLOT_H_
#define REALTIMEPLOT_H_

#include <iostream>


#include "opencv/cxcore.h"
#include "opencv/highgui.h"
#include "CascadeImageMatchingConfig.h"
#include "image_signal/simple_image_signal.h"
#include "image_signal/Utility.h"

using _basic_image_signal::wrapped_image_signal;
using CascadeCVSTemplate::image_signal;

template<class CT> class RealTimePlot{
	static const int DEFAULT_CAPACITY = 128;
	IplImage* const cvImage;
	image_signal<CT>* const image;
	const char* const window;

	int plot_capacity;
	int nof_arrays;
	int* plot_array_lengths;
	const float**  plot_arrays;
	float* plot_offsets;
	float* plot_scales;
	CT* plot_colors;




	RealTimePlot(const RealTimePlot&);
	RealTimePlot& operator=(const RealTimePlot&);

	void redraw_plot();
	void redraw_plot(int first_index, int last_index);

public:
	RealTimePlot(const char* window_name, IplImage* im, int capacity = DEFAULT_CAPACITY);

	int add_plot(const float* array, int length, CT color, float offset = 0, float scale = 1);
	void remove_plot(int index);
	void updateplot();
	void updateplot(int first_index, int last_index);
};

template<class CT>  RealTimePlot<CT>::RealTimePlot(const char* window_name, IplImage* im, int capacity):
	cvImage(im),
	image( new wrapped_image_signal<CT>(im) ),
	window(window_name),
	plot_capacity(capacity),
	nof_arrays(0),
	plot_array_lengths(new int[capacity]),
	plot_arrays(new const float*[capacity]),
	plot_offsets(new float[capacity]),
	plot_scales(new float[capacity]),
	plot_colors(new CT[capacity])
{
	for(int i=0;i<capacity;i++){
		plot_array_lengths[i] = -1;
		plot_arrays[i] = 0;
		plot_offsets[i] = 0;
		plot_scales[i] = 1;
	}
}


template<class CT> int RealTimePlot<CT>::add_plot(const float* array, int length, CT color, float offset, float scale){
	if(nof_arrays < plot_capacity){
		plot_array_lengths[nof_arrays] = length;
		plot_arrays[nof_arrays] = array;
		plot_colors[nof_arrays] = color;
		plot_offsets[nof_arrays] = offset;
		plot_scales[nof_arrays] = scale;
		return nof_arrays++;
	}
	return -1;
}

template<class CT> void RealTimePlot<CT>::remove_plot(int index){
	plot_array_lengths[index] = -1;
	plot_arrays[index] = 0;
}


template<class CT> void RealTimePlot<CT>::updateplot(){
	redraw_plot();
	cvShowImage(window, cvImage);
}

template<class CT> void RealTimePlot<CT>::updateplot(int first_index, int last_index){
	redraw_plot(first_index, last_index);
	cvShowImage(window, cvImage);
}




template<class CT> void RealTimePlot<CT>::redraw_plot(){
	using _basic_image_signal::array_max_min;
	using _basic_image_signal::euclidean_remainder;

	float total_max = 0;
	float total_min = 0;
	int total_length = 0;

	for(int n = 0; n<nof_arrays;n++){
		if(plot_array_lengths[n] > 0 && plot_arrays[n]){
			float array_max = 0;
			float array_min = 0;
			array_max_min(&array_max, &array_min, plot_arrays[n], plot_array_lengths[n]);

			array_max = plot_scales[n]*array_max + plot_offsets[n];
			array_min = plot_scales[n]*array_min + plot_offsets[n];


			if(array_max>total_max)
				total_max = array_max;
			if(array_min<total_min)
				total_min = array_min;
			if(plot_array_lengths[n]>total_length)
				total_length = plot_array_lengths[n];
		}
	}

//	cerr << total_max << " " <<total_min << " "  << total_length;

	const int width = image->width;
	const int height = image->height;

	/****
	 * Clear image
	 */
	for(int r = 0; r < height; r++)
		memset(image->array + r * image->width_step, 0, sizeof(CT[width]));


	const double fix_inf = 1e-3;
	const double scale = (double)(height-1) / (2*fix_inf + total_max - total_min);
	const double offset = (height-1) + (scale*(total_min-fix_inf));



	for(int n = 0; n < nof_arrays; n++){
		if(!(plot_array_lengths[n] > 0 && plot_arrays[n]))
			continue;

		/***
		 * i: index of image
		 * source_index_last: index of array
		 *
		 *	number of steps: width + 1
		 *	width pixels and one extra when counting pixel sides
		 *
		 */

//		const double current_scale = plot_scales[n] * scale;
//		const double current_offset = offset;// - scale * plot_offsets[n];

		const double step_size = static_cast<double>(total_length) / (width + 1);
		double current_source_index = step_size;

		int source_index_last = 0;
		float interpolated_value_last = plot_arrays[n][0];

		for(int i = 0; ( i < width ) && ( current_source_index + 1 <= plot_array_lengths[n] ) ;i++){
			const int source_index = static_cast<int>(current_source_index);
			const double remainder = euclidean_remainder(current_source_index,1.0);

			//TODO: dangerous: source_index + 1 possibly out of range
			const float interpolated_value = plot_arrays[n][source_index]*(1 - remainder) + plot_arrays[n][source_index + 1]*remainder;
			const bool larger_than_last = interpolated_value > interpolated_value_last;
			float region_max = larger_than_last ? interpolated_value : interpolated_value_last;
			float region_min = larger_than_last ? interpolated_value_last : interpolated_value;

			const int source_span = source_index - ( source_index_last + 1);
			if(source_span > 0){
				float partial_max;
				float partial_min;
				array_max_min(&partial_max, &partial_min, plot_arrays[n] + source_index_last + 1, source_span-1);
				region_max = max(region_max, partial_max);
				region_min = min(region_min, partial_min);
			}

			region_max = plot_scales[n]*region_max + plot_offsets[n];
			region_min = plot_scales[n]*region_min + plot_offsets[n];


			if(i == width -1){
				cerr << "Span: " << source_index_last << " " << source_index_last + source_span<< " " << source_index << " " << source_span << "\n";
			}

			drawVerticalLine(image,i,offset - scale*region_max, offset - scale*region_min, plot_colors[n]);

			current_source_index += step_size;
			source_index_last = source_index;
			interpolated_value_last = interpolated_value;
		}
	}


}



template<class CT> void RealTimePlot<CT>::redraw_plot(int first_index, int last_index){
	using _basic_image_signal::array_max_min;
	using _basic_image_signal::euclidean_remainder;

	float total_max = 0;
	float total_min = 0;
	int total_length = 0;

	for(int n = 0; n<nof_arrays;n++){
		if(plot_array_lengths[n] > 0 && plot_arrays[n]){
			float array_max = 0;
			float array_min = 0;
			array_max_min(&array_max, &array_min, plot_arrays[n], plot_array_lengths[n]);

			if(array_max>total_max)
				total_max = array_max;
			if(array_min<total_min)
				total_min = array_min;
			if(plot_array_lengths[n]>total_length)
				total_length = plot_array_lengths[n];
		}
	}

	cerr << total_max << " " <<total_min << " "  << total_length;

	const int width = image->width;
	const int height = image->height;



	const double scale = static_cast<double>(height-1) / (total_max - total_min);
	const double offset = (height-1) + scale*total_min;



	for(int n = 0; n < nof_arrays; n++){
		if(!(plot_array_lengths[n] > 0 && plot_arrays[n]))
			continue;

		/***
		 * i: index of image
		 * source_index_last: index of array
		 *
		 *	number of steps: width + 1
		 *	width pixels and one extra when counting pixel sides
		 *
		 */

		double step_size = static_cast<double>(total_length) / (width + 1);
		double current_source_index = step_size;

		int source_index_last = 0;
		float interpolated_value_last = plot_arrays[n][0];

		for(int i = 0; ( i < width ) && ( current_source_index <= plot_array_lengths[n] ) ;i++){
			const int source_index = static_cast<int>(current_source_index);
			const double remainder = euclidean_remainder(current_source_index,1.0);

			//TODO: dangerous: source_index + 1 possibly out of range
			const float interpolated_value = plot_arrays[n][source_index]*(1 - remainder) + plot_arrays[n][source_index + 1]*remainder;
			const bool larger_than_last = interpolated_value > interpolated_value_last;
			float region_max = larger_than_last ? interpolated_value : interpolated_value_last;
			float region_min = larger_than_last ? interpolated_value_last : interpolated_value;

			const int source_span = source_index - source_index_last;
			if(source_span > 0){
				float partial_max;
				float partial_min;
				array_max_min(&partial_max, &partial_min, plot_arrays[n] + source_index, source_span);
				region_max = max(region_max, partial_max);
				region_min = min(region_min, partial_min);
			}


			drawVerticalLine(image,i,offset -scale*region_max, offset - scale*region_min, plot_colors[n]);

			current_source_index += step_size;
			source_index_last = source_index;
			interpolated_value_last = interpolated_value;
		}
	}


}


#endif /* REALTIMEPLOT_H_ */
