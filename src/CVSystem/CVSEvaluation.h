/*
 * CascadeSURFEvaluation.h
 *
 *  Created on: 9 dec 2010
 *      Author: Johan
 */

#ifndef CASCADECVSEVALUATION_H_
#define CASCADECVSEVALUATION_H_

#include "../CascadeCVSEvaluationInterface.h"
#include "../CascadeImageMatchingConfig.h"
#include "CascadeDataStructures.h"

#include "../image_signal/Utility.h"
#include <cstring>

namespace CascadeCVSTemplate{

template<int N>
class CascadeCVSEvaluation: public CascadeCVSEvaluationInterface{
public:
	enum Time_Measurements{
		TimeToFilter, TimeToSelectInterestPoints, TimeToCreateDescriptors, TimeToMatch,
		TimeToCreateIntegralImage, TimeForFilterOctave, TimeForSelectOctave = TimeForFilterOctave + N, TimeToSelectInterestPointsWithoutInterpolation = TimeForSelectOctave + N,
		TimeToCalculateOrientation, TimeToCalculateDescriptorsWithOrientation,
		NOF_TIME_MEASUREMENTS
	};
private:
	static const int EVALUATION_MEMORY_LENGTH = 128;

	/*****
	 * Evaluation depending on time
	 */
	int current_index;
	int valid_length;
	bool recordtimes[ NOF_TIME_MEASUREMENTS ];
	float times[ NOF_TIME_MEASUREMENTS ][ EVALUATION_MEMORY_LENGTH ];
	int nof_interest_point_positions[ EVALUATION_MEMORY_LENGTH ];
	int nof_rejected_interest_point_positions[ EVALUATION_MEMORY_LENGTH ];
	int nof_interest_points[ EVALUATION_MEMORY_LENGTH ];
	int nof_matches[ EVALUATION_MEMORY_LENGTH ];

	/******
	 * Evaluation of current run
	 */
	image_signal<BGR8>* debug_image;



	/*****
	 * Avoid accidental copying
	 */
	CascadeCVSEvaluation(const CascadeCVSEvaluation<N>&);
	CascadeCVSEvaluation<N>& operator=(const CascadeCVSEvaluation<N>&);
public:
	CascadeCVSEvaluation():
		current_index(0),
		valid_length(0),
		debug_image(nullptr){
		for(int i = 0; i < NOF_TIME_MEASUREMENTS; i++){
			recordtimes[i] = false;
		}
		std::memset(times, 0, sizeof(times));
	}
	virtual ~CascadeCVSEvaluation() override {}

	void tick(){
		if(++current_index >= EVALUATION_MEMORY_LENGTH )
			current_index = 0;
		if(valid_length< EVALUATION_MEMORY_LENGTH)
			valid_length++;
	}

	void set_image(_basic_image_signal::basic_image_signal<BGR8>* new_debug_image) override {
		debug_image = new_debug_image;
	}

	_basic_image_signal::basic_image_signal<BGR8>* get_image() override {
		return debug_image;
	}

	void add_time_measurement(int t, float time) override {
		times[t][current_index] = time;
	}
	void add_nof_interest_points(int positions, int rejected_positions, int descriptors) override {
		nof_interest_point_positions[current_index] = positions;
		nof_rejected_interest_point_positions[current_index] = rejected_positions;
		nof_interest_points[current_index] = descriptors;
	}
	const float* get_time_series(int t) override {
		return times[t];
	}
	int get_series_length() override {
		return EVALUATION_MEMORY_LENGTH;
	}

	int* const get_nof_interest_point_series() override {
		return nof_interest_points;
	}

};

}


#endif /* CASCADECVSEVALUATION_H_ */
