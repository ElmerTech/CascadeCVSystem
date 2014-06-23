/*
 * SURFSystemWorkingSet.h
 *
 *  Created on: 17 dec 2010
 *      Author: Johan
 */

#ifndef CASCADESURFSYSTEMWORKINGSET_H_
#define CASCADESURFSYSTEMWORKINGSET_H_

namespace CascadeCVSTemplate{

template <typename CVSystemConfig>
class CascadedCVSystem<CVSystemConfig>::CVSystemWorkingSet{
	bool busy;
public:
	static const int MIN_INTEGRAL_IMAGE_WIDTH = 800;
	int max_width;
	int max_height;
	simple_image_signal<filtering_data_type> work_image;
	cvs_scale_pyramid scale_discern_pyramid;
	internal_interest_point_position_set interest_point_position_set;

	CVSystemWorkingSet( int width, int height );
	~CVSystemWorkingSet(){}
	bool lock_valid(int width, int height){
		if(!busy && max_width >= width && max_height >= height){
			busy = true;
			interest_point_position_set.clear();
			return true;
		}
		return false;
	}
	void release(){
		busy = false;
	}
	CVSystemWorkingSet( CVSystemWorkingSet&& ) = default;
private:
	CVSystemWorkingSet( const CVSystemWorkingSet& ) = delete;
	CVSystemWorkingSet& operator=( const CVSystemWorkingSet& ) = delete;
};


template <typename CVSystemConfig>
CascadedCVSystem<CVSystemConfig>::CVSystemWorkingSet::CVSystemWorkingSet(int width, int height):
	busy(false),
	max_width(width),
	max_height(height),
	work_image(
			width,
			height,
			( width < MIN_INTEGRAL_IMAGE_WIDTH ) ? ( MIN_INTEGRAL_IMAGE_WIDTH - width ) : 0 ),
	scale_discern_pyramid( width, height ),
	interest_point_position_set()
{

}

}

#endif /* CASCADESURFSYSTEMWORKINGSET_H_ */
