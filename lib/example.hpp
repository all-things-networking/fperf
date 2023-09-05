//
//  example.hpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 12/15/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef example_hpp
#define example_hpp

#include <map>
#include <vector>
#include <fstream>
#include <deque>

#include "util.hpp"

using namespace std;

struct Example{
    unsigned int total_time;
    cid_t query_qid;
    map<cid_t, vector<unsigned int>> enqs;
    map<cid_t, vector<unsigned int>> deqs;

    map<cid_t, vector<vector<int>>> enqs_meta1;
    map<cid_t, vector<vector<int>>> enqs_meta2;

    friend ostream& operator<<(ostream& os, const Example& eg);
};

struct IndexedExample{
    unsigned int total_time;
    cid_t query_qid;
    vector<vector<unsigned int>> enqs;
    vector<vector<unsigned int>> deqs;
    vector<vector<vector<int>>> enqs_meta1;
    vector<vector<vector<int>>> enqs_meta2;
 
    friend ostream& operator<<(ostream& os, const IndexedExample& eg);
};

struct Trace{
    unsigned int total_time;
    vector<vector<unsigned int>> enqs;
    
    friend ostream& operator<<(ostream& os, const Trace& tr);
};

void write_examples_to_file(deque<Example*>& examples,
                            string fname);

void read_examples_from_file(deque<Example*>& examples,
                             string fname);

#endif /* example_hpp */
