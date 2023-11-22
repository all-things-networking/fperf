//
//  QMeta1.cpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/17/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "qmeta1.hpp"

QMeta1::QMeta1(Queue* queue,
    unsigned int total_time,
    NetContext& net_ctx) :
    Metric(metric_t::QMeta1, queue, total_time, net_ctx)
{
    init(net_ctx);
}

unsigned int QMeta1::eval(const IndexedExample* eg, 
                            unsigned int time,
                            unsigned int qind) {
    unsigned int res;
    res = 0;
    return res; // ??

}

void QMeta1::add_vars(NetContext& net_ctx) {
    (void)net_ctx;
}


void QMeta1::add_constrs(NetContext& net_ctx,
    std::map<std::string, expr>& constr_map) {
     // do nothing?
    return;
   /* char constr_name[100];

    expr blocked = net_ctx.pkt2val(queue->elem(0)[0]) && queue->deq_cnt(0) == 0;
    expr constr_expr = val_[0] == ite(blocked, net_ctx.int_val(1), net_ctx.int_val(0));
    sprintf(constr_name, "%s_val[0]", id.c_str());
    constr_map.insert(named_constr(constr_name, constr_expr));

    for (unsigned int t = 1; t < total_time; t++) {
        blocked = net_ctx.pkt2val(queue->elem(0)[t]) && queue->deq_cnt(t) == 0;
        constr_expr = val_[t] == ite(blocked, val_[t - 1] + 1, net_ctx.int_val(0));
        sprintf(constr_name, "%s_val[%d]", id.c_str(), t);
        constr_map.insert(named_constr(constr_name, constr_expr));
    }*/
}
