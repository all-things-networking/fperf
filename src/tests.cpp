//
//  tests.cpp
//  FPerf
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
#include "tbf.hpp"
#include <ldd.h>
#include <tvpi.h>

Workload run(ContentionPoint* cp,
         IndexedExample* base_eg,
         unsigned int good_example_cnt,
         string good_examples_file,
         unsigned int bad_example_cnt,
         string bad_examples_file,
         Query& query,
         unsigned int max_spec,
         SharedConfig* config);

void prio(string good_examples_file, string bad_examples_file) {

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
                Op(Op::Type::GT),
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
    dists_params.random_seed = 2000;

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

void rr(string good_examples_file, string bad_examples_file) {

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
            wl.add_spec(TimedSpec(
                new Comp(new Indiv(metric_t::CENQ, q), Op(Op::Type::GE), new Constant(i * rate)),
                time_range_t(i * period - 1, i * period - 1),
                total_time));
        }
    }

    wl.add_spec(TimedSpec(new Comp(new Indiv(metric_t::CENQ, queue1),
                                   Op(Op::Type::GT),
                                   new Indiv(metric_t::CENQ, queue2)),
                          time_range_t(total_time - 1, total_time - 1),
                          total_time));

    cout << "base workload: " << endl << wl << endl;

    rr->set_base_workload(wl);

    // Query
    cid_t queue1_id = rr->get_in_queues()[queue1]->get_id();
    cid_t queue2_id = rr->get_in_queues()[queue2]->get_id();

    Query query(query_quant_t::FORALL,
                time_range_t(total_time - 1 - (period - 1), total_time - 1),
                qdiff_t(queue2_id, queue1_id),
                metric_t::CDEQ,
                Op(Op::Type::GE),
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
    dists_params.random_seed = 29663;

    Dists* dists = new Dists(dists_params);
    SharedConfig* config = new SharedConfig(total_time, rr->in_queue_cnt(), target_queues, dists);
    bool config_set = rr->set_shared_config(config);
    if (!config_set) return;

    // Workload final_wl = run(rr,
    //     base_eg,
    //     good_example_cnt,
    //     good_examples_file,
    //     bad_example_cnt,
    //     bad_examples_file,
    //     query,
    //     10,
    //     config);

    // Set up Final Workload (don't need to perform search each time)
    Workload final_wl(10, in_queue_cnt, total_time);

    // Add specs
    // [1, 5]: cenq(2, t) >= t
    final_wl.add_spec(TimedSpec(new Comp(new Indiv(metric_t::CENQ, 2), Op(Op::Type::GE), new Time(1)),
                               time_range_t(0, 4),
                               total_time));

    // [1, 4]: SUM_[q in {0, 1, 3, 4, }] cenq(q ,t) <= 0
    qset_t qset;
    qset.insert(0);
    qset.insert(1);
    qset.insert(3);
    qset.insert(4);
    final_wl.add_spec(TimedSpec(new Comp(new QSum(qset, metric_t::CENQ), Op(Op::Type::LE), new Constant(0)),
                               time_range_t(0, 3),
                               total_time));

    // Print final workload
    cout << "Final workload: " << endl << final_wl << endl;

    // LDD Testing
    DdManager *cudd;
    LddManager* ldd;
    theory_t * t;

    // I don't know what this does
    t = tvpi_create_theory(3);

    cout << "Creating the world...\n" << endl;
    cudd = Cudd_Init (0, 0, CUDD_UNIQUE_SLOTS, 127, 0);

    cout << "Initializing the theory...\n" << endl;
    ldd = Ldd_Init (cudd, t);

    // Calculate number of enqs = number of queues * timesteps
    unsigned int num_enqs = in_queue_cnt * total_time;

    // Create array of length num_enqs to represent coefficients of linterms
    int* cf = new int[num_enqs];

    /*
     Variable ordering:
        0: q0_t0, 1: q0_t1, 2: q0_t2, 3: q0_t3, 4: q0_t4, 5: q0_t5, 6: q0_t6,
        7: q1_t0, 8: q1_t1, 9: q1_t2, 10: q1_t3, 11: q1_t4, 12: q1_t5, 13: q1_t6,
        ...
     */

    // Create set of LDD constraints
    set<lincons_t> ldd_constraints;

    set<TimedSpec> specs = final_wl.get_all_specs();

    std::cout << "Adding non-zero constraints" << std::endl;
    // Constrain each enq to be non-negative
    for (unsigned int i = 0; i < num_enqs; i++) {
        cf[i] = 0;
    }
    for (unsigned int i = 0; i < num_enqs; i++) {
        // enq[i] >= 0 is equivalent to -enq[i] <= 0
        int q = floor(i / total_time);
        int time = i % total_time + 1;
        std::cout << "Constraint: enq[" << q << "][" << time << "] >= 0" << std::endl;
        cf[i] = -1;
        linterm_t single_enq_term = t->create_linterm(cf, num_enqs);
        ldd_constraints.insert(t->create_cons(single_enq_term, 0, t->create_int_cst(0)));
        cf[i] = 0;
    }

    std::cout << "Adding workload constraints" << std::endl;
    // Loop through Workload constraints, create an ldd constraint for each of them
    for (set<TimedSpec>::iterator it = specs.begin(); it != specs.end(); it++) {
        // Loop through time range
        std::cout << "Spec: " << *it << std::endl;
        for (unsigned int time = it->time_range.first; time <= it->time_range.second; time++) {
            // Set all coefficients to 0
            for (unsigned int i = 0; i < num_enqs; i++) {
                cf[i] = 0;
            }

            // Create set of queues
            qset_t queues;
            WlSpec* wl_spec = it->wl_spec;
            Comp* comp = dynamic_cast<Comp*>(wl_spec); // All specs are Comp objects
            MExpr* lhs = comp->lhs;
            if (dynamic_cast<Indiv*>(lhs)) {
                Indiv* indiv = dynamic_cast<Indiv*>(lhs);
                queues.insert(indiv->queue);
            } else if (dynamic_cast<QSum*>(lhs)) {
                QSum* qsum = dynamic_cast<QSum*>(lhs);
                queues = qsum->qset;
            }

            // Set coefficients for queues in set
            for(unsigned int q_id : queues) {
                for (unsigned int t = 0; t < total_time; t++) {
                    unsigned int idx = q_id * total_time + t;
                    cf[idx] = 1;
                }
            }

            // Get rhs constant value
            unsigned int c = 0;
            if (Expr* rhs = comp->rhs) {
                if (auto* constant = dynamic_cast<Constant*>(rhs)) {
                    c = constant->value;
                } else if (auto* time_rhs = dynamic_cast<Time*>(rhs)) {
                    c = time * time_rhs->coeff;
                }
            }else {
                cout << "ERROR: no RHS in constraint" << endl;
                return;
            }

            // Get comparison operator
            Op op = comp->op;
            Op::Type op_type = op.type;

            // Create ldd constraints:
            linterm_t linear_term = t->create_linterm(cf, num_enqs);
            for(unsigned int i = 0; i < num_enqs; i++) {
                cf[i] = -cf[i];
            }
            linterm_t neg_linear_term = t->create_linterm(cf, num_enqs);


            switch (op_type) {
                case Op::Type::LE:
                    ldd_constraints.insert(t->create_cons(linear_term, 0, t->create_int_cst(c)));
                    break;
                case Op::Type::LT:
                    ldd_constraints.insert(t->create_cons(linear_term, 1, t->create_int_cst(c)));
                    break;
                case Op::Type::EQ:
                    // In the EQ case, we must create two constraints:
                    // linear_term <= c and -linear_term <= -c
                    ldd_constraints.insert(t->create_cons(linear_term, 0, t->create_int_cst(c)));
                    ldd_constraints.insert(t->create_cons(neg_linear_term, 0, t->create_int_cst(-c)));
                    break;
                case Op::Type::GE:
                    // linear_term >= c is equivalent to -linear_term <= -c
                    ldd_constraints.insert(t->create_cons(neg_linear_term, 0, t->create_int_cst(-c)));
                    break;
                case Op::Type::GT:
                    // linear_term > c is equivalent to -linear_term < -c
                    ldd_constraints.insert(t->create_cons(neg_linear_term, 1, t->create_int_cst(-c)));
                    break;
                default:
                    break;
            }

        }
    }

    // Print LDD
    cout << "LDD Constraints: " << endl;
    for (lincons_t cons : ldd_constraints) {
        LddNode* dd = Ldd_FromCons(ldd, cons);
        Ldd_Ref(dd);
        Ldd_PrintMinterm(ldd, dd);
    }

    // Ldd_DumpSmtLibV1(ldd, dd, NULL, 'd', stdout);

    // Clean up
    std::cout << "Destroying the world..." << std::endl;
    Ldd_Quit (ldd);
    tvpi_destroy_theory (t);
    Cudd_Quit (cudd);
}

