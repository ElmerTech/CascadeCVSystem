/*
 * DataStructures.h
 *
 *  Created on: 1 sep 2010
 *      Author: Johan
 */

#ifndef CASCADEDATASTRUCTURES_H_
#define CASCADEDATASTRUCTURES_H_

#include "../CascadeImageMatchingConfig.h"
#include "CascadeSURFDescriptor.h"
#include <vector>

namespace CascadeCVSTemplate{
/****
 * Unique image identifying handle
 */
typedef void* ImageHandle;



/****
 * Yet unspecified matching image structure
 */
typedef void* MatchingInfo;



/****
 * A structure storing a set of interest point positions
 * Always used associated to a specific image
 */
typedef std::vector< interest_point_position > interest_point_position_set;


class internal_interest_point_position_set{
public:
	interest_point_position_set maximum_positions;
	interest_point_position_set minimum_positions;
	interest_point_position_set rejected_maximum_positions;
	interest_point_position_set rejected_minimum_positions;
public:
	internal_interest_point_position_set():
		maximum_positions(),
		minimum_positions(),
		rejected_maximum_positions(),
		rejected_minimum_positions()
{}

	void sort_positions(float (*merit_norm)(interest_point_position*)=0);

	void clear(){
		maximum_positions.clear();
		minimum_positions.clear();
		rejected_maximum_positions.clear();
		rejected_minimum_positions.clear(); }
};


/****
 * A structure describing an interest point, both position and appearance
 * Always used associated to a specific image
 */
struct interest_point{
	interest_point_position pos;
	int descriptor_index;
};


/****
 * A structure storing a set of interest points
 * Always used associated to a specific image
 */
class interest_point_set{
	static const int DEFAULT_MAX_INTEREST_POINTS = 8 << 10;

	std::vector<interest_point> interest_points;
	std::vector<CascadeDescriptor> descriptorArray;

public:
	interest_point_set(int capacity = DEFAULT_MAX_INTEREST_POINTS){
		interest_points.reserve(capacity);
		descriptorArray.reserve(capacity);
	}
	virtual ~interest_point_set(){}

	int get_nof_interest_points() const { return interest_points.size();}

	interest_point& pos(size_t index) { return interest_points[index]; }
	const interest_point& pos(size_t index) const  { return interest_points[index]; }

	const CascadeDescriptor& get_descriptor(size_t index) const {return descriptorArray[ interest_points[index].descriptor_index ]; }

	void addInterestPoint( interest_point_position const & i_p_p, const CascadeDescriptor& desc ){
		descriptorArray.push_back( desc );
		interest_points.push_back( interest_point{ i_p_p , static_cast<int>(descriptorArray.size()-1) } );
	}
	const interest_point& getLastInterestPoint(){
		return interest_points.back();
	}
	void clear(){
		interest_points.clear();
		descriptorArray.clear();}

};

typedef std::pair<int,int> matching_interest_point_pair;


struct CascadeMatchDescriptor{
	const interest_point_set& first;
	const interest_point_set& second;
	std::vector< matching_interest_point_pair > matchingPairs;

	CascadeMatchDescriptor(
			const interest_point_set& _first,
			const interest_point_set& _second):
				first(_first),
				second(_second),
				matchingPairs()
			{}
};


}

#endif /* CASCADEDATASTRUCTURES_H_ */
