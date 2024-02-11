//
//  cenq.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 2/11/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "icenq.hpp"
#include <cmath>

ICEnq::ICEnq(Queue* queue, int id, unsigned int total_time, NetContext& net_ctx):
Metric(id == 1 ? metric_t::ICENQ1 : metric_t::ICENQ2, queue, total_time, net_ctx),
id(id) {
    init(net_ctx);
}

void ICEnq::eval(Example* eg, unsigned int time, cid_t qind, metric_val& res) {
    res.valid = true;
    res.value = 0;
    for (unsigned int t = 0; t <= time; t++) {
        vector<int> enqs_meta1 = eg->enqs_meta1[qind][time];
        int meta1 = enqs_meta1[0];
        if (meta1 == id) res.value++;
    }
}

void ICEnq::populate_val_exprs(NetContext& net_ctx) {
    for (unsigned int t = 0; t < total_time; t++) {
        expr pkt = queue->enqs(0)[t];
        expr val = net_ctx.pkt2val(pkt);
        expr meta1 = net_ctx.pkt2meta1(pkt);
        expr sum = net_ctx.int_val(0);
        if (t > 0) sum = value_[t - 1];
        value_[t] = sum + ite(val && meta1 == net_ctx.int_val(id),
                              net_ctx.int_val(1),
                              net_ctx.int_val(0));
    }

    // valid
    for (unsigned int t = 0; t < total_time; t++) {
        valid_[t] = net_ctx.bool_val(true);
    }
}
