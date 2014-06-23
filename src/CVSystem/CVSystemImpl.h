/*
 * CVSystemImpl.h
 *
 *  Created on: 6 jun 2013
 *      Author: johan
 */

#ifndef CASCADECVSSYSTEMIMPL_H_
#define CASCADECVSSYSTEMIMPL_H_


#include "CVSystem.h"
#include "CVSEvaluation.h"
//#include "CVSWorkingSet.h"
#include "CVSHelpFunctions.h"
#include "CVSFilterImpl.h"
#include "CVSSelectImpl.h"
#include "../CascadeCVSDrawing.h"

namespace CascadeCVSTemplate{

template <typename CVSystemConfig>
const int CascadedCVSystem<CVSystemConfig>::NEW_FILRERS_PER_OCTAVE = FILTERS_PER_OCTAVE - 2;
template <typename CVSystemConfig>
const int CascadedCVSystem<CVSystemConfig>::NOF_FILTERS = FILTERS_PER_OCTAVE /* first octave*/ + NEW_FILRERS_PER_OCTAVE * ( NUMBER_OF_OCTAVES - 1 )/* all but the first octave*/; // first octave has #FILTERS_PER_OCTAVE# filters, next octave inherits two filters from the first one and so on.


template <typename CVSystemConfig>
CascadeCVSEvaluationInterface* CascadedCVSystem<CVSystemConfig>::get_eval_object(){
	return eval_object;
}

template <typename CVSystemConfig>
void CascadedCVSystem<CVSystemConfig>::set_debug_image(
		image_signal<BGR8>* debug_image
){
	eval_object->set_image(debug_image);
}


//typedef CascadeCVSTemplate::CVSystemTemplate<NUMBER_OF_OCTAVES, NUMBER_OF_OCTAVES, FILTERS_PER_OCTAVE> CVSystemPart;

//template <typename CVSystemConfig>
//void CascadedCVSystem<CVSystemConfig>::CreateDiscernMap(
//		const CascadeSourceImage& image,
//		image_signal<filtering_data_type>& work_image,
//		scale_pyramid& result
//){
//	CVSystemPart::CreateDiscernMap(
//			image,
//			work_image,
//			result,
//			dynamic_cast< CascadeSURFEval* >(eval_object));
//}
//template <typename CVSystemConfig>
//void CascadedCVSystem<CVSystemConfig>::SelectInterestPoints(
//		const scale_pyramid& discern_map,
//		internal_interest_point_position_set& intrest_point_positions,
//		int cap
//){
//	CVSystemPart::SelectInterestPoints(
//			discern_map,
//			intrest_point_positions,
//			cap,
//			dynamic_cast<CascadeSURFEval*>(eval_object));
//}
//template <typename CVSystemConfig>
//void CascadedCVSystem<CVSystemConfig>::CreateInterestPointDescriptors(
//		const scale_pyramid& _scale_pyramid,
//		internal_interest_point_position_set& positions,
//		interest_point_set& interest_points
//){
//	CVSystemPart::CreateInterestPointDescriptors(
//			_scale_pyramid,
//			positions,
//			interest_points,
//			dynamic_cast<CascadeSURFEval*>( eval_object ) );
//}
template <typename CVSystemConfig>
void CascadedCVSystem<CVSystemConfig>::MatchImages(
		CascadeMatchDescriptor& match_descriptor
){
//	CVSystemPart::MatchImages(
//			match_descriptor,
//			dynamic_cast<CascadeCVSEval*>(eval_object));
}






template <typename CVSystemConfig>
CascadedCVSystem<CVSystemConfig>::CascadedCVSystem( int img_cap, int work_cap ):
	working_set_capacity(work_cap),
	image_capacity(img_cap),
	work_sets(),
	absolute_threshold( DEFAULT_ABSOLUTE_THRESHOLD ),
	serial_image_number(0),
	image_indices(new int[image_capacity]),
	interest_point_sets(new interest_point_set*[image_capacity]),
	eval_object( new CascadeCVSEval() )
{
	for(int i=0;i<image_capacity;i++){
		image_indices[i] = -1;
		interest_point_sets[i] = new interest_point_set();;
	}
	std::cout << "Creating CVSystem"
			", nof_filters: " << NOF_LEVELS <<
			", nof_octaves: " << NOF_OCTAVES << "\n";

}



template <typename CVSystemConfig>
int CascadedCVSystem<CVSystemConfig>::addImage(
		const CascadeSourceImage& image
){
	const int image_no = serial_image_number++;

	CVSystemWorkingSet& work_set = get_working_set( image.raw_width(), image.raw_height() );

	internal_interest_point_position_set intrest_point_positions;

	CreateDiscernMap<CascadeCVSTemplate::source_type>(
			image,
			work_set.work_image,
			work_set.scale_discern_pyramid);
	SelectInterestPoints(
			work_set.scale_discern_pyramid,
			work_set.interest_point_position_set,
			absolute_threshold,
			eval_object );
	CreateInterestPointDescriptors(
			work_set.scale_discern_pyramid,
			work_set.interest_point_position_set,
			interest_point_sets[image_no % image_capacity]);
	image_indices[image_no % image_capacity] = image_no;
	work_set.release();
	return image_no;
}

template <typename CVSystemConfig>
int CascadedCVSystem<CVSystemConfig>::addImage(
		const image_signal<MONO8>& image
){
	const int image_no = serial_image_number++;

	CVSystemWorkingSet& work_set = get_working_set( image.raw_width(), image.raw_height() );

	internal_interest_point_position_set intrest_point_positions;
	CreateDiscernMap<MONO8>(
			image,
			work_set.work_image,
			work_set.scale_discern_pyramid/*,
			dynamic_cast<CascadeSURFEval*>(eval_object)*/ );
	SelectInterestPoints(
			work_set.scale_discern_pyramid,
			work_set.interest_point_position_set,
			absolute_threshold,
			eval_object );
	CreateInterestPointDescriptors(
			work_set.scale_discern_pyramid,
			work_set.interest_point_position_set,
			*(interest_point_sets[image_no % image_capacity]) );
	image_indices[image_no % image_capacity] = image_no;
	work_set.release();
	return image_no;

}


template <typename CVSystemConfig>
CascadeMatchDescriptor* CascadedCVSystem<CVSystemConfig>::matchImages( int first, int second ){
	/**
	 * Don't allow matching with self OR
	 * Don't allow matching if supplied indices don't correspond to available data
	 */
	if( first==second || first != image_indices[ first % image_capacity ] || second != image_indices[second%image_capacity])
		return 0;

	CascadeMatchDescriptor result(
			interest_point_sets[first % image_capacity],
			interest_point_sets[second % image_capacity]
	);

	MatchImages( result );

	return result;
}

template <typename CVSystemConfig>
typename CascadedCVSystem<CVSystemConfig>::cvs_scale_pyramid& CascadedCVSystem<CVSystemConfig>::get_pyramid(int image_no){
	return work_sets[0].scale_discern_pyramid;
}


template <typename CVSystemConfig>
void CascadedCVSystem<CVSystemConfig>::CreateInterestPointDescriptors(
		const cvs_scale_pyramid& _scale_pyramid,
		internal_interest_point_position_set& positions,
		interest_point_set& interest_points
){
	static const int INTERESTPOINT_LIMIT = 50000;

	interest_points.clear();

	image_signal<BGR8>* image = eval_object ? eval_object->get_image() : 0;

	const int nof_positions = positions.maximum_positions.size();
	int nof_interest_points = 0;

	/*****
	 * First assign orientation for all interest points
	 */
	for(int i = 0; i < nof_positions && nof_interest_points < INTERESTPOINT_LIMIT; i++){
		interest_point_position& current_position = positions.maximum_positions[i];
		nof_interest_points++;
		if(image)
			drawInterest(*image, current_position, BGR8(255 , 0 , 0));
	}

	const int nof_min_positions = positions.minimum_positions.size();
	for(int i = 0; i < nof_min_positions && nof_interest_points < INTERESTPOINT_LIMIT; i++){
		interest_point_position& current_position = positions.minimum_positions[i];
		nof_interest_points++;
		if(image)
			draw_minimum(*image, current_position, BGR8(0 , 0 , 255));
	}

}



}
#endif /* CASCADECVSSYSTEMIMPL_H_ */
