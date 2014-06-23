/*
 * SURFDrawing.cpp
 *
 *  Created on: 15 dec 2010
 *      Author: Johan
 */


//#include "CascadeCVSDrawing.h"
//#include "image_signal/simple_image_signal_utility.h"
//#include "CVSystem/CVSTemplateUtilities.h"

namespace CascadeCVSTemplate{


//void drawDiscernImage( image_signal<BGR8>& image, const scale_pyramid& discern_map, int red_scale, int green_scale, int blue_scale){
//	static const bool show_discern = true;
//	using _basic_image_signal::ImageArea;
//	using _basic_image_signal::ImageLength;
//	using _basic_image_signal::BGR_RED_8;
//	using _basic_image_signal::BGR_GREEN_8;
//	using _basic_image_signal::BGR_BLUE_8;
//
//	//		const int red_offset = discern_map->info[red_scale].offset;
//	const ImageArea red_destination_area = {0, 0, static_cast<ImageLength>(image.raw_width()) , static_cast<ImageLength>(image.raw_height()) };
//	const ImageArea red_source_area = {0, 0, static_cast<ImageLength>(discern_map.maps[red_scale].raw_width()-1), static_cast<ImageLength>(discern_map.maps[red_scale].raw_height()-1)};
//
//	//		const int green_offset = discern_map->info[green_scale].offset;
//	const ImageArea green_destination_area = {0, 0, static_cast<ImageLength>(image.raw_width()) , static_cast<ImageLength>(image.raw_height())};
//	const ImageArea green_source_area = {0, 0, static_cast<ImageLength>(discern_map.maps[green_scale].raw_width()-1), static_cast<ImageLength>(discern_map.maps[green_scale].raw_height()-1)};
//
//	//		const int blue_offset = discern_map->info[blue_scale].offset;
//	const ImageArea blue_destination_area = {0, 0, static_cast<ImageLength>(image.raw_width()) , static_cast<ImageLength>(image.raw_height()) };
//	const ImageArea blue_source_area = {0, 0, static_cast<ImageLength>(discern_map.maps[blue_scale].raw_width()-1), static_cast<ImageLength>(discern_map.maps[blue_scale].raw_height()-1)};
//
//	//		copyImage(discern_map->maps[red_scale], reinterpret_cast<image_signal<BGR_RED_8>* >( image), red_destination_area, red_source_area,1.0);
//	//		copyImage(discern_map->maps[green_scale], reinterpret_cast<image_signal<BGR_GREEN_8>* >(image), green_destination_area, green_source_area,1.0);
//	//		copyImage(discern_map->maps[blue_scale], reinterpret_cast<image_signal<BGR_BLUE_8>* >(image), blue_destination_area, blue_source_area,1.0);
//
//	using _basic_image_signal::simple_image_signal;
//	simple_image_signal<discern_type> tmp_image(discern_map.maps[0].raw_width(), discern_map.maps[0].raw_height());
//
//	if(show_discern){
//		calculate_response_image( discern_map.maps[red_scale] ,tmp_image );
//		const float red_ratio = discern_map.info[red_scale].sigma;
//		const float red_step = discern_map.info[red_scale].step_size;
//		copy_into_image_scale_offset(
//				tmp_image,
//				reinterpret_cast<image_signal<BGR_RED_8>& >( image),
//				red_destination_area,
//				red_source_area,
//				red_ratio * red_ratio * red_ratio * red_ratio * 4e-3/( red_step * red_step * red_step * red_step ), 127.0);
//		calculate_response_image( discern_map.maps[green_scale], tmp_image );
//		const float green_ratio = discern_map.info[green_scale].sigma;
//		const float green_step = discern_map.info[green_scale].step_size;
//		copy_into_image_scale_offset(
//				tmp_image,
//				reinterpret_cast<image_signal<BGR_GREEN_8>& >(image),
//				green_destination_area,
//				green_source_area,
//				green_ratio * green_ratio * green_ratio * green_ratio * 4e-3/( green_step * green_step * green_step * green_step ), 127.0);
//		calculate_response_image( discern_map.maps[blue_scale], tmp_image );
//		const float blue_ratio = discern_map.info[blue_scale].sigma;
//		const float blue_step = discern_map.info[blue_scale].step_size;
//		copy_into_image_scale_offset(
//				tmp_image,
//				reinterpret_cast<image_signal<BGR_BLUE_8>& >(image),
//				blue_destination_area,
//				blue_source_area,
//				blue_ratio * blue_ratio * blue_ratio * blue_ratio * 4e-3/( blue_step * blue_step * blue_step * blue_step ), 127.0);
//	}else{
//		copy_into_image(
//				discern_map.maps[red_scale],
//				reinterpret_cast<image_signal<BGR_RED_8>& >( image),
//				red_destination_area,
//				red_source_area,
//				1.0 );
//		copy_into_image(
//				discern_map.maps[green_scale],
//				reinterpret_cast<image_signal<BGR_GREEN_8>& >(image),
//				green_destination_area,
//				green_source_area,
//				1.0 );
//		copy_into_image(
//				discern_map.maps[blue_scale],
//				reinterpret_cast<image_signal<BGR_BLUE_8>& >(image),
//				blue_destination_area,
//				blue_source_area,
//				1.0 );
//	}
//}

}
