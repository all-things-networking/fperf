//
// Created by Amir Hossein Seyhani on 2/11/24.
//

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

void generate_base_example(ContentionPoint* cp, Workload& base_wl, Query& query){
}

void execute(ContentionPoint* cp, Workload& base_wl, Query& query, DistsParams params) {
    IndexedExample* base_eg = new IndexedExample();
    cp->set_base_workload(base_wl);
    cp->set_query(query);

    qset_t target_queues;
    bool res = cp->generate_base_example(base_eg, target_queues, base_wl.get_queue_cnt());
    if (!res) {
        cout << "ERROR: couldn't generate base example" << endl;
        return;
    }
    return;

    Dists* dists = new Dists(params);
    SharedConfig* config = new SharedConfig(params.total_time,
                                            cp->in_queue_cnt(),
                                            target_queues,
                                            dists);
    bool config_set = cp->set_shared_config(config);
    if (!config_set) return;
    int max_spec = 10;

    deque<IndexedExample*> good_examples;
    cp->generate_good_examples2(base_eg, GOOD_EXAMPLES_COUNT, good_examples);
    deque<IndexedExample*> bad_examples;
    cp->generate_bad_examples(BAD_EXAMPLES_COUNT, bad_examples);
    auto start_time = noww();
    Search search(cp, query, max_spec, config, good_examples, bad_examples);
    search.run();
    cout << "search time: " << (get_diff_millisec(start_time, noww())) << " ms" << endl;
}

void rr_new_ex() {
    cout << "rr" << endl;
    time_typ start_time = noww();
    uint total_time = 16;
    uint last_t = 11;
    uint in_queue_cnt = 2;

    RRScheduler* rr = new RRScheduler(in_queue_cnt, total_time);

    Workload wl(100, in_queue_cnt, total_time);

    cout << "Base_Wl: " << endl << wl << endl;

    cid_t queue1_id = rr->get_in_queues()[0]->get_id();
    cid_t queue2_id = rr->get_in_queues()[1]->get_id();
    cid_t outq_id = rr->get_out_queue(0)->get_id();
    Query query(query_quant_t::EXISTS,
                time_range_t(last_t, last_t),
                outq_id,
                metric_t::ICENQ1,
                op_t::EQ,
                5);

    rr->set_base_workload(wl);
    rr->set_query(query);

    cout << rr->satisfy_query() << endl;

    IndexedExample* base_eg = new IndexedExample();
    qset_t target_queues;
    bool res = rr->generate_base_example(base_eg, target_queues, wl.get_queue_cnt());
    if (!res) {
        cout << "ERROR: couldn't generate base example" << endl;
        return;
    }
    return;

    DistsParams dists_params;
    dists_params.in_queue_cnt = rr->in_queue_cnt();
    dists_params.total_time = total_time;
    dists_params.pkt_meta1_val_max = 3;
    dists_params.pkt_meta2_val_max = 3;
    dists_params.random_seed = 29663;

    //    execute(rr, wl, query, dists_params);
}

#endif // MAIN_FPERF_ENVELOPE_H
