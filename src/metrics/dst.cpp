//
//  dst.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 06/06/23.
//  Copyright © 2023 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "dst.hpp"

Dst::Dst(Queue* queue, unsigned int total_time, NetContext& net_ctx):
Metric(metric_t::CENQ, queue, total_time, net_ctx) {
    init(net_ctx);
}

void Dst::eval(Example* eg, unsigned int time, cid_t qind, metric_val& res) {

    vector<int> enqs_meta1 = eg->enqs_meta1[qind][time];
    if (enqs_meta1.size() == 0) {
        res.valid = false;
        return;
    }

    res.value = enqs_meta1[0];
    for (unsigned int i = 1; i < enqs_meta1.size(); i++) {
        if (enqs_meta1[i] != res.value) {
            res.valid = false;
            return;
        }
    }

    res.valid = true;
}

void Dst::populate_val_exprs(NetContext& net_ctx) {

    for (unsigned int t = 0; t < total_time; t++) {
        value_[t] = net_ctx.pkt2meta1(queue->enqs(0)[t]);
    }

    for (unsigned int t = 0; t < total_time; t++) {
        expr base_meta1 = net_ctx.pkt2meta1(queue->enqs(0)[t]);
        valid_[t] = net_ctx.pkt2val(queue->enqs(0)[t]);
        for (unsigned int e = 1; e < queue->max_enq(); e++) {
            expr pkt = queue->enqs(e)[t];
            expr val = net_ctx.pkt2val(pkt);
            expr meta1 = net_ctx.pkt2meta1(pkt);
            valid_[t] = valid_[t] && (!val || (meta1 == base_meta1));
        }
    }
}
