//
//  main.cpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 3/2/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#include <time.h>
#include <random>
#include <vector>
#include <iostream>

#include "util.hpp"
#include "tests.hpp"

#ifdef DEBUG
bool debug = true;
#else
bool debug = false;
#endif


int main(int argc, const char * argv[]) {
    (void) argc;
    (void) argv;
   
    prio();
    //rr();
    //fq_codel();
    //loom();  
    //leaf_spine_bw();
    
    return 0; 
}


