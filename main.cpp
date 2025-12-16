//
//  main.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 3/2/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//


#include "priority_scheduler.hpp"
#include "src/gen/utils.hpp"
#include "src/gen/wl_parser.hpp"
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
    const char* envVar = std::getenv("FPERF_OUTPUT_WL_PATH");
    cout << "FPERF WL FILE:" << envVar << endl;
    if (test_case == "prio")
        prio(buf_size);
    else if (test_case == "rr")
        rr(buf_size);
    else if (test_case == "loom")
        loom_mem(buf_size);
    else if (test_case == "fq")
        fq_codel(buf_size);
    else
        throw invalid_argument("Unknown test case");
}
