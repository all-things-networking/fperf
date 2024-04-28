// Expression generator pseudocode

// Abstract Visitor class
class Visitor {
public:
    virtual void visit(Workload& workload) = 0;
    virtual void visit(TimedSpec& timedSpec) = 0;
    virtual void visit(WlSpec& wlSpec) = 0;
    virtual void visit(Comp& comp) = 0;
    virtual void visit(MExpr& mExpr) = 0;
    virtual void visit(Rhs& rhs) = 0;
    virtual void visit(Op& op) = 0;
    // Additional visit methods for other concrete elements if necessary
    virtual ~Visitor() = default;
};

// Accept methods
class Workload {
    // Existing methods...
public:
    void accept(Visitor& visitor) {
        for(auto& timedSpec : timedSpecs) {
            timedSpec.accept(visitor); // Visitor will store the expression in the context
        }
        visitor.visit(*this); // Let the visitor process the Workload
    }
};

class TimedSpec {
    // Existing methods...
public:
    void accept(Visitor& visitor) {
        wlSpec.accept(visitor);
        visitor.visit(*this); // Let the visitor process the TimedSpec
    }
};

class WlSpec {
    // Existing methods...
public:
    // Virtual accept method
    virtual void accept(Visitor& visitor) = 0;
};

class Comp {
    // Existing methods...
public:
    void accept(Visitor& visitor) {
        // Visitor should store the lhs, op, and rhs expressions
        lhs->accept(visitor);
        rhs->accept(visitor);
        op->accept(visitor);
        visitor.visit(*this); // Let the visitor process the Comp
    }
};

// For MExpr, Rhs, and Op: leaf nodes, so accept method only needs to call visitor.visit(*this)
// There are no sub-elements to visit.

class MExpr { // lhs
    // Existing methods...
public:
    void accept(Visitor& visitor) {
        visitor.visit(*this); // Let the visitor process the MExpr
    }
};

class Rhs {
    // Existing methods...
public:
    void accept(Visitor& visitor) {
        visitor.visit(*this); // Let the visitor process the Rhs
    }
};

class Op {
    // Existing methods...
public:
    void accept(Visitor& visitor) {
        visitor.visit(*this); // Let the visitor process the Op
    }
};

class ExpressionGeneratorVisitor : public Visitor {
private:
    // Context for storing intermediate results
    // such as lhs, op, rhs, and a vector of TimedSpecs.
    std::vector<expr> timedSpecsContext;

    // Store function pointers t --> expr for lhs, rhs, and op
    std::function<expr(unsigned int)> lhs_expr_func;
    std::function<expr(unsigned int)> rhs_expr_func;
    std::function<expr(unsigned int)> op_expr_func;

    // Store t --> expr for WlSpec
    std::function<expr(unsigned int)> wlSpec_expr_func;

public:
    void visit(Workload& workload) override {
        // Assume TimedSpecsContext is filled with TimedSpecs expressions
        combineExpressions(timedSpecsContext);
    }

    void visit(TimedSpec& timedSpec) override {
        // Loop through time range
        time_range_t time_range = timedSpec.get_time_range();
        for(unsigned int t = time_range.first; t <= time_range.second; t++) {
            expr expression = wlSpec_expr_func(t);
            timedSpecsContext.push_back(expression);
        }
    }

    void visit(Comp& comp) override {
        wlSpec_expr_func = [this](unsigned int t) {
            // Use lhs_expr_func, rhs_expr_func, and op_expr_func to generate expression for the whole COMP
            expr lhs_expr = lhs_expr_func(t);
            expr rhs_expr = rhs_expr_func(t);
            expr op_expr = op_expr_func(t);
            return combineExpressions(lhs_expr, rhs_expr, op_expr);
        };
    }

    void visit(MExpr& mExpr) override {
        // Create t --> lhs expression function pointer and store it in the context.
        lhs_expr_func = [this](unsigned int t) {
            return generate_expression(mExpr.get_lhs(), t);
        };
    }

    void visit(Rhs& rhs) override {
        // Create t --> rhs expression function pointer and store it in the context.
        rhs_expr_func = [this](unsigned int t) {
            return generate_expression(rhs, t);
        };
    }

    void visit(Op& op) override {
        // Create t --> op expression function pointer and store it in the context.
        op_expr_func = [this](unsigned int t) {
            return generate_expression(op, t);
        };
    }

    // Utility methods to generate and retrieve the final expression.
    // get_expr, combineExpressions, generate_expression, etc.
};
