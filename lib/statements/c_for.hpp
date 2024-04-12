#ifndef c_for_hpp
#define c_for_hpp

#include "statement.hpp"

class C_for : public Statement {
private:
    string idx_var = "";
    std::pair<int, int> range;
    vector<Statement*> &instrs;

public:
    C_for(string idx_var, std::pair<int, int> range, vector<Statement*> &instrs);
    ~C_for();
    expr generate_constr(expr prev_cond,
                         NetContext& net_ctx,
                         map<string, expr>& constr_map,
                         Global_Var &global);
};
#endif //c_for_hpp
