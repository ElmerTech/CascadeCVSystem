/*
 * RealTimePlot.h
 *
 *  Created on: 13 dec 2010
 *      Author: Johan
 */

#ifndef DOTPLOT_H_
#define DOTPLOT_H_

#include <iostream>


#include "opencv/cxcore.h"
#include "opencv/highgui.h"
#include "CascadeImageMatchingConfig.h"

using CascadeCVSTemplate::image_signal;

template<class CT> class DotPlot{
	static const int DEFAULT_CAPACITY = 128;
	IplImage* const cvImage;
	image_signal<CT>* const image;
	const char* const window;

	int plot_capacity;
	int nof_arrays;
	int plot_array_length;
	const int* plot_indices;
	const float**  plot_arrays;
	float* plot_offsets;
	float* plot_scales;
	CT* plot_colors;




	DotPlot(const DotPlot&);
	DotPlot& operator=(const DotPlot&);

	void redraw_plot();
	void redraw_plot(int first_index, int last_index);

public:
	DotPlot(const char* window_name, IplImage* im, const int* indices, int length, int capacity = DEFAULT_CAPACITY);

	void set_indices(const int* indices, int length);
	int add_plot(const float* array, CT color, float offset = 0, float scale = 1);
	void remove_plot(int index);
	void updateplot();
};

template<class CT>  DotPlot<CT>::DotPlot(const char* window_name, IplImage* im, const int* indices, int length, int capacity):
	cvImage(im),
	image(new wrapped_image_signal<CT>(im)),
	window(window_name),
	plot_capacity(capacity),
	nof_arrays(0),
	plot_array_length(indices ? length : 0),
	plot_indices(indices),
	plot_arrays(new const float*[capacity]),
	plot_offsets(new float[capacity]),
	plot_scales(new float[capacity]),
	plot_colors(new CT[capacity])
{
	for(int i=0;i<capacity;i++){
		plot_arrays[i] = 0;
		plot_offsets[i] = 0;
		plot_scales[i] = 1;
	}
}

template<class CT> void DotPlot<CT>::set_indices(const int* indices, int length){
	plot_indices = indices;
	plot_array_length = indices ? length : 0;
}


template<class CT> int DotPlot<CT>::add_plot(const float* array, CT color, float offset, float scale){
	if(nof_arrays < plot_capacity){
		plot_arrays[nof_arrays] = array;
		plot_colors[nof_arrays] = color;
		plot_offsets[nof_arrays] = offset;
		plot_scales[nof_arrays] = scale;
		return nof_arrays++;
	}
	return -1;
}

template<class CT> void DotPlot<CT>::remove_plot(int index){
	plot_arrays[index] = 0;
}


template<class CT> void DotPlot<CT>::updateplot(){
	redraw_plot();
	cvShowImage(window, cvImage);
}



template<class CT> void DotPlot<CT>::redraw_plot(){
	using _basic_image_signal::array_max_min;

	const int width = image->width;
	const int height = image->height;

	/****
	 * Clear image
	 */
	for(int r = 0; r < height; r++)
		memset(image->array + r * image->width_step, 0, sizeof(CT[width]));




	if(!plot_indices)
		return;

	int index_max = 0;
	int index_min = 0;
	array_max_min(&index_max, &index_min, plot_indices, plot_array_length);
	index_min = min(index_min, 0);

	const int index_span = index_max - index_min;
	if(index_span<1)
		return;


	float total_max = 0;
	float total_min = 0;

	for(int n = 0; n<nof_arrays;n++){
		if(plot_arrays[n]){
			float array_max = 0;
			float array_min = 0;
			array_max_min(&array_max, &array_min, plot_arrays[n], plot_array_length);

			array_max = plot_scales[n]*array_max + plot_offsets[n];
			array_min = plot_scales[n]*array_min + plot_offsets[n];


			if(array_max>total_max)
				total_max = array_max;
			if(array_min<total_min)
				total_min = array_min;
		}
	}



	const double index_scale = (double)(width-1) / index_span;

	const double fix_inf = 1e-3;
	const double scale = static_cast<double>(height-1) / (2*fix_inf + total_max - total_min);
	const double offset = (height-1) + (scale*(total_min-fix_inf));


	for(int i=0; i< plot_array_length; i++){
		const int pos_x = index_scale * (plot_indices[i]- index_min);

		for(int n = 0; n < nof_arrays; n++){
			if(!plot_arrays[n])
				continue;

			const int pos_y = offset - scale*(plot_offsets[n] + plot_arrays[n][i]*plot_scales[n]);
			image->operator [](pos_y)[pos_x] = plot_colors[n];

		}
	}


}


#endif /* DOTPLOT_H_ */
