//
//  tests.hpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 12/05/22.
//  Copyright © 2022 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef tests_hpp
#define tests_hpp

#include <string>

using namespace std;

typedef void e2e_test_func_t(string, string);

void prio(string good_examples_file = "", string bad_examples_file = "");

void rr(string good_examples_file = "", string bad_examples_file = "");

void fq_codel(string good_examples_file = "", string bad_examples_file = "");

void loom(string good_examples_file = "", string bad_examples_file = "");

void leaf_spine_bw(string good_examples_file = "", string bad_examples_file = "");

void tbf(string good_examples_file = "", string bad_examples_file = "");
#endif /* tests_hpp */
