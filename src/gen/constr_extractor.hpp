#pragma once

#include "fperfBaseVisitor.h"
#include "workload.hpp"

using namespace std;


class ConstrExtractor : public fperfBaseVisitor {
    string metric;
    string op;
    int begin;
    int end;
    vector<int> tmp_ids;
    bool rhs_linear;
    int rhs;
    Workload* wl;


public:
    ConstrExtractor(Workload* wl);
    Op get_op();
    Expr* get_rhs();
    metric_t get_metric();
    MExpr* get_lhs();
    void parse_cenq();

    any visitCon(fperfParser::ConContext* ctx) override;

    any visitLhs(fperfParser::LhsContext* ctx) override;

    any visitM(fperfParser::MContext* ctx) override;

    any visitMm(fperfParser::MmContext* ctx) override;

    any visitQ(fperfParser::QContext* ctx) override;

    any visitRhs(fperfParser::RhsContext* ctx) override;

    any visitInterval(fperfParser::IntervalContext* ctx) override;

    any visitSet(fperfParser::SetContext* ctx) override;

    any visitComp_op(fperfParser::Comp_opContext* ctx) override;
};
