//
//  priority_qm.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/12/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "priority_qm.hpp"
#include "../lib/statements/c_for.hpp"  // ????????????
#include "../lib/statements/c_if.hpp"
#include "../lib/statements/c_move.hpp"
#include "../lib/statements/c_break.hpp"
#include "../lib/statements/not_empty.hpp"

PriorityQM::PriorityQM(cid_t id,
                       unsigned int total_time,
                       vector<QueueInfo> in_queue_info,
                       QueueInfo out_queue_info,
                       NetContext& net_ctx):
QueuingModule(id, total_time, in_queue_info, vector<QueueInfo>{out_queue_info}, net_ctx) {
    init(net_ctx);
}


void PriorityQM::add_proc_vars(NetContext& net_ctx) {
    (void) net_ctx;
}

Statement* PriorityQM::schedule() {
//    std::cout << "!!!!!!! DEBUG User creating scheduler logics" << std::endl;
//    C_move *c_move = new C_move();
//    C_break *c_break = new C_break();
//    vector<Statement*> if_instrs = {c_move, c_break};
//    Not_Empty *not_empty = new Not_Empty();
//    C_if *c_if = new C_if(not_empty, if_instrs);
//    vector<Statement*> for_instrs = {c_if};
//    C_for *c_for = new C_for(in_queues.size(), for_instrs);
//    return c_for;
    return nullptr;
}

void PriorityQM::add_constrs(NetContext& net_ctx, map<string, expr>& constr_map) {
    Global_Var global = {net_ctx, id, in_queues, out_queues, (int)total_time}; // total_time: cast from unsigned int to int
    //Statement *top_st = schedule();    ????????????
    int in_q_size = in_queues.size();
    C_move *c_move = new C_move("q");
    C_break *c_break = new C_break();
    vector<Statement*> if_instrs = {c_move, c_break};
    Not_Empty *not_empty = new Not_Empty("q");
    C_if *c_if = new C_if(not_empty, if_instrs);
    vector<Statement*> for_instrs = {c_if};
    std::pair<int, int> range(0, in_q_size);
    C_for *c_for = new C_for("q", range, for_instrs);
    for (unsigned int t = 0; t < total_time; t++) {
        global.curr_t = t;
        c_for->generate_constr(net_ctx.bool_val(true), net_ctx, constr_map, global);
        t_generate_constrs(net_ctx, constr_map, global);
        global.break_cond = net_ctx.bool_val(false);
    }

    char constr_name[100];
    for (unsigned int t = 0; t < total_time; t++) {
        // Push to output queue accordingly
        Queue* outq = out_queues[0];
        for (unsigned int q = 0; q < in_q_size; q++) {
            sprintf(constr_name, "%s_output_from_%d_%d", id.c_str(), q, t);
            expr constr_expr = implies(in_queues[q]->deq_cnt(t) == 1,
                                       outq->enqs(0)[t] == in_queues[q]->elem(0)[t]);
            constr_map.insert(named_constr(constr_name, constr_expr));
        }

        // handle the case where all input queues are empty
        expr_vector all_empty(net_ctx.z3_ctx());
        for (unsigned int q = 0; q < in_q_size; q++) {
            all_empty.push_back(!net_ctx.pkt2val(in_queues[q]->elem(0)[t]));
        }
        sprintf(constr_name, "%s_all_empty_input_%d", id.c_str(), t);
        expr constr_expr = implies(mk_and(all_empty), outq->enqs(0)[t] == net_ctx.null_pkt());
        constr_map.insert(named_constr(constr_name, constr_expr));

        // Make sure nothing else gets pushed to the output queue
        for (unsigned int i = 1; i < outq->max_enq(); i++) {
            sprintf(constr_name, "%s_only_one_output_%d_%d", id.c_str(), i, t);
            expr constr_expr = outq->enqs(i)[t] == net_ctx.null_pkt();
            constr_map.insert(named_constr(constr_name, constr_expr));
        }
    }

    // TODO: clean memory
    // delete c_for;   ????????
}
