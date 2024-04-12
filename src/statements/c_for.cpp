#include "../../lib/statements/c_for.hpp"
using namespace std;

C_for::C_for(string idx_var, std::pair<int, int> range, vector<Statement*> &instrs): Statement(), idx_var(idx_var), range(range), instrs(instrs) {}

C_for::~C_for() {
    for (Statement *st : instrs) {
        delete st;
    }
}
// prev_cond is reference ???
expr C_for::generate_constr(expr prev_cond, NetContext& net_ctx, map<string, expr>& constr_map, Global_Var &global) {
    global.int_vars[idx_var] = range.first;
    int idx = range.first;
    expr copy_prev = prev_cond && net_ctx.bool_val(true);
    for ( ; idx < range.second; idx++) {
        global.int_vars[idx_var] = idx;

        for (Statement *instr: instrs) {
            instr->generate_constr(prev_cond, net_ctx, constr_map, global);
        }
        // TODO: break check needs to be added to other statements as well
        prev_cond = prev_cond && !global.break_cond;
        if (idx < range.second - 1) global.break_cond = net_ctx.bool_val(false);
        else global.break_cond = !(prev_cond || !copy_prev);
    }
    global.int_vars.erase(idx_var);
    return net_ctx.bool_val(false);
}