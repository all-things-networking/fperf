//
//  cblocked.cpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/17/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "cblocked.hpp"

CBlocked::CBlocked(Queue* queue,
                   unsigned int total_time,
                   NetContext& net_ctx):
Metric(metric_t::CBLOCKED, queue, total_time, net_ctx)
{
    init(net_ctx);
}

void CBlocked::eval(const IndexedExample* eg,
                    unsigned int time,
                    unsigned int qind,
                    metric_val& res){
    res.valid = true;
    unsigned int enq_sum = 0;
    unsigned int deq_sum = 0;
    unsigned int cblocked = 0;
    for (unsigned int t = 0; t <= time; t++){
        // Only if deq is always less than the length
        // of the queue at that moment
        unsigned int len = enq_sum - deq_sum;
        if (len > 0 && eg->deqs[qind][t] == 0){
            cblocked++;
        }
        else {
            cblocked = 0;
        }
        enq_sum += eg->enqs[qind][t];
        deq_sum += eg->deqs[qind][t];
    }
    res.value = cblocked;
}

void CBlocked::populate_val_exprs(NetContext& net_ctx){
    char constr_name[100];
   
    // Value 
    expr blocked = net_ctx.pkt2val(queue->elem(0)[0]) && queue->deq_cnt(0) == 0;
    value_[0] = ite(blocked, net_ctx.int_val(1), net_ctx.int_val(0));
    
    for (unsigned int t = 1; t < total_time; t++){
        blocked = net_ctx.pkt2val(queue->elem(0)[t]) && queue->deq_cnt(t) == 0;
        value_[t] = ite(blocked, value_[t - 1] + 1, net_ctx.int_val(0));
    }

    // Valid
    for (unsigned int t = 0; t < total_time; t++){
        valid_[t] = net_ctx.bool_val(true);
    }
}
