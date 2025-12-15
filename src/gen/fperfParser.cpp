
// Generated from fperf.g4 by ANTLR 4.13.2


#include "fperfVisitor.h"

#include "fperfParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct FperfParserStaticData final {
  FperfParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  FperfParserStaticData(const FperfParserStaticData&) = delete;
  FperfParserStaticData(FperfParserStaticData&&) = delete;
  FperfParserStaticData& operator=(const FperfParserStaticData&) = delete;
  FperfParserStaticData& operator=(FperfParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag fperfParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
std::unique_ptr<FperfParserStaticData> fperfParserStaticData = nullptr;

void fperfParserInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (fperfParserStaticData != nullptr) {
    return;
  }
#else
  assert(fperfParserStaticData == nullptr);
#endif
  auto staticData = std::make_unique<FperfParserStaticData>(
    std::vector<std::string>{
      "con", "lhs", "m", "mm", "q", "rhs", "interval", "set", "comp_op"
    },
    std::vector<std::string>{
      "", "':'", "'('", "', t)'", "'SUM_[q in'", "']'", "'(q ,t)'", "'cenq'", 
      "'aipg'", "'ecmp'", "'dst'", "'t'", "'['", "','", "'{'", "'}'", "'>='", 
      "'<='", "'>'", "'<'", "'=='", "'!='"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "ID", "INT", "WS"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,24,79,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,7,
  	7,7,2,8,7,8,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,41,8,1,1,2,1,2,1,3,1,3,1,4,1,4,1,5,
  	1,5,1,5,1,5,3,5,53,8,5,1,6,1,6,1,6,1,6,1,6,1,6,1,7,1,7,1,7,1,7,5,7,65,
  	8,7,10,7,12,7,68,9,7,3,7,70,8,7,1,7,3,7,73,8,7,1,7,1,7,1,8,1,8,1,8,0,
  	0,9,0,2,4,6,8,10,12,14,16,0,3,1,0,7,8,1,0,9,10,1,0,16,21,76,0,18,1,0,
  	0,0,2,40,1,0,0,0,4,42,1,0,0,0,6,44,1,0,0,0,8,46,1,0,0,0,10,52,1,0,0,0,
  	12,54,1,0,0,0,14,60,1,0,0,0,16,76,1,0,0,0,18,19,3,12,6,0,19,20,5,1,0,
  	0,20,21,3,2,1,0,21,22,3,16,8,0,22,23,3,10,5,0,23,1,1,0,0,0,24,25,3,4,
  	2,0,25,26,5,2,0,0,26,27,3,8,4,0,27,28,5,3,0,0,28,41,1,0,0,0,29,30,3,6,
  	3,0,30,31,5,2,0,0,31,32,3,8,4,0,32,33,5,3,0,0,33,41,1,0,0,0,34,35,5,4,
  	0,0,35,36,3,14,7,0,36,37,5,5,0,0,37,38,3,4,2,0,38,39,5,6,0,0,39,41,1,
  	0,0,0,40,24,1,0,0,0,40,29,1,0,0,0,40,34,1,0,0,0,41,3,1,0,0,0,42,43,7,
  	0,0,0,43,5,1,0,0,0,44,45,7,1,0,0,45,7,1,0,0,0,46,47,5,23,0,0,47,9,1,0,
  	0,0,48,53,5,23,0,0,49,53,5,11,0,0,50,51,5,23,0,0,51,53,5,11,0,0,52,48,
  	1,0,0,0,52,49,1,0,0,0,52,50,1,0,0,0,53,11,1,0,0,0,54,55,5,12,0,0,55,56,
  	5,23,0,0,56,57,5,13,0,0,57,58,5,23,0,0,58,59,5,5,0,0,59,13,1,0,0,0,60,
  	69,5,14,0,0,61,66,5,23,0,0,62,63,5,13,0,0,63,65,5,23,0,0,64,62,1,0,0,
  	0,65,68,1,0,0,0,66,64,1,0,0,0,66,67,1,0,0,0,67,70,1,0,0,0,68,66,1,0,0,
  	0,69,61,1,0,0,0,69,70,1,0,0,0,70,72,1,0,0,0,71,73,5,13,0,0,72,71,1,0,
  	0,0,72,73,1,0,0,0,73,74,1,0,0,0,74,75,5,15,0,0,75,15,1,0,0,0,76,77,7,
  	2,0,0,77,17,1,0,0,0,5,40,52,66,69,72
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  fperfParserStaticData = std::move(staticData);
}

}

fperfParser::fperfParser(TokenStream *input) : fperfParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

fperfParser::fperfParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  fperfParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *fperfParserStaticData->atn, fperfParserStaticData->decisionToDFA, fperfParserStaticData->sharedContextCache, options);
}

fperfParser::~fperfParser() {
  delete _interpreter;
}

const atn::ATN& fperfParser::getATN() const {
  return *fperfParserStaticData->atn;
}

