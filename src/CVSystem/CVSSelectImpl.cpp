/*
 * CVSSelectImpl.cpp
 *
 *  Created on: 1 jun 2014
 *      Author: johan
 */

#include "CVSSelectImpl.h"



namespace CascadeCVSTemplate{


void select_interest_points_state_machine_impl(
		const image_signal<discern_type>& cur_im,
		internal_interest_point_position_set& intrest_point_positions,
		const float sigma,
		discern_type threshold,
		CascadeCVSEvaluationInterface* eval_object
){


	static constexpr bool DEBUG_DRAW_REJECTED = true;
	static constexpr bool SKIP_INTERPOLATION = true;

	value_storage< discern_type > packed_values( cur_im.raw_width() );

	const int start_y = 1;
	const int stop_y = cur_im.raw_height()-1;

	const int start_x = 1;
	const int stop_x = cur_im.raw_width()-1;

	const discern_type min_level_threshold = -threshold;
	const discern_type max_level_threshold = threshold;

//	evaluation_parameters eval_params;
//	for( int i = 0; i < 9; i++ )
//		eval_params.passed[i] = 0;
//	for(int i=8;i>=0;i--){
//		cerr<<i<<": "<< eval_params.passed[i]<<", ";
//	}
//	cerr<<"\n";

	discern_type a,b;
	for(int n = start_y; n < stop_y; n++){

		const const_image_signal_point<discern_type> cur_line( cur_im, n, 0);
		int m = start_x;

//		packed_values.set_current_index(m-1);
//		packed_values.current() = getResponse( cur_line.shifted(0,m-1) );
//		packed_values.next() = getResponse( cur_line.shifted(0,m) );
		int next_m = m;
		switch( ( stop_x - next_m ) & 0x1 ){
		case 0:
			a = getResponse( cur_line.shifted(0,m-1) );
			b = getResponse( cur_line.shifted(0,m) );
			break;
		case 1:
			b = getResponse( cur_line.shifted(0,m-1) );
			a = getResponse( cur_line.shifted(0,m) );
			break;
		}

		if( packed_values.current() < packed_values.next() )
			goto find_maximum;

		/*** State find minimum ***/
		find_minimum: {
			const bool end_of_line = find_extrema< extrema::minimum, DEBUG_DRAW_REJECTED, SKIP_INTERPOLATION >(
					cur_im,
					cur_line,
					n,
					next_m,
					stop_x,
					packed_values,
					a,
					b,
					min_level_threshold,
					intrest_point_positions,
					sigma,
					eval_object
			);
			if( end_of_line )
				continue;
			if(packed_values.current() == packed_values.next()){
//				assert(packed_values.current() != packed_values.next());
				skip_level_part(cur_line, next_m, stop_x, packed_values);
				if( packed_values.current() > packed_values.next() )
					goto find_minimum;
			}
		}

		/*** State find minimum ***/
		find_maximum: {
			/***                    ***/
			const bool end_of_line = find_extrema< extrema::maximum, DEBUG_DRAW_REJECTED, SKIP_INTERPOLATION >(
					cur_im,
					cur_line,
					n,
					next_m,
					stop_x,
					packed_values,
					a,
					b,
					max_level_threshold,
					intrest_point_positions,
					sigma,
					eval_object
			);
			if( !end_of_line ){
//				assert(packed_values.current() != packed_values.next());
				if(packed_values.current() == packed_values.next()){
					skip_level_part(cur_line, next_m, stop_x, packed_values);
					if( packed_values.current() < packed_values.next() )
						goto find_maximum;
				}
				goto find_minimum;
			}
		}
	}
//	for(int i=8;i>=0;i--){
//		cerr<<i<<": "<< eval_params.passed[i]<<", ";
//	}
//	cerr<<"\n";
//	cerr << "nof_pos: " << intrest_point_positions.accepted_positions.size() << " (" << intrest_point_positions.rejected_positions.size() << ")\n";

}

void select_interest_points_impl(
		const image_signal<discern_type>& cur_im,
		internal_interest_point_position_set& intrest_point_positions,
		const float sigma,
		discern_type threshold,
		CascadeCVSEvaluationInterface* eval_object
){
	static const bool DEBUG_DRAW_REJECTED = true;
	static const bool SKIP_INTERPOLATION = true;

	mptl::data_holder<discern_type> packed_values(32);

	int passed[9];
	for(int i=0;i<9;i++)
		passed[i] = 0;

	const int start_y = 1;
	const int stop_y = cur_im.raw_height()-1;

	const int start_x = 1;
	const int stop_x = cur_im.raw_width()-1;

	discern_type level_threshold = threshold;

//	discern_type pos_sum=0;
//	int nof_pos=0;
	for(int n = start_y; n < stop_y; n++){

		discern_type last_p = getResponse(cur_im.point(n, start_x-1));
		for(int m = start_x; m < stop_x; m++){
			const int current_m = m;
			const const_image_signal_point<discern_type> cur_p( cur_im, n, m);
//			int p_count = 0;

			const discern_type c_p = getResponse( cur_p );
//			discern_type laplace = 0;
//			const discern_type c_p = getResponse_with_laplacian( cur_p, &laplace );
//			if( c_p > 0 ){
//				pos_sum+=c_p;
//				nof_pos++;
//			}
			if(c_p > level_threshold ){

//				passed[p_count++]++;
				packed_values[4] = c_p;

//				if(laplace*laplace > 9.f*c_p)
//					continue;
				const discern_type cnss = last_p;
				if(c_p <= cnss)
					continue;
//				passed[p_count++]++;
				packed_values[3] = cnss;

				last_p = c_p;

				const discern_type cpss = getResponse(cur_p.shifted(0, 1));
				if(c_p <= cpss)
					continue;
//				passed[p_count++]++;
				packed_values[5] = cpss;

				const discern_type cprs = getResponse(cur_p.shifted( 1,0));
				if(c_p <= cprs)
					continue;
//				passed[p_count++]++;
				packed_values[7] = cprs;


				const discern_type cnrs = getResponse(cur_p.shifted(-1,0));
				if(c_p <= cnrs)
					continue;
//				passed[p_count++]++;
				packed_values[1] = cnrs;

				packed_values[6] = getResponse(cur_p.shifted(1,-1));
				if(c_p <= packed_values[6])
					continue;
//				passed[p_count++]++;

				packed_values[2] = getResponse(cur_p.shifted(-1, 1));
				if(c_p <= packed_values[2])
					continue;
//				passed[p_count++]++;

				packed_values[8] = getResponse(cur_p.shifted(1,1));
				if(c_p <= packed_values[8])
					continue;
//				passed[p_count++]++;

				packed_values[0] = getResponse(cur_p.shifted(-1,-1));
				if(c_p <= packed_values[0])
					continue;
//				passed[p_count++]++;
			}else if( c_p < -level_threshold ){
				packed_values[4] = c_p;

//				if(laplace*laplace > 9.f*c_p)
//					continue;

				const discern_type cnss = last_p;
				if(c_p >= cnss)
					continue;
				packed_values[3] = cnss;

				last_p = c_p;

				const discern_type cpss = getResponse(cur_p.shifted(0, 1));
				if(c_p >= cpss)
					continue;
				packed_values[5] = cpss;

				const discern_type cprs = getResponse(cur_p.shifted( 1,0));
				if(c_p >= cprs)
					continue;
				packed_values[7] = cprs;


				const discern_type cnrs = getResponse(cur_p.shifted(-1,0));
				if(c_p >= cnrs)
					continue;
				packed_values[1] = cnrs;

				packed_values[6] = getResponse(cur_p.shifted(1,-1));
				if(c_p >= packed_values[6])
					continue;

				packed_values[2] = getResponse(cur_p.shifted(-1, 1));
				if(c_p >= packed_values[2])
					continue;

				packed_values[8] = getResponse(cur_p.shifted(1,1));
				if(c_p >= packed_values[8])
					continue;

				packed_values[0] = getResponse(cur_p.shifted(-1,-1));
				if(c_p >= packed_values[0])
					continue;
			}else{
				last_p = c_p;
				continue;
			}

			auto offset = cur_im.offset();
			auto step_size = cur_im.sample_factor();
			interest_point_position interest_point = {
					interest_point.pos = {
							static_cast<interest_point_position::position_type>( offset.x + current_m * step_size ),
							static_cast<interest_point_position::position_type>( offset.y + n * step_size ) },
							interest_point.scale = sigma,
							interest_point.magnitude = static_cast<interest_point_position::position_type>(c_p) };

			if(SKIP_INTERPOLATION){
				if(c_p > 0){
					intrest_point_positions.maximum_positions.push_back( interest_point );
				}else{
					intrest_point_positions.minimum_positions.push_back( interest_point );
				}
			}else{
				if( interpolate_position_2d( packed_values.start(), &interest_point, step_size, step_size*1.2f/3))
					if(c_p > 0){
						intrest_point_positions.maximum_positions.push_back( interest_point );
					}else{
						intrest_point_positions.minimum_positions.push_back( interest_point );
					}
				else{
					if(c_p > 0){
						intrest_point_positions.rejected_maximum_positions.push_back( interest_point );
					}else{
						intrest_point_positions.rejected_minimum_positions.push_back( interest_point );
					}
					if(DEBUG_DRAW_REJECTED && eval_object){
						image_signal<BGR8>* debugIm = eval_object->get_image();
						if(debugIm)
							drawInterest( *debugIm, interest_point,BGR8(255,0,0) );
					}
				}
			}
		}
	}
	//		cerr<<"nof_pos: "<<nof_pos<<", pos_avg: "<< abs_factor * pos_sum / nof_pos<<"\n";
	//		for(int i=26;i>=0;i--){
	//			cerr<<i<<": "<< passed[i]<<", ";
	//		}
	//		cerr<<"\n";
//	cerr << "nof_pos: " << intrest_point_positions.accepted_positions.size() << " ("<< intrest_point_positions.rejected_positions.size() << ")\n";

}

}
