//
//  priority_qm.cpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/12/20.
//  Copyright � 2020 Mina Tahmasbi Arashloo. All rights reserved.
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
    
    for (int t = 0; t < total_time+1; t++) {
        std::sprintf(vname, "%s_%d_pause_state_[%d]", sid.c_str(), return_to_sender, t);
        pause_state.push_back(net_ctx.bool_const(vname));
    }

    
    for (int t = 0; t < total_time; t++) {
        std::sprintf(vname, "%s_sent_pause_[%d]", sid.c_str(), t);
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
    std::sprintf(constr_name, "%s_%d_pause_state_[%d]", sid.c_str(), return_to_sender, 0);
    expr constr_expr = pause_state[1] == net_ctx.bool_val(false);
    constr_map.insert(named_constr(constr_name, constr_expr));

    //std::sprintf(vname, "%s_sent_pause_[%d]", id.c_str(), 0);
    //constr_expr = net_ctx.bool_const(vname) == net_ctx.bool_val(false);
    std::sprintf(constr_name, "%s_%d_sent_pause_[%d]", sid.c_str(), return_to_sender, 0);
    //constr_expr = net_ctx.bool_const(vname) == net_ctx.bool_val(false);
    constr_expr = sent_pause[0] == net_ctx.bool_val(false);
    constr_map.insert(named_constr(constr_name, constr_expr));

    expr thresh = net_ctx.int_val(threshold);
    for (unsigned int t = 0; t < total_time; t++) {
        // set to dummy variable
        expr send_pause = net_ctx.bool_val(false);
        expr send_unpause = net_ctx.bool_val(false);
        if (t != 0) {
            // ports full
            expr voq_sum = net_ctx.int_val(0);
            for (int i = 0; i < out_queues.size(); i++) {
                sprintf(constr_name, "%s_full_port%d_at_%d", id.c_str(), i, t);
                voq_sum = voq_sum + out_queues[i]->curr_size(t);
            }
            send_pause = voq_sum > thresh && !sent_pause[t-1];
            send_unpause = voq_sum <= thresh && sent_pause[t - 1];
        }


        for (unsigned int i = 0; i < in_queues.size(); i++) {
            Queue* in_queue = in_queues[i];

            expr in_pkt = in_queue->elem(0)[t];
            expr pkt_val = net_ctx.pkt2val(in_pkt);
            expr pkt_dst = net_ctx.pkt2meta1(in_pkt);
            expr pkt_tag = net_ctx.pkt2meta2(in_pkt);

            // Consume pause packet
            sprintf(constr_name, "%s_consume_pause_packet_%d", id.c_str(), t);
            constr_expr = implies(pkt_val && pkt_tag == net_ctx.int_val(10), pause_state[t+1]);
            constr_map.insert(named_constr(constr_name, constr_expr));

            // Consume unpause packet
            sprintf(constr_name, "%s_consume_unpause_packet_%d", id.c_str(), t);
            constr_expr = implies(pkt_val && pkt_tag == net_ctx.int_val(11), !pause_state[t+1]);
            constr_map.insert(named_constr(constr_name, constr_expr));

            //
            if (t != 0) {
                sprintf(constr_name, "%s_maintain_pause_state_%d", id.c_str(), t);
                constr_expr = implies(!(pkt_val && (pkt_tag == net_ctx.int_val(11) || pkt_tag == net_ctx.int_val(10))),
                    pause_state[t+1] == pause_state[t+1 - 1]);
                constr_map.insert(named_constr(constr_name, constr_expr));
            }

            // Set deq_cnt for input queue
            sprintf(constr_name, "%s_in_queue_deq_cnt_is_zero_or_one_at_%d", id.c_str(), t);
            constr_expr = implies(pkt_val, in_queue->deq_cnt(t) == 1) &&
                implies(!pkt_val, in_queue->deq_cnt(t) == 0);
            constr_map.insert(named_constr(constr_name, constr_expr));

            unsigned int dst_port = control_flow[pkt_dst];

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
