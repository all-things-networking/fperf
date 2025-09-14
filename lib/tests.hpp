//
//  tests.hpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 12/05/22.
//  Copyright © 2022 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef tests_hpp
#define tests_hpp

#include "contention_point.hpp"
#include "query.hpp"
#include "shared_config.hpp"


#include <string>

using namespace std;

typedef void e2e_test_func_t(string, string);

void prio(int buf_size);

void rr(int buf_size);

void fq_codel(int buf_size, int rand_seed);

void loom_non_mem(int buf_size);

void loom_mem(int buf_size);

void leaf_spine_bw(int buf_size);

void tbf(string good_examples_file = "", string bad_examples_file = "");

void run(ContentionPoint* cp,
         IndexedExample* base_eg,
         unsigned int good_example_cnt,
         string good_examples_file,
         unsigned int bad_example_cnt,
         string bad_examples_file,
         Query& query,
         unsigned int max_spec,
         SharedConfig* config);

#endif
