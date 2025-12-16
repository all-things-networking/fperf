//
//  main.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 3/2/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//


#include "priority_scheduler.hpp"
#include "tests.hpp"
#include "wl_checks.hpp"


#include <map>

#ifdef DEBUG
bool debug = true;
#else
bool debug = false;
#endif

using namespace std;

int main(int argc, const char* argv[]) {
    string test_case = argv[1];
    int buf_size = stoi(argv[2]);
    string input_file = argv[3];
    const char* envVar = std::getenv("WL_FILE");
    cout << "WL FILE:" << envVar << endl;
    if (test_case == "prio")
        check_prio(buf_size, input_file);
    else if (test_case == "rr")
        check_rr(buf_size, input_file);
    else if (test_case == "fq")
        check_fq(buf_size, input_file);
    else if (test_case == "loom")
        check_loom(buf_size, input_file);
    else
        throw invalid_argument("Unknown test case");
}
