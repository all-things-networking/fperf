#pragma once
//
//  rr_scheduler.hpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 2/18/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef roce_scheduler_hpp
#define roce_scheduler_hpp

#include "contention_point.hpp"
#include "cenq.hpp"
#include "cdeq.hpp"
#include "qsize.hpp"
#include "aipg.hpp"
#include "cblocked.hpp"

class RoceScheduler : public ContentionPoint {
public:
    RoceScheduler(unsigned int total_time);

private:
    unsigned const int switch_cnt = 4;
    unsigned int threshold = 1;
    unsigned int prio_voq_start = 3;
    std::vector<CEnq*> cenq;
    std::vector<CDeq*> cdeq;
    std::vector<AIPG*> aipg;
    std::vector<QSize*> qsize;
    std::vector<std::map<unsigned int, unsigned int>> control_flows;

    std::vector<unsigned int> ingress;
    std::vector<unsigned int> cengress;
    std::vector<unsigned int> egress;

    std::vector<std::vector<unsigned int>> voq_input_maps;
    std::vector<std::vector<unsigned int>> voq_output_maps;

    vector<unsigned int> voq_input_map;
    vector<unsigned int> voq_output_map;

    void add_nodes();
    void add_edges();
    void add_metrics();

    std::string cp_model_metric(model& m, unsigned int t, string qid, bool in_q, int q_index, metric_t metric);

    std::string cp_model_str(model& m, NetContext& net_ctx, unsigned int t);
};

#endif /* rr_scheduler_hpp */
