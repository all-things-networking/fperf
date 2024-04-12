#ifndef is_equal_hpp
#define is_equal_hpp

#include "statement.hpp"

class Is_Equal : public Statement {
private:
    // RHS and LHS can only be name of a global variable
    string var_name_1 = "";
    string var_name_2 = "";

public:
    Is_Equal(string var_name_1, string var_name_2);
    ~Is_Equal();
    expr generate_constr(expr prev_cond,
                         NetContext& net_ctx,
                         map<string, expr>& constr_map,
                         Global_Var &global);
};

#endif //is_equal_hpp
