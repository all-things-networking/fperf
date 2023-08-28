//
//  tests.cpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 12/05/22.
//  Copyright © 2022 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "tests.hpp"

#include "buggy_2l_rr_scheduler.hpp"
#include "leaf_spine.hpp"
#include "loom_mqprio.hpp"
#include "params.hpp"
#include "priority_scheduler.hpp"
#include "query.hpp"
#include "rr_scheduler.hpp"
#include "search.hpp"

void run(ContentionPoint* cp,
         IndexedExample* base_eg,
         unsigned int good_example_cnt,
         std::string good_examples_file,
         unsigned int bad_example_cnt,
         std::string bad_examples_file,
         Query& query,
         unsigned int max_spec,
         SharedConfig* config);

void prio(std::string good_examples_file, std::string bad_examples_file) {

    cout << "Prio" << endl;
    time_typ start_time = noww();

    unsigned int prio_levels = 4;
    unsigned int query_thresh = 5;

    unsigned int good_example_cnt = 50;
    unsigned int bad_example_cnt = 50;
    unsigned int total_time = 7;

    PrioScheduler* prio = new PrioScheduler(prio_levels, total_time);

    cid_t query_qid = prio->get_in_queues()[2]->get_id();
    Query query(query_quant_t::EXISTS,
                time_range_t(0, prio->get_total_time() - 1),
                query_qid,
                metric_t::CBLOCKED,
                comp_t::GT,
                query_thresh);

    prio->set_query(query);

    cout << "cp setup: " << (get_diff_millisec(start_time, noww()) / 1000.0) << " s" << endl;

    // generate base example
    start_time = noww();
    IndexedExample* base_eg = new IndexedExample();
    qset_t target_queues;

    bool res = prio->generate_base_example(base_eg, target_queues, prio_levels);

    if (!res) {
        cout << "ERROR: couldn't generate base example" << endl;
        return;
    }

    cout << "base example generation: " << (get_diff_millisec(start_time, noww()) / 1000.0) << " s"
         << endl;


    // Set shared config
    DistsParams dists_params;
    dists_params.in_queue_cnt = prio->in_queue_cnt();
    dists_params.total_time = total_time;
    dists_params.pkt_meta1_val_max = 2;
    dists_params.pkt_meta2_val_max = 2;

    Dists* dists = new Dists(dists_params);
    SharedConfig* config = new SharedConfig(total_time, prio->in_queue_cnt(), target_queues, dists);
    bool config_set = prio->set_shared_config(config);
    if (!config_set) return;

    run(prio,
        base_eg,
        good_example_cnt,
        good_examples_file,
        bad_example_cnt,
        bad_examples_file,
        query,
        8,
        config);
}

void rr(std::string good_examples_file, std::string bad_examples_file) {

    cout << "rr" << endl;
    time_typ start_time = noww();

    unsigned int in_queue_cnt = 5;
    unsigned int period = 5;
    unsigned int recur = 2;
    unsigned int rate = 4;

    unsigned int good_example_cnt = 25;
    unsigned int bad_example_cnt = 25;
    unsigned int total_time = recur * period;

    // Create contention point
    RRScheduler* rr = new RRScheduler(in_queue_cnt, total_time);

    unsigned int queue1 = 1;
    unsigned int queue2 = 2;

    // Base workload
    Workload wl(100, in_queue_cnt, total_time);

    for (unsigned int i = 1; i <= recur; i++) {
        for (unsigned int q = 0; q < in_queue_cnt; q++) {
            wl.add_wl_spec(TimedSpec(WlSpec(TONE(metric_t::CENQ, q), comp_t::GE, i * rate),
                                     time_range_t(i * period - 1, i * period - 1),
                                     total_time));
        }
    }

    wl.add_wl_spec(
        TimedSpec(WlSpec(TONE(metric_t::CENQ, queue1), comp_t::GT, TONE(metric_t::CENQ, queue2)),
                  time_range_t(total_time - 1, total_time - 1),
                  total_time));

    std::cout << "base workload: " << std::endl << wl << std::endl;

    rr->set_base_workload(wl);

    // Query
    cid_t queue1_id = rr->get_in_queues()[queue1]->get_id();
    cid_t queue2_id = rr->get_in_queues()[queue2]->get_id();

    Query query(query_quant_t::FORALL,
                time_range_t(total_time - 1 - (period - 1), total_time - 1),
                qdiff_t(queue2_id, queue1_id),
                metric_t::CDEQ,
                comp_t::GE,
                3);

    rr->set_query(query);

    cout << "cp setup: " << (get_diff_millisec(start_time, noww()) / 1000.0) << " s" << endl;

    // generate base example
    start_time = noww();
    IndexedExample* base_eg = new IndexedExample();
    qset_t target_queues;

    bool res = rr->generate_base_example(base_eg, target_queues, in_queue_cnt);

    if (!res) {
        cout << "ERROR: couldn't generate base example" << endl;
        return;
    }

    cout << "base example generation: " << (get_diff_millisec(start_time, noww()) / 1000.0) << " s"
         << endl;


    // Set shared config
    DistsParams dists_params;
    dists_params.in_queue_cnt = rr->in_queue_cnt();
    dists_params.total_time = total_time;
    dists_params.pkt_meta1_val_max = 2;
    dists_params.pkt_meta2_val_max = 2;

    Dists* dists = new Dists(dists_params);
    SharedConfig* config = new SharedConfig(total_time, rr->in_queue_cnt(), target_queues, dists);
    bool config_set = rr->set_shared_config(config);
    if (!config_set) return;

    run(rr,
        base_eg,
        good_example_cnt,
        good_examples_file,
        bad_example_cnt,
        bad_examples_file,
        query,
        10,
        config);
}