std::string fperfParser::getGrammarFileName() const {
  return "fperf.g4";
}

const std::vector<std::string>& fperfParser::getRuleNames() const {
  return fperfParserStaticData->ruleNames;
}

const dfa::Vocabulary& fperfParser::getVocabulary() const {
  return fperfParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView fperfParser::getSerializedATN() const {
  return fperfParserStaticData->serializedATN;
}


//----------------- ConContext ------------------------------------------------------------------

fperfParser::ConContext::ConContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

fperfParser::IntervalContext* fperfParser::ConContext::interval() {
  return getRuleContext<fperfParser::IntervalContext>(0);
}

fperfParser::LhsContext* fperfParser::ConContext::lhs() {
  return getRuleContext<fperfParser::LhsContext>(0);
}

fperfParser::Comp_opContext* fperfParser::ConContext::comp_op() {
  return getRuleContext<fperfParser::Comp_opContext>(0);
}

fperfParser::RhsContext* fperfParser::ConContext::rhs() {
  return getRuleContext<fperfParser::RhsContext>(0);
}


size_t fperfParser::ConContext::getRuleIndex() const {
  return fperfParser::RuleCon;
}


std::any fperfParser::ConContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<fperfVisitor*>(visitor))
    return parserVisitor->visitCon(this);
  else
    return visitor->visitChildren(this);
}

fperfParser::ConContext* fperfParser::con() {
  ConContext *_localctx = _tracker.createInstance<ConContext>(_ctx, getState());
  enterRule(_localctx, 0, fperfParser::RuleCon);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(18);
    interval();
    setState(19);
    match(fperfParser::T__0);
    setState(20);
    lhs();
    setState(21);
    comp_op();
    setState(22);
    rhs();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LhsContext ------------------------------------------------------------------

fperfParser::LhsContext::LhsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

fperfParser::MContext* fperfParser::LhsContext::m() {
  return getRuleContext<fperfParser::MContext>(0);
}

fperfParser::QContext* fperfParser::LhsContext::q() {
  return getRuleContext<fperfParser::QContext>(0);
}

fperfParser::MmContext* fperfParser::LhsContext::mm() {
  return getRuleContext<fperfParser::MmContext>(0);
}

fperfParser::SetContext* fperfParser::LhsContext::set() {
  return getRuleContext<fperfParser::SetContext>(0);
}


size_t fperfParser::LhsContext::getRuleIndex() const {
  return fperfParser::RuleLhs;
}


std::any fperfParser::LhsContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<fperfVisitor*>(visitor))
    return parserVisitor->visitLhs(this);
  else
    return visitor->visitChildren(this);
}

fperfParser::LhsContext* fperfParser::lhs() {
  LhsContext *_localctx = _tracker.createInstance<LhsContext>(_ctx, getState());
  enterRule(_localctx, 2, fperfParser::RuleLhs);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(40);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case fperfParser::T__6:
      case fperfParser::T__7: {
        enterOuterAlt(_localctx, 1);
        setState(24);
        m();
        setState(25);
        match(fperfParser::T__1);
        setState(26);
        q();
        setState(27);
        match(fperfParser::T__2);
        break;
      }

      case fperfParser::T__8:
      case fperfParser::T__9: {
        enterOuterAlt(_localctx, 2);
        setState(29);
        mm();
        setState(30);
        match(fperfParser::T__1);
        setState(31);
        q();
        setState(32);
        match(fperfParser::T__2);
        break;
      }

      case fperfParser::T__3: {
        enterOuterAlt(_localctx, 3);
        setState(34);
        match(fperfParser::T__3);
        setState(35);
        set();
        setState(36);
        match(fperfParser::T__4);
        setState(37);
        m();
        setState(38);
        match(fperfParser::T__5);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MContext ------------------------------------------------------------------

fperfParser::MContext::MContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t fperfParser::MContext::getRuleIndex() const {
  return fperfParser::RuleM;
}


std::any fperfParser::MContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<fperfVisitor*>(visitor))
    return parserVisitor->visitM(this);
  else
    return visitor->visitChildren(this);
}

fperfParser::MContext* fperfParser::m() {
  MContext *_localctx = _tracker.createInstance<MContext>(_ctx, getState());
  enterRule(_localctx, 4, fperfParser::RuleM);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(42);
    _la = _input->LA(1);
    if (!(_la == fperfParser::T__6

    || _la == fperfParser::T__7)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MmContext ------------------------------------------------------------------

fperfParser::MmContext::MmContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t fperfParser::MmContext::getRuleIndex() const {
  return fperfParser::RuleMm;
}


std::any fperfParser::MmContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<fperfVisitor*>(visitor))
    return parserVisitor->visitMm(this);
  else
    return visitor->visitChildren(this);
}

