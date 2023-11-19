//
//  qsize.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 2/19/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "qsize.hpp"

QSize::QSize(Queue* queue, unsigned int total_time, NetContext& net_ctx):
Metric(metric_t::QSIZE, queue, total_time, net_ctx) {
    init(net_ctx);
}

// TODO: FIX for leaf_spine_lat
void QSize::eval(const IndexedExample* eg, unsigned int time, unsigned int qind, metric_val& res) {
    res.valid = true;
    unsigned int enq_sum = 0;
    unsigned int deq_sum = 0;
    for (unsigned int t = 0; t < time; t++) {
        enq_sum += eg->enqs[qind][t];
        deq_sum += eg->deqs[qind][t];
    }
    // Only if deq is always less than the length
    // of the queue at any point in time
    res.value = enq_sum - deq_sum;
}

void QSize::populate_val_exprs(NetContext& net_ctx) {

    // Value
    for (unsigned int t = 0; t < total_time; t++) {
        expr size_expr = net_ctx.int_val(0);
        for (int e = 0; e <= queue->size() - 1; e++) {
            expr pkt = queue->elem(e)[t];
            expr val = net_ctx.pkt2val(pkt);
            size_expr = ite(val, net_ctx.int_val(e + 1), size_expr);
        }

        value_[t] = size_expr;
    }

    // Valid
    for (unsigned int t = 0; t < total_time; t++) {
        valid_[t] = net_ctx.bool_val(true);
    }
}