void fq_codel(std::string good_examples_file, std::string bad_examples_file) {

    cout << "fq_codel" << endl;
    time_typ start_time = noww();

    unsigned int in_queue_cnt = 5;
    unsigned int total_time = 14;
    unsigned int query_thresh = (total_time / in_queue_cnt) + 3;
    unsigned int last_queue = in_queue_cnt - 1;

    unsigned int good_example_cnt = 50;
    unsigned int bad_example_cnt = 50;

    // Create contention point
    Buggy2LRRScheduler* cp = new Buggy2LRRScheduler(in_queue_cnt, total_time);

    // Base Workload
    Workload wl(in_queue_cnt * 5, in_queue_cnt, total_time);
    for (unsigned int q = 0; q < last_queue; q++) {
        wl.add_wl_spec(TimedSpec(
            WlSpec(TONE(metric_t::CENQ, q), comp_t::GE, TIME(1)), total_time, total_time));
    }

    cp->set_base_workload(wl);

    // Query
    cid_t query_qid = cp->get_in_queues()[last_queue]->get_id();
    Query query(query_quant_t::FORALL,
                time_range_t(total_time - 1, total_time - 1),
                query_qid,
                metric_t::CDEQ,
                comp_t::GE,
                query_thresh);

    cp->set_query(query);

    cout << "cp setup: " << (get_diff_millisec(start_time, noww()) / 1000.0) << " s" << endl;

    // generate base example
    start_time = noww();
    IndexedExample* base_eg = new IndexedExample();
    qset_t target_queues;

    bool res = cp->generate_base_example(base_eg, target_queues, in_queue_cnt);

    if (!res) {
        cout << "ERROR: couldn't generate base example" << endl;
        return;
    }

    cout << "base example generation: " << (get_diff_millisec(start_time, noww()) / 1000.0) << " s"
         << endl;


    // Set shared config
    DistsParams dists_params;
    dists_params.in_queue_cnt = cp->in_queue_cnt();
    dists_params.total_time = total_time;
    dists_params.pkt_meta1_val_max = 2;
    dists_params.pkt_meta2_val_max = 2;

    Dists* dists = new Dists(dists_params);
    SharedConfig* config = new SharedConfig(total_time, cp->in_queue_cnt(), target_queues, dists);
    bool config_set = cp->set_shared_config(config);
    if (!config_set) return;

    run(cp,
        base_eg,
        good_example_cnt,
        good_examples_file,
        bad_example_cnt,
        bad_examples_file,
        query,
        10,
        config);
}

