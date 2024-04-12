#ifndef statement_hpp
#define statement_hpp

#include "net_context.hpp"
#include "queue.hpp"

#include <vector>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>

struct z3_constr_accumulator {
    expr disjuct_cond;
    int constr_cnt = 0; // avoid duplicate constr name
};

struct Global_Var {
    NetContext& net_ctx;
    cid_t id;
    vector<Queue*> &in_queues;
    vector<Queue*> &out_queues;
    int total_time = 0;
    int curr_t = -1;
    expr break_cond;
    unordered_map<string, int> int_vars = {};
    unordered_map<string, expr_vector> z3_vars = {};
    unordered_map<string, z3_constr_accumulator> z3_vars_conds = {};
    unordered_map<int, expr> move_conds;

    Global_Var(NetContext& net_ctx, cid_t id, vector<Queue*> &in_queues, vector<Queue*> &out_queues, int total_time):
        net_ctx(net_ctx), id(id), in_queues(in_queues), out_queues(out_queues), total_time(total_time), break_cond(net_ctx.bool_val(false)) {
        int in_q_size = in_queues.size();
        for (int i = 0; i < in_q_size; ++i) {
            move_conds.insert({i, net_ctx.bool_val(false)});
        }
    }
};

class Statement {
public:
    Statement();

    virtual expr generate_constr(expr prev_cond,
                                 NetContext& net_ctx,
                                 map<string, expr>& constr_map,
                                 Global_Var &global);
};
#endif //statement_hpp
