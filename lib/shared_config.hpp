//
//  shared_config.hpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 12/07/22.
//  Copyright © 2022 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef shared_config_hpp
#define shared_config_hpp

#include <random>
#include <vector>

#include "metric.hpp"
#include "params.hpp"
#include "util.hpp"

struct DistsParams {
    vector<double> rhs_selection_weights = DEFAULT_RHS_SELECTION_WEIGHTS;

    unsigned int rhs_const_min = DEFAULT_RHS_CONST_MIN;
    unsigned int rhs_const_max = DEFAULT_RHS_CONST_MAX;

    unsigned int rhs_time_coeff_min = DEFAULT_RHS_TIME_COEFF_MIN;
    unsigned int rhs_time_coeff_max = DEFAULT_RHS_TIME_COEFF_MAX;

    vector<double> trf_selection_weights = DEFAULT_TRF_SELECTION_WEIGHTS;
    vector<double> wl_metric_selection_weights = DEFAULT_WL_METRIC_SELECTION_WEIGHTS;

    unsigned int in_queue_cnt = 0;
    unsigned int total_time = 0;

    unsigned int op_range_min = DEFAULT_COMP_RANGE_MIN;
    unsigned int op_range_max = DEFAULT_COMP_RANGE_MAX;

    unsigned int enq_range_min = DEFAULT_ENQ_RANGE_MIN;
    unsigned int enq_range_max = DEFAULT_ENQ_RANGE_MAX;

    unsigned int pkt_meta1_val_max = 0;
    unsigned int pkt_meta2_val_max = 0;

    unsigned int random_seed = DEFAULT_RANDOM_SEED;
};

class Dists {
public:
    Dists(DistsParams params);

    mt19937& get_gen();
    double real_zero_to_one();

    unsigned int rhs();
    unsigned int rhs_const();
    unsigned int rhs_time_coeff();

    unsigned int trf();

    metric_t wl_metric();

    unsigned int input_queue();
    unsigned int input_queue_cnt();

    op_t op();

    unsigned int timestep();
    unsigned int enq();

    unsigned int pkt_metric1_val();
    unsigned int pkt_metric2_val();

    uniform_int_distribution<unsigned int>& get_pkt_meta1_val_dist();
    uniform_int_distribution<unsigned int>& get_pkt_meta2_val_dist();
    uniform_int_distribution<unsigned int>& get_rhs_const_dist();

private:
    mt19937 gen;
    uniform_real_distribution<double> real_zero_to_one_dist;

    discrete_distribution<unsigned int> rhs_dist;
    uniform_int_distribution<unsigned int> rhs_const_dist;
    uniform_int_distribution<unsigned int> rhs_time_coeff_dist;
    discrete_distribution<unsigned int> trf_dist;
    discrete_distribution<unsigned int> wl_metric_dist;
    uniform_int_distribution<unsigned int> input_queue_dist;
    uniform_int_distribution<unsigned int> input_queue_cnt_dist;
    uniform_int_distribution<unsigned int> op_dist;

    uniform_int_distribution<unsigned int> timestep_dist;
    uniform_int_distribution<unsigned int> enq_dist;

    uniform_int_distribution<unsigned int> pkt_meta1_val_dist;
    uniform_int_distribution<unsigned int> pkt_meta2_val_dist;
};

class SharedConfig {
public:
    SharedConfig(unsigned int total_time,
                 unsigned int in_queue_cnt,
                 qset_t target_queues,
                 Dists* dists);

    unsigned int get_total_time();
    unsigned int get_in_queue_cnt();
    qset_t get_target_queues();
    Dists* get_dists();

private:
    unsigned int total_time;
    unsigned int in_queue_cnt;
    qset_t target_queues;
    Dists* dists = NULL;
};

#endif /* shared_config_hpp */
