//
//  main.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 3/2/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//


#include "tests.hpp"


// #include <map>

// #ifdef DEBUG
// bool debug = true;
// #else
// bool debug = false;
// #endif

// using namespace std;


int main(int argc, const char* argv[]) {
    int buf_size = stoi(argv[1]);
    int rand_seed = stoi(argv[2]);
    const char* envVar = std::getenv("WL_FILE");
    cout << "WL FILE:" << envVar << endl;
    // prio(buf_size);
    // rr(buf_size);
    // loom_non_mem(buf_size);
    // loom_mem(buf_size);
    // fq_codel(buf_size, rand_seed);
    leaf_spine_bw(buf_size);
}
