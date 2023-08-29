//
//  spec_factory.hpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 12/14/22.
//  Copyright © 2022 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef spec_factory_hpp
#define spec_factory_hpp

#include <variant>
#include <vector>
#include <map>
#include <iostream>
#include <tuple>
#include <set>

#include "util.hpp"
#include "metric.hpp"
#include "example.hpp"
#include "cost.hpp"
#include "shared_config.hpp"
#include "workload.hpp"

class SpecFactory{

public:
    SpecFactory(SharedConfig* shared_config);

    //**** TimedSpec ****//
    TimedSpec random_timed_spec();
    void pick_neighbors(TimedSpec& spec, vector<TimedSpec>& neighbors);

    //**** WlSpec ****//
    WlSpec random_wl_spec();
    void pick_neighbors(WlSpec& spec, vector<WlSpec>& neighbors);

    //**** rhs_t ****//
    rhs_t random_rhs();
    rhs_t random_rhs(bool time_valid, 
                     std::uniform_int_distribution<unsigned int> const_dist);
    void pick_rhs_neighbors(rhs_t rhs, vector<rhs_t>& neighbors);
    void pick_rhs_neighbors(rhs_t rhs, vector<rhs_t>& neighbors,
                        bool time_valid, std::uniform_int_distribution<unsigned int> const_dist);
    
    //**** lhs_t ****//
    lhs_t random_lhs();
    void pick_lhs_neighbors(lhs_t lhs, vector<lhs_t>& neighbors);
 
    //**** trf_t ****//
    trf_t random_trf();
    void pick_trf_neighbors(trf_t trf, vector<trf_t>& neighbors);
 
    //**** TSUM ****//
    qset_t random_tsum_qset(); 
    TSUM random_tsum();
    void pick_neighbors(TSUM& tsum, vector<trf_t>& neighbors);
   
    //**** TONE ****//
    TONE random_tone();  
    void pick_neighbors(TONE& tone, vector<trf_t>& neighbors);

    //**** TIME ****//
    TIME random_time();

    //**** COMP ****/
    comp_t random_comp();

private:
    SharedConfig* shared_config = NULL;
    unsigned int total_time;
    unsigned int in_queue_cnt;
    qset_t target_queues;
    Dists* dists = NULL;
};

#endif /* spec_factory_hpp */
