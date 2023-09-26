//
//  ecmp.cpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 06/06/23.
//  Copyright © 2023 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "ecmp.hpp"

Ecmp::Ecmp(Queue* queue,
         unsigned int total_time,
         NetContext& net_ctx):
Metric(metric_t::CENQ, queue, total_time, net_ctx)
{
    init(net_ctx);
}

void Ecmp::eval(const IndexedExample* eg,
               unsigned int time,
               unsigned int qind,
               metric_val& res){

    vector<int> enqs_meta2 = eg->enqs_meta2[qind][time];
    if (enqs_meta2.size() == 0){
        res.valid = false;
        return;
    }

    res.value = enqs_meta2[0];
    for (unsigned int i = 1; i < enqs_meta2.size(); i++){
        if (enqs_meta2[i] != res.value){
            res.valid = false;
            return;
        }
    }
    
    res.valid = true;
}

void Ecmp::populate_val_exprs(NetContext& net_ctx){

    for (unsigned int t = 0; t < total_time; t++){
        value_[t] = net_ctx.pkt2meta2(queue->enqs(t)[0]);
    }        

    // Define valid_[t]
    for (unsigned int t = 0; t < total_time; t++){
        expr base_meta2 = net_ctx.pkt2meta2(queue->enqs(t)[0]);
        valid_[t] = net_ctx.pkt2val(queue->enqs(t)[0]);
        for (unsigned int e = 1; e < queue->max_enq(); e++){
            expr pkt = queue->enqs(t)[e];
            expr val = net_ctx.pkt2val(pkt);
            expr meta2 = net_ctx.pkt2meta2(pkt);
            valid_[t] = valid_[t] && (!val || (meta2 == base_meta2));
        }
    }
}