fperfParser::MmContext* fperfParser::mm() {
  MmContext *_localctx = _tracker.createInstance<MmContext>(_ctx, getState());
  enterRule(_localctx, 6, fperfParser::RuleMm);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(44);
    _la = _input->LA(1);
    if (!(_la == fperfParser::T__8

    || _la == fperfParser::T__9)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- QContext ------------------------------------------------------------------

fperfParser::QContext::QContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* fperfParser::QContext::INT() {
  return getToken(fperfParser::INT, 0);
}


size_t fperfParser::QContext::getRuleIndex() const {
  return fperfParser::RuleQ;
}


std::any fperfParser::QContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<fperfVisitor*>(visitor))
    return parserVisitor->visitQ(this);
  else
    return visitor->visitChildren(this);
}

fperfParser::QContext* fperfParser::q() {
  QContext *_localctx = _tracker.createInstance<QContext>(_ctx, getState());
  enterRule(_localctx, 8, fperfParser::RuleQ);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(46);
    match(fperfParser::INT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RhsContext ------------------------------------------------------------------

fperfParser::RhsContext::RhsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* fperfParser::RhsContext::INT() {
  return getToken(fperfParser::INT, 0);
}


size_t fperfParser::RhsContext::getRuleIndex() const {
  return fperfParser::RuleRhs;
}


std::any fperfParser::RhsContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<fperfVisitor*>(visitor))
    return parserVisitor->visitRhs(this);
  else
    return visitor->visitChildren(this);
}

fperfParser::RhsContext* fperfParser::rhs() {
  RhsContext *_localctx = _tracker.createInstance<RhsContext>(_ctx, getState());
  enterRule(_localctx, 10, fperfParser::RuleRhs);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(52);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 1, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(48);
      match(fperfParser::INT);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(49);
      match(fperfParser::T__10);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(50);
      match(fperfParser::INT);
      setState(51);
      match(fperfParser::T__10);
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IntervalContext ------------------------------------------------------------------

fperfParser::IntervalContext::IntervalContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> fperfParser::IntervalContext::INT() {
  return getTokens(fperfParser::INT);
}

tree::TerminalNode* fperfParser::IntervalContext::INT(size_t i) {
  return getToken(fperfParser::INT, i);
}


size_t fperfParser::IntervalContext::getRuleIndex() const {
  return fperfParser::RuleInterval;
}


std::any fperfParser::IntervalContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<fperfVisitor*>(visitor))
    return parserVisitor->visitInterval(this);
  else
    return visitor->visitChildren(this);
}

fperfParser::IntervalContext* fperfParser::interval() {
  IntervalContext *_localctx = _tracker.createInstance<IntervalContext>(_ctx, getState());
  enterRule(_localctx, 12, fperfParser::RuleInterval);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(54);
    match(fperfParser::T__11);
    setState(55);
    match(fperfParser::INT);
    setState(56);
    match(fperfParser::T__12);
    setState(57);
    match(fperfParser::INT);
    setState(58);
    match(fperfParser::T__4);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SetContext ------------------------------------------------------------------

fperfParser::SetContext::SetContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> fperfParser::SetContext::INT() {
  return getTokens(fperfParser::INT);
}

tree::TerminalNode* fperfParser::SetContext::INT(size_t i) {
  return getToken(fperfParser::INT, i);
}


size_t fperfParser::SetContext::getRuleIndex() const {
  return fperfParser::RuleSet;
}


std::any fperfParser::SetContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<fperfVisitor*>(visitor))
    return parserVisitor->visitSet(this);
  else
    return visitor->visitChildren(this);
}

fperfParser::SetContext* fperfParser::set() {
  SetContext *_localctx = _tracker.createInstance<SetContext>(_ctx, getState());
  enterRule(_localctx, 14, fperfParser::RuleSet);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(60);
    match(fperfParser::T__13);
    setState(69);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == fperfParser::INT) {
      setState(61);
      match(fperfParser::INT);
      setState(66);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 2, _ctx);
      while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
        if (alt == 1) {
          setState(62);
          match(fperfParser::T__12);
          setState(63);
          match(fperfParser::INT); 
        }
        setState(68);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 2, _ctx);
      }
    }
    setState(72);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == fperfParser::T__12) {
      setState(71);
      match(fperfParser::T__12);
    }
    setState(74);
    match(fperfParser::T__14);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Comp_opContext ------------------------------------------------------------------

fperfParser::Comp_opContext::Comp_opContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t fperfParser::Comp_opContext::getRuleIndex() const {
  return fperfParser::RuleComp_op;
}


std::any fperfParser::Comp_opContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<fperfVisitor*>(visitor))
    return parserVisitor->visitComp_op(this);
  else
    return visitor->visitChildren(this);
}

fperfParser::Comp_opContext* fperfParser::comp_op() {
  Comp_opContext *_localctx = _tracker.createInstance<Comp_opContext>(_ctx, getState());
  enterRule(_localctx, 16, fperfParser::RuleComp_op);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(76);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 4128768) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

void fperfParser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  fperfParserInitialize();
#else
  ::antlr4::internal::call_once(fperfParserOnceFlag, fperfParserInitialize);
#endif
}
