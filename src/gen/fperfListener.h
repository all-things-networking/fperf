
// Generated from fperf.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "fperfParser.h"


/**
 * This interface defines an abstract listener for a parse tree produced by fperfParser.
 */
class  fperfListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterCon(fperfParser::ConContext *ctx) = 0;
  virtual void exitCon(fperfParser::ConContext *ctx) = 0;

  virtual void enterLhs(fperfParser::LhsContext *ctx) = 0;
  virtual void exitLhs(fperfParser::LhsContext *ctx) = 0;

  virtual void enterM(fperfParser::MContext *ctx) = 0;
  virtual void exitM(fperfParser::MContext *ctx) = 0;

  virtual void enterQ(fperfParser::QContext *ctx) = 0;
  virtual void exitQ(fperfParser::QContext *ctx) = 0;

  virtual void enterRhs(fperfParser::RhsContext *ctx) = 0;
  virtual void exitRhs(fperfParser::RhsContext *ctx) = 0;

  virtual void enterInterval(fperfParser::IntervalContext *ctx) = 0;
  virtual void exitInterval(fperfParser::IntervalContext *ctx) = 0;

  virtual void enterSet(fperfParser::SetContext *ctx) = 0;
  virtual void exitSet(fperfParser::SetContext *ctx) = 0;

  virtual void enterComp_op(fperfParser::Comp_opContext *ctx) = 0;
  virtual void exitComp_op(fperfParser::Comp_opContext *ctx) = 0;


};

