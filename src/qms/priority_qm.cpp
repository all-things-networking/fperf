//
//  priority_qm.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/12/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "priority_qm.hpp"

PriorityQM::PriorityQM(cid_t id,
                       unsigned int total_time,
                       std::vector<QueueInfo> in_queue_info,
                       QueueInfo out_queue_info,
                       NetContext& net_ctx):
QueuingModule(id, total_time, in_queue_info, std::vector<QueueInfo>{out_queue_info}, net_ctx) {
    init(net_ctx);
}


void PriorityQM::add_proc_vars(NetContext& net_ctx) {
    (void) net_ctx;
}

void PriorityQM::add_constrs(NetContext& net_ctx, std::map<std::string, expr>& constr_map) {
    char constr_name[100];

    for (unsigned int t = 0; t < total_time; t++) {
        // Find the highest priority queue to pull from
        expr not_empty = net_ctx.pkt2val(in_queues[0]->elem(0)[t]);
        sprintf(constr_name, "%s_deq_cnt_0_%d", id.c_str(), t);
        expr constr_expr = in_queues[0]->deq_cnt(t) ==
                           ite(not_empty, net_ctx.int_val(1), net_ctx.int_val(0));

        constr_map.insert(named_constr(constr_name, constr_expr));

        expr higher_empty = !not_empty;
        for (unsigned int q = 1; q < in_queues.size(); q++) {
            not_empty = net_ctx.pkt2val(in_queues[q]->elem(0)[t]);
            sprintf(constr_name, "%s_deq_cnt_%d_%d", id.c_str(), q, t);
            expr constr_expr = in_queues[q]->deq_cnt(t) == ite(higher_empty && not_empty,
                                                               net_ctx.int_val(1),
                                                               net_ctx.int_val(0));
            constr_map.insert(named_constr(constr_name, constr_expr));

            higher_empty = higher_empty && !not_empty;
        }

        // Push to output queue accordingly
        Queue* outq = out_queues[0];
        for (unsigned int q = 0; q < in_queues.size(); q++) {
            sprintf(constr_name, "%s_output_from_%d_%d", id.c_str(), q, t);
            expr constr_expr = implies(in_queues[q]->deq_cnt(t) == 1,
                                       outq->enqs(0)[t] == in_queues[q]->elem(0)[t]);
            constr_map.insert(named_constr(constr_name, constr_expr));
        }

        // handle the case where all input queues are empty
        expr_vector all_empty(net_ctx.z3_ctx());
        for (unsigned int q = 0; q < in_queues.size(); q++) {
            all_empty.push_back(!net_ctx.pkt2val(in_queues[q]->elem(0)[t]));
        }
        sprintf(constr_name, "%s_all_empty_input_%d", id.c_str(), t);
        constr_expr = implies(mk_and(all_empty), outq->enqs(0)[t] == net_ctx.null_pkt());
        constr_map.insert(named_constr(constr_name, constr_expr));

        // Make sure nothing else gets pushed to the output queue
        for (unsigned int i = 1; i < outq->max_enq(); i++) {
            sprintf(constr_name, "%s_only_one_output_%d_%d", id.c_str(), i, t);
            expr constr_expr = outq->enqs(i)[t] == net_ctx.null_pkt();
            constr_map.insert(named_constr(constr_name, constr_expr));
        }
    }
}
