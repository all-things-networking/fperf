#include "statement.hpp"

Statement::Statement() {
    std::cout << "DEBUG: Created Statement" << std::endl;
}

expr Statement::generate_constr(expr prev_cond, NetContext& net_ctx, map<string, expr>& constr_map, Global_Var &global) {
    std::cout << "DEBUG: Statement generate should not be called" << std::endl;
    return net_ctx.bool_val(false);
}