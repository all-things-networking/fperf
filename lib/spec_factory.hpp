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
    WlSpec* random_wl_spec();
    void pick_neighbors(WlSpec* spec, std::vector<WlSpec*>& neighbors);

    //**** COMP ****//
    WlSpec* random_comp();
    void pick_neighbors(Comp& spec, vector<Comp>& neighbors);

    //**** Rhs* ****//
    Rhs* random_rhs();
    Rhs* random_rhs(RandomSpecGenerationParameters params);
    void pick_rhs_neighbors(Rhs* rhs, vector<Rhs*>& neighbors);
    void
    pick_rhs_neighbors(Rhs* rhs, vector<Rhs*>& neighbors, RandomSpecGenerationParameters params);

    //**** MExpr* ****//
    MExpr* random_m_expr();
    void pick_m_expr_neighbors(MExpr* trf, vector<MExpr*>& neighbors);

    //**** QSUM ****//
    qset_t random_qsum_qset();
    QSum random_qsum();
    void pick_neighbors(QSum& qsum, vector<MExpr*>& neighbors);

    //**** INDIV ****//
    Indiv random_indiv();
    void pick_neighbors(Indiv& indiv, vector<MExpr*>& neighbors);

    //**** TIME ****//
    Time random_time();

    //**** COMP ****/
    Op random_op();

    typedef WlSpec* (SpecFactory::*SpecGeneratorFuncPtr)();
   vector<SpecGeneratorFuncPtr> spec_generators;

private:
    void initializeSpecs();

    SharedConfig* shared_config = NULL;
    unsigned int total_time;
    unsigned int in_queue_cnt;
    qset_t target_queues;
    Dists* dists = NULL;
};

#endif /* spec_factory_hpp */
