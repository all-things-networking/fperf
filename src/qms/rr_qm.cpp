//
//  rr_qm.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 2/17/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "rr_qm.hpp"
#include "../lib/statements/c_for.hpp"  // ????????????
#include "../lib/statements/c_if.hpp"
#include "../lib/statements/c_move.hpp"
#include "../lib/statements/c_break.hpp"
#include "../lib/statements/not_empty.hpp"
#include "../lib/statements/is_equal.hpp"
#include "../lib/statements/c_assign.hpp"

RRQM::RRQM(cid_t id,
           unsigned int total_time,
           vector<QueueInfo> in_queue_info,
           QueueInfo out_queue_info,
           NetContext& net_ctx):
QueuingModule(id, total_time, in_queue_info, vector<QueueInfo>{out_queue_info}, net_ctx) {
    init(net_ctx);
}

void RRQM::add_proc_vars(NetContext& net_ctx) {
    (void) net_ctx;
}

void RRQM::add_constrs(NetContext& net_ctx, map<string, expr>& constr_map) {
    char constr_name[100];
    unsigned long in_queue_cnt = in_queues.size();

    Global_Var global = {net_ctx, id, in_queues, out_queues, (int)total_time};
    //Statement *top_st = schedule();    ????????????
    C_move *c_move = new C_move("serve_q");
    C_assign *c_assign1 = new C_assign("lsq", RHS("serve_q"));
    C_break *c_break = new C_break();
    vector<Statement*> if_instrs_1 = {c_move, c_assign1, c_break};
    Not_Empty *not_empty = new Not_Empty("serve_q");
    C_if *c_if_1 = new C_if(not_empty, if_instrs_1);

    C_assign *c_assign2 = new C_assign("serve_q", RHS("(q + i) % Q"));
    vector<Statement*> for_instrs_1 = {c_assign2, c_if_1};
    std::pair<int, int> range_1(1, in_queue_cnt+1);
    C_for *c_for_1 = new C_for("i", range_1, for_instrs_1);

    Is_Equal *is_equal = new Is_Equal("lsq", "q");
    vector<Statement*> if_instrs_2 = {c_for_1};
    C_if *c_if_2 = new C_if(is_equal, if_instrs_2);
    std::pair<int, int> range_2(0, in_queue_cnt);
    vector<Statement*> for_instrs_2 = {c_if_2};
    C_for *c_for_2 = new C_for("q", range_2, for_instrs_2);

    C_assign* init_lsq = new C_assign("lsq", RHS("", in_queue_cnt-1));

    vector<Statement*> global_scope = {init_lsq};
    vector<Statement*> inner_scope = {c_for_2};
    for (Statement *glb_st : global_scope) {
        glb_st->generate_constr(net_ctx.bool_val(true), net_ctx, constr_map, global);
    }
    for (unsigned int t = 0; t < total_time; t++) {
        global.curr_t = t;
        for (Statement *instr: inner_scope) {
            instr->generate_constr(net_ctx.bool_val(true), net_ctx, constr_map, global);
        }
        t_generate_constrs(net_ctx, constr_map, global);
        global.break_cond = net_ctx.bool_val(false);
    }

    for (unsigned int t = 0; t < total_time; t++) {

        // Push to output queue accordingly
        Queue* outq = out_queues[0];
        for (unsigned int q = 0; q < in_queue_cnt; q++) {
            sprintf(constr_name, "%s_output_from_%d_%d", id.c_str(), q, t);
            expr constr_expr = implies(in_queues[q]->deq_cnt(t) == 1,
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
}

