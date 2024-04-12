#ifndef c_assign_hpp
#define c_assign_hpp

#include "statement.hpp"

struct RHS {
    string expression = "";
    int number = 0;
    RHS(string expression = "", int number = 0): expression(expression), number(number) {}
};

class C_assign : public Statement {
private:
    string var_name;
    RHS val;
    int parse_rhs(Global_Var &global);

public:
    C_assign(string var_name, RHS val);
    ~C_assign();
    expr generate_constr(expr prev_cond,
                         NetContext& net_ctx,
                         map<string, expr>& constr_map,
                         Global_Var &global);
};

#endif //c_assign_hpp
