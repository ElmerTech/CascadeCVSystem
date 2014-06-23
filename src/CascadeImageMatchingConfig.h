/*
 * CascadeImageMatchingConfig.h
 *
 *  Created on: 2 sep 2010
 *      Author: Johan
 */

#ifndef CASCADEIMAGEMATCHINGCONFIG_H_
#define CASCADEIMAGEMATCHINGCONFIG_H_

#include "config.h"
#include "image_signal/color_types.h"
#include "image_signal/simple_image_signal.h"
#include "container/point.h"
#include <array>
#include <vector>
//#include "cvDebug.h"


namespace CascadeCVSTemplate{

template<int Size> class CascadeSURFDescriptor;
template<int Size> class CascadeCVSEvaluation;
struct discern_map_info;

using _basic_image_signal::BGR8;
using _basic_image_signal::MONO8;

/****
 * Definition of color type
 */
typedef int integral_type;
typedef int32_t filtering_data_type;
typedef int64_t discern_type;
typedef typename _basic_image_signal::BGR8 source_type;

typedef int pyramid_data_type;
typedef int subsample_data_type;



template <typename Value_T>
using image_signal = _basic_image_signal::basic_image_signal<Value_T>;
template <typename Value_T>
using image_signal_point = typename _basic_image_signal::point_reference<Value_T>;
template <typename Value_T>
using const_image_signal_point = typename _basic_image_signal::const_point_reference<Value_T>;

using _basic_image_signal::simple_image_signal;
using _basic_image_signal::wrapped_image_signal;

/****
 * Image type defs
 */
typedef image_signal<source_type> CascadeSourceImage;
typedef image_signal_point<discern_type> CascadeDiscernPoint;
typedef image_signal_point<integral_type> CascadeIntegralPoint;


using _basic_image_signal::point2D;

/****
 * A structure specifying the position of an interest point in an image
 * Always used associated to a specific image
 */
struct interest_point_position{
	typedef float position_type;
	point2D<position_type> pos;
	position_type scale;
	position_type orientation;
	discern_type magnitude;
} ;

/****
 * interest point descriptor
 */
typedef CascadeSURFDescriptor<4> CascadeDescriptor;

}

#include "CVSystem/CVSScalePyramid.h"

#endif /* CASCADEIMAGEMATCHINGCONFIG_H_ */
