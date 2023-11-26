//
//  loom_nic_enq_qm.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 5/3/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "loom_nic_enq_qm.hpp"
#include <exception>

LoomNICEnqQM::LoomNICEnqQM(cid_t id,
                           unsigned int total_time,
                           vector<QueueInfo> in_queue_info,
                           QueueInfo out_queue_info1,
                           QueueInfo out_queue_info2,
                           NetContext& net_ctx):
QueuingModule(
    id, total_time, in_queue_info, vector<QueueInfo>{out_queue_info1, out_queue_info2}, net_ctx) {

    unsigned min_max_out_enq = out_queue_info1.max_enq;
    if (out_queue_info2.max_enq < min_max_out_enq) {
        min_max_out_enq = out_queue_info2.max_enq;
    }

    if (in_queue_info.size() % min_max_out_enq != 0) {
        throw "LoomNICEnqQM invalid configuartion";
    }
    // TODO: shouldn't this be reversed?
    per_queue_share = (unsigned int) in_queue_info.size() / min_max_out_enq;
    init(net_ctx);
}


void LoomNICEnqQM::add_proc_vars(NetContext& net_ctx) {
    (void) net_ctx;
}

void LoomNICEnqQM::constrs_if_not_taken(NetContext& net_ctx, map<string, expr>& constr_map) {
    char constr_name[100];

    const unsigned int tenant1_spark = 1;
    const unsigned int tenant2_spark = 2;
    const unsigned int tenant2_memcached = 3;


    for (unsigned int t = 0; t < total_time; t++) {
        for (unsigned int q = 0; q < in_queues.size(); q++) {
            for (unsigned int i = 0; i < per_queue_share; i++) {
                Queue* inq = in_queues[q];

                expr in_pkt = inq->elem(i)[t];
                expr pkt_val = net_ctx.pkt2val(in_pkt);

                // Set deq_cnt bounds
                snprintf(constr_name, 100, "%s_input_deq_cnt_lb[%d][%d]_%d", id.c_str(), q, t, i);
                expr constr_expr = implies(pkt_val, inq->deq_cnt(t) > (int) i);
                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }

        // The memcached output queue
        Queue* memcached_outq = out_queues[0];

        unsigned int in_queue_cnt = (unsigned int) in_queues.size();
        vector<expr> le_i_gets_j(per_queue_share * in_queue_cnt, net_ctx.bool_val(false));
        for (unsigned int i = 0; i < per_queue_share * in_queue_cnt; i++) {
            expr dst_pkt = memcached_outq->enqs(i)[t];

            expr prev_elems_invalid = net_ctx.bool_val(true);
            for (unsigned int j = i; j < per_queue_share * in_queue_cnt; j++) {
                unsigned int src_qid = j / per_queue_share;
                unsigned int src_elem_id = j % per_queue_share;

                expr src_pkt = in_queues[src_qid]->elem(src_elem_id)[t];
                expr pkt_val = net_ctx.pkt2val(src_pkt);
                expr pkt_class = net_ctx.pkt2meta1(src_pkt);

                snprintf(constr_name,
                         100,
                         "%s_memcached_enq[%d]_in[%d][%d]_%d",
                         id.c_str(),
                         i,
                         src_qid,
                         src_elem_id,
                         t);
                expr cond = (prev_elems_invalid && pkt_val && pkt_class == (int) tenant2_memcached);
                expr constr_expr = implies(cond && !le_i_gets_j[j], dst_pkt == src_pkt);
                constr_map.insert(named_constr(constr_name, constr_expr));

                prev_elems_invalid = prev_elems_invalid &&
                                     (!pkt_val || pkt_class != (int) tenant2_memcached ||
                                      le_i_gets_j[j]);

                le_i_gets_j[j] = le_i_gets_j[j] || cond;
            }

            snprintf(constr_name, 100, "%s_memcached_enq[%d]_null_%d", id.c_str(), i, t);
            expr constr_expr = implies(prev_elems_invalid, dst_pkt == net_ctx.null_pkt());
            constr_map.insert(named_constr(constr_name, constr_expr));
        }

        // The Spark output queue
        Queue* spark_outq = out_queues[1];

        le_i_gets_j = vector<expr>(per_queue_share * in_queue_cnt, net_ctx.bool_val(false));
        for (unsigned int i = 0; i < per_queue_share * in_queue_cnt; i++) {
            expr dst_pkt = spark_outq->enqs(i)[t];

            expr prev_elems_invalid = net_ctx.bool_val(true);
            for (unsigned int j = i; j < per_queue_share * in_queue_cnt; j++) {
                unsigned int src_qid = j / per_queue_share;
                unsigned int src_elem_id = j % per_queue_share;

                expr src_pkt = in_queues[src_qid]->elem(src_elem_id)[t];
                expr pkt_val = net_ctx.pkt2val(src_pkt);
                expr pkt_class = net_ctx.pkt2meta1(src_pkt);

                snprintf(constr_name,
                         100,
                         "%s_spark_enq[%d]_in[%d][%d]_%d",
                         id.c_str(),
                         i,
                         src_qid,
                         src_elem_id,
                         t);
                expr cond = (prev_elems_invalid && pkt_val &&
                             (pkt_class == (int) tenant1_spark ||
                              pkt_class == (int) tenant2_spark));
                expr constr_expr = implies(cond && !le_i_gets_j[j], dst_pkt == src_pkt);
                constr_map.insert(named_constr(constr_name, constr_expr));

                prev_elems_invalid = prev_elems_invalid && (!pkt_val ||
                                                            (pkt_class != (int) tenant1_spark &&
                                                             pkt_class != (int) tenant2_spark) ||
                                                            le_i_gets_j[j]);

                le_i_gets_j[j] = le_i_gets_j[j] || cond;
            }

            snprintf(constr_name, 100, "%s_spark_enq[%d]_null_%d", id.c_str(), i, t);
            expr constr_expr = implies(prev_elems_invalid, dst_pkt == net_ctx.null_pkt());
            constr_map.insert(named_constr(constr_name, constr_expr));
        }

        // Make sure nothing else gets pushed to the output queue
        for (unsigned int q = 0; q < out_queues.size(); q++) {
            Queue* outq = out_queues[q];
            for (unsigned int i = per_queue_share * in_queue_cnt; i < outq->max_enq(); i++) {
                snprintf(constr_name, 100, "%s_null_rest_of_output_%d_%d_%d", id.c_str(), q, i, t);
                expr constr_expr = outq->enqs(i)[t] == net_ctx.null_pkt();
                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }
    }
}

void LoomNICEnqQM::add_constrs(NetContext& net_ctx, map<string, expr>& constr_map) {
    constrs_if_not_taken(net_ctx, constr_map);
}
