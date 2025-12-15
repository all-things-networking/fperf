#pragma once

#include "fperfBaseVisitor.h"
#include <iostream>

class ASTTypeVisitor : public fperfBaseVisitor {
public:
    std::any visitCon(fperfParser::ConContext *ctx) override {
        std::cout << "Node Type: Constraint" << std::endl;
        return visitChildren(ctx);
    }

    std::any visitLhs(fperfParser::LhsContext *ctx) override {
        std::cout << "Node Type: Left Hand Side" << std::endl;
        return visitChildren(ctx);
    }

    std::any visitM(fperfParser::MContext *ctx) override {
        std::cout << "Node Type: Measure" << std::endl;
        return visitChildren(ctx);
    }

    std::any visitQ(fperfParser::QContext *ctx) override {
        std::cout << "Node Type: Query ID" << std::endl;
        return visitChildren(ctx);
    }

    std::any visitRhs(fperfParser::RhsContext *ctx) override {
        std::cout << "Node Type: Right Hand Side" << std::endl;
        return visitChildren(ctx);
    }

    std::any visitInterval(fperfParser::IntervalContext *ctx) override {
        std::cout << "Node Type: Interval" << std::endl;
        return visitChildren(ctx);
    }

    std::any visitSet(fperfParser::SetContext *ctx) override {
        std::cout << "Node Type: Set" << std::endl;
        return visitChildren(ctx);
    }

    std::any visitComp_op(fperfParser::Comp_opContext *ctx) override {
        std::cout << "Node Type: Comparison Operator" << std::endl;
        return visitChildren(ctx);
    }
};
