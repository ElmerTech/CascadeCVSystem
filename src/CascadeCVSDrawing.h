/*
 * SURFDrawing.h
 *
 *  Created on: 13 okt 2010
 *      Author: Johan
 */

#ifndef CASCADESURFDRAWING_H_
#define CASCADESURFDRAWING_H_

#include "image_signal/simple_image_signal_draw.h"
#include "image_signal/simple_image_signal_utility.h"
#include "CVSystem/CascadeDataStructures.h"
#include "CVSystem/CascadeSURFDescriptor.h"
#include "CVSystem/CVSScalePyramid.h"
#include "utilities/special_functions.h"


namespace CascadeCVSTemplate{

template<class CT>
inline void drawAllInterestPointPositions(
		image_signal<CT>& image,
		const interest_point_position_set& interest_points,
		CT color
){
	for(int i=0; i < interest_points.size(); i++){
		drawInterest(image,interest_points[i],color);
	}
}


template<class CT>
inline void drawAllInterestPoints(
		image_signal<CT>& image,
		const interest_point_set& interest_points,
		CT color
){
	for( int i=0; i < interest_points.get_nof_interest_points(); i++ ){
		drawInterest( image, interest_points.pos(i), color );
	}
}

template<class CT>
inline void drawInterest(
		image_signal<CT>& image,
		const interest_point_position& interest_point,
		CT color
){
	const int x = interest_point.pos.x;
	const int y = interest_point.pos.y;
	const int r  = 1.5f*interest_point.scale;
	if((x-r > 0 ) && (x+r < static_cast<int>(image.raw_width())) && (y-r)>0 && (y+r)<static_cast<int>(image.raw_height()))
		drawCircle( image, x, y, r, color, 1);
}

template<class CT>
inline void draw_minimum(
		image_signal<CT>& image,
		const interest_point_position& interest_point,
		CT color
){
	const int x = interest_point.pos.x;
	const int y = interest_point.pos.y;
	const int r  = 1.5f*interest_point.scale;
	if((x-r > 0 ) && (x+r < static_cast<int>(image.raw_width())) && (y-r)>0 && (y+r)<static_cast<int>(image.raw_height())){
		_basic_image_signal::drawVerticalLine( image, x, y-r, y+r, color);
		_basic_image_signal::drawHorizontalLine(image,y,x-r,x+r,color);
	}
}

template<class CT>
inline void drawInterest(
		image_signal<CT>& image,
		const interest_point& interest_point,
		CT color
){
	drawInterest(image, interest_point.pos, color);

	const int x = interest_point.pos.pos.x;
	const int y = interest_point.pos.pos.y;
	const int r  = 2*interest_point.pos.scale;
	const double o = interest_point.pos.orientation;
	drawLine(image, x, y, x + r*cos(o), y + r*sin(o), color);
}


template<class CT>
inline void drawMatches(
		image_signal<CT>& image,
		const CascadeMatchDescriptor& matches,
		CT color
){
	interest_point_set& first_interest_point_set = matches.first;
	interest_point_set& second_interest_point_set = matches.second;
	for( auto& match_pair: matches.matchingPairs){
		const interest_point& pip = first_interest_point_set.pos(match_pair.first);
		const interest_point& sip = second_interest_point_set.pos(match_pair.second);
		drawLine(image,pip.pos.pos.x,pip.pos.pos.y,sip.pos.pos.x,sip.pos.pos.y,color);
	}
}


//template <class S, class T>
//void convert_to_discern( image_signal<S>& image, const image_signal<T>& scale_image){
//	typedef typename image_signal<T>::size_type size_type;
//	image.copy_size(scale_image);
//	for( size_type y = 1; y < scale_image.raw_height()-1 ; y++ ){
//		for( size_type x = 1; x < scale_image.raw_width()-1; x++){
//			image[y][x] = calcResponse( scale_image.point( y, x ) );
//		}
//	}
//	image.apply_shift({1,1});
//	image.shift_data_offset({1,1});
//	image.raw_width_diff(-2);
//	image.raw_height_diff(-2);
//
//}


//void drawDiscernImage(
//		image_signal<BGR8>& image,
//		const scale_pyramid& discern_map,
//		int red_index,
//		int green_index,
//		int blue_index );


template <typename scale_pyramid_t>
void drawDiscernImage( image_signal<BGR8>& image, const scale_pyramid_t& discern_map, int red_scale, int green_scale, int blue_scale){
	static const bool show_discern = true;
	using _basic_image_signal::ImageArea;
	using _basic_image_signal::ImageLength;
	using _basic_image_signal::BGR_RED_8;
	using _basic_image_signal::BGR_GREEN_8;
	using _basic_image_signal::BGR_BLUE_8;

	//		const int red_offset = discern_map->info[red_scale].offset;
//	const ImageArea red_destination_area = {0, 0, static_cast<ImageLength>(image.raw_width()) , static_cast<ImageLength>(image.raw_height()) };
//	const ImageArea red_source_area = {0, 0, static_cast<ImageLength>(discern_map.maps[red_scale].raw_width()-1), static_cast<ImageLength>(discern_map.maps[red_scale].raw_height()-1)};

	//		const int green_offset = discern_map->info[green_scale].offset;
//	const ImageArea green_destination_area = {0, 0, static_cast<ImageLength>(image.raw_width()) , static_cast<ImageLength>(image.raw_height())};
//	const ImageArea green_source_area = {0, 0, static_cast<ImageLength>(discern_map.maps[green_scale].raw_width()-1), static_cast<ImageLength>(discern_map.maps[green_scale].raw_height()-1)};

	//		const int blue_offset = discern_map->info[blue_scale].offset;
//	const ImageArea blue_destination_area = {0, 0, static_cast<ImageLength>(image.raw_width()) , static_cast<ImageLength>(image.raw_height()) };
//	const ImageArea blue_source_area = {0, 0, static_cast<ImageLength>(discern_map.maps[blue_scale].raw_width()-1), static_cast<ImageLength>(discern_map.maps[blue_scale].raw_height()-1)};


	using _basic_image_signal::simple_image_signal;
	simple_image_signal<discern_type> tmp_image(discern_map.maps[0].raw_width(), discern_map.maps[0].raw_height());

	clear_to(image,BGR8(0));

	if(show_discern){
		static const double norm_fact = 2e-1;
		calculate_response_image( discern_map.maps[red_scale], tmp_image );
		const float red_ratio = discern_map.info[red_scale].sigma;
//		copy_into_image_scale_offset(
//				tmp_image,
//				reinterpret_cast<image_signal<BGR_RED_8>& >( image),
//				red_destination_area,
//				red_source_area,
//				norm_fact * mptl::quad( red_ratio / discern_map.maps[red_scale].sample_factor()) , 127.0);
		copy_into_image_scale_offset(
				tmp_image,
				reinterpret_cast<image_signal<BGR_RED_8>& >( image),
				norm_fact * mptl::quad( red_ratio / discern_map.maps[red_scale].sample_factor()) , 127.0);
		calculate_response_image( discern_map.maps[green_scale], tmp_image );
		const float green_ratio = discern_map.info[green_scale].sigma;
//		copy_into_image_scale_offset(
//				tmp_image,
//				reinterpret_cast<image_signal<BGR_GREEN_8>& >(image),
//				green_destination_area,
//				green_source_area,
//				norm_fact * mptl::quad( green_ratio / discern_map.maps[green_scale].sample_factor()), 127.0);
		copy_into_image_scale_offset(
				tmp_image,
				reinterpret_cast<image_signal<BGR_GREEN_8>& >( image),
				norm_fact * mptl::quad( green_ratio / discern_map.maps[green_scale].sample_factor()) , 127.0);
		calculate_response_image( discern_map.maps[blue_scale], tmp_image );
		const float blue_ratio = discern_map.info[blue_scale].sigma;
//		copy_into_image_scale_offset(
//				tmp_image,
//				reinterpret_cast<image_signal<BGR_BLUE_8>& >(image),
//				blue_destination_area,
//				blue_source_area,
//				norm_fact * mptl::quad( blue_ratio / discern_map.maps[blue_scale].sample_factor()), 127.0);
		copy_into_image_scale_offset(
				tmp_image,
				reinterpret_cast<image_signal<BGR_BLUE_8>& >( image),
				norm_fact * mptl::quad( blue_ratio / discern_map.maps[blue_scale].sample_factor()) , 127.0);
	}else{
		copy_into_image(
				discern_map.maps[red_scale],
				reinterpret_cast<image_signal<BGR_RED_8>& >( image),
				1.0 );
		copy_into_image(
				discern_map.maps[green_scale],
				reinterpret_cast<image_signal<BGR_GREEN_8>& >(image),
				1.0 );
		copy_into_image(
				discern_map.maps[blue_scale],
				reinterpret_cast<image_signal<BGR_BLUE_8>& >(image),
				1.0 );
	}
}

} // namespace CascadeCVSTemplate



#endif /* CASCADESURFDRAWING_H_ */
