#include "../../lib/statements/c_move.hpp"

C_move::C_move(string in_q_var, int num_pkt, int out_q): in_q_var(in_q_var), num_pkt(num_pkt), out_q(out_q) { }

C_move::~C_move() {}

expr C_move::generate_constr(expr prev_cond, NetContext& net_ctx, map<string, expr>& constr_map, Global_Var &global) {
    // TODO: generalize output and move to output queue
    // Queue* outq = out_queues[out_q];
    int in_q = global.int_vars[in_q_var];
    global.move_conds.at(in_q) = global.move_conds.at(in_q) || prev_cond;
    return net_ctx.bool_val(false);
}