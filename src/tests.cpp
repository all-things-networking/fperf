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

#include <random>
#include <iterator>
#include <set>
#include <vector>

void run(ContentionPoint* cp,
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
                op_t::GT,
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

std::vector<std::set<unsigned int>> find_contiguous_sets(const std::set<unsigned int>& input_set) {
    std::vector<std::set<unsigned int>> contiguous_sets;
    std::set<unsigned int> current_set;

    auto it = input_set.begin();
    unsigned int previous = *it;
    current_set.insert(previous);

    for (it = std::next(it); it != input_set.end(); ++it) {
        // If the current element is consecutive, add it to the current set
        if (*it == previous + 1) {
            current_set.insert(*it);
        } else {
            // If not consecutive, start a new set
            contiguous_sets.push_back(current_set);
            current_set.clear();
            current_set.insert(*it);
        }
        previous = *it;
    }

    // Add the last set
    if (!current_set.empty()) {
        contiguous_sets.push_back(current_set);
    }

    // Remove sets of size 1
    contiguous_sets.erase(std::remove_if(contiguous_sets.begin(), contiguous_sets.end(), [](const std::set<unsigned int>& s){return s.size() == 1;}), contiguous_sets.end());

    return contiguous_sets;
}

Workload broaden(Workload wl, Search& search) { // Need to pass in search so we can check if the new spec is valid
    //    // EXAMPLE: Need a systematic process which would turn a wl containing the following specs:
////    [1, 1]: SUM_[q in {0, 1, }] cenq(q ,t) = 1
////    [2, 2]: SUM_[q in {0, 1, }] cenq(q ,t) = 2
////    [3, 3]: SUM_[q in {0, 1, }] cenq(q ,t) = 3
////    [4, 4]: SUM_[q in {0, 1, }] cenq(q ,t) = 4
////    [5, 5]: SUM_[q in {0, 1, }] cenq(q ,t) = 5
////    [6, 6]: SUM_[q in {0, 1, }] cenq(q ,t) = 6
//// Into the following:
////    [1, 6]: SUM_[q in {0, 1, }] cenq(q ,t) = t
//// Which expresses the exact same information, but is more concise.
//
//    // In general: If we find a set of specs of the form:
//    // [t1, t1]: (SUM_[q in Q] OR INDIV(q)) cenq(q ,t) = c1
//    // [t2, t2]: (SUM_[q in Q] OR INDIV(q)) cenq(q ,t) = c2
//    // ...
//    // [tn, tn]: (SUM_[q in Q] OR INDIV(q)) cenq(q ,t) = cn
//    // Where t1 < t2 < ... < tn, and c1 < c2 < ... < cn and they are all either SUM over the same set of queues or INDIV over the same queue
//    // [t1, tn]: (SUM_[q in Q] OR INDIV(q)) cenq(q ,t) = t
//
//    // Code:
//    // Get all specs
//    set<TimedSpec> specs = wl.get_all_specs();
//    // For each spec, check if it's of the form [t, t]: cenq(q, t) = c where c = t
//    // If so, we need to keep track of: q, c, t

// Step 1: Find all specs of the form [t, t]: cenq(q, t) = c where c = t

    set<TimedSpec> specs = wl.get_all_specs();

    // Keep track of sets (of size 1 if INDIV, of size>1 if SUM) of q's to t's

    std::map<std::set<unsigned int>, std::set<unsigned int>> qset_to_tset;

    for (const TimedSpec& spec : specs) {
        if (spec.get_time_range().first == spec.get_time_range().second) {
            wl_spec_t wl_spec = spec.get_wl_spec();
            if (holds_alternative<Comp>(wl_spec)) {
                Comp comp = get<Comp>(wl_spec);
                if(holds_alternative<unsigned int>(comp.rhs)){
                    unsigned int c = get<unsigned int>(comp.rhs);
                    unsigned int t = spec.get_time_range().first + 1; // +1 because time starts at 1
                    if (holds_alternative<Indiv>(comp.lhs)) {
                        Indiv indiv = get<Indiv>(comp.lhs);
                        unsigned int q = indiv.queue;
                        if (t == c && indiv.get_metric() == metric_t::CENQ) {
                            // Add this spec to the set of specs for this q
                            qset_to_tset[{q}].insert(t);
                        }
                    } else if (holds_alternative<QSum>(comp.lhs)) {
                        QSum qsum = get<QSum>(comp.lhs);
                        std::set<unsigned int> qset = qsum.qset;
                        if (t == c && qsum.get_metric() == metric_t::CENQ) {
                            // Add this spec to the set of specs for this qset
                            qset_to_tset[qset].insert(t);
                        }
                    }
                }
            }
        }
    }

//    cout << "qset_to_tset: " << endl;
//    for(auto const& [qset, tset] : qset_to_tset){
//        cout << "qset: " << qset << ", tset: " << tset << endl;
//    }

// Step 2: For each set of t's, merge the consecutive ones into a single spec
    for(auto const& [qset, tset] : qset_to_tset) {
        // For each qset, we need to check if there is a set of c's that are consecutive
        // If so, we check if the corresponding set of t's are consecutive
        // If so, we can replace the set of specs with a single spec
        // We can do this by removing all the old specs and adding a new spec
        // Implementation:
        // Check if tset is consecutive
        std::vector<std::set<unsigned int>> contiguous_tsets = find_contiguous_sets(tset);
        for (auto const &contiguous_tset: contiguous_tsets) {
            // Merge into one spec
            // Step 1: Add a new spec (QSum or Indiv depending on whether qset has size 1 or not) of form [t1, tn]: (SUM_[q in Q] OR INDIV(q)) cenq(q ,t) = t
            if (qset.size() == 1) {
                TimedSpec spec_to_add = TimedSpec(Comp(Indiv(metric_t::CENQ, *qset.begin()), op_t::EQ, Time(1)), time_range_t(*contiguous_tset.begin() - 1, *contiguous_tset.rbegin() - 1), wl.get_total_time());
                cout << "BROADEN: Adding spec: " << spec_to_add << endl;
                wl.add_spec(spec_to_add);
            } else {
                TimedSpec spec_to_add = TimedSpec(Comp(QSum(qset, metric_t::CENQ), op_t::EQ, Time(1)), time_range_t(*contiguous_tset.begin() - 1, *contiguous_tset.rbegin() - 1), wl.get_total_time());
//                cout << "BROADEN: Adding spec: " << spec_to_add << endl;
                wl.add_spec(spec_to_add);
            }

            // Step 2: Remove all the old specs
            for (auto const &t : contiguous_tset) {
                if (qset.size() == 1) {
                    // This spec is part of the contiguous set of t's
                    // Remove it
                    TimedSpec spec_to_remove = TimedSpec(Comp(Indiv(metric_t::CENQ, *qset.begin()), op_t::EQ, t), time_range_t(t - 1, t - 1), wl.get_total_time());
//                    cout << "BROADEN: Removing spec: " << spec_to_remove << endl;
                    wl.rm_spec(spec_to_remove);
                } else {
                    // This spec is part of the contiguous set of t's
                    // Remove it
                    TimedSpec spec_to_remove = TimedSpec(Comp(QSum(qset, metric_t::CENQ), op_t::EQ, t), time_range_t(t - 1, t - 1), wl.get_total_time());
//                    cout << "BROADEN: Removing spec: " << spec_to_remove << endl;
                    wl.rm_spec(spec_to_remove);
                }
            }
        }
    }


    // For COMP specs, try to broaden operations (change = to <=, >=, change < to <=, change > to >=) and see if it's still valid
    // If so, replace the spec with the new spec
    // If not, keep the old spec
    specs = wl.get_all_specs();
    for (const TimedSpec& spec : specs) {
        wl_spec_t wl_spec = spec.get_wl_spec();
        if (holds_alternative<Comp>(wl_spec)) {
            Comp comp = get<Comp>(wl_spec);
            if (holds_alternative<Indiv>(comp.lhs)) {
                Indiv indiv = get<Indiv>(comp.lhs);
                unsigned int q = indiv.queue;
                if (indiv.get_metric() == metric_t::CENQ) {
                    // Try to broaden the op
                    op_t op = comp.get_op();
                    if (op == op_t::EQ) {
                        // Try <=
                        TimedSpec spec_to_try = TimedSpec(Comp(Indiv(metric_t::CENQ, q), op_t::LE, comp.rhs),
                                                          spec.get_time_range(), wl.get_total_time());
                        // Create temp workload
                        Workload temp_wl = wl;
                        temp_wl.rm_spec(spec);
                        temp_wl.add_spec(spec_to_try);
                        if (search.check(temp_wl)) {
                            // It's valid, replace the spec
                            wl.rm_spec(spec);
                            wl.add_spec(spec_to_try);
                            continue;
                        } else {
                            // It's not valid, keep the old spec
                        }

                        // Try >=
                        spec_to_try = TimedSpec(Comp(Indiv(metric_t::CENQ, q), op_t::GE, comp.rhs),
                                                          spec.get_time_range(), wl.get_total_time());
                        // Create temp workload
                        temp_wl = wl;
                        temp_wl.rm_spec(spec);
                        temp_wl.add_spec(spec_to_try);
                        if (search.check(temp_wl)) {
                            // It's valid, replace the spec
                            wl.rm_spec(spec);
                            wl.add_spec(spec_to_try);
                            continue;
                        } else {
                            // It's not valid, keep the old spec
                        }
                    } else if (op == op_t::LT) {
                        // Try <=
                        TimedSpec spec_to_try = TimedSpec(Comp(Indiv(metric_t::CENQ, q), op_t::LE, comp.rhs),
                                                          spec.get_time_range(), wl.get_total_time());
                        // Create temp workload
                        Workload temp_wl = wl;
                        temp_wl.rm_spec(spec);
                        temp_wl.add_spec(spec_to_try);
                        if (search.check(temp_wl)) {
                            // It's valid, replace the spec
                            wl.rm_spec(spec);
                            wl.add_spec(spec_to_try);
                            continue;
                        } else {
                            // It's not valid, keep the old spec
                        }
                    } else if (op == op_t::GT) {
                        // Try >=
                        TimedSpec spec_to_try = TimedSpec(Comp(Indiv(metric_t::CENQ, q), op_t::GE, comp.rhs),
                                                          spec.get_time_range(), wl.get_total_time());
                        // Create temp workload
                        Workload temp_wl = wl;
                        temp_wl.rm_spec(spec);
                        temp_wl.add_spec(spec_to_try);
                        if (search.check(temp_wl)) {
                            // It's valid, replace the spec
                            wl.rm_spec(spec);
                            wl.add_spec(spec_to_try);
                            continue;
                        } else {
                            // It's not valid, keep the old spec
                        }
                    }
                }
            }
        }
    }


    return wl;
}

void prio_test(string good_examples_file, string bad_examples_file) {

    cout << "prio_test" << endl;
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
                op_t::GT,
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

    cout << "Base example: " << endl << *base_eg << endl;

    // Create a workload that perfectly describes the base example
    // That is, for each queue, for each timestep, the workload has a spec of the form:
    // [t, t]: cenq(queue_id, t) = sum from 0 to t of enqs[queue_id][t]

    vector<vector<unsigned int>> enqs = base_eg->enqs;
    vector<vector<unsigned int>> sums(prio->in_queue_cnt(), vector<unsigned int>(total_time, 0));
    for (unsigned int q = 0; q < enqs.size(); q++) {
        for (unsigned int t = 0; t < enqs[q].size(); t++) {
            sums[q][t] = (t == 0) ? enqs[q][t] : sums[q][t - 1] + enqs[q][t];
        }
    }

    Workload wl(100, prio->in_queue_cnt(), total_time);
    for (unsigned int q = 0; q < enqs.size(); q++) {
        for (unsigned int t = 0; t < enqs[q].size(); t++) {
//            cout << "cenq(q" << q << ", " << t << ") = " << sums[q][t] << endl;
            wl.add_spec(TimedSpec(Comp(Indiv(metric_t::CENQ, q), op_t::EQ, sums[q][t]),
                                  time_range_t(t, t),
                                  total_time));
        }
    }

    cout << "Original Workload: " << endl << wl << endl;

    Search search(prio, query, 8, config, good_examples_file, bad_examples_file);

    // Keep randomly removing specs until we hit 'minimum' workload
    // (i.e. the workload that is valid but removing any spec makes it invalid)

    std::random_device rd;
    std::mt19937 gen(rd());
    Workload lastValidWl = wl;  // Store the initial workload state.

    std::set<TimedSpec> attemptedSpecs; // To track specs we've attempted to remove.
    std::set<TimedSpec> essentialSpecs; // To track specs confirmed as essential.

    while (true) {
        auto specs = wl.get_all_specs();  // Get the current specs.

        // Break the loop if all current specs are confirmed as essential.
        if (essentialSpecs.size() == specs.size()) {
            break;
        }

        std::uniform_int_distribution<> dis(0, specs.size() - 1);
        auto it = std::next(specs.begin(), dis(gen));
        TimedSpec specToRemove = *it;

        // If we've already attempted to remove this spec, skip it.
        if (attemptedSpecs.find(specToRemove) != attemptedSpecs.end()) {
            continue;
        }

        // Attempt to remove the spec.
        Workload tempWl = wl;
        tempWl.rm_spec(specToRemove);

        if (search.check(tempWl)) {
            wl = tempWl; // Update the original workload if the temp one is valid.
            lastValidWl = wl; // Update the last valid state.
            attemptedSpecs.clear(); // Reset attempted specs since the workload has changed.
        } else {
            attemptedSpecs.insert(specToRemove); // Mark this spec as attempted.
            essentialSpecs.insert(specToRemove); // Confirm this spec as essential.
        }
    }

    lastValidWl = search.refine(lastValidWl);
    lastValidWl = broaden(lastValidWl, search);
    cout << "Final Workload (Random Approach): " << endl << lastValidWl << endl;


    // Try another approach: start from the original workload and remove specs one by one
    // by doing the following procedure:
    // For each queue q:
    // For t from 1 to total_time:
    // Remove the spec that specifies cenq(q, t) = sum from 0 to t of enqs[q][t] until the workload becomes invalid (then put the last removed spec back and move on to the next queue)

    Workload wl2 = wl;
    for (unsigned int q = 0; q < enqs.size(); q++) {
        for (unsigned int t = 0; t < enqs[q].size(); t++) {
            TimedSpec specToRemove(Comp(Indiv(metric_t::CENQ, q), op_t::EQ, sums[q][t]),
                                   time_range_t(t, t),
                                   total_time);
            wl2.rm_spec(specToRemove);
//            cout << "Removing spec: " << specToRemove << endl;
            if (!search.check(wl2)) {
//                cout << "Workload is invalid" << endl;
                wl2.add_spec(specToRemove);
                break;
            }
        }
    }

    wl2 = search.refine(wl2);
    wl2 = broaden(wl2, search);
    cout << "Final Workload (Front-to-Back Iterative Approach): " << endl << wl2 << endl;


    // Back-to-Front approach: same as above, except we iterate on t from total_time to 1
    // WARNING: time starts at 1, can never equal 0

    Workload wl3 = wl;

    for (unsigned int q = 0; q < enqs.size(); q++) {
//        cout << "Number of timesteps for queue " << q << ": " << enqs[q].size() << endl;
        if (!enqs[q].empty()) {
            for (int t = static_cast<int>(enqs[q].size()) - 1; t >= 0; t--) {
                TimedSpec specToRemove(Comp(Indiv(metric_t::CENQ, q), op_t::EQ, sums[q][t]),
                time_range_t(static_cast<unsigned int>(t), static_cast<unsigned int>(t)),
                        total_time);
                // Check if spec is in the workload
                if (wl3.get_all_specs().find(specToRemove) == wl3.get_all_specs().end()) {
//                    cout << "Spec " << specToRemove << " not in workload" << endl;
                    continue;
                }
//                cout << "Removing spec: " << specToRemove << endl;
                wl3.rm_spec(specToRemove);
                if (!search.check(wl3)) {
//                    cout << "Workload is invalid" << endl;
                    wl3.add_spec(specToRemove);
                    break;
                } else {
//                    cout << "Workload is valid" << endl;
                }
            }
        }
    }


    wl3 = search.refine(wl3);
    wl3 = broaden(wl3, search);
    cout << "Final Workload (Back-to-Front Iterative Approach): " << endl << wl3 << endl;

    //    run(prio,
//        base_eg,
//        good_example_cnt,
//        good_examples_file,
//        bad_example_cnt,
//        bad_examples_file,
//        query,
//        8,
//        config);
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
            wl.add_spec(TimedSpec(Comp(Indiv(metric_t::CENQ, q), op_t::GE, i * rate),
                                  time_range_t(i * period - 1, i * period - 1),
                                  total_time));
        }
    }

    wl.add_spec(
        TimedSpec(Comp(Indiv(metric_t::CENQ, queue1), op_t::GT, Indiv(metric_t::CENQ, queue2)),
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
                op_t::GE,
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
        wl.add_spec(
            TimedSpec(Comp(Indiv(metric_t::CENQ, q), op_t::GE, Time(1)), total_time, total_time));
    }

    cp->set_base_workload(wl);

    // Query
    cid_t query_qid = cp->get_in_queues()[last_queue]->get_id();
    Query query(query_quant_t::FORALL,
                time_range_t(total_time - 1, total_time - 1),
                query_qid,
                metric_t::CDEQ,
                op_t::GE,
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
    wl.add_spec(TimedSpec(Comp(QSum(tenant1_qset, metric_t::CENQ), op_t::GE, Time(1)),
                          total_time,
                          total_time));
    wl.add_spec(TimedSpec(Comp(QSum(tenant2_qset, metric_t::CENQ), op_t::GE, Time(1)),
                          total_time,
                          total_time));

    for (unsigned int q = 0; q < cp->in_queue_cnt(); q++) {
        if (q % 3 == 2) {
            wl.add_spec(
                TimedSpec(Comp(Indiv(metric_t::CENQ, q), op_t::LE, 0u), total_time, total_time));
        }
    }

    cp->set_base_workload(wl);

    // Query
    Query query(query_quant_t::FORALL,
                time_range_t(total_time - 1 - query_time, total_time - 1),
                qdiff_t(cp->get_out_queue(1)->get_id(), cp->get_out_queue(0)->get_id()),
                metric_t::CENQ,
                op_t::GT,
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

    wl.add_spec(TimedSpec(Comp(Indiv(metric_t::CENQ, src_server), op_t::GE, Time(1)),
                          total_time - 1,
                          total_time));

    wl.add_spec(TimedSpec(Comp(Indiv(metric_t::DST, src_server), op_t::EQ, dst_server),
                          total_time - 1,
                          total_time));

    for (unsigned int q = 0; q < in_queue_cnt; q++) {
        Same s(metric_t::DST, q);
        wl.add_spec(TimedSpec(s, time_range_t(0, total_time - 1), total_time));
    }

    qset_t unique_qset;
    for (unsigned int q = 0; q < in_queue_cnt; q++)
        unique_qset.insert(q);
    Unique uniq(metric_t::DST, unique_qset);
    wl.add_spec(TimedSpec(uniq, time_range_t(0, total_time - 1), total_time));

    cp->set_base_workload(wl);

    // Query
    cid_t query_qid = cp->get_out_queue(dst_server)->get_id();
    Query query(query_quant_t::FORALL,
                time_range_t(total_time - 1, total_time - 1),
                query_qid,
                metric_t::CENQ,
                op_t::LE,
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
        wl.add_spec(
            TimedSpec(Comp(Indiv(metric_t::CENQ, 0), op_t::GE, (unsigned int) (i + 1) * link_rate),
                      time_range_t(start + i, start + i),
                      total_time));
    }
    tbf->set_base_workload(wl);

    cid_t queue_id = tbf->get_in_queue()->get_id();

    Query query(query_quant_t::EXISTS,
                time_range_t(0, total_time - 1),
                queue_id,
                metric_t::DEQ,
                op_t::GT,
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
