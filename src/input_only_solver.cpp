//
//  input_only_solver.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 12/10/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "input_only_solver.hpp"

solver_res_t InputOnlySolver::check_workload_without_query(Workload wl){
    (void) wl;
    time_typ start_time = noww();
    
    
    // TODO: implement
    
    
    //------------ Timing Stats
    time_typ end_time = noww();
    unsigned long long int seconds = get_diff_sec(start_time, end_time);
    
    check_workload_without_query_total_invoc++;
    check_workload_without_query_total_time += seconds;
    if (seconds > check_workload_without_query_max_time){
        check_workload_without_query_max_time = seconds;
    }
    //-------------------------
    
    return solver_res_t::UNKNOWN;
}

solver_res_t InputOnlySolver::check_workload_with_query(Workload wl, IndexedExample* eg){
    (void) wl;
    (void) eg;

    time_typ start_time = noww();
    
    
    // TODO: implement
    
    
    //------------ Timing Stats
    time_typ end_time = noww();
    unsigned long long int seconds = get_diff_sec(start_time, end_time);
    
    check_workload_with_query_total_invoc++;
    check_workload_with_query_total_time += seconds;
    if (seconds > check_workload_with_query_max_time){
        check_workload_with_query_max_time = seconds;
    }
    //-------------------------
    
    return solver_res_t::UNKNOWN;

}
