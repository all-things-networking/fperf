//
//  aipg.cpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 8/5/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "aipg.hpp"

AIPG::AIPG(Queue* queue, unsigned int total_time, NetContext& net_ctx):
Metric(metric_t::AIPG, queue, total_time, net_ctx) {
    init(net_ctx);
}

void AIPG::eval(const IndexedExample* eg, unsigned int time, unsigned int qind, metric_val& res) {

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

        for (int t2 = total_time - 1; t2 > t1; t2--) {
            val_expr = ite(queue->enq_cnt(t2) > 0, net_ctx.int_val(t2 - t1), val_expr);
        }

        for (unsigned int t2 = 0; t2 < t1; t2++) {
            val_expr = ite(queue->enq_cnt(t2) > 0, net_ctx.int_val(t1 - t2), val_expr);
        }

        value_[t1] = ite(queue->enq_cnt(t1) > 1,
                         net_ctx.int_val(0),
                         ite(queue->enq_cnt(t1) <= 0, value_[t1 - 1], val_expr));

        /*
        snprintf(constr_name, 100, "%s_value[%d]_1", id.c_str(), t1);
        constr_expr = implies(queue->enq_cnt(t1) > 1, value_[t1] == 0);
        constr_map.insert(named_constr(constr_name, constr_expr));

        expr prev_zero = net_ctx.bool_val(true);
        for (int t2 = t1 - 1; t2 >= 0; t2--){
            int new_val = t1 - t2;

            snprintf(constr_name, 100, "%s_value[%d]_2_%d", id.c_str(), t1, t2);
            constr_expr = implies(queue->enq_cnt(t1) == 1 &&
                                  prev_zero &&
                                  queue->enq_cnt(t2) > 0,
                                  value_[t1] == new_val);
            constr_map.insert(named_constr(constr_name, constr_expr));

            prev_zero = prev_zero && queue->enq_cnt(t2) <= 0;
        }

        expr next_zero = net_ctx.bool_val(true);
        for (unsigned int t2 = t1 + 1; t2 < total_time; t2++){
            int new_val = t2 - t1;
            snprintf(constr_name, 100, "%s_value[%d]_3", id.c_str(), t1);
            constr_expr = implies(queue->enq_cnt(t1) == 1 &&
                                  prev_zero &&
                                  next_zero &&
                                  queue->enq_cnt(t2) > 0,
                                  value_[t1] == new_val);
            constr_map.insert(named_constr(constr_name, constr_expr));

            next_zero = next_zero && queue->enq_cnt(t2) <= 0;
        }

        snprintf(constr_name, 100, "%s_value[%d]_4", id.c_str(), t1);
        constr_expr = implies(queue->enq_cnt(t1) == 1 &&
                              prev_zero && next_zero,
                              value_[t1] == (int) total_time);
        constr_map.insert(named_constr(constr_name, constr_expr));

        snprintf(constr_name, 100, "%s_value[%d]_5", id.c_str(), t1);
        constr_expr = implies(queue->enq_cnt(t1) <= 0, value_[t1] == value_[t1 - 1]);
        constr_map.insert(named_constr(constr_name, constr_expr));
    }
    */

        // valid
        for (unsigned int t = 0; t < total_time; t++) {
            valid_[t] = net_ctx.bool_val(true);
        }
    }
}