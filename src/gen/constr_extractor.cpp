#pragma once

#include "constr_extractor.hpp"
#include "fperfBaseVisitor.h"
#include <format>
#include <iostream>

#include "workload.hpp"

using namespace std;

ConstrExtractor::ConstrExtractor(Workload* wl): wl(wl) {
}

Op ConstrExtractor::get_op() {
    if (op == "<") return Op(Op::Type::LT);
    if (op == "<=") return Op(Op::Type::LE);
    if (op == ">") return Op(Op::Type::GT);
    if (op == ">=") return Op(Op::Type::GE);
    if (op == "==") return Op(Op::Type::EQ);
    throw std::invalid_argument("Unknown comparison operator: " + op);
}

Expr* ConstrExtractor::get_rhs() {
    if (rhs_linear) return new Time(rhs);
    return new Constant(rhs);
}

metric_t ConstrExtractor::get_metric() {
    if (metric == "cenq") return metric_t::CENQ;
    if (metric == "aipg") return metric_t::AIPG;
    throw std::invalid_argument("Unknown metric: " + metric);
}

MExpr* ConstrExtractor::get_lhs() {
    metric_t metric_type = get_metric();
    if (tmp_ids.size() == 1)
        return new Indiv(metric_type, tmp_ids[0]);
    else {
        return new QSum(qset_t(tmp_ids.begin(), tmp_ids.end()), metric_type);
    }
}

void ConstrExtractor::parse_cenq() {
}

any ConstrExtractor::visitCon(fperfParser::ConContext* ctx) {
    auto result = visitChildren(ctx);
    MExpr* wl_lhs = get_lhs();
    Op wl_op = get_op();
    Expr* wl_rhs = get_rhs();
    time_range_t time_range(begin - 1, end - 1);
    wl->add_spec(TimedSpec(new Comp(wl_lhs, wl_op, wl_rhs), time_range, wl->get_total_time()));
    return result;
}

any ConstrExtractor::visitLhs(fperfParser::LhsContext* ctx) {
    return visitChildren(ctx);
}

any ConstrExtractor::visitM(fperfParser::MContext* ctx) {
    metric = ctx->getText();
    return visitChildren(ctx);
}

any ConstrExtractor::visitMm(fperfParser::MmContext* ctx) {
    metric = ctx->getText();
    return visitChildren(ctx);
}

any ConstrExtractor::visitQ(fperfParser::QContext* ctx) {
    int q = stoi(ctx->INT()->getText());
    tmp_ids.push_back(q);
    return visitChildren(ctx);
}

any ConstrExtractor::visitRhs(fperfParser::RhsContext* ctx) {
    rhs_linear = false;
    if (ctx->INT() && ctx->children.size() == 1)
        rhs = std::stoull(ctx->INT()->getText());
    else if (ctx->getText() == "t") {
        rhs = 1;
        rhs_linear = true;
    } else if (ctx->INT() && ctx->children.size() == 2) {
        rhs = std::stoull(ctx->INT()->getText());
        rhs_linear = true;
    }
    return visitChildren(ctx);
}

any ConstrExtractor::visitInterval(fperfParser::IntervalContext* ctx) {
    begin = stoi(ctx->INT(0)->getText());
    end = stoi(ctx->INT(1)->getText());
    return visitChildren(ctx);
}

any ConstrExtractor::visitSet(fperfParser::SetContext* ctx) {
    for (auto intNode : ctx->INT()) {
        tmp_ids.push_back(stoi(intNode->getText()));
    }
    return visitChildren(ctx);
}

any ConstrExtractor::visitComp_op(fperfParser::Comp_opContext* ctx) {
    op = ctx->getText();
    return visitChildren(ctx);
}
