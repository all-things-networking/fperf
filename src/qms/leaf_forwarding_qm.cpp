//
//  leaf_forwarding_qm.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 12/23/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "leaf_forwarding_qm.hpp"

LeafForwardingQM::LeafForwardingQM(cid_t id,
                                   unsigned int total_time,
                                   unsigned int leaf_id,
                                   unsigned int fw_id,
                                   unsigned int servers_per_leaf,
                                   unsigned int server_cnt,
                                   unsigned int spine_cnt,
                                   map<unsigned int, unsigned int> output_voq_map,
                                   QueueInfo in_queue_info,
                                   vector<QueueInfo> out_queue_info,
                                   NetContext& net_ctx):
QueuingModule(id, total_time, vector<QueueInfo>{in_queue_info}, out_queue_info, net_ctx),
leaf_id(leaf_id),
fw_id(fw_id),
servers_per_leaf(servers_per_leaf),
server_cnt(server_cnt),
spine_cnt(spine_cnt),
output_voq_map(output_voq_map) {
    init(net_ctx);
}


void LeafForwardingQM::add_proc_vars(NetContext& net_ctx) {
    (void) net_ctx;
}

void LeafForwardingQM::add_constrs(NetContext& net_ctx, map<string, expr>& constr_map) {
    char constr_name[100];

    Queue* in_queue = in_queues[0];


    for (unsigned int t = 0; t < total_time; t++) {
        expr in_pkt = in_queue->elem(0)[t];
        expr pkt_val = net_ctx.pkt2val(in_pkt);
        expr pkt_dst = net_ctx.pkt2meta1(in_pkt);
        expr dst_spine = net_ctx.pkt2meta2(in_pkt);

        // Set deq_cnt for input queue
        snprintf(constr_name, 100, "%s_in_queue_deq_cnt_is_zero_or_one_at_%d", id.c_str(), t);
        expr constr_expr = implies(pkt_val, in_queue->deq_cnt(t) == 1) &&
                           implies(!pkt_val, in_queue->deq_cnt(t) == 0);
        constr_map.insert(named_constr(constr_name, constr_expr));

        // Forward
        /*for (unsigned int dst = 0; dst < server_cnt; dst++){
            unsigned int dst_port = (dst % spine_cnt) + servers_per_leaf;
            if (dst >= leaf_id * servers_per_leaf && dst < (leaf_id + 1) * servers_per_leaf){
                dst_port = dst % servers_per_leaf;
            }

            if (output_voq_map.find(dst_port) == output_voq_map.end()) continue;

            unsigned int dst_queue = output_voq_map[dst_port];
            snprintf(constr_name, 100, "%s_forward_dst_%d_to_port%d(q%d)_at_%d", id.c_str(), dst,
        dst_port, dst_queue, t); constr_expr = implies(pkt_val && pkt_dst == (int) dst,
                                  out_queues[dst_queue]->enqs(0)[t] == in_pkt);
            constr_map.insert(named_constr(constr_name, constr_expr));
        }*/

        unsigned int inside_lb = leaf_id * servers_per_leaf;
        unsigned int inside_ub = (leaf_id + 1) * servers_per_leaf - 1;
        expr dst_outside = pkt_dst < (int) inside_lb || pkt_dst > (int) inside_ub;

        // Forwarding to spines
        if (fw_id < servers_per_leaf) {
            for (unsigned int s = 0; s < spine_cnt; s++) {
                unsigned int dst_port = servers_per_leaf + s;
                if (output_voq_map.find(dst_port) == output_voq_map.end()) continue;
                unsigned int voq_ind = output_voq_map[dst_port];

                snprintf(constr_name, 100, "%s_forward_spine_%d_at_%d", id.c_str(), s, t);
                constr_expr = implies(pkt_val && dst_outside && dst_spine == (int) s,
                                      out_queues[voq_ind]->enqs(0)[t] == in_pkt);
                constr_map.insert(named_constr(constr_name, constr_expr));

                snprintf(constr_name, 100, "%s_dont_forward_spine_%d_at_%d", id.c_str(), s, t);

                constr_expr = implies(!dst_outside || dst_spine != (int) s,
                                      out_queues[voq_ind]->enqs(0)[t] == net_ctx.null_pkt());
                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }

        // Forwarding within the same leaf

        for (unsigned int dst = inside_lb; dst <= inside_ub; dst++) {
            unsigned int dst_port = dst % servers_per_leaf;
            if (output_voq_map.find(dst_port) == output_voq_map.end()) continue;

            unsigned int voq_ind = output_voq_map[dst_port];
            snprintf(constr_name, 100, "%s_forward_dst_%d_at_%d", id.c_str(), dst, t);
            constr_expr = implies(pkt_val && pkt_dst == (int) dst,
                                  out_queues[voq_ind]->enqs(0)[t] == in_pkt);
            constr_map.insert(named_constr(constr_name, constr_expr));

            snprintf(constr_name, 100, "%s_dont_forward_dst_%d_at_%d", id.c_str(), dst, t);
            constr_expr = implies(pkt_dst != (int) dst,
                                  out_queues[voq_ind]->enqs(0)[t] == net_ctx.null_pkt());
            constr_map.insert(named_constr(constr_name, constr_expr));
        }

        // Don't enqueue into a port if not destined for that port
        /*for (unsigned int i = 0; i < servers_per_leaf; i++){
            if (output_voq_map.find(i) == output_voq_map.end()) continue;

            unsigned int voq_ind = output_voq_map[i];
            snprintf(constr_name, 100, "%s_no_match_port%d(q%d)_at_%d", id.c_str(), i, voq_ind, t);
            constr_expr = implies(pkt_dst != (int) ((leaf_id * servers_per_leaf) + i),
                                  out_queues[voq_ind]->enqs(0)[t] == net_ctx.null_pkt());

            constr_map.insert(named_constr(constr_name, constr_expr));
        }

        for (unsigned int i = 0; i < spine_cnt; i++){
            unsigned int port_id = servers_per_leaf + i;
            if (output_voq_map.find(port_id) == output_voq_map.end()) continue;

            unsigned int voq_ind = output_voq_map[port_id];
            expr_vector not_valid_dsts(net_ctx.z3_ctx());
            for (unsigned int dst = 0; dst < server_cnt; dst++){
                if ( (dst < leaf_id * servers_per_leaf ||
                     dst >= (leaf_id + 1) * servers_per_leaf) &&
                     dst % spine_cnt == i){
                    not_valid_dsts.push_back(pkt_dst != (int) dst);
                }
            }
            snprintf(constr_name, 100, "%s_no_match_port%d(q%d)_at_%d", id.c_str(), port_id,
        voq_ind, t); constr_expr = implies(mk_and(not_valid_dsts), out_queues[voq_ind]->enqs(0)[t]
        == net_ctx.null_pkt()); constr_map.insert(named_constr(constr_name, constr_expr));
        }*/

        // Bounds on packet meta
        if (fw_id >= servers_per_leaf) {
            Queue* queue = in_queues[0];
            for (unsigned int p = 0; p < queue->size(); p++) {
                expr pkt = queue->elem(p)[t];
                expr pkt_val = net_ctx.pkt2val(pkt);
                expr pkt_meta = net_ctx.pkt2meta1(pkt);

                expr_vector meta_bounds(net_ctx.z3_ctx());
                for (unsigned int i = 0; i < servers_per_leaf; i++) {
                    unsigned int dst = (leaf_id * servers_per_leaf) + i;
                    meta_bounds.push_back(pkt_meta == (int) dst);
                }
                snprintf(constr_name,
                         100,
                         "%s_dst_meta_bounds_for_input_queue[%d][%d]",
                         id.c_str(),
                         p,
                         t);
                constr_expr = implies(pkt_val, mk_or(meta_bounds));
                constr_map.insert(named_constr(constr_name, constr_expr));
            }

            for (unsigned int p = 0; p < queue->max_enq(); p++) {
                expr pkt = queue->enqs(p)[t];
                expr pkt_val = net_ctx.pkt2val(pkt);
                expr pkt_meta = net_ctx.pkt2meta1(pkt);

                expr_vector meta_bounds(net_ctx.z3_ctx());
                for (unsigned int i = 0; i < servers_per_leaf; i++) {
                    unsigned int dst = (leaf_id * servers_per_leaf) + i;
                    meta_bounds.push_back(pkt_meta == (int) dst);
                }
                snprintf(constr_name,
                         100,
                         "%s_dst_meta_bounds_for_input_queue_enq[%d][%d]",
                         id.c_str(),
                         p,
                         t);
                constr_expr = implies(pkt_val, mk_or(meta_bounds));
                constr_map.insert(named_constr(constr_name, constr_expr));
            }

            for (unsigned int p = 0; p < queue->size(); p++) {
                expr pkt = queue->elem(p)[t];
                expr pkt_val = net_ctx.pkt2val(pkt);
                expr pkt_meta = net_ctx.pkt2meta2(pkt);

                unsigned int spine_id = fw_id - servers_per_leaf;
                snprintf(constr_name,
                         100,
                         "%s_flow_meta_bounds_for_input_queue[%d][%d]",
                         id.c_str(),
                         p,
                         t);
                constr_expr = implies(pkt_val, pkt_meta == (int) spine_id);
                constr_map.insert(named_constr(constr_name, constr_expr));
            }

            for (unsigned int p = 0; p < queue->max_enq(); p++) {
                expr pkt = queue->enqs(p)[t];
                expr pkt_val = net_ctx.pkt2val(pkt);
                expr pkt_meta = net_ctx.pkt2meta2(pkt);

                unsigned int spine_id = fw_id - servers_per_leaf;
                snprintf(constr_name,
                         100,
                         "%s_flow_meta_bounds_for_input_queue_enq[%d][%d]",
                         id.c_str(),
                         p,
                         t);
                constr_expr = implies(pkt_val, pkt_meta == (int) spine_id);
                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        } else {
            Queue* queue = in_queues[0];

            for (unsigned int p = 0; p < queue->max_enq(); p++) {
                expr pkt = queue->enqs(p)[t];
                expr pkt_val = net_ctx.pkt2val(pkt);
                expr pkt_meta = net_ctx.pkt2meta1(pkt);

                snprintf(constr_name,
                         100,
                         "%s_dst_meta_bounds_for_input_queue_enq[%d][%d]",
                         id.c_str(),
                         p,
                         t);
                unsigned int qid = (leaf_id * servers_per_leaf) + fw_id;
                constr_expr = pkt_meta >= 0 && pkt_meta < (int) server_cnt && pkt_meta != (int) qid;
                constr_map.insert(named_constr(constr_name, constr_expr));
            }

            for (unsigned int p = 0; p < queue->max_enq(); p++) {
                expr pkt = queue->enqs(p)[t];
                expr pkt_val = net_ctx.pkt2val(pkt);
                expr pkt_meta = net_ctx.pkt2meta2(pkt);

                snprintf(constr_name,
                         100,
                         "%s_flow_meta_bounds_for_input_queue_enq[%d][%d]",
                         id.c_str(),
                         p,
                         t);
                constr_expr = implies(pkt_val, pkt_meta >= 0 && pkt_meta < (int) spine_cnt);
                constr_map.insert(named_constr(constr_name, constr_expr));
            }

            /*
            for (unsigned int s = 0; s < spine_cnt; s++){
                unsigned int dst_port = servers_per_leaf + s;
                if (output_voq_map.find(dst_port) == output_voq_map.end()) continue;
                unsigned int voq_ind = output_voq_map[dst_port];

                Queue* queue = out_queues[voq_ind];
                for (unsigned int p = 0; p < queue->size(); p++){
                    expr pkt = queue->elem(p)[t];
                    expr pkt_val = net_ctx.pkt2val(pkt);
                    expr pkt_meta = net_ctx.pkt2meta2(pkt);

                    snprintf(constr_name, 100, "%s_flow_meta_bounds_for_output_queue[%d][%d][%d]",
            id.c_str(), voq_ind, p, t); constr_expr = implies(pkt_val, pkt_meta == (int) s);
                    constr_map.insert(named_constr(constr_name, constr_expr));
                }
            } */
        }

        // Don't enqueue into any port if packet is invalid

        for (unsigned int i = 0; i < out_queue_cnt(); i++) {
            snprintf(constr_name, 100, "%s_invalid_pkt_port_%d_at_%d", id.c_str(), i, t);
            constr_expr = implies(!pkt_val, out_queues[i]->enqs(0)[t] == net_ctx.null_pkt());
            constr_map.insert(named_constr(constr_name, constr_expr));
        }

        // Make sure nothing else gets pushed to the output queue
        for (unsigned int i = 0; i < out_queue_cnt(); i++) {
            for (unsigned int e = 1; e < out_queues[i]->max_enq(); e++) {
                snprintf(constr_name, 100, "%s_out_%d_enqs[%d][%d]_is_null", id.c_str(), i, e, t);
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

                snprintf(constr_name, 100, "%s_in_%d_enqs[%d][%d]_meta3_bound", id.c_str(), i, e,
        t); expr constr_expr = meta3 <= (int) t && meta3 >= 0;
                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }

        for (unsigned int i = 0; i < out_queue_cnt(); i++){
            Queue* queue = out_queues[i];
            for (unsigned int e = 0; e < queue->max_enq(); e++){
                expr pkt = queue->enqs(e)[t];
                expr meta3 = net_ctx.pkt2meta3(pkt);

                snprintf(constr_name, 100, "%s_out_%d_enqs[%d][%d]_meta3_bound", id.c_str(), i, e,
        t); unsigned int meta3_bound = t; expr constr_expr = meta3 <= (int) meta3_bound && meta3 >=
        0; constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }
        */
        /*
        // Bounds on meta3 (arrival time)
        if (fw_id >= servers_per_leaf){
            for (unsigned int i = 0; i < in_queue_cnt(); i++){
                Queue* queue = in_queues[i];
                for (unsigned int e = 0; e < queue->max_enq(); e++){
                    expr pkt = queue->enqs(e)[t];
                    expr meta3 = net_ctx.pkt2meta3(pkt);

                    snprintf(constr_name, 100, "%s_in_%d_enqs[%d][%d]_meta3_bound", id.c_str(), i,
        e, t); unsigned int meta3_bound = 0; if (t >= 2) meta3_bound = t - 2; expr constr_expr =
        meta3 <= (int) meta3_bound && meta3 >= 0; constr_map.insert(named_constr(constr_name,
        constr_expr));
                }
            }

            for (unsigned int i = 0; i < out_queue_cnt(); i++){
                Queue* queue = out_queues[i];
                for (unsigned int e = 0; e < queue->max_enq(); e++){
                    expr pkt = queue->enqs(e)[t];
                    expr meta3 = net_ctx.pkt2meta3(pkt);

                    snprintf(constr_name, 100, "%s_out_%d_enqs[%d][%d]_meta3_bound", id.c_str(), i,
        e, t); unsigned int meta3_bound = 0; if (t >= 3) meta3_bound = t - 3; expr constr_expr =
        meta3 <= (int) meta3_bound && meta3 >= 0; constr_map.insert(named_constr(constr_name,
        constr_expr));
                }
            }
        }
        else {
            for (unsigned int i = 0; i < in_queue_cnt(); i++){
                Queue* queue = in_queues[i];
                for (unsigned int e = 0; e < queue->max_enq(); e++){
                    expr pkt = queue->enqs(e)[t];
                    expr meta3 = net_ctx.pkt2meta3(pkt);

                    snprintf(constr_name, 100, "%s_in_%d_enqs[%d][%d]_meta3_bound", id.c_str(), i,
        e, t); expr constr_expr = meta3 <= (int) t && meta3 >= 0;
                    constr_map.insert(named_constr(constr_name, constr_expr));
                }
            }

            for (unsigned int i = 0; i < out_queue_cnt(); i++){
                Queue* queue = out_queues[i];
                for (unsigned int e = 0; e < queue->max_enq(); e++){
                    expr pkt = queue->enqs(e)[t];
                    expr meta3 = net_ctx.pkt2meta3(pkt);

                    snprintf(constr_name, 100, "%s_out_%d_enqs[%d][%d]_meta3_bound", id.c_str(), i,
        e, t); unsigned int meta3_bound = 0; if (t >= 1) meta3_bound = t - 1; expr constr_expr =
        meta3 <= (int) meta3_bound && meta3 >= 0; constr_map.insert(named_constr(constr_name,
        constr_expr));
                }
            }
        }*/
    }
}
