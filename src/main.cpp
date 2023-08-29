//
//  main.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 3/2/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#include <iostream>
#include <map>
#include <vector>

#include "tests.hpp"
#include "util.hpp"

#ifdef DEBUG
bool debug = true;
#else
bool debug = false;
#endif

using namespace std;

map<string, e2e_test_func_t*> e2e_tests = {{"prio", prio},
                                           {"rr", rr},
                                           {"fq_codel", fq_codel},
                                           {"loom", loom},
                                           {"leaf_spine_bw", leaf_spine_bw},
                                           {"tbf", tbf}};

const string help_message = "Usage: ./fperf TEST_NAME";

int main(int argc, const char* argv[]) {
    vector<string> arguments(argv + 1, argv + argc);

    if (arguments.size() != 1) throw invalid_argument("Invalid number of arguments");

    if (arguments[0] == "--help") {
        cout << help_message << endl;
        return 0;
    }

    string test_name = arguments[0];
    if (e2e_tests.find(test_name) == e2e_tests.end())
        throw invalid_argument("Unknown test: " + test_name);

    e2e_tests.find(test_name)->second("", "");

    return 0;
}
