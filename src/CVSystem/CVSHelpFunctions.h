/*
 * SURFSystemHelpFunctions.h
 *
 *  Created on: 17 dec 2010
 *      Author: Johan
 */

#ifndef CASCADESURFSYSTEMHELPFUNCTIONS_H_
#define CASCADESURFSYSTEMHELPFUNCTIONS_H_

#include <vector>


namespace CascadeCVSTemplate{

template <typename CVSystemConfig>
typename CascadedCVSystem<CVSystemConfig>::CVSystemWorkingSet&
CascadedCVSystem<CVSystemConfig>::get_working_set(int min_width, int min_height){
	for( CVSystemWorkingSet& w_set: work_sets){
		if( w_set.lock_valid( min_width, min_height ) ){
			return w_set;
		}
	}
	work_sets.push_back( CVSystemWorkingSet( min_width, min_height ) );
	CVSystemWorkingSet& w_set = work_sets.back();
	std::cerr<< "Created new working set of size " << w_set.work_image.raw_width() << "x"<<  w_set.work_image.raw_height() << " ( "<< w_set.work_image.width_step() << "x"<<  w_set.work_image.raw_height() << " )\n";
	w_set.lock_valid( min_width, min_height );
	return w_set;
}









//template <typename CVSystemConfig>
//void CascadedCVSystem<CVSystemConfig>::allocate_scale_pyramid( scale_pyramid& result ){
//	const int max_width = result.max_width;
//	const int max_height = result.max_height;
//
//	//allocate memory for filters response maps
//	result.subSampleMaps.reserve( NUMBER_OF_OCTAVES );
//	result.subSampleMaps.push_back( simple_image_signal< scale_pyramid::subsample_type >( max_width, max_height ) );
//
//	//allocate memory for filters response maps
//	result.maps.reserve( NOF_FILTERS );
//	result.info.reserve( NOF_FILTERS );
//
//	for(int i = 0; i< FILTERS_PER_OCTAVE; i++){
//		result.maps.push_back( simple_image_signal<pyramid_data_type>(max_width, max_height) );
//		clear_image( result.maps.back() );
//		result.info.push_back(
//				{ 0, 1, static_cast<float>(OctaveHelp::filter_sizes(1,i))} );
//	}
//	for(int oct = 1; oct < NUMBER_OF_OCTAVES; oct++){
//		const int octaceWidth = result.max_width >> oct;
//		const int octaceHeight = result.max_height >> oct;
//
//		result.subSampleMaps.push_back( simple_image_signal< scale_pyramid::subsample_type>(octaceWidth, octaceHeight) );
//		for(int i = 0; i < NEW_FILRERS_PER_OCTAVE; i++){
////			int index = FILTERS_PER_OCTAVE + NEW_FILRERS_PER_OCTAVE * (oct - 1) + i;
//			result.maps.push_back( simple_image_signal<pyramid_data_type>(octaceWidth, octaceHeight) );
//			clear_image( result.maps.back() );
//			result.info.push_back({
//					0,
//					1 << oct ,
//					static_cast<float>(OctaveHelp::filter_sizes( oct+1, FILTERS_PER_OCTAVE - NEW_FILRERS_PER_OCTAVE + i))} );
//		}
//	}
//}
//
//template <typename CVSystemConfig>
//void CascadedCVSystem<CVSystemConfig>::allocate_scale_pyramid( cvs_scale_pyramid& result ){
//
//}
//
//


}

#endif /* CASCADESURFSYSTEMHELPFUNCTIONS_H_ */
