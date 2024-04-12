#include "../../lib/statements/is_equal.hpp"

Is_Equal::Is_Equal(string var_name_1, string var_name_2): var_name_1(var_name_1), var_name_2(var_name_2) {}

Is_Equal::~Is_Equal() {}

expr Is_Equal::generate_constr(expr prev_cond, NetContext& net_ctx, map<string, expr>& constr_map, Global_Var &global) {
    // TODO: need to handle different combinations of types for var1 and var2
    bool is_z3_var = (global.int_vars.count(var_name_1) != 0) ? false : true;
    int rhs_val = global.int_vars[var_name_2];
    expr ret = net_ctx.bool_val(false);
    if (is_z3_var) {
        ret = (global.z3_vars.at(var_name_1)[global.curr_t] == rhs_val);
        return ret;
    }
    return ret;
}
