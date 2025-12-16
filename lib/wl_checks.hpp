//
// Created by Amir Hossein Seyhani on 12/15/25.
//

#ifndef FPERF_WL_CHECKS_HPP
#define FPERF_WL_CHECKS_HPP
#include "contention_point.hpp"


#include <vector>

using namespace std;

void print_vector(vector<string> wl_lines);

void read_wl(ContentionPoint* cp);

void check_prio(int buf_size, string input_file);

void check_rr(int buf_size, string input_file);

void check_loom(int buf_size, string input_file);

void check_fq(int buf_size, string input_file);

#endif // FPERF_WL_CHECKS_HPP
