//
//  rr_qm.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 2/17/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "rr_qm.hpp"

RRQM::RRQM(cid_t id,
           unsigned int total_time,
           vector<QueueInfo> in_queue_info,
           QueueInfo out_queue_info,
           NetContext& net_ctx):
QueuingModule(id, total_time, in_queue_info, vector<QueueInfo>{out_queue_info}, net_ctx) {
    last_served_queue_ = new vector<expr>[in_queues.size()];
    init(net_ctx);
}

void RRQM::add_proc_vars(NetContext& net_ctx) {
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        for (unsigned int t = 0; t < total_time; t++) {
            char vname[100];
            snprintf(vname,100, "%s_last_served_queue[%d][%d]", id.c_str(), q, t);
            last_served_queue_[q].push_back(net_ctx.bool_const(vname));
        }
    }
}

void RRQM::add_constrs(NetContext& net_ctx, map<string, expr>& constr_map) {
    char constr_name[100];
    unsigned long in_queue_cnt = in_queues.size();



    for (unsigned int q = 0; q < in_queues.size() - 1; q++) {
        snprintf(constr_name,100, "%s_%d_not_lsq_at_0", id.c_str(), q);
        expr constr_expr = !last_served_queue_[q][0];
        constr_map.insert(named_constr(constr_name, constr_expr));
    }
    snprintf(constr_name,100, "%s_%d_lsq_at_0", id.c_str(), (unsigned int) in_queues.size() - 1);
    expr constr_expr = last_served_queue_[in_queues.size() - 1][0];
    constr_map.insert(named_constr(constr_name, constr_expr));


    for (unsigned int t = 0; t < total_time; t++) {

        // find the first non_empty queue after head
        // for each value of head
        for (unsigned int h = 0; h < in_queue_cnt; h++) {

            unsigned int q = (h + 1) % in_queue_cnt;
            expr not_empty = net_ctx.pkt2val(in_queues[q]->elem(0)[t]);
            snprintf(constr_name,100, "%s_head_%d_deq_cnt_%d_%d", id.c_str(), h, q, t);
            constr_expr = implies(last_served_queue_[h][t],
                                  in_queues[q]->deq_cnt(t) ==
                                      ite(not_empty, net_ctx.int_val(1), net_ctx.int_val(0)));

            constr_map.insert(named_constr(constr_name, constr_expr));

            expr prev_empty = !not_empty;

            for (unsigned int i = 2; i <= in_queue_cnt; i++) {
                q = (h + i) % in_queue_cnt;

                not_empty = net_ctx.pkt2val(in_queues[q]->elem(0)[t]);
                snprintf(constr_name,100, "%s_head_%d_deq_cnt_%d_%d", id.c_str(), h, q, t);
                constr_expr = implies(last_served_queue_[h][t],
                                      in_queues[q]->deq_cnt(t) == ite(prev_empty && not_empty,
                                                                      net_ctx.int_val(1),
                                                                      net_ctx.int_val(0)));
                constr_map.insert(named_constr(constr_name, constr_expr));

                prev_empty = prev_empty && !not_empty;
            }
        }

        // Push to output queue accordingly
        Queue* outq = out_queues[0];
        for (unsigned int q = 0; q < in_queue_cnt; q++) {
            snprintf(constr_name,100, "%s_output_from_%d_%d", id.c_str(), q, t);
            constr_expr = implies(in_queues[q]->deq_cnt(t) == 1,
                                  outq->enqs(0)[t] == in_queues[q]->elem(0)[t]);
            constr_map.insert(named_constr(constr_name, constr_expr));
        }

        // handle the case where all input queues are empty
        expr_vector all_empty(net_ctx.z3_ctx());
        expr_vector non_empty(net_ctx.z3_ctx());
        for (unsigned int q = 0; q < in_queues.size(); q++) {
            all_empty.push_back(!net_ctx.pkt2val(in_queues[q]->elem(0)[t]));
            non_empty.push_back(net_ctx.pkt2val(in_queues[q]->elem(0)[t]));
        }
        snprintf(constr_name,100, "%s_all_empty_input_%d", id.c_str(), t);
        constr_expr = implies(mk_and(all_empty), outq->enqs(0)[t] == net_ctx.null_pkt());
        constr_map.insert(named_constr(constr_name, constr_expr));

        // Update head
        if (t != total_time - 1) {
            for (unsigned int q = 0; q < in_queue_cnt; q++) {
                snprintf(constr_name,100, "%s_lsq_%d_at_%d_is_one", id.c_str(), q, t);
                constr_expr = implies(in_queues[q]->deq_cnt(t) > 0, last_served_queue_[q][t + 1]);
                constr_map.insert(named_constr(constr_name, constr_expr));

                snprintf(constr_name,100, "%s_lsq_%d_at_%d_is_zero", id.c_str(), q, t);
                constr_expr = implies(in_queues[q]->deq_cnt(t) == 0 && mk_or(non_empty),
                                      !last_served_queue_[q][t + 1]);
                constr_map.insert(named_constr(constr_name, constr_expr));

                snprintf(constr_name,100, "%s_lsq_%d_at_%d_stays_same", id.c_str(), q, t);
                constr_expr = implies(mk_and(all_empty),
                                      last_served_queue_[q][t + 1] == last_served_queue_[q][t]);
                constr_map.insert(named_constr(constr_name, constr_expr));
            }

            // TODO: Seems redundant but the solver sometimes likes it
            //       and sometimes not smh
            //            for (unsigned int q = 0; q < in_queues.size(); q++){
            //                snprintf(constr_name,100, "%s_lsq[%d]_same_%d", id.c_str(), q, t);
            //                constr_expr = implies(mk_and(all_empty),
            //                                      last_served_queue_[q][t + 1] ==
            //                                      last_served_queue_[q][t]);
            //                constr_map.insert(named_constr(constr_name, constr_expr));
            //            }
        }


        // Make sure nothing else gets pushed to the output queue
        for (unsigned int i = 1; i < outq->max_enq(); i++) {
            snprintf(constr_name,100, "%s_only_one_output_%d_%d", id.c_str(), i, t);
            expr constr_expr = outq->enqs(i)[t] == net_ctx.null_pkt();
            constr_map.insert(named_constr(constr_name, constr_expr));
        }
    }
}

expr RRQM::last_served_queue(unsigned int q, unsigned int t) {
    return last_served_queue_[q][t];
}
