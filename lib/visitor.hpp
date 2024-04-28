//
//  visitor.hpp
//  FPerf
//
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef visitor_hpp
#define visitor_hpp

#include <vector>
#include <stack>

#include "workload.hpp"

class Visitor {
public:
    virtual void visit(Workload& workload) = 0;
    virtual void visit(TimedSpec& timedSpec) = 0;

    virtual void visit(Same& same) = 0;
    virtual void visit(Unique& uniq) = 0;
    virtual void visit(Incr& incr) = 0;
    virtual void visit(Decr& decr) = 0;
    virtual void visit(Comp& comp) = 0;

    virtual void visit(Constant& c) = 0;
    virtual void visit(Time& time) = 0;
    virtual void visit(Indiv& indiv) = 0;
    virtual void visit(QSum& qsum) = 0;
    virtual void visit(Op& op) = 0;

    void set_time_range(time_range_t time_range);

protected:
    time_range_t time_range; // Context for passing time range information down AST (from Timedspec downwards)

};

class ExpressionGeneratorVisitor : public Visitor {
public:

    // Constructor
    explicit ExpressionGeneratorVisitor(NetContext& net_ctx, const vector<Queue*>& in_queues);

    void visit(Workload& workload) override;
    void visit(TimedSpec& timedSpec) override;

    void visit(Same& same) override;
    void visit(Unique& uniq) override;
    void visit(Incr& incr) override;
    void visit(Decr& decr) override;
    void visit(Comp& comp) override;

    void visit(Constant& c) override;
    void visit(Time& time) override;
    void visit(Indiv& indiv) override;
    void visit(QSum& qsum) override;
    void visit(Op& op) override;

    expr get_workload_expr();

private:
    NetContext& net_ctx; // Store the net context for generating expressions
    const vector<Queue*>& in_queues; // Store the in queues for generating expressions

    expr workload_expr; // Context for storing the final expression
    std::stack<expr> timedSpecs_exprs; // Context for storing timedSpec expressions

    expr wlspec_expr; // Optional context for storing non-Comp WlSpec expressions
    std::vector<m_val_expr_t> lhs_exprs; // Context for storing lhs expressions
    std::vector<m_val_expr_t> rhs_exprs; // Context for storing rhs expressions
    std::vector<Op> operations; // Context for storing operations

    // Methods to generate expressions
    // TODO: Might not need all the get_expr methods
    expr get_expr(TimedSpec tspec);
    expr get_expr(Unique uniq, time_range_t time_range);
    expr get_expr(Same same, time_range_t time_range);
    expr get_expr(Incr incr, time_range_t time_range);
    expr get_expr(Decr decr, time_range_t time_range);
    expr get_expr(Comp comp, time_range_t time_range);
    expr get_expr(Comp comp, unsigned int t);
    m_val_expr_t get_expr(Expr* rhs, unsigned int t);
    m_val_expr_t get_expr(MExpr* lhs, unsigned int t);
    m_val_expr_t get_expr(Constant c, unsigned int t);
    m_val_expr_t get_expr(Time time, unsigned int t);
    m_val_expr_t get_expr(Indiv indiv, unsigned int t);
    m_val_expr_t get_expr(QSum qsum, unsigned int t);
};

#endif /* visitor_hpp */