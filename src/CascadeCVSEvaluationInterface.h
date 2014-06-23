/*
 * CascadeCVSEvaluationInterface.h
 *
 *  Created on: 2 mar 2011
 *      Author: Johan
 */

#ifndef CASCADECVSEVALUATIONINTERFACE_H_
#define CASCADECVSEVALUATIONINTERFACE_H_

#include "image_signal/color_types.h"
#include "image_signal/simple_image_signal.h"

class CascadeCVSEvaluationInterface{
public:
	template <typename T>
	using basic_image_signal = _basic_image_signal::basic_image_signal<T>;
	typedef _basic_image_signal::BGR8 BGR8;

	virtual ~CascadeCVSEvaluationInterface() = 0;
	virtual void tick()=0;
	virtual void set_image( basic_image_signal<BGR8>* new_debug_image) = 0;
	virtual basic_image_signal<BGR8>* get_image()=0;
	virtual void add_time_measurement(int t, float time)=0;
	virtual void add_nof_interest_points(int positions, int rejected_positions, int descriptors)=0;
	virtual const float* get_time_series(int t)=0;
	virtual int get_series_length()=0;
	virtual int* const get_nof_interest_point_series()=0;
};

inline CascadeCVSEvaluationInterface::~CascadeCVSEvaluationInterface(){}


#endif /* CASCADECVSEVALUATIONINTERFACE_H_ */
