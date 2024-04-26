//
//  search.hpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/20/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef search_hpp
#define search_hpp

#include <deque>
#include <random>

#include "contention_point.hpp"
#include "cost.hpp"
#include "example.hpp"
#include "input_only_solver.hpp"
#include "params.hpp"
#include "query.hpp"
#include "shared_config.hpp"
#include "spec_factory.hpp"
#include "workload.hpp"

class Search {
public:
    Search(ContentionPoint* cp,
           Query query,
           unsigned int max_spec,
           SharedConfig* shared_config,
           string good_examples_fname,
           string bad_examples_fname);

    Search(ContentionPoint* cp,
           Query query,
           unsigned int max_spec,
           SharedConfig* shared_config,
           deque<IndexedExample*>& good_ex,
           deque<IndexedExample*>& bad_ex);

    void run();
    bool check(Workload wl, string function_name="");
    Workload refine(Workload wl);

    // Research project
    Workload setup_refinement(Workload wl);
    Workload remove_specs(Workload wl);
    Workload tighten_constant_bounds(Workload wl);
    Workload aggregate_indivs_to_sums(Workload wl);

    unsigned int cost(Workload wl, string function_name="");

    ContentionPoint* cp;



    void print_stats();
    void clear_stats();



private:
    InputOnlySolver* input_only_solver;
    Query query;
    unsigned int max_spec;
    SharedConfig* shared_config = NULL;
    Dists* dists = NULL;
    qset_t target_queues;
    unsigned int in_queue_cnt;
    unsigned int total_time;

    SpecFactory spec_factory;
    Workload wl_last_step;
    bool last_input_infeasible;
    vector<Workload> infeasible_wls;

    vector<Workload> solutions;

    unsigned int close_count;

    deque<IndexedExample*> bad_examples;
    deque<IndexedExample*> good_examples;

    void init_wl(Workload& wl);
    void search(Workload wl);

    void pick_neighbors(Workload wl, vector<Workload>& neighbors);
    Workload random_neighbor(Workload wl, unsigned int hops = 1);
    bool satisfies_bad_example(Workload wl);
    void populate_bad_examples(string fname);
    void populate_good_examples(string fname);
    unsigned int bad_example_match_count(Workload wl);
    unsigned int good_example_match_count(Workload wl);

    /* ********** stats ********** */
    unsigned int round_no;
    std::map<string, unsigned long long int> sum_check_time;
    std::map<string, unsigned long long int> max_check_time;

    unsigned int rounds_in_local_search;
    unsigned int reset_cnt;
    std::map<string, unsigned int> no_solver_call;
    std::map<string, unsigned int> input_only_solver_call;
    std::map<string, unsigned int> query_only_solver_call;
    std::map<string, unsigned int> full_solver_call;
    unsigned int infeasible_input_cnt;

    /* ********** stats ********** */
    std::map<string, unsigned long long int> sum_call_time;
    unsigned int call_cnt;
};

#endif /* search_hpp */
