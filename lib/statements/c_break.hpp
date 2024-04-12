#ifndef c_break_hpp
#define c_break_hpp

#include "statement.hpp"

class C_break : public Statement {
public:
    C_break();
    ~C_break();
    expr generate_constr(expr prev_cond,
                         NetContext& net_ctx,
                         map<string, expr>& constr_map,
                         Global_Var &global);
};
#endif //c_break_hpp