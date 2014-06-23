/*
 * SURFOrientation.h
 *
 *  Created on: 6 dec 2010
 *      Author: Johan
 */

#ifndef CASCADESURFORIENTATION_H_
#define CASCADESURFORIENTATION_H_

#include "../CascadeImageMatchingConfig.h"
#include "CVSScalePyramid.h"
#include "CVSystem.h"

namespace CascadeCVSTemplate{

template <typename scale_pyramid_type>
float assignOrientation(
		const scale_pyramid_type& scale_pyramid,
		interest_point_position& position );
}
#endif /* CASCADESURFORIENTATION_H_ */
