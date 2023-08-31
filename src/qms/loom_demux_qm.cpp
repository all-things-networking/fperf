//
//  loom_demux_qm.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 5/4/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "loom_demux_qm.hpp"

LoomDemuxQM::LoomDemuxQM(cid_t id,
                         unsigned int total_time,
                         QueueInfo in_queue_info,
                         QueueInfo out_queue_info1,
                         QueueInfo out_queue_info2,
                         NetContext& net_ctx):
QueuingModule(id,
              total_time,
              std::vector<QueueInfo>{in_queue_info},
              std::vector<QueueInfo>{out_queue_info1, out_queue_info2},
              net_ctx) {
    init(net_ctx);
}


void LoomDemuxQM::add_proc_vars(NetContext& net_ctx) {
    (void) net_ctx;
}

void LoomDemuxQM::add_constrs(NetContext& net_ctx, std::map<std::string, expr>& constr_map) {
    char constr_name[100];

    const unsigned int tenant1_spark = 1;
    const unsigned int tenant2_spark = 2;
    const unsigned int tenant2_memcached = 3;

    Queue* input_queue = in_queues[0];

    for (unsigned int t = 0; t < total_time; t++) {
        expr head_pkt = input_queue->elem(0)[t];
        expr not_empty = net_ctx.pkt2val(head_pkt);

        // If not empty, deq one from the input queue
        sprintf(constr_name, "%s_input_deq_cnt_%d", id.c_str(), t);
        expr constr_expr = implies(not_empty, input_queue->deq_cnt(t) == 1);
        constr_map.insert(named_constr(constr_name, constr_expr));

        // Decide which output queue to put the packet in
        expr pkt_class = net_ctx.pkt2meta1(head_pkt);
        sprintf(constr_name, "%s_tenant1_pkt_%d", id.c_str(), t);
        constr_expr = implies(not_empty && pkt_class == (int) tenant1_spark,
                              out_queues[0]->enqs(0)[t] == head_pkt);
        constr_map.insert(named_constr(constr_name, constr_expr));

        sprintf(constr_name, "%s_not_tenant1_pkt_%d", id.c_str(), t);
        constr_expr = implies(not_empty && pkt_class != (int) tenant1_spark,
                              out_queues[0]->enqs(0)[t] == net_ctx.null_pkt());
        constr_map.insert(named_constr(constr_name, constr_expr));

        sprintf(constr_name, "%s_tenant2_pkt_%d", id.c_str(), t);
        constr_expr = implies(not_empty && (pkt_class == (int) tenant2_spark ||
                                            pkt_class == (int) tenant2_memcached),
                              out_queues[1]->enqs(0)[t] == head_pkt);
        constr_map.insert(named_constr(constr_name, constr_expr));

        sprintf(constr_name, "%s_not_tenant2_pkt_%d", id.c_str(), t);
        constr_expr = implies(not_empty && (pkt_class != (int) tenant2_spark &&
                                            pkt_class != (int) tenant2_memcached),
                              out_queues[1]->enqs(0)[t] == net_ctx.null_pkt());
        constr_map.insert(named_constr(constr_name, constr_expr));

        // Take care of the case where the input queue is empty
        sprintf(constr_name, "%s_empty_input_no_deq_%d", id.c_str(), t);
        constr_expr = implies(!not_empty, input_queue->deq_cnt(t) == 0);
        constr_map.insert(named_constr(constr_name, constr_expr));

        for (unsigned int q = 0; q < out_queues.size(); q++) {
            sprintf(constr_name, "%s_empty_input_no_enq_%d_%d", id.c_str(), q, t);
            expr constr_expr = implies(!not_empty, out_queues[q]->enqs(0)[t] == net_ctx.null_pkt());
            constr_map.insert(named_constr(constr_name, constr_expr));
        }


        // Make sure nothing else gets pushed to the output queue
        for (unsigned int q = 0; q < out_queues.size(); q++) {
            Queue* outq = out_queues[q];
            for (unsigned int i = 1; i < outq->max_enq(); i++) {
                sprintf(constr_name, "%s_only_one_output_%d_%d_%d", id.c_str(), q, i, t);
                expr constr_expr = outq->enqs(i)[t] == net_ctx.null_pkt();
                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }
    }
}
