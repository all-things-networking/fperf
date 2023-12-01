//
//  priority_qm.cpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/12/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "roce_qm.hpp"


RoCEQM::RoCEQM(cid_t id,
    cid_t sid,
    unsigned int total_time,
    unsigned int buffer_size,
    unsigned int threshold,
    unsigned int return_to_sender,
    std::vector<QueueInfo> in_queue_info,
    std::vector<QueueInfo> out_queue_info,
    std::map<unsigned int, unsigned int> control_flow,
    NetContext& net_ctx) :
    QueuingModule(id, total_time, in_queue_info,
        out_queue_info, net_ctx),
    sid(sid),
    buffer_size(buffer_size),
    threshold(threshold),
    return_to_sender(return_to_sender),
    pause_state(expr_vector(net_ctx.z3_ctx())),
    sent_pause(expr_vector(net_ctx.z3_ctx()))
{
    assert(in_queues.size() == 1);

    port_cnt = (unsigned int)out_queues.size();
    init(net_ctx);
}


void RoCEQM::add_proc_vars(NetContext& net_ctx) {
    char vname[100];

    // pause/unpause state
    
    for (int t = 0; t < total_time; t++) {
        std::sprintf(vname, "%s_%d_pause_state_[%d]", sid.c_str(), return_to_sender, t);
        pause_state.push_back(net_ctx.bool_const(vname));
    }

    
    for (int t = 0; t < total_time; t++) {
        std::sprintf(vname, "%s_%d_sent_pause_[%d]", sid.c_str(), return_to_sender, t);
        sent_pause.push_back(net_ctx.bool_const(vname));
    }
    
}

