//
//  cdeq.cpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 2/19/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "cdeq.hpp"

CDeq::CDeq(Queue* queue, unsigned int total_time, NetContext& net_ctx):
Metric(metric_t::CDEQ, queue, total_time, net_ctx) {
    init(net_ctx);
}

unsigned int CDeq::eval(const IndexedExample* eg, unsigned int time, unsigned int qind) {
    unsigned int res = 0;
    for (unsigned int t = 0; t <= time; t++) {
        res += eg->deqs[qind][t];
    }
    return res;
}

void CDeq::add_vars(NetContext& net_ctx) {
    (void) net_ctx;
}


void CDeq::add_constrs(NetContext& net_ctx, std::map<std::string, expr>& constr_map) {

    (void) net_ctx;

    char constr_name[100];

    // Constraints for the value of cdeq
    expr constr_expr = val_[0] == queue->deq_cnt(0);
    sprintf(constr_name, "%s_val[0]", id.c_str());
    constr_map.insert(named_constr(constr_name, constr_expr));

    for (unsigned int t = 1; t < total_time; t++) {
        constr_expr = val_[t] == val_[t - 1] + queue->deq_cnt(t);
        sprintf(constr_name, "%s_val[%d]", id.c_str(), t);
        constr_map.insert(named_constr(constr_name, constr_expr));
    }
}
