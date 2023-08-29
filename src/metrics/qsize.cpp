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
unsigned int QSize::eval(const IndexedExample* eg, unsigned int time, unsigned int qind) {
    unsigned int enq_sum = 0;
    unsigned int deq_sum = 0;
    for (unsigned int t = 0; t < time; t++) {
        enq_sum += eg->enqs[qind][t];
        deq_sum += eg->deqs[qind][t];
    }
    // Only if deq is always less than the length
    // of the queue at any point in time
    return enq_sum - deq_sum;
}

void QSize::add_vars(NetContext& net_ctx) {
    (void) net_ctx;
}


void QSize::add_constrs(NetContext& net_ctx, std::map<std::string, expr>& constr_map) {

    char constr_name[100];


    // Constraints for the value of qsize

    for (unsigned int t = 0; t < total_time; t++) {

        sprintf(constr_name, "%s_val[%d]_is_0", id.c_str(), t);
        expr constr_expr = implies(!net_ctx.pkt2val(queue->elem(0)[t]), val_[t] == 0);
        constr_map.insert(named_constr(constr_name, constr_expr));

        for (unsigned int e = 0; e < queue->size() - 1; e++) {
            expr pkt1 = queue->elem(e)[t];
            expr val1 = net_ctx.pkt2val(pkt1);
            expr pkt2 = queue->elem(e + 1)[t];
            expr val2 = net_ctx.pkt2val(pkt2);

            sprintf(constr_name, "%s_val[%d]_is_%d", id.c_str(), t, e + 1);
            constr_expr = implies(val1 && !val2, val_[t] == (int) (e + 1));
            constr_map.insert(named_constr(constr_name, constr_expr));
        }

        unsigned int last_elem = queue->size() - 1;
        sprintf(constr_name, "%s_val[%d]_is_%d", id.c_str(), t, last_elem + 1);
        constr_expr = implies(net_ctx.pkt2val(queue->elem(last_elem)[t]),
                              val_[t] == (int) (last_elem + 1));
        constr_map.insert(named_constr(constr_name, constr_expr));
    }
    /*
    expr constr_expr = val_[0] == net_ctx.int_val(0);
    sprintf(constr_name, "%s_val[0]", id.c_str());
    constr_map.insert(named_constr(constr_name, constr_expr));

    for (unsigned int t = 1; t < total_time; t++){
        constr_expr = val_[t] == val_[t - 1] + queue->enq_cnt(t - 1) - queue->deq_cnt(t - 1);
        sprintf(constr_name, "%s_val[%d]", id.c_str(), t);
        constr_map.insert(named_constr(constr_name, constr_expr));
    }
    */
}
