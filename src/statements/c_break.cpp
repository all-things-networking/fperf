#include "../../lib/statements/c_break.hpp"

C_break::C_break() {}

C_break::~C_break() {}

expr C_break::generate_constr(expr prev_cond, NetContext& net_ctx, map<string, expr>& constr_map, Global_Var &global) {
    global.break_cond = prev_cond;
    return net_ctx.bool_val(false);
}