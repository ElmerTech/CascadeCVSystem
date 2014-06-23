/*
 * CascadedCVSystem.h
 *
 *  Created on: 17 dec 2010
 *      Author: Johan
 */

#ifndef CASCADESURFSYSTEM_H_
#define CASCADESURFSYSTEM_H_

#include <vector>

#include "../CascadeImageMatchingConfig.h"
#include "CascadeDataStructures.h"
#include "../CascadeCVSEvaluationInterface.h"
#include "CompileConfig.h"

namespace CascadeCVSTemplate{

static const int DEFAULT_MAX_INTEREST_POINT_POSITIONS = 16 << 10;
static const int DEFAULT_MAX_INTEREST_POINTS = 8 << 10;

static const int NUMBER_OF_OCTAVES = 5;
static const int FILTERS_PER_OCTAVE = 4;



template <typename CVSystemConfig>
class CascadedCVSystem{
public:
	typedef CascadeCVSEvaluation<NUMBER_OF_OCTAVES> CascadeCVSEval;
private:
	static const int NEW_FILRERS_PER_OCTAVE;
	static const int NOF_FILTERS;
	static constexpr int NOF_OCTAVES = CVSystemConfig::nof_downsamples + 1;
	static constexpr int NOF_LEVELS = CVSystemConfig::nof_levels;

	static constexpr discern_type DEFAULT_ABSOLUTE_THRESHOLD = 50.0f;

	const int working_set_capacity;
	const int image_capacity;

	class CVSystemWorkingSet;

public:
	typedef scale_pyramid_template< NOF_OCTAVES, NOF_LEVELS > cvs_scale_pyramid;

private:

	/*********
	 *
	 * SURF System variables

	 */

	std::vector< CVSystemWorkingSet > work_sets;
	discern_type absolute_threshold;

	int serial_image_number;
	int* image_indices;
	interest_point_set** interest_point_sets;
	CascadeMatchDescriptor* matching_pairs;


	/**********
	 *
	 *	SURF Evaluation related variables
	 *
	 */
	CascadeCVSEvaluationInterface* eval_object;


	static void allocate_scale_pyramid(
			cvs_scale_pyramid& result );

	template< typename source_type >
	void CreateDiscernMap(
			const image_signal< source_type >& image,
			image_signal<filtering_data_type>& integral_image,
			cvs_scale_pyramid& result );
//	void SelectInterestPoints(
//			const cvs_scale_pyramid& discern_map,
//			internal_interest_point_position_set& intrest_point_positions,
//			int cap );
	void CreateInterestPointDescriptors(
			const cvs_scale_pyramid& _scale_pyramid,
			internal_interest_point_position_set& positions,
			interest_point_set& interest_points );
	void MatchImages(
			CascadeMatchDescriptor& match_descriptor );

	void select_interest_points_level(
			const cvs_scale_pyramid& discern_map,
			const int level,
			internal_interest_point_position_set& intrest_point_positions,
			int threshold/*,
			CascadeSURFEvaluation<NOF_OCTAVES>* eval_object*/ );

//	template< typename source_type >
//	void CreateDiscernMap(
//			const image_signal< source_type >& image,
//			image_signal<filtering_data_type>& integral_image,
//			cvs_scale_pyramid& result );
	void SelectInterestPoints(
			const cvs_scale_pyramid& discern_map,
			internal_interest_point_position_set& intrest_point_positions,
			int cap,
			CascadeCVSEvaluationInterface* eval_object );
//	void CreateInterestPointDescriptors(
//			const cvs_scale_pyramid& _scale_pyramid,
//			internal_interest_point_position_set& positions,
//			interest_point_set& interest_points );

//	void select_interest_points_level(
//			const cvs_scale_pyramid& discern_map,
//			const int level,
//			internal_interest_point_position_set& intrest_point_positions,
//			int threshold/*,
//			CascadeSURFEvaluation<NOF_OCTAVES>* eval_object*/
//	);

	void SelectInterestPointsImpl(
			const cvs_scale_pyramid& discern_map,
			internal_interest_point_position_set& intrest_point_positions,
			int cap
	);
//	CascadedCVSystem( const CascadedCVSystem& ) = delete;
//	CascadedCVSystem& operator=( const CascadedCVSystem& ) = delete;
public:
	CascadedCVSystem( int img_cap = 10, int work_cap = 5 );
	CVSystemWorkingSet& get_working_set( int min_width, int min_height );

	int addImage(const CascadeSourceImage& image);
	int addImage(const image_signal<MONO8>& image);

	CascadeMatchDescriptor* matchImages(int first, int second);

	CascadeCVSEvaluationInterface* get_eval_object();

	discern_type get_absolute_threshold()const { return absolute_threshold; }
	void set_absolute_threshold( discern_type threshold ){
		absolute_threshold = threshold;
	}


	void set_debug_image( image_signal<BGR8>* debug_image );

	int nof_filters(){ return NOF_LEVELS; }

//	scale_pyramid& get_pyramid( int image_no );
	cvs_scale_pyramid& get_pyramid(int image_no);

	int get_nof_interest_points(int image_no){
		return image_indices[image_no % image_capacity] == image_no ? interest_point_sets[image_no % image_capacity]->get_nof_interest_points() : -1;
	}
	interest_point_set* get_interest_points_set(int image_no){
		return image_indices[image_no % image_capacity] == image_no ? interest_point_sets[image_no % image_capacity] : 0;
	}

	int get_nof_interest_point_positions(int image_no){
		return image_indices[image_no % image_capacity] == image_no ? interest_point_sets[image_no % image_capacity]->get_nof_interest_points() : -1;
	}
	int get_nof_rejected_interest_point_positions(int image_no){
		return image_indices[image_no % image_capacity] == image_no ? interest_point_sets[image_no % image_capacity]->get_nof_interest_points() : -1;
	}
};


}


#include "CVSystemImpl.h"
#include "CVSHelpFunctions.h"
#include "CVSWorkingSet.h"

#endif /* CASCADESURFSYSTEM_H_ */
