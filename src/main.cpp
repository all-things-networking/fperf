//
//  main.cpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 3/2/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#include <iostream>
#include <random>
#include <time.h>
#include <vector>

#include "tests.hpp"
#include "util.hpp"

#ifdef DEBUG
bool debug = true;
#else
bool debug = false;
#endif


int main(int argc, const char* argv[]) {
    (void) argc;
    (void) argv;
    std::vector<std::string> args(argv + 1, argv + argc);

    //    prio();
    //     rr(args[0]);
    //     fq_codel(args[0]);
    //     loom();
    //        leaf_spine_bw(args[0]);
    leaf_spine_bw();

    std::cout << "FINISHED " << args[0] << std::endl;

    return 0;
}
