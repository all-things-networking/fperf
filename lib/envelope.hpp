#ifndef MAIN_FPERF_ENVELOPE_H
#define MAIN_FPERF_ENVELOPE_H

#include <iostream>

#include "tests.hpp"

#include "params.hpp"
#include "query.hpp"
#include "rr_scheduler.hpp"
#include "search.hpp"

using namespace std;

const int GOOD_EXAMPLES_COUNT = 25;
const int BAD_EXAMPLES_COUNT = 25;

void execute(ContentionPoint* cp, Workload& base_wl, Query& query, DistsParams params);

void rr_base_ex() ;

void rr_new_ex() ;

#endif // MAIN_FPERF_ENVELOPE_H
