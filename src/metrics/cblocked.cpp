//
//  cblocked.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/17/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "cblocked.hpp"

CBlocked::CBlocked(Queue* queue, unsigned int total_time, NetContext& net_ctx):
Metric(metric_t::CBLOCKED, queue, total_time, net_ctx) {
    init(net_ctx);
}

unsigned int CBlocked::eval(const IndexedExample* eg, unsigned int time, unsigned int qind) {
    unsigned int res = 0;
    unsigned int enq_sum = 0;
    unsigned int deq_sum = 0;
    unsigned int cblocked = 0;
    for (unsigned int t = 0; t <= time; t++) {
        // Only if deq is always less than the length
        // of the queue at that moment
        unsigned int len = enq_sum - deq_sum;
        if (len > 0 && eg->deqs[qind][t] == 0) {
            cblocked++;
        } else {
            cblocked = 0;
        }
        enq_sum += eg->enqs[qind][t];
        deq_sum += eg->deqs[qind][t];
    }
    return res;
}

void CBlocked::add_vars(NetContext& net_ctx) {
    (void) net_ctx;
}


void CBlocked::add_constrs(NetContext& net_ctx, std::map<std::string, expr>& constr_map) {

    char constr_name[100];

    expr blocked = net_ctx.pkt2val(queue->elem(0)[0]) && queue->deq_cnt(0) == 0;
    expr constr_expr = val_[0] == ite(blocked, net_ctx.int_val(1), net_ctx.int_val(0));
    sprintf(constr_name, "%s_val[0]", id.c_str());
    constr_map.insert(named_constr(constr_name, constr_expr));

    for (unsigned int t = 1; t < total_time; t++) {
        blocked = net_ctx.pkt2val(queue->elem(0)[t]) && queue->deq_cnt(t) == 0;
        constr_expr = val_[t] == ite(blocked, val_[t - 1] + 1, net_ctx.int_val(0));
        sprintf(constr_name, "%s_val[%d]", id.c_str(), t);
        constr_map.insert(named_constr(constr_name, constr_expr));
    }
}
