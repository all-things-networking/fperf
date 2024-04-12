#ifndef c_move_hpp
#define c_move_hpp

#include "statement.hpp"

class C_move : public Statement {
private:
    string in_q_var = "";
    int num_pkt = 1;
    int out_q = 0;

public:
    C_move(string in_q_var, int num_pkt = 1, int out_q = 0);  // add output queue
    ~C_move();
    expr generate_constr(expr prev_cond,
                         NetContext& net_ctx,
                         map<string, expr>& constr_map,
                         Global_Var &global);
};
#endif //c_move_hpp