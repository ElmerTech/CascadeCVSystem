/*
 * test_functions.h
 *
 *  Created on: 17 mar 2014
 *      Author: johan
 */

#ifndef CVS_DEBUG_TEST_FUNCTIONS_H_
#define CVS_DEBUG_TEST_FUNCTIONS_H_

#include "../../CascadeImageMatchingConfig.h"
#include "../../CascadeCVSEvaluationInterface.h"
#include "../CVSTemplateUtilities.h"
#include "../CascadeDataStructures.h"
#include <iostream>
#include <iomanip>
#include <type_traits>

namespace CascadeCVSTemplate{

template < typename cvs_scale_pyramid>
void debug_test_state_machine_approach(const cvs_scale_pyramid& discern_map, size_t level_no, discern_type sigma, discern_type cap, discern_type norm, CascadeCVSEvaluationInterface* eval_object)
{
	internal_interest_point_position_set int_pos_state;
	select_interest_points_state_machine_impl(discern_map.maps[level_no], int_pos_state, sigma, cap * mptl::quad<double>(discern_map.maps[level_no].sample_factor()) * norm, eval_object);
	internal_interest_point_position_set int_pos;
	select_interest_points_impl(discern_map.maps[level_no], int_pos, sigma, cap * mptl::quad<double>(discern_map.maps[level_no].sample_factor()) * norm, eval_object);
	std::cout << "level " << level_no << " ( "
			<< int_pos_state.maximum_positions.size() << " / "
			<< int_pos.maximum_positions.size() <<" ), ("
			<< int_pos_state.minimum_positions.size()<< " / "
			<< int_pos.minimum_positions.size() << " )\n";
	if(
			int_pos_state.maximum_positions.size() != int_pos.maximum_positions.size() ||
			int_pos_state.minimum_positions.size() != int_pos.minimum_positions.size()
	) {
		int nof_max_pos = std::min( int_pos_state.maximum_positions.size(), int_pos.maximum_positions.size() );
		for(int i=0;i<nof_max_pos;i++) {
			if(
					int_pos_state.maximum_positions[i].pos.x != int_pos.maximum_positions[i].pos.x ||
					int_pos_state.maximum_positions[i].pos.y != int_pos.maximum_positions[i].pos.y
			) {
				if(i>0) {
					std::cout << "i-1: " << " ( " << int_pos_state.maximum_positions[i-1].pos << "\n";
				}
				{
					std::cout.precision(20);
					auto&& position = int_pos_state.maximum_positions[i].pos;
					typename std::remove_reference<decltype(discern_map.maps[level_no])>::type::offset_type pos(position.x,position.y);
					auto&& raw_offset = discern_map.maps[level_no].get_raw_offset(pos);
					auto&& c_value = calcResponse( discern_map.maps[level_no].point( raw_offset.y, raw_offset.x ) );
					std::cout << std::setw(16) << (calcResponse( discern_map.maps[level_no].point( raw_offset.y-1, raw_offset.x-2 ) ) - c_value )<< " "
							<< std::setw(16) << (calcResponse( discern_map.maps[level_no].point( raw_offset.y-1, raw_offset.x-1 ) ) - c_value )<< " "
							<< std::setw(16) << (calcResponse( discern_map.maps[level_no].point( raw_offset.y-1, raw_offset.x ) ) - c_value )<< " "
							<< std::setw(16) << (calcResponse( discern_map.maps[level_no].point( raw_offset.y-1, raw_offset.x+1 ) ) - c_value )<< "\n";
					std::cout << std::setw(16) << (calcResponse( discern_map.maps[level_no].point( raw_offset.y, raw_offset.x-2 ) ) - c_value )<< " "
							<< std::setw(16) << (calcResponse( discern_map.maps[level_no].point( raw_offset.y, raw_offset.x-1 ) ) - c_value )<< " "
							<< std::setw(16) << (calcResponse( discern_map.maps[level_no].point( raw_offset.y, raw_offset.x ) ) - c_value )<< " "
							<< std::setw(16) << (calcResponse( discern_map.maps[level_no].point( raw_offset.y, raw_offset.x+1 ) ) - c_value )<< "\n";
					std::cout << std::setw(16) << (calcResponse( discern_map.maps[level_no].point( raw_offset.y+1, raw_offset.x-2 ) ) - c_value )<< " "
							<< std::setw(16) << (calcResponse( discern_map.maps[level_no].point( raw_offset.y+1, raw_offset.x-1 ) ) - c_value )<< " "
							<< std::setw(16) << (calcResponse( discern_map.maps[level_no].point( raw_offset.y+1, raw_offset.x ) ) - c_value )<< " "
							<< std::setw(16) << (calcResponse( discern_map.maps[level_no].point( raw_offset.y+1, raw_offset.x+1 ) ) - c_value )<< "\n";
				}
				{
					auto&& position = int_pos.maximum_positions[i].pos;
					typename std::remove_reference<decltype(discern_map.maps[level_no])>::type::offset_type pos(position.x,position.y);
					auto&& raw_offset = discern_map.maps[level_no].get_raw_offset(pos);
					std::cout << std::setw(16) << calcResponse( discern_map.maps[level_no].point( raw_offset.y-1, raw_offset.x-2 ) ) << " "
							<< std::setw(16) << calcResponse( discern_map.maps[level_no].point( raw_offset.y-1, raw_offset.x-1 ) ) << " "
							<< std::setw(16) << calcResponse( discern_map.maps[level_no].point( raw_offset.y-1, raw_offset.x ) ) << " "
							<< std::setw(16) << calcResponse( discern_map.maps[level_no].point( raw_offset.y-1, raw_offset.x+1 ) ) << "\n";
					std::cout << std::setw(16) << calcResponse( discern_map.maps[level_no].point( raw_offset.y, raw_offset.x-2 ) ) << " "
							<< std::setw(16) << calcResponse( discern_map.maps[level_no].point( raw_offset.y, raw_offset.x-1 ) ) << " "
							<< std::setw(16) << calcResponse( discern_map.maps[level_no].point( raw_offset.y, raw_offset.x ) ) << " "
							<< std::setw(16) << calcResponse( discern_map.maps[level_no].point( raw_offset.y, raw_offset.x+1 ) ) << "\n";
					std::cout << std::setw(16) << calcResponse( discern_map.maps[level_no].point( raw_offset.y+1, raw_offset.x-2 ) ) << " "
							<< std::setw(16) << calcResponse( discern_map.maps[level_no].point( raw_offset.y+1, raw_offset.x-1 ) ) << " "
							<< std::setw(16) << calcResponse( discern_map.maps[level_no].point( raw_offset.y+1, raw_offset.x ) ) << " "
							<< std::setw(16) << calcResponse( discern_map.maps[level_no].point( raw_offset.y+1, raw_offset.x+1 ) ) << "\n";
				}
				std::cout << "i: " << i << " ( " << int_pos_state.maximum_positions[i].pos << " <> "
						<< int_pos.maximum_positions[i].pos << " )\n";
				break;
			}
		}
		assert(int_pos_state.maximum_positions.size() == int_pos.maximum_positions.size() );
		int nof_min_pos = std::min(int_pos_state.minimum_positions.size(), int_pos.minimum_positions.size());
		for(int i=0;i<nof_max_pos;i++) {
			if(
					int_pos_state.minimum_positions[i].pos.x != int_pos.minimum_positions[i].pos.x ||
					int_pos_state.minimum_positions[i].pos.y != int_pos.minimum_positions[i].pos.y
			) {
				if(i>0) {
					std::cout << "i-1: " << " ( " << int_pos_state.minimum_positions[i-1].pos << "\n";
				}
				{
					std::cout.precision(20);
					auto&& position = int_pos_state.minimum_positions[i].pos;
					typename std::remove_reference<decltype(discern_map.maps[level_no])>::type::offset_type pos(position.x,position.y);
					auto&& raw_offset = discern_map.maps[level_no].get_raw_offset(pos);
					auto&& c_value = calcResponse( discern_map.maps[level_no].point( raw_offset.y, raw_offset.x ) );
					std::cout << std::setw(16) << (calcResponse( discern_map.maps[level_no].point( raw_offset.y-1, raw_offset.x-2 ) ) - c_value )<< " "
							<< std::setw(16) << (calcResponse( discern_map.maps[level_no].point( raw_offset.y-1, raw_offset.x-1 ) ) - c_value )<< " "
							<< std::setw(16) << (calcResponse( discern_map.maps[level_no].point( raw_offset.y-1, raw_offset.x ) ) - c_value )<< " "
							<< std::setw(16) << (calcResponse( discern_map.maps[level_no].point( raw_offset.y-1, raw_offset.x+1 ) ) - c_value )<< "\n";
					std::cout << std::setw(16) << (calcResponse( discern_map.maps[level_no].point( raw_offset.y, raw_offset.x-2 ) ) - c_value )<< " "
							<< std::setw(16) << (calcResponse( discern_map.maps[level_no].point( raw_offset.y, raw_offset.x-1 ) ) - c_value )<< " "
							<< std::setw(16) << (calcResponse( discern_map.maps[level_no].point( raw_offset.y, raw_offset.x ) ) - c_value )<< " "
							<< std::setw(16) << (calcResponse( discern_map.maps[level_no].point( raw_offset.y, raw_offset.x+1 ) ) - c_value )<< "\n";
					std::cout << std::setw(16) << (calcResponse( discern_map.maps[level_no].point( raw_offset.y+1, raw_offset.x-2 ) ) - c_value )<< " "
							<< std::setw(16) << (calcResponse( discern_map.maps[level_no].point( raw_offset.y+1, raw_offset.x-1 ) ) - c_value )<< " "
							<< std::setw(16) << (calcResponse( discern_map.maps[level_no].point( raw_offset.y+1, raw_offset.x ) ) - c_value )<< " "
							<< std::setw(16) << (calcResponse( discern_map.maps[level_no].point( raw_offset.y+1, raw_offset.x+1 ) ) - c_value )<< "\n";
				}
				{
					auto&& position = int_pos.minimum_positions[i].pos;
					typename std::remove_reference<decltype(discern_map.maps[level_no])>::type::offset_type pos(position.x,position.y);
					auto&& raw_offset = discern_map.maps[level_no].get_raw_offset(pos);
					std::cout << std::setw(16) << calcResponse( discern_map.maps[level_no].point( raw_offset.y-1, raw_offset.x-2 ) ) << " "
							<< std::setw(16) << calcResponse( discern_map.maps[level_no].point( raw_offset.y-1, raw_offset.x-1 ) ) << " "
							<< std::setw(16) << calcResponse( discern_map.maps[level_no].point( raw_offset.y-1, raw_offset.x ) ) << " "
							<< std::setw(16) << calcResponse( discern_map.maps[level_no].point( raw_offset.y-1, raw_offset.x+1 ) ) << "\n";
					std::cout << std::setw(16) << calcResponse( discern_map.maps[level_no].point( raw_offset.y, raw_offset.x-2 ) ) << " "
							<< std::setw(16) << calcResponse( discern_map.maps[level_no].point( raw_offset.y, raw_offset.x-1 ) ) << " "
							<< std::setw(16) << calcResponse( discern_map.maps[level_no].point( raw_offset.y, raw_offset.x ) ) << " "
							<< std::setw(16) << calcResponse( discern_map.maps[level_no].point( raw_offset.y, raw_offset.x+1 ) ) << "\n";
					std::cout << std::setw(16) << calcResponse( discern_map.maps[level_no].point( raw_offset.y+1, raw_offset.x-2 ) ) << " "
							<< std::setw(16) << calcResponse( discern_map.maps[level_no].point( raw_offset.y+1, raw_offset.x-1 ) ) << " "
							<< std::setw(16) << calcResponse( discern_map.maps[level_no].point( raw_offset.y+1, raw_offset.x ) ) << " "
							<< std::setw(16) << calcResponse( discern_map.maps[level_no].point( raw_offset.y+1, raw_offset.x+1 ) ) << "\n";
				}
				std::cout << "i: " << i << " ( " << int_pos_state.minimum_positions[i].pos << " <> "
						<< int_pos.minimum_positions[i].pos << " )\n";
				break;
			}
		}
		assert(int_pos_state.minimum_positions.size() == int_pos.minimum_positions.size() );
	}
}

}

#endif /* CVS_DEBUG_TEST_FUNCTIONS_H_ */
