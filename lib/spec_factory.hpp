//
//  spec_factory.hpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 12/14/22.
//  Copyright © 2022 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef spec_factory_hpp
#define spec_factory_hpp

#include <iostream>
#include <map>
#include <set>
#include <tuple>
#include <variant>
#include <vector>

#include "cost.hpp"
#include "example.hpp"
#include "metric.hpp"
#include "shared_config.hpp"
#include "util.hpp"
#include "workload.hpp"

struct RandomSpecGenerationParameters {
    bool time_valid;
    uniform_int_distribution<unsigned int> const_dist;
    bool bound_with_dist = false;
};

class SpecFactory {

public:
    RandomSpecGenerationParameters get_metric_params(metric_t metric_type);

    SpecFactory(SharedConfig* shared_config);

    //**** TimedSpec ****//
    TimedSpec random_timed_spec();
    void pick_neighbors(TimedSpec& spec, vector<TimedSpec>& neighbors);

    //**** WlSpec ****//
    wl_spec_t random_wl_spec();
    void pick_neighbors(wl_spec_t& spec, vector<wl_spec_t>& neighbors);

    //**** COMP ****//
    Comp random_comp();
    void pick_neighbors(Comp& spec, vector<Comp>& neighbors);

    //**** rhs_t ****//
    rhs_t random_rhs();
    rhs_t random_rhs(RandomSpecGenerationParameters params);
    void pick_rhs_neighbors(rhs_t rhs, vector<rhs_t>& neighbors);
    void
    pick_rhs_neighbors(rhs_t rhs, vector<rhs_t>& neighbors, RandomSpecGenerationParameters params);

    //**** lhs_t ****//
    lhs_t random_lhs();
    void pick_lhs_neighbors(lhs_t lhs, vector<lhs_t>& neighbors);

    //**** m_expr_t ****//
    m_expr_t random_m_expr();
    void pick_m_expr_neighbors(m_expr_t trf, vector<m_expr_t>& neighbors);

    //**** QSUM ****//
    qset_t random_qsum_qset();
    QSum random_qsum();
    void pick_neighbors(QSum& qsum, vector<m_expr_t>& neighbors);

    //**** INDIV ****//
    Indiv random_indiv();
    void pick_neighbors(Indiv& indiv, vector<m_expr_t>& neighbors);

    //**** TIME ****//
    Time random_time();

    //**** COMP ****/
    op_t random_op();

private:
    SharedConfig* shared_config = NULL;
    unsigned int total_time;
    unsigned int in_queue_cnt;
    qset_t target_queues;
    Dists* dists = NULL;
};

#endif /* spec_factory_hpp */
