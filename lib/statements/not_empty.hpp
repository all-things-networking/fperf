#ifndef not_empty_hpp
#define not_empty_hpp

#include "statement.hpp"

class Not_Empty : public Statement {
private:
    string q_var = "";
public:
    Not_Empty(string q_var);
    ~Not_Empty();
    expr generate_constr(expr prev_cond,
                         NetContext& net_ctx,
                         map<string, expr>& constr_map,
                         Global_Var &global);
};
#endif //not_empty_hpp
