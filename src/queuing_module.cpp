//
//  queuing_module.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/9/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//
#include <iostream>
#include "queuing_module.hpp"

QueuingModule::QueuingModule(cid_t id,
                             unsigned int total_time,
                             vector<QueueInfo> in_queue_info,
                             vector<QueueInfo> out_queue_info,
                             NetContext& net_ctx):
id(id),
total_time(total_time),
out_queue_info(out_queue_info) {
    unsigned int qid = 0;
    // Instantiate input queues
    cid_t base_in_id = id;

    for (unsigned int i = 0; i < in_queue_info.size(); i++) {
        QueueInfo qinfo = in_queue_info[i];
        Queue* q;
        switch (qinfo.type) {
            case queue_t::LINK: {
                q = new Link(base_in_id, to_string(qid), total_time, net_ctx);
                break;
            }
            case queue_t::IMM_QUEUE: {
                q = new ImmQueue(base_in_id,
                                 to_string(qid),
                                 qinfo.size,
                                 qinfo.max_enq,
                                 qinfo.max_deq,
                                 total_time,
                                 net_ctx);
                break;
            }
            default: {
                q = new Queue(base_in_id,
                              to_string(qid),
                              qinfo.size,
                              qinfo.max_enq,
                              qinfo.max_deq,
                              total_time,
                              net_ctx);
                break;
            }
        }
        in_queues.push_back(q);
        qid++;
    }

    // Placeholder pointers for output queues
    for (unsigned int i = 0; i < out_queue_info.size(); i++) {
        Queue* q = nullptr;
        out_queues.push_back(q);
    }
}

void QueuingModule::init(NetContext& net_ctx) {
    add_proc_vars(net_ctx);
}

unsigned long QueuingModule::in_queue_cnt() {
    return in_queues.size();
}

unsigned long QueuingModule::out_queue_cnt() {
    return out_queues.size();
}

Queue* QueuingModule::get_in_queue(unsigned int ind) {
    return in_queues[ind];
}

Queue* QueuingModule::get_out_queue(unsigned int ind) {
    return out_queues[ind];
}

void QueuingModule::set_out_queue(unsigned int ind, Queue* queue) {
    out_queues[ind] = queue;
}

QueueInfo QueuingModule::get_out_queue_info(unsigned int ind) {
    return out_queue_info[ind];
}

cid_t QueuingModule::get_id() {
    return id;
}

Statement* QueuingModule::schedule() {
    return nullptr;
}

void QueuingModule::t_generate_constrs(NetContext& net_ctx, map<string, expr>& constr_map, Global_Var& global) {
    int in_q_size = global.in_queues.size();
    int t = global.curr_t;
    char constr_name[100];

    for (int q = 0; q < in_q_size; q++) {
        expr conds = global.move_conds.at(q);
        expr true_constr = implies(conds, global.in_queues[q]->deq_cnt(t) == 1);
        expr false_constr = implies(!conds, global.in_queues[q]->deq_cnt(t) == 0);

        sprintf(constr_name, "%s_deq_cnt_%d_%d_is_one", global.id.c_str(), q, t);
        constr_map.insert(named_constr(constr_name, true_constr));
        sprintf(constr_name, "%s_deq_cnt_%d_%d_is_zero", global.id.c_str(), q, t);
        constr_map.insert(named_constr(constr_name, false_constr));

        // TODO: clear move_conds?
        // resize(0);

        global.move_conds.erase(q);
        global.move_conds.insert({q, net_ctx.bool_val(false)});
    }

    // for z3_vars
    if (t >= global.total_time - 1) return;
    unordered_map<string, z3_constr_accumulator>::iterator it = global.z3_vars_conds.begin();
    for (; it != global.z3_vars_conds.end(); ++it) {
        string var_name = it->first;
        if (global.z3_vars.count(var_name) == 0) {
            global.z3_vars_conds.erase(var_name);
            continue;
        }
        // no update on this z3_var
        expr false_constr = implies(!(it->second).disjuct_cond, global.z3_vars.at(var_name)[t+1] == global.z3_vars.at(var_name)[t]);
        sprintf(constr_name, "%s_%s_%d_stays_the_same", global.id.c_str(), var_name.c_str(), t+1);
        constr_map.insert(named_constr(constr_name, false_constr));
        // TODO: clear z3_var_conds?
        // (i->second).resize(0);

        global.z3_vars_conds.erase(var_name);
        z3_constr_accumulator cond = {net_ctx.bool_val(false), 0};
        global.z3_vars_conds.insert({var_name, cond});
    }
}