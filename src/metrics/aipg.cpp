//
//  aipg.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 8/5/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "aipg.hpp"

AIPG::AIPG(Queue* queue,
           unsigned int total_time,
           NetContext& net_ctx):
Metric(metric_t::AIPG, queue, total_time, net_ctx)
{
    init(net_ctx);
}

unsigned int AIPG::eval(const IndexedExample* eg,
                        unsigned int time,
                        unsigned int qind){
    if (time == 0) return 0;
    if (eg->enqs[qind][time] == 0) return eval(eg, time - 1, qind);
    if (eg->enqs[qind][time] > 1) return 0;

    int last_enq_time = -1;
    for (int t = time - 1; t >= 0; t--){
        if (eg->enqs[qind][t] > 0){
            last_enq_time = t;
            break;
        }
    }

    if (last_enq_time >= 0) return time - last_enq_time;
    else {
        for (int t = time + 1; t < (int) eg->total_time; t++){
            if (eg->enqs[qind][t] > 0){
                last_enq_time = t;
                break;
            }
        }
    }
    
    if (last_enq_time >= 0) return last_enq_time - time;
    else return eg->total_time;
}

void AIPG::add_vars(NetContext& net_ctx){
  (void) net_ctx;
}


void AIPG::add_constrs(NetContext& net_ctx,
                       std::map<std::string, expr>& constr_map){
    
    (void) net_ctx;

    char constr_name[100];
    
    // Constraints for the value of AIPG
    sprintf(constr_name, "%s_val[0]", id.c_str());
    expr constr_expr = val_[0] == 0;
    constr_map.insert(named_constr(constr_name, constr_expr));
    
    for (unsigned int t1 = 1; t1 < total_time; t1++){
        sprintf(constr_name, "%s_val[%d]_1", id.c_str(), t1);
        constr_expr = implies(queue->enq_cnt(t1) > 1, val_[t1] == 0);
        constr_map.insert(named_constr(constr_name, constr_expr));
        
        expr prev_zero = net_ctx.bool_val(true);
        for (int t2 = t1 - 1; t2 >= 0; t2--){
            int new_val = t1 - t2;
            
            sprintf(constr_name, "%s_val[%d]_2_%d", id.c_str(), t1, t2);
            constr_expr = implies(queue->enq_cnt(t1) == 1 &&
                                  prev_zero &&
                                  queue->enq_cnt(t2) > 0,
                                  val_[t1] == new_val);
            constr_map.insert(named_constr(constr_name, constr_expr));
        
            prev_zero = prev_zero && queue->enq_cnt(t2) <= 0;
        }
        
        expr next_zero = net_ctx.bool_val(true);
        for (unsigned int t2 = t1 + 1; t2 < total_time; t2++){
            int new_val = t2 - t1;
            sprintf(constr_name, "%s_val[%d]_3", id.c_str(), t1);
            constr_expr = implies(queue->enq_cnt(t1) == 1 &&
                                  prev_zero &&
                                  next_zero &&
                                  queue->enq_cnt(t2) > 0,
                                  val_[t1] == new_val);
            constr_map.insert(named_constr(constr_name, constr_expr));
            
            next_zero = next_zero && queue->enq_cnt(t2) <= 0;
        }
        
        sprintf(constr_name, "%s_val[%d]_4", id.c_str(), t1);
        constr_expr = implies(queue->enq_cnt(t1) == 1 &&
                              prev_zero && next_zero,
                              val_[t1] == (int) total_time);
        constr_map.insert(named_constr(constr_name, constr_expr));
        
        sprintf(constr_name, "%s_val[%d]_5", id.c_str(), t1);
        constr_expr = implies(queue->enq_cnt(t1) <= 0, val_[t1] == val_[t1 - 1]);
        constr_map.insert(named_constr(constr_name, constr_expr));
    }
}

