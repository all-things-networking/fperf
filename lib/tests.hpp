//
//  tests.hpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 12/05/22.
//  Copyright © 2022 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef tests_hpp
#define tests_hpp

#include <string>

typedef void cp_test_func_t(std::string, std::string);

void prio(std::string good_examples_file = "",
          std::string bad_examples_file = "");

void rr(std::string good_examples_file = "",
        std::string bad_examples_file = "");

void fq_codel(std::string good_examples_file = "",
              std::string bad_examples_file = "");

void loom(std::string good_examples_file = "",
          std::string bad_examples_file = "");

void leaf_spine_bw(std::string good_examples_file = "",
                   std::string bad_examples_file = "");

void tbf(std::string good_examples_file = "",
         std::string bad_examples_file = "");

#endif /* tests_hpp */
