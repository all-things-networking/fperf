//
//  main.cpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 12/07/22.
//  Copyright © 2022 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "shared_config.hpp"

/******************************* Dists *****************************/
Dists::Dists(DistsParams params){
    unsigned int seed = (unsigned int) time(NULL);
    //unsigned int seed = 1680979592;
    std::cout << "seed is: " << seed << std::endl;
    gen.seed(seed);
 
    real_zero_to_one_dist = std::uniform_real_distribution<double>(0.0, 1.0);

    rhs_dist = std::discrete_distribution<unsigned int>(begin(params.rhs_selection_weights),
                                                        end(params.rhs_selection_weights));
    
    rhs_const_dist = std::uniform_int_distribution<unsigned int>(params.rhs_const_min,
                                                                 params.rhs_const_max);
    
    rhs_time_coeff_dist = std::uniform_int_distribution<unsigned int>(params.rhs_time_coeff_min,
                                                                      params.rhs_time_coeff_max);
    
    trf_dist = std::discrete_distribution<unsigned int>(begin(params.trf_selection_weights),
                                                        end(params.trf_selection_weights));
    

    wl_metric_dist = std::discrete_distribution<unsigned int>(begin(params.wl_metric_selection_weights),
                                                              end(params.wl_metric_selection_weights));

    input_queue_dist = std::uniform_int_distribution<unsigned int>(0, params.in_queue_cnt - 1);
    input_queue_cnt_dist = std::uniform_int_distribution<unsigned int>(0, params.in_queue_cnt + 1);

    comp_dist = std::uniform_int_distribution<unsigned int>(params.comp_range_min, 
                                                            params.comp_range_max);

    timestep_dist = std::uniform_int_distribution<unsigned int>(0, params.total_time - 1); 
    
    enq_dist = std::uniform_int_distribution<unsigned int>(params.enq_range_min, 
                                                           params.enq_range_max);

    pkt_meta1_val_dist = std::uniform_int_distribution<unsigned int>(0, params.pkt_meta1_val_max);
    pkt_meta2_val_dist = std::uniform_int_distribution<unsigned int>(0, params.pkt_meta2_val_max);
}

std::mt19937& Dists::get_gen(){return gen;}

double Dists::real_zero_to_one(){return real_zero_to_one_dist(gen);}

unsigned int Dists::rhs(){return rhs_dist(gen);}

unsigned int Dists::rhs_const(){return rhs_const_dist(gen);}
unsigned int Dists::rhs_time_coeff(){return rhs_time_coeff_dist(gen);}

unsigned int Dists::trf(){return trf_dist(gen);}

metric_t Dists::wl_metric(){return (metric_t) wl_metric_dist(gen);}

unsigned int Dists::input_queue(){return input_queue_dist(gen);}
unsigned int Dists::input_queue_cnt(){return input_queue_cnt_dist(gen);}

comp_t Dists::comp(){return (comp_t) comp_dist(gen);}

unsigned int Dists::timestep(){return timestep_dist(gen);}
unsigned int Dists::enq(){return enq_dist(gen);}

unsigned int Dists::pkt_metric1_val(){return pkt_meta1_val_dist(gen);}
unsigned int Dists::pkt_metric2_val(){return pkt_meta2_val_dist(gen);}

std::uniform_int_distribution<unsigned int>& Dists::get_pkt_meta1_val_dist(){
    return pkt_meta1_val_dist;
}

std::uniform_int_distribution<unsigned int>& Dists::get_pkt_meta2_val_dist(){
    return pkt_meta2_val_dist;
}

/******************************* SharedConfig *****************************/

SharedConfig::SharedConfig(unsigned int total_time,
                           unsigned int in_queue_cnt,
                           qset_t target_queues,
                           Dists* dists):
total_time(total_time),
in_queue_cnt(in_queue_cnt),
target_queues(target_queues),
dists(dists)
{}

unsigned int SharedConfig::get_total_time(){
    return total_time;
}

unsigned int SharedConfig::get_in_queue_cnt(){
    return in_queue_cnt;
}

qset_t SharedConfig::get_target_queues(){
    return target_queues;
}

Dists* SharedConfig::get_dists(){
    return dists;
}
