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

    prio();
    // rr();
    // fq_codel();
    // loom();
    // leaf_spine_bw();

    return 0;
}