void loom(std::string good_examples_file, std::string bad_examples_file) {

    cout << "loom" << endl;
    time_typ start_time = noww();

    unsigned int nic_tx_queue_cnt = 4;
    unsigned int per_core_flow_cnt = 3;
    unsigned int query_time = 3;

    unsigned int good_example_cnt = 50;
    unsigned int bad_example_cnt = 50;
    unsigned int total_time = 10;

    // Create contention point
    LoomMQPrio* cp = new LoomMQPrio(nic_tx_queue_cnt, per_core_flow_cnt, total_time);


    qset_t tenant1_qset;
    qset_t tenant2_qset;

    for (unsigned int i = 0; i < cp->in_queue_cnt(); i++) {
        if (i % 3 == 0)
            tenant1_qset.insert(i);
        else
            tenant2_qset.insert(i);
    }

    // Base Workload

    Workload wl(20, cp->in_queue_cnt(), total_time);
    wl.add_wl_spec(TimedSpec(
        WlSpec(TSUM(tenant1_qset, metric_t::CENQ), comp_t::GE, TIME(1)), total_time, total_time));
    wl.add_wl_spec(TimedSpec(
        WlSpec(TSUM(tenant2_qset, metric_t::CENQ), comp_t::GE, TIME(1)), total_time, total_time));

    for (unsigned int q = 0; q < cp->in_queue_cnt(); q++) {
        if (q % 3 == 2) {
            wl.add_wl_spec(
                TimedSpec(WlSpec(TONE(metric_t::CENQ, q), comp_t::LE, 0u), total_time, total_time));
        }
    }

    cp->set_base_workload(wl);

    // Query
    Query query(query_quant_t::FORALL,
                time_range_t(total_time - 1 - query_time, total_time - 1),
                qdiff_t(cp->get_out_queue(1)->get_id(), cp->get_out_queue(0)->get_id()),
                metric_t::CENQ,
                comp_t::GT,
                3u);

    cp->set_query(query);

    cout << "cp setup: " << (get_diff_millisec(start_time, noww()) / 1000.0) << " s" << endl;

    // generate base example
    start_time = noww();
    IndexedExample* base_eg = new IndexedExample();
    qset_t target_queues;

    bool res = cp->generate_base_example(base_eg, target_queues, cp->in_queue_cnt());

    if (!res) {
        cout << "ERROR: couldn't generate base example" << endl;
        return;
    }

    cout << "base example generation: " << (get_diff_millisec(start_time, noww()) / 1000.0) << " s"
         << endl;


    // Set shared config
    DistsParams dists_params;
    dists_params.in_queue_cnt = cp->in_queue_cnt();
    dists_params.total_time = total_time;
    dists_params.pkt_meta1_val_max = 3;
    dists_params.pkt_meta2_val_max = 2;

    Dists* dists = new Dists(dists_params);
    SharedConfig* config = new SharedConfig(total_time, cp->in_queue_cnt(), target_queues, dists);
    bool config_set = cp->set_shared_config(config);
    if (!config_set) return;

    run(cp,
        base_eg,
        good_example_cnt,
        good_examples_file,
        bad_example_cnt,
        bad_examples_file,
        query,
        24,
        config);
}

