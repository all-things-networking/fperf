
// Generated from fperf.g4 by ANTLR 4.13.2


#include "fperfLexer.h"


using namespace antlr4;



using namespace antlr4;

namespace {

struct FperfLexerStaticData final {
  FperfLexerStaticData(std::vector<std::string> ruleNames,
                          std::vector<std::string> channelNames,
                          std::vector<std::string> modeNames,
                          std::vector<std::string> literalNames,
                          std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), channelNames(std::move(channelNames)),
        modeNames(std::move(modeNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  FperfLexerStaticData(const FperfLexerStaticData&) = delete;
  FperfLexerStaticData(FperfLexerStaticData&&) = delete;
  FperfLexerStaticData& operator=(const FperfLexerStaticData&) = delete;
  FperfLexerStaticData& operator=(FperfLexerStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> channelNames;
  const std::vector<std::string> modeNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag fperflexerLexerOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
std::unique_ptr<FperfLexerStaticData> fperflexerLexerStaticData = nullptr;

void fperflexerLexerInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (fperflexerLexerStaticData != nullptr) {
    return;
  }
#else
  assert(fperflexerLexerStaticData == nullptr);
#endif
  auto staticData = std::make_unique<FperfLexerStaticData>(
    std::vector<std::string>{
      "T__0", "T__1", "T__2", "T__3", "T__4", "T__5", "T__6", "T__7", "T__8", 
      "T__9", "T__10", "T__11", "T__12", "T__13", "T__14", "T__15", "T__16", 
      "T__17", "T__18", "T__19", "T__20", "ID", "INT", "WS"
    },
    std::vector<std::string>{
      "DEFAULT_TOKEN_CHANNEL", "HIDDEN"
    },
    std::vector<std::string>{
      "DEFAULT_MODE"
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
  	4,0,24,141,6,-1,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,
  	6,2,7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,
  	7,14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,
  	7,21,2,22,7,22,2,23,7,23,1,0,1,0,1,1,1,1,1,2,1,2,1,2,1,2,1,2,1,3,1,3,
  	1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,4,1,4,1,5,1,5,1,5,1,5,1,5,1,5,1,5,1,
  	6,1,6,1,6,1,6,1,6,1,7,1,7,1,7,1,7,1,7,1,8,1,8,1,8,1,8,1,8,1,9,1,9,1,9,
  	1,9,1,10,1,10,1,11,1,11,1,12,1,12,1,13,1,13,1,14,1,14,1,15,1,15,1,15,
  	1,16,1,16,1,16,1,17,1,17,1,18,1,18,1,19,1,19,1,19,1,20,1,20,1,20,1,21,
  	1,21,5,21,125,8,21,10,21,12,21,128,9,21,1,22,4,22,131,8,22,11,22,12,22,
  	132,1,23,4,23,136,8,23,11,23,12,23,137,1,23,1,23,0,0,24,1,1,3,2,5,3,7,
  	4,9,5,11,6,13,7,15,8,17,9,19,10,21,11,23,12,25,13,27,14,29,15,31,16,33,
  	17,35,18,37,19,39,20,41,21,43,22,45,23,47,24,1,0,4,3,0,65,90,95,95,97,
  	122,4,0,48,57,65,90,95,95,97,122,1,0,48,57,3,0,9,10,13,13,32,32,143,0,
  	1,1,0,0,0,0,3,1,0,0,0,0,5,1,0,0,0,0,7,1,0,0,0,0,9,1,0,0,0,0,11,1,0,0,
  	0,0,13,1,0,0,0,0,15,1,0,0,0,0,17,1,0,0,0,0,19,1,0,0,0,0,21,1,0,0,0,0,
  	23,1,0,0,0,0,25,1,0,0,0,0,27,1,0,0,0,0,29,1,0,0,0,0,31,1,0,0,0,0,33,1,
  	0,0,0,0,35,1,0,0,0,0,37,1,0,0,0,0,39,1,0,0,0,0,41,1,0,0,0,0,43,1,0,0,
  	0,0,45,1,0,0,0,0,47,1,0,0,0,1,49,1,0,0,0,3,51,1,0,0,0,5,53,1,0,0,0,7,
  	58,1,0,0,0,9,68,1,0,0,0,11,70,1,0,0,0,13,77,1,0,0,0,15,82,1,0,0,0,17,
  	87,1,0,0,0,19,92,1,0,0,0,21,96,1,0,0,0,23,98,1,0,0,0,25,100,1,0,0,0,27,
  	102,1,0,0,0,29,104,1,0,0,0,31,106,1,0,0,0,33,109,1,0,0,0,35,112,1,0,0,
  	0,37,114,1,0,0,0,39,116,1,0,0,0,41,119,1,0,0,0,43,122,1,0,0,0,45,130,
  	1,0,0,0,47,135,1,0,0,0,49,50,5,58,0,0,50,2,1,0,0,0,51,52,5,40,0,0,52,
  	4,1,0,0,0,53,54,5,44,0,0,54,55,5,32,0,0,55,56,5,116,0,0,56,57,5,41,0,
  	0,57,6,1,0,0,0,58,59,5,83,0,0,59,60,5,85,0,0,60,61,5,77,0,0,61,62,5,95,
  	0,0,62,63,5,91,0,0,63,64,5,113,0,0,64,65,5,32,0,0,65,66,5,105,0,0,66,
  	67,5,110,0,0,67,8,1,0,0,0,68,69,5,93,0,0,69,10,1,0,0,0,70,71,5,40,0,0,
  	71,72,5,113,0,0,72,73,5,32,0,0,73,74,5,44,0,0,74,75,5,116,0,0,75,76,5,
  	41,0,0,76,12,1,0,0,0,77,78,5,99,0,0,78,79,5,101,0,0,79,80,5,110,0,0,80,
  	81,5,113,0,0,81,14,1,0,0,0,82,83,5,97,0,0,83,84,5,105,0,0,84,85,5,112,
  	0,0,85,86,5,103,0,0,86,16,1,0,0,0,87,88,5,101,0,0,88,89,5,99,0,0,89,90,
  	5,109,0,0,90,91,5,112,0,0,91,18,1,0,0,0,92,93,5,100,0,0,93,94,5,115,0,
  	0,94,95,5,116,0,0,95,20,1,0,0,0,96,97,5,116,0,0,97,22,1,0,0,0,98,99,5,
  	91,0,0,99,24,1,0,0,0,100,101,5,44,0,0,101,26,1,0,0,0,102,103,5,123,0,
  	0,103,28,1,0,0,0,104,105,5,125,0,0,105,30,1,0,0,0,106,107,5,62,0,0,107,
  	108,5,61,0,0,108,32,1,0,0,0,109,110,5,60,0,0,110,111,5,61,0,0,111,34,
  	1,0,0,0,112,113,5,62,0,0,113,36,1,0,0,0,114,115,5,60,0,0,115,38,1,0,0,
  	0,116,117,5,61,0,0,117,118,5,61,0,0,118,40,1,0,0,0,119,120,5,33,0,0,120,
  	121,5,61,0,0,121,42,1,0,0,0,122,126,7,0,0,0,123,125,7,1,0,0,124,123,1,
  	0,0,0,125,128,1,0,0,0,126,124,1,0,0,0,126,127,1,0,0,0,127,44,1,0,0,0,
  	128,126,1,0,0,0,129,131,7,2,0,0,130,129,1,0,0,0,131,132,1,0,0,0,132,130,
  	1,0,0,0,132,133,1,0,0,0,133,46,1,0,0,0,134,136,7,3,0,0,135,134,1,0,0,
  	0,136,137,1,0,0,0,137,135,1,0,0,0,137,138,1,0,0,0,138,139,1,0,0,0,139,
  	140,6,23,0,0,140,48,1,0,0,0,4,0,126,132,137,1,6,0,0
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  fperflexerLexerStaticData = std::move(staticData);
}

}

fperfLexer::fperfLexer(CharStream *input) : Lexer(input) {
  fperfLexer::initialize();
  _interpreter = new atn::LexerATNSimulator(this, *fperflexerLexerStaticData->atn, fperflexerLexerStaticData->decisionToDFA, fperflexerLexerStaticData->sharedContextCache);
}

fperfLexer::~fperfLexer() {
  delete _interpreter;
}

std::string fperfLexer::getGrammarFileName() const {
  return "fperf.g4";
}

const std::vector<std::string>& fperfLexer::getRuleNames() const {
  return fperflexerLexerStaticData->ruleNames;
}

const std::vector<std::string>& fperfLexer::getChannelNames() const {
  return fperflexerLexerStaticData->channelNames;
}

const std::vector<std::string>& fperfLexer::getModeNames() const {
  return fperflexerLexerStaticData->modeNames;
}

const dfa::Vocabulary& fperfLexer::getVocabulary() const {
  return fperflexerLexerStaticData->vocabulary;
}

antlr4::atn::SerializedATNView fperfLexer::getSerializedATN() const {
  return fperflexerLexerStaticData->serializedATN;
}

const atn::ATN& fperfLexer::getATN() const {
  return *fperflexerLexerStaticData->atn;
}




void fperfLexer::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  fperflexerLexerInitialize();
#else
  ::antlr4::internal::call_once(fperflexerLexerOnceFlag, fperflexerLexerInitialize);
#endif
}
