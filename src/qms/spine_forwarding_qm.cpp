//
//  spine_forwarding_qm.cpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 12/23/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "spine_forwarding_qm.hpp"

SpineForwardingQM::SpineForwardingQM(cid_t id,
                                     unsigned int total_time,
                                     unsigned int spine_id,
                                     unsigned int leaf_cnt,
                                     unsigned int servers_per_leaf,
                                     map<unsigned int, unsigned int> output_voq_map,
                                     QueueInfo in_queue_info,
                                     std::vector<QueueInfo> out_queue_info,
                                     NetContext& net_ctx):
QueuingModule(id, total_time, std::vector<QueueInfo>{in_queue_info}, out_queue_info, net_ctx),
spine_id(spine_id),
leaf_cnt(leaf_cnt),
servers_per_leaf(servers_per_leaf),
output_voq_map(output_voq_map) {
    init(net_ctx);
}


void SpineForwardingQM::add_proc_vars(NetContext& net_ctx) {
    (void) net_ctx;
}

void SpineForwardingQM::add_constrs(NetContext& net_ctx, std::map<std::string, expr>& constr_map) {
    char constr_name[100];

    Queue* in_queue = in_queues[0];

    unsigned int server_cnt = leaf_cnt * servers_per_leaf;

    for (unsigned int t = 0; t < total_time; t++) {
        expr in_pkt = in_queue->elem(0)[t];
        expr pkt_val = net_ctx.pkt2val(in_pkt);
        expr pkt_dst = net_ctx.pkt2meta1(in_pkt);

        // Set deq_cnt for input queue
        sprintf(constr_name, "%s_in_queue_deq_cnt_is_zero_or_one_at_%d", id.c_str(), t);
        expr constr_expr = implies(pkt_val, in_queue->deq_cnt(t) == 1) &&
                           implies(!pkt_val, in_queue->deq_cnt(t) == 0);
        constr_map.insert(named_constr(constr_name, constr_expr));

        // Forward
        for (unsigned int dst = 0; dst < server_cnt; dst++) {
            unsigned int dst_port = dst / servers_per_leaf;
            if (output_voq_map.find(dst_port) == output_voq_map.end()) continue;
            unsigned int voq_ind = output_voq_map[dst_port];
            sprintf(constr_name,
                    "%s_forward_dst_%d_to_port%d(q%d)_at_%d",
                    id.c_str(),
                    dst,
                    dst_port,
                    voq_ind,
                    t);
            constr_expr = implies(pkt_val && pkt_dst == (int) dst,
                                  out_queues[voq_ind]->enqs(0)[t] == in_pkt);
            constr_map.insert(named_constr(constr_name, constr_expr));
        }

        // Don't enqueue into a port if not destined for the port
        for (unsigned int i = 0; i < leaf_cnt; i++) {
            if (output_voq_map.find(i) == output_voq_map.end()) continue;
            unsigned int voq_ind = output_voq_map[i];
            sprintf(constr_name, "%s_no_match_port%d(q%d)_at_%d", id.c_str(), i, voq_ind, t);
            constr_expr = implies(pkt_dst < (int) (i * servers_per_leaf) ||
                                      pkt_dst >= (int) ((i + 1) * servers_per_leaf),
                                  out_queues[voq_ind]->enqs(0)[t] == net_ctx.null_pkt());
            constr_map.insert(named_constr(constr_name, constr_expr));
        }

        // Flow meta bounds
        for (unsigned int q = 0; q < in_queues.size(); q++) {
            Queue* queue = in_queues[q];
            for (unsigned int p = 0; p < queue->size(); p++) {
                expr pkt = queue->elem(p)[t];
                expr pkt_val = net_ctx.pkt2val(pkt);
                expr pkt_meta = net_ctx.pkt2meta2(pkt);

                sprintf(constr_name,
                        "%s_flow_meta_bounds_input_queue[%d][%d][%d]",
                        id.c_str(),
                        q,
                        p,
                        t);
                constr_expr = implies(pkt_val, pkt_meta == (int) spine_id);
                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }

        for (unsigned int q = 0; q < out_queues.size(); q++) {
            Queue* queue = out_queues[q];
            for (unsigned int p = 0; p < queue->size(); p++) {
                expr pkt = queue->elem(p)[t];
                expr pkt_val = net_ctx.pkt2val(pkt);
                expr pkt_meta = net_ctx.pkt2meta2(pkt);

                sprintf(constr_name,
                        "%s_flow_meta_bounds_output_queue[%d][%d][%d]",
                        id.c_str(),
                        q,
                        p,
                        t);
                constr_expr = implies(pkt_val, pkt_meta == (int) spine_id);
                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }

        // Don't enqueue into any port if packet is invalid

        for (unsigned int i = 0; i < out_queue_cnt(); i++) {
            sprintf(constr_name, "%s_invalid_pkt_port_%d_at_%d", id.c_str(), i, t);
            constr_expr = implies(!pkt_val, out_queues[i]->enqs(0)[t] == net_ctx.null_pkt());
            constr_map.insert(named_constr(constr_name, constr_expr));
        }

        // Make sure nothing else gets pushed to the output queue
        for (unsigned int i = 0; i < out_queue_cnt(); i++) {
            for (unsigned int e = 1; e < out_queues[i]->max_enq(); e++) {
                sprintf(constr_name, "%s_out_%d_elem[%d][%d]_is_null", id.c_str(), i, e, t);
                expr constr_expr = out_queues[i]->enqs(e)[t] == net_ctx.null_pkt();
                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }
        /*
        // Bounds on meta3 (arrival time)
        for (unsigned int i = 0; i < in_queue_cnt(); i++){
            Queue* queue = in_queues[i];
            for (unsigned int e = 0; e < queue->max_enq(); e++){
                expr pkt = queue->enqs(e)[t];
                expr meta3 = net_ctx.pkt2meta3(pkt);

                sprintf(constr_name, "%s_in_%d_enqs[%d][%d]_meta3_bound", id.c_str(), i, e, t);
                expr constr_expr = meta3 <= (int) t && meta3 >= 0;
                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }

        for (unsigned int i = 0; i < out_queue_cnt(); i++){
            Queue* queue = out_queues[i];
            for (unsigned int e = 0; e < queue->max_enq(); e++){
                expr pkt = queue->enqs(e)[t];
                expr meta3 = net_ctx.pkt2meta3(pkt);

                sprintf(constr_name, "%s_out_%d_enqs[%d][%d]_meta3_bound", id.c_str(), i, e, t);
                expr constr_expr = meta3 <= (int) t && meta3 >= 0;
                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }
        */
    }
}
