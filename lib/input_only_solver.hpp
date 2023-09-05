//
//  input_only_solver.hpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 12/10/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef input_only_solver_hpp
#define input_only_solver_hpp

#include "solver.hpp"

class InputOnlySolver : Solver{
public:
    InputOnlySolver() {};
    
    solver_res_t check_workload_without_query(Workload wl);
    solver_res_t check_workload_with_query(Workload wl, IndexedExample* eg);
    
};

#endif /* input_only_solver_hpp */
