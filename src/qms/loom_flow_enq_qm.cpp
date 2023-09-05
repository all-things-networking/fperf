//
//  loom_flow_enq_qm.cpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 5/3/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "loom_flow_enq_qm.hpp"
#include <exception>

LoomFlowEnqQM::LoomFlowEnqQM(cid_t id,
                             unsigned int total_time,
                             std::vector<QueueInfo> in_queue_info,
                             QueueInfo out_queue_info1,
                             QueueInfo out_queue_info2,
                             NetContext& net_ctx):
QueuingModule(id, total_time, in_queue_info,
              std::vector<QueueInfo>{out_queue_info1, out_queue_info2},
              net_ctx)
{
    // Ensure the outqueue for tenant 2's max enq is even even number
    if (out_queue_info2.max_enq % 2 != 0){
        throw "LoomFlowEnqQM invalid configuartion";
    }
    
    init(net_ctx);
}


void LoomFlowEnqQM::add_proc_vars(NetContext& net_ctx){
  (void) net_ctx;
}

void LoomFlowEnqQM::constrs_if_not_taken(NetContext& net_ctx,
                                         std::map<std::string, expr>& constr_map){
    char constr_name[100];
    
    const unsigned int tenant1_spark = 1;
    const unsigned int tenant2_spark = 2;
    const unsigned int tenant2_memcached = 3;
    
    // Tenant 1

    Queue* tenant1_inq = in_queues[0];
    Queue* tenant1_outq = out_queues[0];
    for (unsigned int t = 0; t < total_time; t++){
        for (unsigned int i = 0; i < tenant1_outq->max_enq(); i++){
            expr in_pkt = tenant1_inq->elem(i)[t];
            // set enqs of the output queue equal to the first max_enq packets of input queue
            sprintf(constr_name, "%s_tenant1_out_enqs[%d][%d]",
                    id.c_str(), i, t);
            expr constr_expr = tenant1_outq->enqs(i)[t] == in_pkt;
            constr_map.insert(named_constr(constr_name, constr_expr));
            
            // set deq_cnt of input queue based on validity of enqs of output queue most max_enq
            sprintf(constr_name, "%s_tenant1_in_deq_cnt[%d]_is_%d",
                    id.c_str(), t, i);
            constr_expr = implies(net_ctx.pkt2val(in_pkt),
                                  tenant1_inq->deq_cnt(t) > (int) i);
            constr_map.insert(named_constr(constr_name, constr_expr));
        }
    }
    
    // Tenant 2
    // 1. Make sure if there are max_enq packets in both queues, they get enqueued.
    //    we can do NICEnq style, but the order of visiting src packets one from
    //    queue 2 and one from queue 3, until max_enq in both queues.
    // 2. Track whether each src packet made it, set the deq_cnt accordingly.
    
    Queue* tenant2_outq = out_queues[1];
    
    for (unsigned int t = 0; t < total_time; t++){
        vector<expr> le_i_gets_j(2 * tenant2_outq->max_enq(),
                                 net_ctx.bool_val(false));
        for (unsigned int i = 0; i < tenant2_outq->max_enq(); i++){
            
            expr prev_elems_invalid = net_ctx.bool_val(true);
            for (unsigned int j = i; j < 2 * tenant2_outq->max_enq(); j++){
                unsigned int qid = 1 + (j % 2);
                unsigned int offset = j / 2;
            
                expr in_pkt = in_queues[qid]->elem(offset)[t];
                expr cond = (prev_elems_invalid &&
                             net_ctx.pkt2val(in_pkt));
                
                sprintf(constr_name, "%s_tenant2_out_enqs[%d][%d]_is_inq[%d][%d]",
                        id.c_str(), i, t, qid, offset);
                expr constr_expr = implies(cond &&
                                           !le_i_gets_j[j],
                                           tenant2_outq->enqs(i)[t] == in_pkt);
                constr_map.insert(named_constr(constr_name, constr_expr));
                
                string app_name = "spark";
                if (qid == 2) app_name = "memcached";
                
                sprintf(constr_name, "%s_tenant2_%s_deq_cnt[%d]_is_gt_%d_because_%d",
                        id.c_str(), app_name.c_str(), t, offset, i);
                constr_expr = implies(cond &&
                                      !le_i_gets_j[j],
                                      in_queues[qid]->deq_cnt(t) > (int) offset);
                constr_map.insert(named_constr(constr_name, constr_expr));
                
                prev_elems_invalid = prev_elems_invalid && (!net_ctx.pkt2val(in_pkt) || le_i_gets_j[j]);
                le_i_gets_j[j] = le_i_gets_j[j] || cond;
            }
            
            sprintf(constr_name, "%s_tenant2_out_enqs[%d][%d]_is_null",
                    id.c_str(), i, t);
            expr constr_expr = implies(prev_elems_invalid,
                                       tenant2_outq->enqs(i)[t] == net_ctx.null_pkt());
            constr_map.insert(named_constr(constr_name, constr_expr));
        }
    }
    
    // make sure all pkts in all queues have valid metadata
    for (unsigned int t = 0; t < total_time; t++){
        for (unsigned int q = 0; q < in_queues.size(); q++){
            
            int queue_class = tenant1_spark;
            if (q == 1) queue_class = tenant2_spark;
            if (q == 2) queue_class = tenant2_memcached;
            
            for (unsigned int i = 0; i < in_queues[q]->max_enq(); i++){
                sprintf(constr_name, "%s_only_valid_meta_in_enq[%d][%d][%d]", id.c_str(), q, i, t);
                expr pkt = in_queues[q]->enqs(i)[t];
                expr pkt_class = net_ctx.pkt2meta1(pkt);
                expr constr_expr = implies(net_ctx.pkt2val(pkt),
                                           pkt_class == queue_class);
                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }
    }
}
void LoomFlowEnqQM::add_constrs(NetContext& net_ctx,
                             std::map<std::string, expr>& constr_map){
    constrs_if_not_taken(net_ctx, constr_map);
}
