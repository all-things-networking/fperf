
// Generated from fperf.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "fperfListener.h"


/**
 * This class provides an empty implementation of fperfListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  fperfBaseListener : public fperfListener {
public:

  virtual void enterCon(fperfParser::ConContext * /*ctx*/) override { }
  virtual void exitCon(fperfParser::ConContext * /*ctx*/) override { }

  virtual void enterLhs(fperfParser::LhsContext * /*ctx*/) override { }
  virtual void exitLhs(fperfParser::LhsContext * /*ctx*/) override { }

  virtual void enterM(fperfParser::MContext * /*ctx*/) override { }
  virtual void exitM(fperfParser::MContext * /*ctx*/) override { }

  virtual void enterQ(fperfParser::QContext * /*ctx*/) override { }
  virtual void exitQ(fperfParser::QContext * /*ctx*/) override { }

  virtual void enterRhs(fperfParser::RhsContext * /*ctx*/) override { }
  virtual void exitRhs(fperfParser::RhsContext * /*ctx*/) override { }

  virtual void enterInterval(fperfParser::IntervalContext * /*ctx*/) override { }
  virtual void exitInterval(fperfParser::IntervalContext * /*ctx*/) override { }

  virtual void enterSet(fperfParser::SetContext * /*ctx*/) override { }
  virtual void exitSet(fperfParser::SetContext * /*ctx*/) override { }

  virtual void enterComp_op(fperfParser::Comp_opContext * /*ctx*/) override { }
  virtual void exitComp_op(fperfParser::Comp_opContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

