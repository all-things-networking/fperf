//
//  cdeq.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 2/19/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "cdeq.hpp"

CDeq::CDeq(Queue* queue, unsigned int total_time, NetContext& net_ctx):
Metric(metric_t::CDEQ, queue, total_time, net_ctx) {
    init(net_ctx);
}

void CDeq::eval(Example* eg, unsigned int time, cid_t qind, metric_val& res) {

    res.valid = true;
    res.value = 0;
    for (unsigned int t = 0; t <= time; t++) {
        res.value += eg->deqs[qind][t];
    }
}

void CDeq::populate_val_exprs(NetContext& net_ctx) {
    // value
    value_[0] = queue->deq_cnt(0);

    for (unsigned int t = 1; t < total_time; t++) {
        value_[t] = value_[t - 1] + queue->deq_cnt(t);
    }

    // valid
    for (unsigned int t = 0; t < total_time; t++) {
        valid_[t] = net_ctx.bool_val(true);
    }
}