void leaf_spine_bw(std::string good_examples_file, std::string bad_examples_file) {

    cout << "leaf_spine_bw" << endl;
    time_typ start_time = noww();

    unsigned int leaf_cnt = 3;
    unsigned int spine_cnt = 2;
    unsigned int servers_per_leaf = 2;
    unsigned int server_cnt = leaf_cnt * servers_per_leaf;
    bool reduce_queues = true;

    unsigned int src_server = 0;
    unsigned int dst_server = (2 * servers_per_leaf) + 1;
    unsigned int query_thresh = 4;

    unsigned int good_example_cnt = 50;
    unsigned int bad_example_cnt = 50;
    unsigned int total_time = 10;

    // Create contention point
    LeafSpine* cp = new LeafSpine(leaf_cnt, spine_cnt, servers_per_leaf, total_time, reduce_queues);

    unsigned int in_queue_cnt = cp->in_queue_cnt();

    // Base Workload
    Workload wl(in_queue_cnt, in_queue_cnt, total_time);

    wl.add_wl_spec(TimedSpec(
        WlSpec(TONE(metric_t::CENQ, src_server), comp_t::GE, TIME(1)), total_time - 1, total_time));

    wl.add_wl_spec(TimedSpec(WlSpec(TONE(metric_t::META1, src_server), comp_t::EQ, dst_server),
                             total_time - 1,
                             total_time));

    cp->set_base_workload(wl);
    for (unsigned int q = 0; q < in_queue_cnt; q++) {
        Same s(metric_t::META1, q);
        cp->add_same_to_base(s, time_range_t(0, total_time - 1));
        cout << "[1, " << total_time << "] " << s << endl;
    }

    qset_t unique_qset;
    for (unsigned int q = 0; q < in_queue_cnt; q++) {
        unique_qset.insert(q);
    }
    Unique u(unique_qset, metric_t::META1);
    cp->add_unique_to_base(u, time_range_t(0, total_time - 1));
    cout << "[1, " << total_time << "] " << u << endl;

    // Query
    cid_t query_qid = cp->get_out_queue(dst_server)->get_id();
    Query query(query_quant_t::FORALL,
                time_range_t(total_time - 1, total_time - 1),
                query_qid,
                metric_t::CENQ,
                comp_t::LE,
                query_thresh);

    cp->set_query(query);

    cout << "cp setup: " << (get_diff_millisec(start_time, noww()) / 1000.0) << " s" << endl;

    // generate base example
    start_time = noww();
    IndexedExample* base_eg = new IndexedExample();
    qset_t target_queues;

    bool res = cp->generate_base_example(base_eg, target_queues, cp->in_queue_cnt());

    if (!res) {
        cout << "ERROR: couldn't generate base example" << endl;
        return;
    }

    cout << "base example generation: " << (get_diff_millisec(start_time, noww()) / 1000.0) << " s"
         << endl;


    // Set shared config
    DistsParams dists_params;
    dists_params.in_queue_cnt = cp->in_queue_cnt();
    dists_params.total_time = total_time;
    dists_params.pkt_meta1_val_max = server_cnt - 1;
    dists_params.pkt_meta2_val_max = spine_cnt - 1;

    Dists* dists = new Dists(dists_params);
    SharedConfig* config = new SharedConfig(total_time, cp->in_queue_cnt(), target_queues, dists);
    bool config_set = cp->set_shared_config(config);
    if (!config_set) return;

    run(cp,
        base_eg,
        good_example_cnt,
        good_examples_file,
        bad_example_cnt,
        bad_examples_file,
        query,
        24,
        config);
}

void run(ContentionPoint* cp,
         IndexedExample* base_eg,
         unsigned int good_example_cnt,
         string good_examples_file,
         unsigned int bad_example_cnt,
         string bad_examples_file,
         Query& query,
         unsigned int max_spec,
         SharedConfig* config) {

    // generate good examples
    time_typ start_time = noww();

    deque<IndexedExample*> good_examples;

    cp->generate_good_examples2(base_eg, good_example_cnt, good_examples);

    cout << "good example generation: " << (get_diff_millisec(start_time, noww()) / 1000.0) << " s"
         << endl;

    // write the good examples to a file
    deque<Example*> good_unindexed_examples;
    if (!good_examples_file.empty()) {
        for (deque<IndexedExample*>::iterator it = good_examples.begin(); it != good_examples.end();
             it++) {
            Example* eg = cp->unindex_example(*it);
            good_unindexed_examples.push_back(eg);
        }

        write_examples_to_file(good_unindexed_examples, good_examples_file);
    }

    // generate bad examples
    start_time = noww();
    deque<IndexedExample*> bad_examples;

    cp->generate_bad_examples(bad_example_cnt, bad_examples);

    cout << "bad example generation: " << (get_diff_millisec(start_time, noww()) / 1000.0) << " s"
         << endl;

    // write the bad examples to a file
    deque<Example*> bad_unindexed_examples;
    if (!bad_examples_file.empty()) {
        for (deque<IndexedExample*>::iterator it = bad_examples.begin(); it != bad_examples.end();
             it++) {
            Example* eg = cp->unindex_example(*it);
            good_unindexed_examples.push_back(eg);
        }

        write_examples_to_file(bad_unindexed_examples, bad_examples_file);
    }

    start_time = noww();
    // search
    Search search(cp, query, max_spec, config, good_examples, bad_examples);

    cout << "search setup: " << (get_diff_millisec(start_time, noww())) << " ms" << endl;
    search.run();
}