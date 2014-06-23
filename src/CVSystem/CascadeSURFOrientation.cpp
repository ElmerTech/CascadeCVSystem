#include "CascadeSURFOrientation.h"
#include "CVSTemplateUtilities.h"

#include "../image_signal/Utility.h"
#include "../image_signal/simple_image_signal_structures.h"
#include "../cvDebug.h"


namespace CascadeCVSTemplate{

/****
 * 	Structure storing haar responses and pre-calculated direction
 */
class gradient_response{
	gradient_response(const gradient_response&);
public:

	gradient_response():dx(0),dy(0),angle(0){}
	gradient_response(const discern_type h_x, const discern_type h_y):dx(h_x),dy(h_y),angle(atan2f(h_y,h_x)){}
	gradient_response(image_signal< pyramid_data_type > const& image, const int x, const int y, const int size, const discern_type weight){
		dx = weight * calc_deriv_x(image, y, x, size);
		dy = weight * calc_deriv_y(image, y, x, size);
		angle = atan2f(dy,dx);
	}

	bool operator<(const gradient_response& other) const { return  angle < other.angle; }

	discern_type dx;
	discern_type dy;
	discern_type angle;
};





void plot_angle_sums(gradient_response** haar_responses, const int total_count);
float getMaximumSector(gradient_response** haar_responses, const int total_count);

inline bool compareHaarPointers(gradient_response* const & a, gradient_response* const & b){
	return *a < *b;
}




/****
 * 	Calculates dominant orientation in accordance with the SURF-descriptor orientation assignment
 *
 *	This version uses .5s offset in responses
 */
template <typename scale_pyramid_type>
float assignOrientation(
		const scale_pyramid_type& scale_pyramid,
		interest_point_position& position
){
	static const bool TIME_MEASURE = false;
	using _basic_image_signal::f2i;
	using _basic_image_signal::ImageLength;
	using _basic_image_signal::merge_sort;


	static const int RESPONSES_ARRAY_SIZE = 128;
	static const discern_type gauss[6][6] = {
		{	.03737806,	.02911006,	.01765614,	.008340172,	.003068178,	.0008790477	},
		{	.02911006,	.02267094,	.01375062,	.006495333,	.002389499,	.000684603	},
		{	.01765614,	.01375062,	.008340172,	.003939618,	.001449305,	0			},
		{	.008340172,	.006495333,	.003939618,	.001860944,	.000684603,	0			},
		{	.003068178,	.002389499,	.001449305,	.000684603, 0, 			0			},
		{	.0008790477,.000684603, 0, 			0, 			0, 			0			}
	};

	const float& cx = position.pos.x;
	const float& cy = position.pos.y;

	int current_index = 0;
	gradient_response haar_responses_array[RESPONSES_ARRAY_SIZE];
	gradient_response* haar_responses[RESPONSES_ARRAY_SIZE];

	//TODO: Select sample layer
	const image_signal<pyramid_data_type>& scale_image = scale_pyramid.maps[0];
//	cerr<< "Need to implement scale selection for orientation assignment";

	const int half_size = f2i(2*position.scale); //Lobe_size should be ( 4 * s ) => half size = 2*s

	for(int pos_y = 0; pos_y < 6; pos_y++){
		const ImageLength dy = position.scale * (pos_y + .5f);
		for(int pos_x = 0; pos_x < 6; pos_x++){
			const ImageLength dx = position.scale * (pos_x + .5f);
			const discern_type weight = gauss[pos_y][pos_x];

			if(weight > 0){
				haar_responses[current_index] = &haar_responses_array[current_index];
				haar_responses_array[current_index++] = gradient_response( scale_image, f2i(cy - dy),f2i(cx - dx), half_size,weight);

				haar_responses[current_index] = &haar_responses_array[current_index];
				haar_responses_array[current_index++] = gradient_response( scale_image, f2i(cy - dy),f2i(cx + dx), half_size,weight);

				haar_responses[current_index] = &haar_responses_array[current_index];
				haar_responses_array[current_index++] = gradient_response( scale_image, f2i(cy + dy),f2i(cx - dx), half_size,weight);

				haar_responses[current_index] = &haar_responses_array[current_index];
				haar_responses_array[current_index++] = gradient_response( scale_image, f2i(cy + dy),f2i(cx + dx), half_size,weight);
			}
		}
	}
	const int total_count = current_index;

	_basic_image_signal::Timer t;
	if(TIME_MEASURE)
		t.mark();

	merge_sort<gradient_response*, compareHaarPointers>(haar_responses, total_count);
	const float orientation = getMaximumSector(haar_responses,total_count);

	if(TIME_MEASURE){
		t.mark();
		cerr << "Using sort-search, finding max took "<< 1000*t.get_time() << " ms\n";
	}


	/****
	 * Evaluate using brute search for maximum
	 */
	//plot_angle_sums(haar_responses,total_count);

	position.orientation = orientation;
	return orientation;
}



inline void plot_angle_sums(gradient_response** haar_responses, const int total_count){
	using _basic_image_signal::euclidean_remainder;
	static const int nof_angles = 256;
	static const bool SHOW_PLOT = false;
	static const bool TIME_MEASURE = false;
	float* norms = new float[nof_angles];
	_basic_image_signal::Timer t;

	if(TIME_MEASURE)
		t.mark();
	for(int k = 0;k<nof_angles;k++){
		const float this_angle = (2*M_PI*k)/(nof_angles) - M_PI;
		float cumlsum_x = 0;
		float cumlsum_y = 0;
		for(int l=0;l<total_count;l++){
			if(euclidean_remainder(haar_responses[l]->angle - this_angle,2*M_PI) <= M_PI/3){
				cumlsum_x += haar_responses[l]->dx;
				cumlsum_y += haar_responses[l]->dy;
			}
			const float norm_squared = cumlsum_x*cumlsum_x + cumlsum_y*cumlsum_y;
			if(SHOW_PLOT)
				norms[k] = sqrt(norm_squared);
		}
	}
	if(TIME_MEASURE){
		t.mark();
		cerr<<"Using "<<nof_angles<<" divisions finding max took " << 1000*t.get_time() << " ms\n";
	}
	if(SHOW_PLOT)
		showDebugPlot(norms,nof_angles,127);
	delete[] norms;
}



inline float getMaximumSector(gradient_response** haar_responses, const int total_count){
	static const float SECTOR_SIZE = M_PI / 3;
	static const bool SHOW_PLOT = false;

	/****
	 * Arrays used for verification using plot
	 */
	float* norms = 0;
	float* angles = 0;

	if(SHOW_PLOT){
		norms = new float[total_count];
		memset(norms, 0, sizeof(float[total_count]));
		angles = new float[total_count];
		memset(angles, 0, sizeof(float[total_count]));
	}

	/****
	 * Algorithm variables
	 */
	float norm_for_maximum = 0;

	discern_type cumulative_sum_X = 0;
	discern_type cumulative_sum_Y = 0;

	discern_type max_X = 0;
	discern_type max_Y = 0;

	int tailing_index = 0;
	int leading_index = 0;

	/*****
	 * At first, search for maximum from angles closest to zero with a sliding window
	 * of SECTOR_SIZE span. Search is carried out until leading angle loops around
	 * the end of the array.
	 */
	while( leading_index < total_count ){//TODO: Fix stop angles
		const float tailing_stop_angle = haar_responses[leading_index]->angle - SECTOR_SIZE;
		if( haar_responses[tailing_index]->angle <= tailing_stop_angle ){
			cumulative_sum_X -= haar_responses[tailing_index]->dx;
			cumulative_sum_Y -= haar_responses[tailing_index]->dy;
			tailing_index++;
		}

		const float leading_stop_angle = haar_responses[tailing_index]->angle + SECTOR_SIZE;
		while( leading_index < total_count && haar_responses[leading_index]->angle <= leading_stop_angle ){
			cumulative_sum_X += haar_responses[leading_index]->dx;
			cumulative_sum_Y += haar_responses[leading_index]->dy;
			leading_index++;
		}

		const discern_type norm_squared = cumulative_sum_X*cumulative_sum_X + cumulative_sum_Y*cumulative_sum_Y;
		if(norm_squared<0){
			cerr<<"norm is negative, cumlsums: ["<<cumulative_sum_X<<","<<cumulative_sum_Y<<"]\n";
		}else if(SHOW_PLOT){
			if(std::isnan(sqrt(norm_squared))){
				cerr<<"norm is error, cumlsums: ["<<cumulative_sum_X<<","<<cumulative_sum_Y<<"], tailing angle:"<<haar_responses[tailing_index]->angle<<"\n";
			}else{
				norms[tailing_index] = sqrt(norm_squared);
				angles[tailing_index] = haar_responses[tailing_index]->angle;
			}
		}
		if(norm_squared > norm_for_maximum){
			norm_for_maximum = norm_squared;
			max_X = cumulative_sum_X;
			max_Y = cumulative_sum_Y;
		}
	}
	leading_index = 0;
	/*****
	 * Secondly, search until the tailing angle reaches end of array.
	 * Difference from first part is the added constant 2*M_PI in
	 * "stop angles" to account for the discontinuity in angle values
	 */
	while( tailing_index < total_count ){
		const float tailing_stop_angle = haar_responses[leading_index]->angle + 2*M_PI - SECTOR_SIZE;
		if( haar_responses[tailing_index]->angle <= tailing_stop_angle ){
			cumulative_sum_X -= haar_responses[tailing_index]->dx;
			cumulative_sum_Y -= haar_responses[tailing_index]->dy;
			tailing_index++;
			if(tailing_index >= total_count){
				//cerr<<"["<<tailing_stop_angle<<","<<haar_responses[total_count-1]->angle<<"]"<<"["<<tailing_stop_angle-haar_responses[total_count-1]->angle<<"/"<<SECTOR_SIZE<<"]\n";
				if(SHOW_PLOT){
					showDebugPlot(angles,norms,total_count);
					delete[] norms;
					delete[] angles;
				}
				return atan2(max_Y,max_X);
			}
		}

		const float leading_stop_angle = haar_responses[tailing_index]->angle - 2*M_PI + SECTOR_SIZE;
		while( haar_responses[leading_index]->angle <= leading_stop_angle ){
			cumulative_sum_X += haar_responses[leading_index]->dx;
			cumulative_sum_Y += haar_responses[leading_index]->dy;
			leading_index++;
		}

		const discern_type norm_squared = cumulative_sum_X*cumulative_sum_X + cumulative_sum_Y*cumulative_sum_Y;
		if(norm_squared<0){
			cerr<<"norm is negative, cumlsums: ["<<cumulative_sum_X<<","<<cumulative_sum_Y<<"]\n";
		}else if(SHOW_PLOT){
			if(std::isnan(sqrt(norm_squared))){
				cerr<<"norm is error, cumlsums: ["<<cumulative_sum_X<<","<<cumulative_sum_Y<<"], tailing angle:"<<haar_responses[tailing_index]->angle<<"\n";
			}else{
				norms[tailing_index] = sqrt(norm_squared);
				angles[tailing_index] = haar_responses[tailing_index]->angle;
			}
		}
		if(norm_squared > norm_for_maximum){
			norm_for_maximum = norm_squared;
			max_X = cumulative_sum_X;
			max_Y = cumulative_sum_Y;
		}
	}

	if(SHOW_PLOT){
		showDebugPlot(angles,norms,total_count);
		delete[] norms;
		delete[] angles;
	}
	return atan2(max_Y,max_X);
}

}


