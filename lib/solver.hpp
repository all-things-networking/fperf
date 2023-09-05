//
//  solver.hpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 12/10/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef solver_hpp
#define solver_hpp

#include "workload.hpp"
#include "example.hpp"

enum class solver_res_t {SAT = 0, UNSAT, UNKNOWN};
std::ostream& operator<<(std::ostream& os, const solver_res_t& res);

class Solver {
public:
    Solver(){}
    virtual solver_res_t check_workload_without_query(Workload wl) = 0;
    virtual solver_res_t check_workload_with_query(Workload wl, IndexedExample* eg) = 0;
    
    unsigned long long int get_check_workload_without_query_max_time();
    unsigned long long int get_check_workload_without_query_avg_time();
    
    unsigned long long int get_check_workload_with_query_max_time();
    unsigned long long int get_check_workload_with_query_avg_time();
    
protected:
    unsigned long long int check_workload_without_query_max_time = 0;
    unsigned long long int check_workload_without_query_total_time = 0;
    unsigned int check_workload_without_query_total_invoc = 0;
    
    unsigned long long int check_workload_with_query_max_time = 0;
    unsigned long long int check_workload_with_query_total_time = 0;
    unsigned int check_workload_with_query_total_invoc = 0;
};

#endif /* solver_hpp */
