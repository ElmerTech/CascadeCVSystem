/*
 * InternalDataStructures.cpp
 *
 *  Created on: 20 jan 2011
 *      Author: Johan
 */

#include "CascadeDataStructures.h"
//#include "InternalDataStructures.h"
#include "../image_signal/Utility.h"
#include "../cvDebug.h"
#include <algorithm>

namespace CascadeCVSTemplate{


inline float default_merit_norm(
		interest_point_position* ipp
){
	return ipp->magnitude;
}

inline bool default_compare_merit_norm(
		const interest_point_position& a,
		const interest_point_position& b
){
	return a.magnitude < b.magnitude;
}


struct sort_info{
	float merit_value;
	interest_point_position position;
	static bool compare(sort_info* const& a, sort_info* const& b){
		return a->merit_value > b->merit_value;
	}
};



void internal_interest_point_position_set::sort_positions(
		float (*merit_norm)(interest_point_position*)
){
	std::sort( maximum_positions.begin(), maximum_positions.end(), default_compare_merit_norm );
	std::sort( minimum_positions.begin(), minimum_positions.end(), default_compare_merit_norm );
}




}
