//
//  aipg.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 8/5/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "aipg.hpp"

AIPG::AIPG(Queue* queue, unsigned int total_time, NetContext& net_ctx):
Metric(metric_t::AIPG, queue, total_time, net_ctx) {
    init(net_ctx);
}

void AIPG::eval(Example* eg, unsigned int time, cid_t qind, metric_val& res) {

    res.valid = true;
    if (time == 0) {
        res.value = 0;
        return;
    }
    if (eg->enqs[qind][time] == 0) {
        eval(eg, time - 1, qind, res);
        return;
    }
    if (eg->enqs[qind][time] > 1) {
        res.value = 0;
        return;
    }

    int last_enq_time = -1;
    for (int t = time - 1; t >= 0; t--) {
        if (eg->enqs[qind][t] > 0) {
            last_enq_time = t;
            break;
        }
    }

    if (last_enq_time >= 0) {
        res.value = time - last_enq_time;
        return;
    } else {
        for (int t = time + 1; t < (int) eg->total_time; t++) {
            if (eg->enqs[qind][t] > 0) {
                last_enq_time = t;
                break;
            }
        }
    }

    if (last_enq_time >= 0)
        res.value = last_enq_time - time;
    else
        res.value = eg->total_time;
}

void AIPG::populate_val_exprs(NetContext& net_ctx) {
    // value
    value_[0] = net_ctx.int_val(0);

    for (unsigned int t1 = 1; t1 < total_time; t1++) {

        // When everything before and after is zero
        // TODO: total_time or total_time - t1?
        expr val_expr = net_ctx.int_val(total_time);

        for (unsigned int t2 = total_time - 1; t2 > t1; t2--) {
            val_expr = ite(queue->enq_cnt(t2) > 0, net_ctx.int_val(t2 - t1), val_expr);
        }

        for (unsigned int t2 = 0; t2 < t1; t2++) {
            val_expr = ite(queue->enq_cnt(t2) > 0, net_ctx.int_val(t1 - t2), val_expr);
        }

        value_[t1] = ite(queue->enq_cnt(t1) > 1,
                         net_ctx.int_val(0),
                         ite(queue->enq_cnt(t1) <= 0, value_[t1 - 1], val_expr));

        // valid
        for (unsigned int t = 0; t < total_time; t++) {
            valid_[t] = net_ctx.bool_val(true);
        }
    }
}