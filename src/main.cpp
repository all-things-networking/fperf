//
//  main.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 3/2/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//


#include "gen/utils.hpp"
#include "gen/wl_parser.hpp"
#include "priority_scheduler.hpp"
#include "tests.hpp"


#include <map>

#ifdef DEBUG
bool debug = true;
#else
bool debug = false;
#endif

using namespace std;

void print_vector(vector<string> wl_lines) {
    for (string line : wl_lines)
        cout << line << endl;
}


void read_wl(int queue_cnt, int total_time, ContentionPoint* cp) {
    string wl_file_path = format("{}/{}.txt", "wls", "prio");
    vector<vector<string>> wls = read_wl_file(wl_file_path);
    for (int i = 0; i < wls.size(); ++i) {
        WorkloadParser parser(queue_cnt, total_time);
        auto wl_lines = wls[i];
        string res_stat = wl_lines[0];
        cout << "WL: " << i + 1 << "/" << wls.size() << " " << res_stat << endl;
        wl_lines.erase(wl_lines.begin());
        parser.parse(wl_lines);
        Workload* wl = parser.wl;
        cout << "Input workload:" << endl;
        print_vector(wl_lines);
        cout << "Parsed workload:" << endl;
        cout << wl << endl;

        auto res = cp->check_wl_and_not_query(*wl);
        if (res_stat == "SAT") {
            assert(res == solver_res_t::SAT);
        } else {
            assert(res == solver_res_t::UNSAT);
        }
        // return;
    }
}

void check_prio(int buf_size) {
    unsigned int prio_levels = 4;
    unsigned int query_thresh = 5;

    unsigned int good_example_cnt = 50;
    unsigned int bad_example_cnt = 50;
    unsigned int total_time = 7;

    PrioScheduler* prio = new PrioScheduler(prio_levels, total_time, buf_size);

    cid_t query_qid = prio->get_in_queues()[2]->get_id();
    Query query(query_quant_t::EXISTS,
                time_range_t(0, prio->get_total_time() - 1),
                query_qid,
                metric_t::CBLOCKED,
                Op(Op::Type::GT),
                query_thresh);
    prio->set_query(query);

    read_wl(prio_levels, total_time, prio);
    // prio->satisfy_query();
    // prio->unsat_not_query();
}


int main(int argc, const char* argv[]) {
    int buf_size = stoi(argv[1]);
    int rand_seed = stoi(argv[2]);
    const char* envVar = std::getenv("WL_FILE");
    cout << "WL FILE:" << envVar << endl;
    check_prio(buf_size);
    // read_wl();
    // prio(buf_size);
    // rr(buf_size);
    // loom_non_mem(buf_size);
    // loom_mem(buf_size);
    // fq_codel(buf_size, rand_seed);
}
