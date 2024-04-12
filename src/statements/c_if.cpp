#include "../../lib/statements/c_if.hpp"

C_if::C_if(Statement *if_cond, vector<Statement*> &if_instrs): if_cond(if_cond), if_instrs(if_instrs) { }

C_if::~C_if() {
    delete if_cond;
    for (Statement *st : if_instrs) {
        delete st;
    }
}

expr C_if::generate_constr(expr prev_cond, NetContext& net_ctx, map<string, expr>& constr_map,
                           Global_Var &global) {
    expr random_expr = net_ctx.bool_val(false);
    expr cond = if_cond->generate_constr(random_expr, net_ctx, constr_map, global);
    expr next_cond = prev_cond && cond;

    for (Statement *instr : if_instrs) {
        instr->generate_constr(next_cond, net_ctx, constr_map, global);
    }
    return random_expr;
}