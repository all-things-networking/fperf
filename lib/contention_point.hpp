//  contention_point.hpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/12/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef contention_point_hpp
#define contention_point_hpp

#include <vector>
#include <map>

#include "queue.hpp"
#include "queuing_module.hpp"
#include "util.hpp"
#include "metric.hpp"
#include "query.hpp"
#include "solver.hpp"
#include "shared_config.hpp"

using namespace std;

class ContentionPoint : public Solver {
public:
    ContentionPoint(unsigned int total_time);

    void set_base_workload(Workload wl);
    void add_same_to_base(Same same, time_range_t time_range);
    void add_unique_to_base(Unique unique, time_range_t time_range);
    Workload get_base_workload();
    expr get_base_wl_expr();
    
    void set_query(Query& query);
    unsigned int in_queue_cnt();
    vector<Queue*> get_in_queues();
    unsigned int get_total_time();
    
    unsigned int out_queue_cnt();
    Queue* get_out_queue(unsigned int ind);
    
    Query get_query();
    bool is_query_set();

    QueuingModule* get_qm(cid_t id);
  
    bool set_shared_config(SharedConfig* shared_config);
    bool is_shared_config_set();
 
    solver_res_t solve();
    solver_res_t satisfy_query();
    solver_res_t check_workload_without_query(Workload wl);
    solver_res_t check_workload_with_query(Workload wl, IndexedExample* eg);

    bool generate_base_example(IndexedExample* eg, 
                               qset_t& target_queues,
                               unsigned int max_queue);

    void generate_good_examples(IndexedExample* base_eg,
                                unsigned int count,
                                deque<IndexedExample*>& examples);
    
    void generate_bad_examples(unsigned int count,
                               deque<IndexedExample*>& examples);

    void generate_good_examples_flow(deque<IndexedExample*>& examples,
                                     unsigned int count);

    void generate_good_examples2(IndexedExample* base_eg,
                                 unsigned int count,
                                 deque<IndexedExample*>& examples);

     

    void generate_good_examples_from_base_flow(deque<IndexedExample*>& examples,
                                                unsigned int count,
                                                IndexedExample* base_eg,
                                                qset_t non_zero_queues);
     
    IndexedExample* index_example(Example* eg);
    Example* unindex_example(IndexedExample* ieg);
    
    bool workload_satisfies_example(Workload wl, IndexedExample* eg);
    double workload_example_match(Workload wl, IndexedExample* eg);
    
    string stats_str();
    
    friend ostream& operator<<(ostream& os, const ContentionPoint& p);

protected:
    NetContext net_ctx;
    solver* z3_solver;
    optimize* z3_optimizer;
    map<string, expr> constr_map;
    
    unsigned int total_time;
    
    vector<cid_t> nodes;
    map<cid_t, vector<cid_t>> module_edges;
    map<cid_pair, vector<qpair>> queue_edges;
    map<cid_t, QueuingModule*> id_to_qm;
    map<cid_t, Queue*> id_to_ioq;
    
    std::vector<Queue*> in_queues;
    std::vector<Queue*> out_queues;
    std::map<metric_t, map<cid_t, Metric*>> metrics;
    
    void init();
        
private:
    Workload base_wl;
    expr base_wl_expr;
    
    Query query;
    expr query_expr;
    bool query_is_set = false;

    Dists* dists = NULL;
    SharedConfig* shared_config = NULL;
    qset_t target_queues;
    bool shared_config_is_set = false;
    
   
    virtual void add_nodes() = 0;
    virtual void add_edges() = 0;
    virtual void add_metrics() = 0;

    void setup_edges();    
    void populate_id_to_ioq();
    void populate_in_queues();
    void populate_out_queues();
    
    void add_node_constrs();
    void add_metric_constrs();
    void add_in_queue_constrs();
    void add_out_queue_constrs();

    void add_constr(expr const &e, char const *p); 
    void add_constr_from_map(map<string, expr> constr_map);
    
    string get_model_str(model& m);
    virtual std::string cp_model_str(model& m, NetContext& net_ctx, unsigned int t) = 0;
    void populate_example_from_model(model& m, IndexedExample* eg);
    
    expr get_random_eg_mod(IndexedExample* eg, unsigned int mod_cnt, qset_t queue_set);
    solver_res_t get_solver_res_t(check_result z3_res);

    
    // generate expressions from workloads and queries
    expr get_expr(Query& query);
    expr get_expr(IndexedExample* eg, vector<metric_t>& metrics);
    expr get_expr(IndexedExample* eg);
    expr get_expr(Workload wl);
    expr get_expr(Workload wl, unsigned int t);
    expr get_expr(TimedSpec tspec, unsigned int t);
    expr get_expr(WlSpec spec, unsigned int t);
    expr get_expr(rhs_t rhs, unsigned int t);
    expr get_expr(lhs_t lhs, unsigned int t);
    expr get_expr(unsigned int c, unsigned int t);
    expr get_expr(TIME time, unsigned int t);
    expr get_expr(TONE t_one, unsigned int t);
    expr get_expr(TSUM t_sum, unsigned int t);
    
    expr mk_comp(expr lhs, comp_t comp, expr rhs);
    
    /* *********** Workload Satisifes Example ************ */
    bool timedspec_satisfies_example(TimedSpec spec, IndexedExample* eg);
    bool eval_spec(TimedSpec spec, IndexedExample* eg, unsigned int time) const;
    bool eval_spec(WlSpec spec, IndexedExample* eg, unsigned int time) const;
    unsigned int eval_rhs(rhs_t rhs, IndexedExample* eg, unsigned int time) const;
    unsigned int eval_lhs(lhs_t lhs, IndexedExample* eg, unsigned int time) const;
    unsigned int eval_trf(trf_t trf, IndexedExample* eg, unsigned int time) const;
    unsigned int eval_trf(TSUM tsum, IndexedExample* eg, unsigned int time) const;
    unsigned int eval_trf(TONE tone, IndexedExample* eg, unsigned int time) const;
    unsigned int eval_TIME(TIME time, IndexedExample* eg, unsigned int t) const;
};

#endif /* contention_point_hpp */
