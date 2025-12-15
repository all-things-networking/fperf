
// Generated from fperf.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "fperfParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by fperfParser.
 */
class  fperfVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by fperfParser.
   */
    virtual std::any visitCon(fperfParser::ConContext *context) = 0;

    virtual std::any visitLhs(fperfParser::LhsContext *context) = 0;

    virtual std::any visitM(fperfParser::MContext *context) = 0;

    virtual std::any visitMm(fperfParser::MmContext *context) = 0;

    virtual std::any visitQ(fperfParser::QContext *context) = 0;

    virtual std::any visitRhs(fperfParser::RhsContext *context) = 0;

    virtual std::any visitInterval(fperfParser::IntervalContext *context) = 0;

    virtual std::any visitSet(fperfParser::SetContext *context) = 0;

    virtual std::any visitComp_op(fperfParser::Comp_opContext *context) = 0;


};

