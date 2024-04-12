#ifndef c_if_hpp
#define c_if_hpp

#include "statement.hpp"

class C_if : public Statement {
private:
    Statement *if_cond;
    vector<Statement*> &if_instrs;
    // add else instrs

public:
    C_if(Statement *if_cond, vector<Statement*> &if_instrs);
    ~C_if();
    expr generate_constr(expr prev_cond,
                         NetContext& net_ctx,
                         map<string, expr>& constr_map,
                         Global_Var &global);
};
#endif //c_if_hpp