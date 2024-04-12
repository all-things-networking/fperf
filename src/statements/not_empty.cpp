#include "../../lib/statements/not_empty.hpp"

Not_Empty::Not_Empty(string q_var): q_var(q_var) {}

Not_Empty::~Not_Empty() {}

expr Not_Empty::generate_constr(expr prev_cond, NetContext& net_ctx, map<string, expr>& constr_map, Global_Var &global) {
    int in_q = global.int_vars[q_var];
    return net_ctx.pkt2val(global.in_queues[in_q]->elem(0)[global.curr_t]);
}