void fq_codel(string good_examples_file, string bad_examples_file) {

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
        wl.add_spec(TimedSpec(new Comp(new Indiv(metric_t::CENQ, q), Op(Op::Type::GE), new Time(1)),
                              total_time,
                              total_time));
    }

    cp->set_base_workload(wl);

    // Query
    cid_t query_qid = cp->get_in_queues()[last_queue]->get_id();
    Query query(query_quant_t::FORALL,
                time_range_t(total_time - 1, total_time - 1),
                query_qid,
                metric_t::CDEQ,
                Op(Op::Type::GE),
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
    dists_params.random_seed = 4854;

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

void loom(string good_examples_file, string bad_examples_file) {

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
    wl.add_spec(
        TimedSpec(new Comp(new QSum(tenant1_qset, metric_t::CENQ), Op(Op::Type::GE), new Time(1)),
                  total_time,
                  total_time));
    wl.add_spec(
        TimedSpec(new Comp(new QSum(tenant2_qset, metric_t::CENQ), Op(Op::Type::GE), new Time(1)),
                  total_time,
                  total_time));

    for (unsigned int q = 0; q < cp->in_queue_cnt(); q++) {
        if (q % 3 == 2) {
            wl.add_spec(TimedSpec(new Comp(new Indiv(metric_t::CENQ, q), Op(Op::Type::LE), 0u),
                                  total_time,
                                  total_time));
        }
    }

    cp->set_base_workload(wl);

    // Query
    Query query(query_quant_t::FORALL,
                time_range_t(total_time - 1 - query_time, total_time - 1),
                qdiff_t(cp->get_out_queue(1)->get_id(), cp->get_out_queue(0)->get_id()),
                metric_t::CENQ,
                Op(Op::Type::GT),
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
    dists_params.random_seed = 13388;

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

void leaf_spine_bw(string good_examples_file, string bad_examples_file) {
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
    Workload wl(in_queue_cnt + 5, in_queue_cnt, total_time);

    wl.add_spec(
        TimedSpec(new Comp(new Indiv(metric_t::CENQ, src_server), Op(Op::Type::GE), new Time(1)),
                  total_time - 1,
                  total_time));

    wl.add_spec(TimedSpec(
        new Comp(new Indiv(metric_t::DST, src_server), Op(Op::Type::EQ), new Constant(dst_server)),
        total_time - 1,
        total_time));

    for (unsigned int q = 0; q < in_queue_cnt; q++) {
        Same* s = new Same(metric_t::DST, q);
        wl.add_spec(TimedSpec(s, time_range_t(0, total_time - 1), total_time));
    }

    qset_t unique_qset;
    for (unsigned int q = 0; q < in_queue_cnt; q++)
        unique_qset.insert(q);
    Unique* uniq = new Unique(metric_t::DST, unique_qset);
    wl.add_spec(TimedSpec(uniq, time_range_t(0, total_time - 1), total_time));

    cp->set_base_workload(wl);

    // Query
    cid_t query_qid = cp->get_out_queue(dst_server)->get_id();
    Query query(query_quant_t::FORALL,
                time_range_t(total_time - 1, total_time - 1),
                query_qid,
                metric_t::CENQ,
                Op(Op::Type::LE),
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
    dists_params.random_seed = 24212;

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

void tbf(std::string good_examples_file, std::string bad_examples_file) {
    unsigned int total_time = 6;
    unsigned int start = 2;
    unsigned int interval = 2;

    unsigned int link_rate = 3;

    TBFInfo info;
    info.link_rate = link_rate;
    info.max_tokens = 6;
    info.max_enq = 10;

    TBF* tbf = new TBF(total_time, info);

    Workload wl(100, 1, total_time);

    for (uint i = 0; i < interval; i++) {
        wl.add_spec(TimedSpec(new Comp(new Indiv(metric_t::CENQ, 0),
                                       Op(Op::Type::GE),
                                       new Constant((unsigned int) (i + 1) * link_rate)),
                              time_range_t(start + i, start + i),
                              total_time));
    }
    tbf->set_base_workload(wl);

    cid_t queue_id = tbf->get_in_queue()->get_id();

    Query query(query_quant_t::EXISTS,
                time_range_t(0, total_time - 1),
                queue_id,
                metric_t::DEQ,
                Op(Op::Type::GT),
                link_rate);

    tbf->set_query(query);

    IndexedExample* base_eg = new IndexedExample();
    qset_t target_queues;

    bool res = tbf->generate_base_example(base_eg, target_queues, 1);

    if (!res) {
        cout << "ERROR: couldn't generate base example" << endl;
        return;
    }

    // Set shared config
    DistsParams dists_params;
    dists_params.in_queue_cnt = tbf->in_queue_cnt();
    dists_params.total_time = total_time;
    dists_params.pkt_meta1_val_max = 2;
    dists_params.pkt_meta2_val_max = 2;
    dists_params.random_seed = 14748;

    Dists* dists = new Dists(dists_params);
    SharedConfig* config = new SharedConfig(total_time, tbf->in_queue_cnt(), target_queues, dists);
    bool config_set = tbf->set_shared_config(config);
    if (!config_set) return;

    run(tbf, base_eg, 50, good_examples_file, 50, bad_examples_file, query, 8, config);
}

Workload run(ContentionPoint* cp,
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
    return search.run();
}