void RoCEQM::add_constrs(NetContext& net_ctx,
    std::map<std::string, expr>& constr_map) {
    char constr_name[100];
    char vname[100];

    // Set starting value of pause state to !paused
    // Note this is pausing the corresponding output port from [return_to_sender]
    //std::sprintf(vname, "%s_%d_pause_state_[%d]", id.c_str(), return_to_sender, 0);
    //expr constr_expr = net_ctx.bool_const(vname) == net_ctx.bool_val(false);
    std::sprintf(constr_name, "%s_%d_pause_state_init_[%d]", sid.c_str(), return_to_sender, 0);
    expr constr_expr = pause_state[0] == net_ctx.bool_val(false);
    constr_map.insert(named_constr(constr_name, constr_expr));

    //std::sprintf(vname, "%s_sent_pause_[%d]", id.c_str(), 0);
    //constr_expr = net_ctx.bool_const(vname) == net_ctx.bool_val(false);
    std::sprintf(constr_name, "%s_%d_sent_pause_init_[%d]", sid.c_str(), return_to_sender, 0);
    //constr_expr = net_ctx.bool_const(vname) == net_ctx.bool_val(false);
    constr_expr = sent_pause[0] == net_ctx.bool_val(false);
    constr_map.insert(named_constr(constr_name, constr_expr));

    expr thresh = net_ctx.int_val(3);
    for (unsigned int t = 0; t < total_time; t++) {

        // set to dummy variable
        expr send_pause = net_ctx.bool_val(false);
        expr send_unpause = net_ctx.bool_val(false);
        if (t != 0) {
            // ports full
            expr voq_sum = net_ctx.int_val(0);
            for (int i = 0; i < out_queues.size(); i++) {
                int switch_id = sid.back() - '0';
                int port_num = return_to_sender + (i * 3);
                std::sprintf(vname, "roce%d.%d_curr_size_[%d]", switch_id, port_num, t);
                sprintf(constr_name, "%s_full_port%d_at_%d", id.c_str(), i, t);
                voq_sum = voq_sum + net_ctx.get_int_const(vname);
            }
            send_pause = voq_sum > thresh && !sent_pause[t-1];
            send_unpause = voq_sum <= thresh && sent_pause[t - 1];

            char name[100];
            sprintf(name, "%s_voqs_sum_at_%d", id.c_str(), t);
            constr_expr = net_ctx.int_const(name) == voq_sum;
            sprintf(constr_name, "%s_voq_sum_at_%d", id.c_str(), t);
            constr_map.insert(named_constr(constr_name, constr_expr));
        }

        // TOCHANGE
        // bounds on meta size
        if (return_to_sender == 2) {
            if (t <= 15)
                constr_expr = in_queues[0]->get_metric(metric_t::CENQ)->val(t) > net_ctx.int_val(t);
            else
                constr_expr = in_queues[0]->get_metric(metric_t::CENQ)->val(t) <= net_ctx.int_val(16);
            sprintf(constr_name, "%s_Min_Packet_Num_at_%d", id.c_str(), t);
            constr_map.insert(named_constr(constr_name, constr_expr));

            for (unsigned int p = 0; p < in_queues[0]->size(); p++) {
                expr in_pkt = in_queues[0]->elem(p)[t];
                expr pkt_val = net_ctx.pkt2val(in_pkt);
                expr pkt_dst = net_ctx.pkt2meta1(in_pkt);
                expr pkt_tag = net_ctx.pkt2meta2(in_pkt);


                constr_expr = implies(pkt_val, net_ctx.int_val(0) < pkt_dst && pkt_dst < net_ctx.int_val(3));
                sprintf(constr_name, "%s_Meta1_Constraint_for_%d_at_%d", id.c_str(), p, t);
                constr_map.insert(named_constr(constr_name, constr_expr));

                constr_expr = implies(pkt_val, pkt_tag != net_ctx.int_val(10) && pkt_tag != net_ctx.int_val(11));
                sprintf(constr_name, "%s_Meta2_Constraint_for_%d_at_%d", id.c_str(), p, t);
                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }
        // TOCHANGE

        for (unsigned int i = 0; i < in_queues.size(); i++) {
            Queue* in_queue = in_queues[i];

            expr in_pkt = in_queue->elem(0)[t];
            expr pkt_val = net_ctx.pkt2val(in_pkt);
            expr pkt_dst = net_ctx.pkt2meta1(in_pkt);
            expr pkt_tag = net_ctx.pkt2meta2(in_pkt);


            // Consume pause packet
            sprintf(constr_name, "%s_consume_pause_packet_%d", id.c_str(), t);
            constr_expr = implies(pkt_val && pkt_tag == net_ctx.int_val(10), pause_state[t]);
            constr_map.insert(named_constr(constr_name, constr_expr));

            // Consume unpause packet
            sprintf(constr_name, "%s_consume_unpause_packet_%d", id.c_str(), t);
            constr_expr = implies(pkt_val && pkt_tag == net_ctx.int_val(11), !pause_state[t]);
            constr_map.insert(named_constr(constr_name, constr_expr));

            //
            if (t != 0) {
                sprintf(constr_name, "%s_maintain_pause_state_%d", id.c_str(), t);
                constr_expr = implies(!(pkt_val && (pkt_tag == net_ctx.int_val(11) || pkt_tag == net_ctx.int_val(10))),
                    pause_state[t] == pause_state[t - 1]);
                constr_map.insert(named_constr(constr_name, constr_expr));

                sprintf(constr_name, "%s_maintain_sent_pause_%d", id.c_str(), t);
                constr_expr = implies(!(send_pause || send_unpause),
                    sent_pause[t] == sent_pause[t - 1]);
                constr_map.insert(named_constr(constr_name, constr_expr));

                sprintf(constr_name, "%s_update_sent_pause_%d", id.c_str(), t);
                constr_expr = implies(send_pause, sent_pause[t]);
                constr_map.insert(named_constr(constr_name, constr_expr));

                sprintf(constr_name, "%s_update_sent_unpause_%d", id.c_str(), t);
                constr_expr = implies(send_unpause, !sent_pause[t]);
                constr_map.insert(named_constr(constr_name, constr_expr));
            }

            // Set deq_cnt for input queue
            sprintf(constr_name, "%s_in_queue_deq_cnt_is_zero_or_one_at_%d", id.c_str(), t);
            constr_expr = implies(pkt_val || send_pause || send_unpause, in_queue->deq_cnt(t) == 1) &&
                implies(!pkt_val && !send_pause && !send_unpause, in_queue->deq_cnt(t) == 0);
            constr_map.insert(named_constr(constr_name, constr_expr));

            // TOCHANGE
            // unsigned int dst_port = control_flow[pkt_dst];
            // Calculate source routing based off of pkt_dst
            unsigned int ternary_index = 1;
            if (sid == "s1") {
                ternary_index = 2;
            }
            else if (sid == "s2") {
                ternary_index = 3;
            }
            else if (sid == "s3") {
                ternary_index = 4;
            }    

            unsigned int dst_port = (pkt_dst * 3 / (net_ctx.int_val(ternary_index * 3))) % 3;
            dst_port = control_flow[pkt_dst];

            if (return_to_sender == 2) {
                sprintf(constr_name, "%s_forward_dst_%d_to_port%d_at_%d", id.c_str(), i, dst_port, t);
                constr_expr = implies(pkt_val,
                    out_queues[dst_port]->enqs(0)[t] == in_pkt);
                constr_map.insert(named_constr(constr_name, constr_expr));

                for (unsigned int i = 0; i < out_queue_cnt(); i++) {
                    sprintf(constr_name, "%s_invalid_pkt_port_%d_at_%d", id.c_str(), i, t);
                    constr_expr = implies(!pkt_val,
                        out_queues[i]->enqs(0)[t] == net_ctx.null_pkt());
                    constr_map.insert(named_constr(constr_name, constr_expr));

                    sprintf(constr_name, "%s_unmatched_pkt_port_%d_at_%d", id.c_str(), i, t);
                    constr_expr = implies(pkt_val && i != dst_port,
                        out_queues[i]->enqs(0)[t] == net_ctx.null_pkt());
                    constr_map.insert(named_constr(constr_name, constr_expr));
                }
            }
            else {
                //TODO: NEED TO COVER ALL OUTPUT PORTS AND NAME USING DST_PORT MIGHT NOT WORK
                // Normal Forward
                sprintf(constr_name, "%s_forward_dst_%d_to_port%d_at_%d", id.c_str(), i, dst_port, t);
                constr_expr = implies(pkt_val && !send_pause && !send_unpause,
                    out_queues[dst_port]->enqs(0)[t] == in_pkt);
                constr_map.insert(named_constr(constr_name, constr_expr));

                // Forward pause packets
                sprintf(constr_name, "%s_forward_pause_%d_to_port%d_at_%d", id.c_str(), i, dst_port, t);
                constr_expr = implies(pkt_val && send_pause,
                    out_queues[return_to_sender]->enqs(0)[t] == net_ctx.pkt_val(true, 0, 10));
                constr_map.insert(named_constr(constr_name, constr_expr));

                // Forward unpause packets
                sprintf(constr_name, "%s_forward_unpause_%d_to_port%d_at_%d", id.c_str(), i, dst_port, t);
                constr_expr = implies(pkt_val && send_unpause,
                    out_queues[return_to_sender]->enqs(0)[t] == net_ctx.pkt_val(true, 0, 11));
                constr_map.insert(named_constr(constr_name, constr_expr));

                for (unsigned int i = 0; i < out_queue_cnt(); i++) {
                    sprintf(constr_name, "%s_invalid_pkt_port_%d_at_%d", id.c_str(), i, t);
                    constr_expr = implies(!pkt_val,
                        out_queues[i]->enqs(0)[t] == net_ctx.null_pkt());
                    constr_map.insert(named_constr(constr_name, constr_expr));

                    sprintf(constr_name, "%s_unmatched_pkt_port_%d_at_%d", id.c_str(), i, t);
                    constr_expr = implies((pkt_val && !send_pause && !send_unpause && i != dst_port) || (pkt_val && (send_pause || send_unpause) && i != return_to_sender),
                        out_queues[i]->enqs(0)[t] == net_ctx.null_pkt());
                    constr_map.insert(named_constr(constr_name, constr_expr));
                }
            }
        }

        // Make sure nothing else gets pushed to the output queue
        // I think this is needed??

        for (unsigned int i = 0; i < out_queue_cnt(); i++) {
            for (unsigned int e = 1; e < out_queues[i]->max_enq(); e++) {
                sprintf(constr_name, "%s_out_%d_elem[%d][%d]_is_null", id.c_str(), i, e, t);
                expr constr_expr = out_queues[i]->enqs(e)[t] == net_ctx.null_pkt();
                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }

    }
}
