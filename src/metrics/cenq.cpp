//
//  cenq.cpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 2/11/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "cenq.hpp"

CEnq::CEnq(Queue* queue,
           unsigned int total_time,
           NetContext& net_ctx):
Metric(metric_t::CENQ, queue, total_time, net_ctx)
{
    init(net_ctx);
}

unsigned int CEnq::eval(const IndexedExample* eg,
                        unsigned int time,
                        unsigned int qind){
    unsigned int res = 0;
    for (unsigned int t = 0; t <= time; t++){
        res += eg->enqs[qind][t];
    }
    return res;
}

void CEnq::add_vars(NetContext& net_ctx){
  (void) net_ctx;
}


void CEnq::add_constrs(NetContext& net_ctx,
                       std::map<std::string, expr>& constr_map){
    
    (void) net_ctx;

    char constr_name[100];
    
    // Constraints for the value of cenq
    expr constr_expr = val_[0] == queue->enq_cnt(0);
    sprintf(constr_name, "%s_val[0]", id.c_str());
    constr_map.insert(named_constr(constr_name, constr_expr));
    
    for (unsigned int t = 1; t < total_time; t++){
        constr_expr = val_[t] == val_[t - 1] + queue->enq_cnt(t);
        sprintf(constr_name, "%s_val[%d]", id.c_str(), t);
        constr_map.insert(named_constr(constr_name, constr_expr));
    }
}

