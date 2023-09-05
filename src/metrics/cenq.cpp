//
//  cenq.cpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 2/11/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "cenq.hpp"

CEnq::CEnq(Queue* queue,
           unsigned int total_time,
           NetContext& net_ctx):
Metric(metric_t::CENQ, queue, total_time, net_ctx)
{
    init(net_ctx);
}

void CEnq::eval(const IndexedExample* eg,
                unsigned int time,
                unsigned int qind,
                metric_val& res){
    res.valid = true;
    res.value = 0;
    for (unsigned int t = 0; t <= time; t++){
        res.value += eg->enqs[qind][t];
    }
}

void CEnq::populate_val_exprs(NetContext& net_ctx){
    char constr_name[100];
    
    // value
    value_[0] = queue->enq_cnt(0);
    for (unsigned int t = 1; t < total_time; t++){
        value_[t] = value_[t - 1] + queue->enq_cnt(t);
    }

    // valid
    for (unsigned int t = 0; t < total_time; t++){
        valid_[t] = net_ctx.bool_val(true);
    }
}

