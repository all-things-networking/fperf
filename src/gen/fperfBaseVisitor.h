
// Generated from fperf.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "fperfVisitor.h"


/**
 * This class provides an empty implementation of fperfVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  fperfBaseVisitor : public fperfVisitor {
public:

  virtual std::any visitCon(fperfParser::ConContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLhs(fperfParser::LhsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitM(fperfParser::MContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMm(fperfParser::MmContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitQ(fperfParser::QContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRhs(fperfParser::RhsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInterval(fperfParser::IntervalContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSet(fperfParser::SetContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitComp_op(fperfParser::Comp_opContext *ctx) override {
    return visitChildren(ctx);
  }


};

