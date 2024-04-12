#include "../../lib/statements/c_assign.hpp"

C_assign::C_assign(string var_name, RHS val): var_name(var_name), val(val) {}

C_assign::~C_assign() {}

int C_assign::parse_rhs(Global_Var &global) {
    string expression = val.expression;
    if (expression == "") {
        // int value
        return val.number;
    } else {
        // handle case i.e. int a = b;
        if (global.int_vars.count(expression)) return global.int_vars[expression];
        // TODO: remove hardcode and generalize parsing expression
        int rhs = (global.int_vars["q"] + global.int_vars["i"]) % global.in_queues.size();
        return rhs;
    }
}

expr C_assign::generate_constr(expr prev_cond, NetContext& net_ctx, map<string, expr>& constr_map, Global_Var &global) {
    bool in_z3_vars = (global.z3_vars.count(var_name) > 0);
    bool in_int_vars = (global.int_vars.count(var_name) > 0);
    int rhs = parse_rhs(global);
    char constr_name[100];

    if (in_z3_vars || global.curr_t == -1) {
        // z3_var
        int time = global.curr_t + 1; // assign for the next timestamp
        if (time >= global.total_time) return net_ctx.bool_val(false);
        if (time == 0) {
            // declare a z3 expr
            expr_vector z3_var(net_ctx.z3_ctx());
            for (unsigned int t = 0; t < global.total_time; t++) {
                char vname[100];
                sprintf(vname, "%s_%s[%d]", global.id.c_str(), var_name.c_str(), t);
                z3_var.push_back(net_ctx.int_const(vname));
            }
            global.z3_vars.insert({var_name, z3_var});
            z3_constr_accumulator cond = {net_ctx.bool_val(false), 0};
            global.z3_vars_conds.insert({var_name, cond});
        } else {
            global.z3_vars_conds.at(var_name).disjuct_cond = global.z3_vars_conds.at(var_name).disjuct_cond || prev_cond;
        }
        int constr_counter = global.z3_vars_conds.at(var_name).constr_cnt;
        sprintf(constr_name, "%s_%s_at_%d_is_%d__%d", global.id.c_str(), var_name.c_str(), time, rhs, constr_counter);
        global.z3_vars_conds.at(var_name).constr_cnt += 1;
        if (constr_map.count(constr_name) > 0) {
            std::cout << "ERROR: duplicate z3 var constr name" << std::endl;
        }
        expr constr_expr = implies(prev_cond, global.z3_vars.at(var_name)[time] == net_ctx.int_val(rhs));
        constr_map.insert(named_constr(constr_name, constr_expr));
    } else {
        // insert or update integer variables
        global.int_vars[var_name] = rhs;
    }
    return net_ctx.bool_val(false);
}
