/*
 * scale_pyramid.h
 *
 *  Created on: 13 jun 2013
 *      Author: johan
 */

#ifndef CASCADE_CV_SCALE_PYRAMID_H_
#define CASCADE_CV_SCALE_PYRAMID_H_

#include "../CascadeImageMatchingConfig.h"
#include <array>

namespace CascadeCVSTemplate{

//struct discern_map_info{
//	int offset;
//	int step_size;
//	float sigma;
//};
//


/****
 * A collection of intensity maps used to find interest points
 */

//struct scale_pyramid {
//	typedef subsample_data_type subsample_type;
//	typedef pyramid_data_type pyramid_type;
//	int max_width;
//	int max_height;
//	int width;
//	int height;
//	std::vector< simple_image_signal< subsample_type > > subSampleMaps;
//	std::vector< simple_image_signal< pyramid_type > > maps;
//	std::vector< discern_map_info > info;
//
//	scale_pyramid( int min_width, int min_height ):
//	max_width(min_width),
//	max_height(min_height),
//	width(min_width),
//	height(min_height),
//	subSampleMaps(),
//	maps(),
//	info()
//	{
//	}
//	scale_pyramid(scale_pyramid&&) = default;
//};


struct scale_pyramid_base {
	typedef subsample_data_type subsample_type;
	typedef pyramid_data_type pyramid_type;
	int max_width;
	int max_height;
	int width;
	int height;

	scale_pyramid_base( int min_width, int min_height ):
	max_width(min_width),
	max_height(min_height),
	width(min_width),
	height(min_height)
	{
	}
	scale_pyramid_base( scale_pyramid_base && ) = default;
};


struct cvs_map_info{
	float sigma;
	int total_gain;
};


template < size_t NOF_OCTAVES, size_t NOF_LEVELS >
struct scale_pyramid_template: public scale_pyramid_base {
	std::array< simple_image_signal< subsample_type >, NOF_OCTAVES > sub_sample_maps;
	std::array< simple_image_signal< pyramid_type >,  NOF_LEVELS> maps;
	std::array< cvs_map_info, NOF_LEVELS > info;

	scale_pyramid_template( int min_width, int min_height ):
	scale_pyramid_base(min_width, min_height),
	sub_sample_maps(),
	maps(),
	info()
	{
		for(simple_image_signal<subsample_type>& img: sub_sample_maps){
			img.set_raw_size(max_width,max_height);
		}
		for(simple_image_signal<pyramid_type>& img: maps){
			img.set_raw_size(max_width,max_height);
		}
	}
	scale_pyramid_template( scale_pyramid_template && ) = default;
};


}

#endif /* CASCADE_CV_SCALE_PYRAMID_H_ */
