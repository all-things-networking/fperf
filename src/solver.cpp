//
//  solver.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 2/3/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "solver.hpp"

unsigned long long int Solver::get_check_workload_without_query_max_time() {
    return check_workload_without_query_max_time;
}

unsigned long long int Solver::get_check_workload_without_query_avg_time() {
    return check_workload_without_query_total_invoc == 0
               ? 0
               : check_workload_without_query_total_time / check_workload_without_query_total_invoc;
}

unsigned long long int Solver::get_check_workload_with_query_max_time() {
    return check_workload_with_query_max_time;
}

unsigned long long int Solver::get_check_workload_with_query_avg_time() {
    return check_workload_with_query_total_invoc == 0
               ? 0
               : check_workload_with_query_total_time / check_workload_with_query_total_invoc;
}

std::ostream& operator<<(std::ostream& os, const solver_res_t& res) {
    switch (res) {
        case solver_res_t::SAT: os << "SAT"; break;
        case solver_res_t::UNSAT: os << "UNSAT"; break;
        default: os << "UNKNOWN"; break;
    }
    return os;
}
