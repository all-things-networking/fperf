//
//  params.hpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 12/5/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef params_hpp
#define params_hpp

#include <vector>

using namespace std;

const unsigned int TOTAL_TIME = 10;
const unsigned int MAX_ENQ = 4;
const unsigned int MAX_QUEUE_SIZE = 10;

const unsigned int UNDEFINED_METRIC_VAL = 31;

const unsigned int GOOD_EXAMPLE_WEIGHT = 1;
const unsigned int BAD_EXAMPLE_WEIGHT = 2;
const unsigned int EXAMPLE_WEIGHT_IN_COST = 10;
const unsigned int SPEC_CNT_WEIGHT = 2;
const unsigned int NUM_QUEUES_WITH_SPECS_WEIGHT = 2;
const unsigned int MAX_SPEC_CNT_PER_QUEUE_WEIGHT = 1;
const unsigned int AVG_SPEC_CNT_OVER_QUEUES = 1;
const unsigned int AVG_TIME_RANGE_OVER_SPECS_WEIGHT = 1;

const unsigned int Z3_RANDOM_SEED = 100;
const unsigned int MAX_SPEC = 6;
const unsigned int LOCAL_SEARCH_THRESH = 10;
const unsigned int RESET_THRESH_SLOW_PROGRESS = 20;
const unsigned int RESET_THRESH_BACKTRACK = 10;
const unsigned int LOCAL_SEARCH_MAX_HOPS = 3;
//TODO: set it in terms of other parameters?
//const unsigned int NEGLIGIBLE_PROGRESS = (5 * (BAD_EXAMPLE_WEIGHT) * EXAMPLE_WEIGHT_IN_COST);
const double NEGLIGIBLE_PROGRESS_FRAC = 0.003;
const unsigned int MIN_CANDIDATES = 5;
const unsigned int MAX_CANDIDATES = 1000;
extern unsigned long MAX_COST;
const double COST_LAMBDA = 1.0;
const unsigned int TIME_RANGE_NEI_CNT = 5;
const unsigned int RANDOM_ADD_CNT = 5;
const unsigned int SOLUTION_REFINEMENT_MAX_ROUNDS = 0;


const double EG_RANDOM_MOD_PERCENT = 0.2;
const unsigned int EG_RANDOM_MOD_START = 10;
const unsigned int EG_RANDOM_MOD_MAX_TRIES = 2;


const vector<double> DEFAULT_RHS_SELECTION_WEIGHTS{1, 1, 1};
const unsigned int DEFAULT_RHS_CONST_MIN = 0;
const unsigned int DEFAULT_RHS_CONST_MAX = 3;
const unsigned int DEFAULT_RHS_TIME_COEFF_MIN = 1;
// TODO: if this is too large, it could cause inputs that flood the queues
const unsigned int DEFAULT_RHS_TIME_COEFF_MAX = TOTAL_TIME;

const vector<double> DEFAULT_TRF_SELECTION_WEIGHTS{1, 1};
const vector<double> DEFAULT_WL_METRIC_SELECTION_WEIGHTS{1, 1, 1, 1};

const unsigned int DEFAULT_COMP_RANGE_MIN = 0;
const unsigned int DEFAULT_COMP_RANGE_MAX = 4;

const unsigned int DEFAULT_ENQ_RANGE_MIN = 1;
const unsigned int DEFAULT_ENQ_RANGE_MAX = MAX_ENQ;

const unsigned int DEFAULT_RANDOM_SEED = 1680979592;

#endif /* params_hpp */
