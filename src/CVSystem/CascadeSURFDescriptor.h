/*
 * CascadeSURFDescriptor.h
 *
 *  Created on: 27 sep 2010
 *      Author: Johan
 */

#ifndef CASCADESURFDESCRIPTOR_H_
#define CASCADESURFDESCRIPTOR_H_


#include "../CascadeImageMatchingConfig.h"
#include "../image_signal/simple_image_signal_structures.h"
#include "../image_signal/simple_image_signal_draw.h"
#include "../image_signal/Utility.h"


namespace CascadeCVSTemplate{

template<int Size> class CascadeSURFDescriptor{
	static const int VECTOR_SIZE = 4*Size*Size;
	float vector[VECTOR_SIZE];

//	CascadeSURFDescriptor(const CascadeSURFDescriptor&) = delete;
//	CascadeSURFDescriptor& operator=(const CascadeSURFDescriptor& orig) = delete;
public:
	CascadeSURFDescriptor(){};
	template <typename scale_pyramid_type>
	CascadeSURFDescriptor(
			const scale_pyramid_type& scale_pyramid,
			const interest_point_position& position,
			image_signal<BGR8>* debug );
	void copy_of(const CascadeSURFDescriptor& orig){
		memcpy(this, &orig, sizeof(CascadeSURFDescriptor<Size>));
	}
	void copy_of(const CascadeSURFDescriptor * const orig){
		memcpy(this, orig, sizeof(CascadeSURFDescriptor<Size>));
	}

	void calculateDescriptor(
			const image_signal<pyramid_data_type>& scale_image,
			const interest_point_position& position,
			image_signal<BGR8>* debug );

	float compare(const CascadeSURFDescriptor& other) const {
		float distance = 0;
		for(int i=0;i<VECTOR_SIZE;i++){
			const float diff = vector[i] - other.vector[i];
			distance += diff*diff;
//			const float diff = vector[i]*other.vector[i];
//			distance += vector[i]*other.vector[i];
		}
		return distance;//2*(1-distance);
	}
	inline float compare(const CascadeSURFDescriptor * const other) const {
		float distance = 0;
		for(int i=0;i<VECTOR_SIZE;i++){
			const float diff = vector[i] - other->vector[i];
			distance += diff*diff;
//			const float diff = vector[i]*other.vector[i];
//			distance += vector[i]*other->vector[i];
		}
		return distance;//2*(1-distance);
	}

	void err_print() const;
};


template<int Size> void CascadeSURFDescriptor<Size>::err_print() const {
	cerr.precision(3);
	for(int i=0; i<VECTOR_SIZE;i++)
		cerr<< vector[i]<<" ";
	cerr<<"\n";
}


} // namespace CascadeCVSTemplate



#endif /* CASCADESURFDESCRIPTOR_H_ */
