/*
 * GaussianKernel.h
 *
 *  Created on: 7 okt 2010
 *      Author: Johan
 */

#ifndef CASCADEGAUSSIANKERNEL_H_
#define CASCADEGAUSSIANKERNEL_H_

namespace CascadeCVSTemplate{

template<int T> class GaussianKernel{
	static constexpr double pi = M_PI;
	float value[T][T];
public:
	GaussianKernel(float sigma){
		const float s2 = sigma*sigma;
		float* gauss = new float[T];
		const float offset = - (T - 1) / 2.f;

		const float normalizing_factor = 1.f/sqrt(2*pi*s2);
		const float coefficient = 1.f /(2*s2);

		for(int i=0;i<T;i++){
			const float r = offset + i;
			gauss[i] = exp(-coefficient*(r*r))*normalizing_factor;
		}


		for(int i = 0;i<T;i++)
			for(int j=0;j<T;j++)
				value[i][j] = gauss[i]*gauss[j];

		delete[] gauss;
	}

	const float* operator[](const int n_y) const {
		return this->value[n_y];
	}
};

}

#endif /* CASCADEGAUSSIANKERNEL_H_ */
