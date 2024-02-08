//
//  contention_point.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/12/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#include <set>
#include <sstream>

#include "contention_point.hpp"

unsigned int INP_QUEUE_RANGE_MAX;
unsigned int TIMESTEP_RANGE_MAX;

ContentionPoint::ContentionPoint(unsigned int total_time):
total_time(total_time),
base_wl(Workload(0, 0, total_time)),
base_wl_expr(expr(net_ctx.z3_ctx())),
query_expr(expr(net_ctx.z3_ctx())) {
}

// NOTE: the constructor of inherited classes
//       (i.e., CPs) should call init.
void ContentionPoint::init() {
    // base_wl
    base_wl_expr = get_expr(base_wl);

    // Vars
    add_nodes();
    setup_edges();
    populate_id_to_ioq();
    populate_in_queues();
    populate_out_queues();

    add_metrics();

    // Initialize the solver
    params p(net_ctx.z3_ctx());

    p.set("unsat_core", true);
    p.set("random_seed", Z3_RANDOM_SEED);
    // p.set("random_seed", (unsigned)time(NULL));

    z3_solver = new solver(net_ctx.z3_ctx());
    z3_solver->set(p);

    // Initialize the optimizer
    z3_optimizer = new optimize(net_ctx.z3_ctx());

    // Add constraints
    add_node_constrs();
    add_metric_constrs();
    add_in_queue_constrs();
    add_out_queue_constrs();

    DEBUG_MSG("#QM: " << nodes.size() << " " << id_to_ioq.size() << endl);
    DEBUG_MSG("#bool_vars: " << net_ctx.get_bool_var_cnt() << endl);
    DEBUG_MSG("#int_vars: "
              << " " << net_ctx.get_int_var_cnt() << endl);
    DEBUG_MSG("#constrs: " << constr_map.size() << endl);
}

void ContentionPoint::setup_edges() {
    add_edges();

    // setup output queues based on composition
    map<cid_pair, vector<qpair>>::iterator it;
    for (it = queue_edges.begin(); it != queue_edges.end(); it++) {
        cid_pair modules = it->first;
        QueuingModule* m1 = id_to_qm[modules.first];
        QueuingModule* m2 = id_to_qm[modules.second];

        vector<qpair> edges = it->second;
        for (unsigned int i = 0; i < edges.size(); i++) {
            qpair edge = edges[i];
            unsigned int q1 = edge.first;
            unsigned int q2 = edge.second;
            m1->set_out_queue(q1, m2->get_in_queue(q2));
        }
    }

    // Instantiate "sink" queues
    for (unsigned int i = 0; i < nodes.size(); i++) {
        QueuingModule* m = id_to_qm[nodes[i]];
        cid_t base_out_id = get_unique_id(m->get_id(), "sink");
        for (unsigned int q = 0; q < m->out_queue_cnt(); q++) {
            Queue* queue = m->get_out_queue(q);
            if (queue == nullptr) {
                QueueInfo qinfo = m->get_out_queue_info(q);
                Queue* queue;
                switch (qinfo.type) {
                    case queue_t::LINK: {
                        queue = new Link(base_out_id, to_string(q), total_time, net_ctx);
                        break;
                    }
                    case queue_t::IMM_QUEUE: {
                        queue = new ImmQueue(base_out_id,
                                             to_string(q),
                                             qinfo.size,
                                             qinfo.max_enq,
                                             qinfo.max_deq,
                                             total_time,
                                             net_ctx);
                        break;
                    }
                    default: {
                        queue = new Queue(base_out_id,
                                          to_string(q),
                                          qinfo.size,
                                          qinfo.max_enq,
                                          qinfo.max_deq,
                                          total_time,
                                          net_ctx);
                        break;
                    }
                }
                m->set_out_queue(q, queue);
            }
        }
    }
}

void ContentionPoint::populate_id_to_ioq() {
    for (unsigned int i = 0; i < nodes.size(); i++) {
        QueuingModule* m = id_to_qm[nodes[i]];
        for (unsigned int q = 0; q < m->in_queue_cnt(); q++) {
            Queue* queue = m->get_in_queue(q);
            id_to_ioq[queue->get_id()] = queue;
        }

        for (unsigned int q = 0; q < m->out_queue_cnt(); q++) {
            Queue* queue = m->get_out_queue(q);
            cid_t qid = queue->get_id();
            if (id_to_ioq.find(qid) == id_to_ioq.end()) {
                id_to_ioq[qid] = queue;
            }
        }
    }
}

void ContentionPoint::populate_in_queues() {
    map<cid_t, bool*> is_in_queue;

    for (unsigned int i = 0; i < nodes.size(); i++) {
        cid_t m_id = nodes[i];
        QueuingModule* m = id_to_qm[m_id];
        unsigned long m_iq_cnt = m->in_queue_cnt();
        is_in_queue[m_id] = new bool[m_iq_cnt];
        for (unsigned int j = 0; j < m_iq_cnt; j++) {
            is_in_queue[m_id][j] = true;
        }
    }

    map<cid_t, vector<cid_t>>::iterator it;
    for (it = module_edges.begin(); it != module_edges.end(); it++) {
        cid_t m_id = it->first;

        vector<cid_t> neighbors = it->second;
        for (unsigned int i = 0; i < neighbors.size(); i++) {
            cid_t n_id = neighbors[i];
            vector<qpair> qpairs = queue_edges[cid_pair(m_id, n_id)];
            for (unsigned int j = 0; j < qpairs.size(); j++) {
                unsigned int offset = qpairs[j].second;
                is_in_queue[n_id][offset] = false;
            }
        }
    }

    map<cid_t, bool*>::iterator it2;
    for (it2 = is_in_queue.begin(); it2 != is_in_queue.end(); it2++) {
        cid_t m_id = it2->first;
        QueuingModule* m = id_to_qm[m_id];
        unsigned long m_iq_cnt = m->in_queue_cnt();

        bool* in_q_info = it2->second;
        for (unsigned int i = 0; i < m_iq_cnt; i++) {
            if (in_q_info[i]) {
                in_queues.push_back(m->get_in_queue(i));
            }
        }
    }

    for (map<cid_t, bool*>::iterator it = is_in_queue.begin(); it != is_in_queue.end(); it++) {
        delete[] it->second;
    }
}

void ContentionPoint::populate_out_queues() {
    map<cid_t, vector<cid_t>>::iterator it;
    for (unsigned int i = 0; i < nodes.size(); i++) {
        cid_t m_id = nodes[i];
        QueuingModule* m = id_to_qm[m_id];
        unsigned long m_oq_cnt = m->out_queue_cnt();
        bool* is_out_q = new bool[m_oq_cnt];
        for (unsigned int j = 0; j < m_oq_cnt; j++) {
            is_out_q[j] = true;
        }

        it = module_edges.find(m_id);
        if (it != module_edges.end()) {
            vector<cid_t> neighbors = it->second;
            for (unsigned int i = 0; i < neighbors.size(); i++) {
                vector<qpair> qpairs = queue_edges[cid_pair(m_id, neighbors[i])];
                for (unsigned int j = 0; j < qpairs.size(); j++) {
                    is_out_q[qpairs[j].first] = false;
                }
            }
        }
        for (unsigned int i = 0; i < m_oq_cnt; i++) {
            if (is_out_q[i]) {
                out_queues.push_back(m->get_out_queue(i));
            }
        }

        delete[] is_out_q;
    }
}

void ContentionPoint::add_node_constrs() {
    for (unsigned int i = 0; i < nodes.size(); i++) {
        cid_t m_id = nodes[i];
        QueuingModule* m = id_to_qm[m_id];

        map<string, expr> new_constr_map;
        m->add_constrs(net_ctx, new_constr_map);
        add_constr_from_map(new_constr_map);
    }

    map<cid_t, Queue*>::iterator it;
    for (it = id_to_ioq.begin(); it != id_to_ioq.end(); it++) {
        Queue* queue = it->second;
        map<string, expr> new_constr_map;
        queue->add_constrs(net_ctx, new_constr_map);
        add_constr_from_map(new_constr_map);
    }
}

void ContentionPoint::add_metric_constrs() {
    map<metric_t, map<cid_t, Metric*>>::iterator it;
    for (it = metrics.begin(); it != metrics.end(); it++) {
        map<cid_t, Metric*>::iterator it2;
        for (it2 = it->second.begin(); it2 != it->second.end(); it2++) {
            Metric* m = it2->second;
            map<string, expr> new_constr_map;

            m->populate_val_exprs(net_ctx);
        }
    }
}

void ContentionPoint::add_in_queue_constrs() {
}

void ContentionPoint::add_out_queue_constrs() {
    char constr_name[100];
    for (unsigned int t = 0; t < total_time; t++) {
        for (unsigned int q = 0; q < out_queues.size(); q++) {
            Queue* queue = out_queues[q];
            expr_vector deq_cnt_vec(net_ctx.z3_ctx());
            unsigned int max_deq = queue->max_deq();
            unsigned int deq_bound = queue->size();
            if (max_deq < deq_bound) deq_bound = max_deq;
            for (unsigned int p = 0; p < deq_bound - 1; p++) {
                deq_cnt_vec.push_back(implies(net_ctx.pkt2val(queue->elem(p)[t]) &&
                                                  !net_ctx.pkt2val(queue->elem(p + 1)[t]),
                                              queue->deq_cnt(t) == (int) (p + 1)));
            }
            deq_cnt_vec.push_back(
                implies(!net_ctx.pkt2val(queue->elem(0)[t]), queue->deq_cnt(t) == 0));

            deq_cnt_vec.push_back(implies(net_ctx.pkt2val(queue->elem(deq_bound - 1)[t]),
                                          queue->deq_cnt(t) == (int) deq_bound));

            expr constr = mk_and(deq_cnt_vec);
            snprintf(constr_name, 100, "%s_deq_cnt_at_%d", queue->get_id().c_str(), t);
            add_constr(constr, constr_name);
            constr_map.insert(named_constr(constr_name, constr));
        }
    }
}

void ContentionPoint::set_base_workload(Workload wl) {
    this->base_wl = wl;
    this->base_wl_expr = get_expr(wl);
    DEBUG_MSG("base workload: " << endl << wl << endl);
}

Workload ContentionPoint::get_base_workload() {
    return this->base_wl;
}

expr ContentionPoint::get_base_wl_expr() {
    return this->base_wl_expr;
}

void ContentionPoint::set_query(Query& query) {
    this->query = query;
    this->query_expr = get_expr(query);
    query_is_set = true;
}

solver_res_t ContentionPoint::solve() {
    check_result res = z3_solver->check();
    switch (res) {
        case unsat: DEBUG_MSG("unsat\n"); break;
        case sat: DEBUG_MSG("sat\n"); break;
        case unknown: DEBUG_MSG("unknown\n"); break;
    }

#ifdef DEBUG
    if (res == sat) {
        model m = z3_solver->get_model();
        cout << get_model_str(m) << endl;
    } else if (res == unsat) {
        expr_vector core = z3_solver->unsat_core();
        cout << "size: " << core.size() << "\n";
        for (unsigned i = 0; i < core.size(); i++) {
            cout << core[i] << "\n";
        }
    }
#endif

    return get_solver_res_t(res);
}

solver_res_t ContentionPoint::satisfy_query() {
    if (!query_is_set) {
        cout << "ContentionPoint::satisfy_query: Query is not set." << endl;
        return solver_res_t::UNKNOWN;
    }

    z3_solver->push();
    z3_solver->add(base_wl_expr, "base_wl");
    z3_solver->add(query_expr, "query");
    solver_res_t res = solve();

    z3_solver->pop();

    return res;
}

solver_res_t ContentionPoint::check_workload_without_query(Workload wl) {
    time_typ start_time = noww();

    z3_solver->push();
    z3_solver->add(base_wl_expr, "base_wl");

    expr wl_expr = get_expr(wl);
    z3_solver->add(wl_expr, "workload");

    // solver_res_t res = solve();

    check_result z3_res = z3_solver->check();
    solver_res_t res = solver_res_t::UNKNOWN;
    if (z3_res == sat) res = solver_res_t::SAT;
    if (z3_res == unsat) res = solver_res_t::UNSAT;

    z3_solver->pop();

    //------------ Timing Stats
    time_typ end_time = noww();
    unsigned long long int milliseconds = get_diff_millisec(start_time, end_time);

    check_workload_without_query_total_invoc++;
    check_workload_without_query_total_time += milliseconds;
    if (milliseconds > check_workload_without_query_max_time) {
        check_workload_without_query_max_time = milliseconds;
    }
    //-------------------------

    return res;
}

solver_res_t ContentionPoint::check_workload_with_query(Workload wl, IndexedExample* eg) {
    time_typ start_time = noww();

    solver_res_t res = solver_res_t::UNKNOWN;
    z3_solver->push();

    z3_solver->add(base_wl_expr, "base_wl");

    expr wl_expr = get_expr(wl);
    z3_solver->add(wl_expr, "workload");
    z3_solver->add(!query_expr, "not_query");

    check_result z3_res = z3_solver->check();
    if (z3_res == unsat) res = solver_res_t::UNSAT;
    if (z3_res == sat) {
        res = solver_res_t::SAT;

        model m = z3_solver->get_model();
        populate_example_from_model(m, eg);
    }

    z3_solver->pop();

    //------------ Timing Stats
    time_typ end_time = noww();
    unsigned long long int milliseconds = get_diff_millisec(start_time, end_time);

    check_workload_with_query_total_invoc++;
    check_workload_with_query_total_time += milliseconds;
    if (milliseconds > check_workload_with_query_max_time) {
        check_workload_with_query_max_time = milliseconds;
    }
    //-------------------------

    return res;
}

expr ContentionPoint::get_random_eg_mod(IndexedExample* eg,
                                        unsigned int mod_cnt,
                                        qset_t queue_set) {
    if (mod_cnt == 0) return net_ctx.bool_val(true);

    expr_vector res_vec(net_ctx.z3_ctx());

    typedef pair<unsigned int, unsigned int> modpair_t;
    set<modpair_t> mods;

    uniform_int_distribution<unsigned int> queue_dist(0, queue_set.size() - 1);
    uniform_int_distribution<unsigned int> time_dist(0, total_time - 3);
    mt19937& gen = dists->get_gen();
    while (mods.size() < mod_cnt) {
        unsigned int ind = queue_dist(gen);
        qset_t::iterator it = queue_set.begin();
        advance(it, ind);
        unsigned int qid = *it;
        unsigned int timestep = time_dist(gen);
        modpair_t modpair(qid, timestep);
        mods.insert(modpair);
    }

    for (set<modpair_t>::iterator it = mods.begin(); it != mods.end(); it++) {
        unsigned int qid = it->first;
        unsigned int timestep = it->second;
        unsigned int enq = eg->enqs[qid][timestep];
        unsigned int new_enq = dists->enq();

        while (new_enq == enq)
            new_enq = dists->enq();

        expr mod_expr = in_queues[qid]->enq_cnt(timestep) == (int) new_enq;
        res_vec.push_back(mod_expr);
    }

    return mk_and(res_vec);
}

/* Generate the base example
 */
bool ContentionPoint::generate_base_example(IndexedExample* base_eg,
                                            qset_t& target_queues,
                                            unsigned int max_empty_queue) {
    char constr_name[100];

    // PUSH
    z3_optimizer->push();

    // add base workload and query
    z3_optimizer->add(base_wl_expr, "base_wl");
    z3_optimizer->add(query_expr, "query");

    // Per_queue_enq_sum[q] is the total number
    // of packets enqueued in input queue q
    // in all timesteps.
    expr_vector per_queue_enq_sum(net_ctx.z3_ctx());
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        expr_vector this_queues_enqs(net_ctx.z3_ctx());
        for (unsigned int t = 0; t < total_time; t++) {
            this_queues_enqs.push_back(in_queues[q]->enq_cnt(t));
        }
        per_queue_enq_sum.push_back(sum(this_queues_enqs));
    }

    // empty_queues_expr_vec[q] is 1 if input queue q
    // stays empty in all timesteps and 0 otherwise.
    expr_vector empty_queues_expr_vec(net_ctx.z3_ctx());

    for (unsigned int q = 0; q < in_queues.size(); q++) {
        expr empty_queue = ite(per_queue_enq_sum[q] == 0, net_ctx.int_val(1), net_ctx.int_val(0));

        empty_queues_expr_vec.push_back(empty_queue);
    }

    // for each queue q and each time step t,
    // smooth_enq_slots_vec contains
    // - an expression that is 2 if input q
    //   does not receive any packets at time t, and
    //   0 otherwise, and
    // - an expression that evaluates to 1 if the
    //   metadata at time t is not the same as the
    //   metadata of the last time packets were enqueued
    //   in the queue and 0 otherwise.
    expr_vector smooth_enq_slots_vec(net_ctx.z3_ctx());
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        for (unsigned int t = 0; t < total_time - 1; t++) {
            expr pkt1 = in_queues[q]->enqs(0)[t + 1];
            expr val1 = net_ctx.pkt2val(pkt1);
            expr meta11 = net_ctx.pkt2meta1(pkt1);
            expr meta12 = net_ctx.pkt2meta2(pkt1);

            expr empty_enq_slot = ite(val1, net_ctx.int_val(0), net_ctx.int_val(2));

            smooth_enq_slots_vec.push_back(empty_enq_slot);

            expr prev_empty = net_ctx.bool_val(true);

            for (int t2 = t; t2 >= 0; t2--) {
                expr pkt2 = in_queues[q]->enqs(0)[t2];
                expr val2 = net_ctx.pkt2val(pkt2);
                expr meta21 = net_ctx.pkt2meta1(pkt2);
                expr meta22 = net_ctx.pkt2meta2(pkt2);

                expr smooth_enq_slot = ite(val1 && prev_empty && val2 &&
                                               (meta11 != meta21 || meta12 != meta22),
                                           net_ctx.int_val(1),
                                           net_ctx.int_val(0));
                smooth_enq_slots_vec.push_back(smooth_enq_slot);

                prev_empty = prev_empty && !val2;
            }
        }

        expr pkt = in_queues[q]->enqs(0)[0];
        expr val = net_ctx.pkt2val(pkt);
        expr meta1 = net_ctx.pkt2meta1(pkt);
        expr meta2 = net_ctx.pkt2meta2(pkt);

        expr empty_enq_slot = ite(val, net_ctx.int_val(0), net_ctx.int_val(2));
        smooth_enq_slots_vec.push_back(empty_enq_slot);
    }

    // Ensure there is a cap on the number of
    // empty queues to avoid trivial examples
    expr constr_expr = sum(empty_queues_expr_vec) <= (int) max_empty_queue;
    snprintf(constr_name, 100, "at_most_%d_empty_queues", max_empty_queue);
    z3_optimizer->add(constr_expr, constr_name);

    // First, maximize the total number of queues
    // with no traffic, ensuring the num
    z3_optimizer->maximize(sum(empty_queues_expr_vec));

    // then, minimize the "unsmoothness" in traffic
    // (e.g., total number of time steps queues do not
    // receive traffic or have "unsmooth" metadata)
    z3_optimizer->minimize(sum(smooth_enq_slots_vec));

    // Finally, minimize the total traffic in the trace
    z3_optimizer->minimize(sum(per_queue_enq_sum));

    // Solve the optimization problem
    check_result res = z3_optimizer->check();

    // Create the base example
    qset_t zero_queues;

    if (res == sat) {
        model m = z3_optimizer->get_model();

        populate_example_from_model(m, base_eg);
        DEBUG_MSG("base example:\n" << *base_eg << endl);

        // populate the "zero" queues, i.e., queues
        // that will receive no traffic in any time step
        for (unsigned int q = 0; q < in_queues.size(); q++) {
            expr total_cenq_val = m.eval(per_queue_enq_sum[q]);
            if (total_cenq_val.is_numeral() && total_cenq_val.get_numeral_int() == 0) {
                zero_queues.insert(q);
            } else {
                target_queues.insert(q);
            }
        }
        DEBUG_MSG("zero_queues: " << zero_queues << endl);
        DEBUG_MSG("target_queues: " << target_queues << endl);
    } else {
        cout << "Could not find base example. Solver returned: " << res << endl;
        z3_optimizer->pop();
        return false;
    }

    // POP
    z3_optimizer->pop();

    return true;
}


/* Generate the rest of the examples
 * from the base example
 */
void ContentionPoint::generate_good_examples(IndexedExample* base_eg,
                                             unsigned int count,
                                             deque<IndexedExample*>& examples) {

    if (!is_shared_config_set()) {
        cout << "ERROR: SharedConfig is not set." << endl;
        return;
    }

    // PUSH
    z3_optimizer->push();

    // add base workload and query
    z3_optimizer->add(base_wl_expr, "base_wl");
    z3_optimizer->add(query_expr, "query");

    // add constraints to ensure the queues that
    // are "zero" in the base example will stay
    // zero in the rest of the examples too.
    char constr_name[100];
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        if (target_queues.find(q) != target_queues.end()) continue;

        Metric* cenq_metric = in_queues[q]->get_metric(metric_t::CENQ);
        for (unsigned int t = 0; t < total_time; t++) {
            snprintf(constr_name, 100, "%d_is_zerod_queue[%d}", q, t);
            expr constr_expr = cenq_metric->val(t).second == 0;
            z3_optimizer->add(constr_expr, constr_name);
        }
    }

    // Packets enqueued in the last time step will not
    // have enough time to be processed. To avoid arbitrary
    // numbers, ensure enq_cnt in the last time step is zero
    // in all examples.
    // Similarly, do not allow more than one packet in the
    // second to last time step (TODO: the exact constant
    // may need to be generalized.
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        snprintf(constr_name, 100, "%d_example_trimming", q);
        expr constr_expr = in_queues[q]->enq_cnt(total_time - 1) == 0 &&
                           in_queues[q]->enq_cnt(total_time - 2) <= 1;
        z3_optimizer->add(constr_expr, constr_name);
    }

    // Add soft constraints to minimize the distance
    // between the base example and the rest of the
    // examples, so that they are not radically different.
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        for (unsigned int t = 0; t < total_time; t++) {
            z3_optimizer->add_soft(in_queues[q]->enq_cnt(t) == (int) base_eg->enqs[q][t], 1);
        }
    }

    // Try to make each example be different from
    // the previous one in P random spots. Start P
    // from mod_thresh, try EG_RANDOM_MOD_MAX_TRIES
    // times, if it didn't work, decrement P and
    // try again.
    // mod_thresh is co-related with the total
    // number of non-zero queues and total time.
    // Intuitively, the larger the example, the
    // more random modification we want to try
    // up to a cap.
    IndexedExample* eg = base_eg;
    unsigned int cap = EG_RANDOM_MOD_START;
    unsigned int mod_thresh = min(cap,
                                  (unsigned int) ceil(EG_RANDOM_MOD_PERCENT * target_queues.size() *
                                                      total_time));

    for (unsigned int i = 1; i < count; i++) {
        bool found = false;

        for (int j = mod_thresh; j >= 0 && !found; j--) {
            for (unsigned int k = 0; k < EG_RANDOM_MOD_MAX_TRIES; k++) {
                expr mod_expr = get_random_eg_mod(eg, j, target_queues);
                // PUSH
                z3_optimizer->push();
                z3_optimizer->add(mod_expr, "prev_eg_constr");

                check_result res = z3_optimizer->check();

                if (res == sat) {
                    found = true;

                    model m = z3_optimizer->get_model();

                    IndexedExample* new_eg = new IndexedExample();
                    populate_example_from_model(m, new_eg);
                    examples.push_back(new_eg);

                    DEBUG_MSG(*new_eg << endl);
                    expr_vector eg_vec(net_ctx.z3_ctx());
                    for (unsigned int q = 0; q < new_eg->enqs.size(); q++) {
                        for (unsigned int t = 0; t < new_eg->total_time; t++) {
                            eg_vec.push_back(in_queues[q]->enq_cnt(t) == (int) new_eg->enqs[q][t]);
                        }
                    }

                    expr not_this_example = !mk_and(eg_vec);
                    eg = new_eg;

                    // POP
                    z3_optimizer->pop();
                    // Make sure we don't get this example again
                    snprintf(constr_name, 100, "example_%d", i);
                    z3_optimizer->add(not_this_example, constr_name);


                    if (i % 10 == 0) cout << "generated " << i << " good examples." << endl;

                    break;

                } else {
                    DEBUG_MSG(j << " mods didn't work" << endl);
                    // POP
                    z3_optimizer->pop();
                }
            }
        }
        if (!found) {
            // TODO: did we find all the traces??? can this help with the search?
            break;
        }
    }

    // POP
    z3_optimizer->pop();
}

void ContentionPoint::generate_bad_examples(unsigned int count, deque<IndexedExample*>& examples) {

    if (!is_shared_config_set()) {
        cout << "ERROR: SharedConfig is not set." << endl;
        return;
    }

    char constr_name[100];

    // PUSH
    z3_solver->push();

    // Add base workload and the negation of query
    z3_solver->add(base_wl_expr, "base_wl");
    z3_solver->add(!query_expr, "not_query");

    // add constraints to ensure the queues that
    // are "zero" in the base example will stay
    // zero in the rest of the examples too.
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        if (target_queues.find(q) == target_queues.end()) {
            Metric* cenq_metric = in_queues[q]->get_metric(metric_t::CENQ);
            for (unsigned int t = 0; t < total_time; t++) {
                snprintf(constr_name, 100, "%d_is_zerod_queue[%d}", q, t);
                expr constr_expr = cenq_metric->val(t).second == 0;
                z3_solver->add(constr_expr, constr_name);
            }
        }
    }

    // Find the first "bad" example
    IndexedExample* eg = new IndexedExample();
    check_result res = z3_solver->check();

    if (res == sat) {
        model m = z3_solver->get_model();
        populate_example_from_model(m, eg);
        examples.push_back(eg);

        DEBUG_MSG(*eg << endl);
        expr_vector eg_vec(net_ctx.z3_ctx());
        for (unsigned int q = 0; q < eg->enqs.size(); q++) {
            for (unsigned int t = 0; t < eg->total_time; t++) {
                eg_vec.push_back(in_queues[q]->enq_cnt(t) == (int) eg->enqs[q][t]);
            }
        }

        // Make sure we don't get this example again
        expr not_this_example = !mk_and(eg_vec);

        z3_solver->add(not_this_example, "example_0");

    } else {
        cout << "ContentionPoint::generate_bad_examples: There are no bad examples." << endl;
        z3_solver->pop();
        return;
    }

    // Generate the rest of bad examples.
    // Try to make each example be different from
    // the previous one in P random spots. Start P
    // from mod_thresh, try EG_RANDOM_MOD_MAX_TRIES
    // times, if it didn't work, decrement P and
    // try again.
    // mod_thresh is co-related with the total
    // number of non-zero queues and total time.
    // Intuitively, the larger the example, the
    // more random modification we want to try
    // up to a cap.
    unsigned int cap = EG_RANDOM_MOD_START;
    unsigned int mod_thresh = min(cap,
                                  (unsigned int) ceil(EG_RANDOM_MOD_PERCENT * target_queues.size() *
                                                      total_time));


    for (unsigned int i = 1; i < count; i++) {
        bool found = false;

        for (int j = mod_thresh; j >= 0 && !found; j--) {
            for (unsigned int k = 0; k < EG_RANDOM_MOD_MAX_TRIES; k++) {

                expr mod_expr = get_random_eg_mod(eg, j, target_queues);
                // PUSH
                z3_solver->push();
                z3_solver->add(mod_expr, "prev_eg_constr");

                res = z3_solver->check();

                if (res == sat) {
                    found = true;

                    model m = z3_solver->get_model();

                    IndexedExample* new_eg = new IndexedExample();
                    populate_example_from_model(m, new_eg);
                    examples.push_back(new_eg);

                    DEBUG_MSG(*new_eg << endl);
                    expr_vector eg_vec(net_ctx.z3_ctx());
                    for (unsigned int q = 0; q < new_eg->enqs.size(); q++) {
                        for (unsigned int t = 0; t < new_eg->total_time; t++) {
                            eg_vec.push_back(in_queues[q]->enq_cnt(t) == (int) new_eg->enqs[q][t]);
                        }
                    }

                    expr not_this_example = !mk_and(eg_vec);
                    eg = new_eg;

                    // POP
                    z3_solver->pop();

                    // Make sure we don't get this example again
                    snprintf(constr_name, 100, "example_%d", i);
                    z3_solver->add(not_this_example, constr_name);


                    if (i % 10 == 0) cout << "generated " << i << " bad examples." << endl;
                    break;

                } else {
                    // POP
                    z3_solver->pop();
                }
            }
        }
        if (!found) {
            // TODO: did we find all the bad traces??? can this help with the search?
            break;
        }
    }
    z3_solver->pop();
}


void ContentionPoint::generate_good_examples_from_base_flow(deque<IndexedExample*>& examples,
                                                            unsigned int count,
                                                            IndexedExample* base_eg,
                                                            qset_t non_zero_queues) {
    // PUSH
    z3_optimizer->push();

    z3_optimizer->add(base_wl_expr, "base_wl");
    z3_optimizer->add(query_expr, "query");

    char constr_name[100];

    for (unsigned int q = 0; q < in_queues.size(); q++) {
        if (non_zero_queues.find(q) != non_zero_queues.end()) continue;
        Metric* cenq_metric = in_queues[q]->get_metric(metric_t::CENQ);
        for (unsigned int t = 0; t < total_time; t++) {
            snprintf(constr_name, 100, "%d_is_zerod_queue[%d}", q, t);
            expr constr_expr = cenq_metric->val(t).second == 0;
            z3_optimizer->add(constr_expr, constr_name);
        }
    }


    for (qset_t::iterator it = non_zero_queues.begin(); it != non_zero_queues.end(); it++) {
        unsigned int q = *it;
        snprintf(constr_name, 100, "%d_example_trimming", q);
        expr constr_expr = in_queues[q]->enq_cnt(total_time - 1) == 0 &&
                           in_queues[q]->enq_cnt(total_time - 2) <= 1;
        z3_optimizer->add(constr_expr, constr_name);
    }

    expr_vector smooth_flow_vec(net_ctx.z3_ctx());
    for (qset_t::iterator it = non_zero_queues.begin(); it != non_zero_queues.end(); it++) {
        unsigned int q = *it;
        for (unsigned int t = 0; t < total_time - 1; t++) {
            expr pkt1 = in_queues[q]->enqs(0)[t + 1];
            expr val1 = net_ctx.pkt2val(pkt1);
            expr meta11 = net_ctx.pkt2meta1(pkt1);
            expr meta12 = net_ctx.pkt2meta2(pkt1);

            expr prev_empty = net_ctx.bool_val(true);

            for (int t2 = t; t2 >= 0; t2--) {
                expr pkt2 = in_queues[q]->enqs(0)[t2];
                expr val2 = net_ctx.pkt2val(pkt2);
                expr meta21 = net_ctx.pkt2meta1(pkt2);
                expr meta22 = net_ctx.pkt2meta2(pkt2);

                expr smooth_flow_at_t = ite(val1 && prev_empty && val2 && meta11 != meta21,
                                            net_ctx.int_val(1),
                                            net_ctx.int_val(0));
                smooth_flow_vec.push_back(smooth_flow_at_t);

                smooth_flow_at_t = ite(val1 && prev_empty && val2 && meta12 != meta22,
                                       net_ctx.int_val(1),
                                       net_ctx.int_val(0));
                smooth_flow_vec.push_back(smooth_flow_at_t);

                prev_empty = prev_empty && !val2;
            }
        }
    }

    z3_optimizer->add(sum(smooth_flow_vec) == 0, "smooth");

    // expr_vector diff_from_base_vec(net_ctx.z3_ctx());
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        for (unsigned int t = 0; t < total_time; t++) {
            z3_optimizer->add_soft(in_queues[q]->enq_cnt(t) == (int) base_eg->enqs[q][t], 1);
        }
    }

    for (unsigned int q = 0; q < in_queues.size(); q++) {
        for (unsigned int t = 0; t < total_time; t++) {
            expr pkt = in_queues[q]->enqs(0)[t];
            if (base_eg->enqs_meta1[q][t].size() > 0) {
                z3_optimizer->add_soft(net_ctx.pkt2meta1(pkt) == (int) base_eg->enqs_meta1[q][t][0],
                                       1);
                z3_optimizer->add_soft(net_ctx.pkt2meta2(pkt) == (int) base_eg->enqs_meta2[q][t][0],
                                       1);
            }
        }
    }

    IndexedExample* eg = base_eg;
    unsigned int cap = EG_RANDOM_MOD_START;
    unsigned int mod_thresh = min(cap,
                                  (unsigned int) ceil(EG_RANDOM_MOD_PERCENT *
                                                      non_zero_queues.size() * total_time));

    unsigned int moving_avg_window = 5;
    deque<unsigned int> past_thresh;
    past_thresh.push_back(mod_thresh);

    for (unsigned int i = 1; i < count; i++) {
        if ((i + 1) % 10 == 0) cout << "generated " << (i + 1) << " good examples." << endl;

        bool found = false;

        for (int j = mod_thresh; j >= 0 && !found; j--) {
            for (unsigned int k = 0; k < EG_RANDOM_MOD_MAX_TRIES; k++) {

                expr mod_expr = get_random_eg_mod(eg, j, non_zero_queues);
                z3_optimizer->push();
                z3_optimizer->add(mod_expr, "prev_eg_constr");

                // z3_optimizer->minimize(sum(smooth_flow_vec));
                // z3_optimizer->minimize(sum(diff_from_base_vec));

                check_result res = z3_optimizer->check();

                if (res == sat) {
                    found = true;

                    // DEBUG_MSG("changes: " << mod_expr << endl);
                    model m = z3_optimizer->get_model();

                    IndexedExample* new_eg = new IndexedExample();
                    populate_example_from_model(m, new_eg);
                    examples.push_back(new_eg);

                    // cout << m.eval(sum(smooth_flow_vec)).get_numeral_int() << endl;

                    DEBUG_MSG(*new_eg << endl);
                    expr_vector eg_vec(net_ctx.z3_ctx());
                    for (unsigned int q = 0; q < new_eg->enqs.size(); q++) {
                        for (unsigned int t = 0; t < new_eg->total_time; t++) {
                            eg_vec.push_back(in_queues[q]->enq_cnt(t) == (int) new_eg->enqs[q][t]);
                        }
                    }

                    expr not_this_example = !mk_and(eg_vec);
                    eg = new_eg;

                    z3_optimizer->pop();
                    snprintf(constr_name, 100, "example_%d", i);
                    z3_optimizer->add(not_this_example, constr_name);

                    past_thresh.push_back(j);
                    if (past_thresh.size() > moving_avg_window) {
                        past_thresh.pop_front();
                    }

                    double sum = 0.0;
                    for (unsigned int h = 0; h < past_thresh.size(); h++) {
                        sum += past_thresh[h];
                    }
                    mod_thresh = ceil(sum / past_thresh.size());

                    break;

                } else {
                    DEBUG_MSG(j << " mods didn't work" << endl);
                    z3_optimizer->pop();
                }
            }
        }
        if (!found) {
            // TODO: did we find all the traces??? can this help with the search?
            break;
        }
    }

    // POP
    z3_optimizer->pop();
}

void ContentionPoint::generate_good_examples_flow(deque<IndexedExample*>& examples,
                                                  unsigned int count) {

    // PUSH
    z3_optimizer->push();
    z3_optimizer->add(base_wl_expr, "base_wl");
    z3_optimizer->add(query_expr, "query");

    expr_vector per_queue_enq_sum(net_ctx.z3_ctx());
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        expr_vector this_queues_enqs(net_ctx.z3_ctx());
        for (unsigned int t = 0; t < total_time; t++) {
            this_queues_enqs.push_back(in_queues[q]->enq_cnt(t));
        }
        per_queue_enq_sum.push_back(sum(this_queues_enqs));
    }

    expr_vector empty_queues_expr_vec(net_ctx.z3_ctx());

    /*
    for (unsigned int q = 0; q < in_queues.size(); q++){
        char constr_name[100];
        expr constr_expr = per_queue_enq_sum[q] >= min_cenq;

        snprintf(constr_name, 100, "min_cenq_%d", q);
        z3_optimizer->add(constr_expr, constr_name);
    }
    */

    for (unsigned int q = 0; q < in_queues.size(); q++) {
        expr empty_queue = ite(per_queue_enq_sum[q] == 0, net_ctx.int_val(1), net_ctx.int_val(0));

        empty_queues_expr_vec.push_back(empty_queue);
    }

    expr_vector smooth_enq_slots_vec(net_ctx.z3_ctx());
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        for (unsigned int t = 0; t < total_time - 1; t++) {
            expr pkt1 = in_queues[q]->enqs(0)[t + 1];
            expr val1 = net_ctx.pkt2val(pkt1);
            expr meta11 = net_ctx.pkt2meta1(pkt1);
            expr meta12 = net_ctx.pkt2meta2(pkt1);

            expr empty_enq_slot = ite(val1, net_ctx.int_val(0), net_ctx.int_val(2));

            smooth_enq_slots_vec.push_back(empty_enq_slot);

            expr prev_empty = net_ctx.bool_val(true);

            for (int t2 = t; t2 >= 0; t2--) {
                expr pkt2 = in_queues[q]->enqs(0)[t2];
                expr val2 = net_ctx.pkt2val(pkt2);
                expr meta21 = net_ctx.pkt2meta1(pkt2);
                expr meta22 = net_ctx.pkt2meta2(pkt2);

                expr smooth_enq_slot = ite(val1 && prev_empty && val2 &&
                                               (meta11 != meta21 || meta12 != meta22),
                                           net_ctx.int_val(1),
                                           net_ctx.int_val(0));
                smooth_enq_slots_vec.push_back(smooth_enq_slot);

                prev_empty = prev_empty && !val2;
            }
        }

        expr pkt = in_queues[q]->enqs(0)[0];
        expr val = net_ctx.pkt2val(pkt);
        expr meta1 = net_ctx.pkt2meta1(pkt);
        expr meta2 = net_ctx.pkt2meta2(pkt);

        expr empty_enq_slot = ite(val, net_ctx.int_val(0), net_ctx.int_val(2));
        smooth_enq_slots_vec.push_back(empty_enq_slot);
    }

    z3_optimizer->maximize(sum(empty_queues_expr_vec));
    // z3_optimizer->maximize(min_cenq);
    z3_optimizer->minimize(sum(smooth_enq_slots_vec));
    z3_optimizer->minimize(sum(per_queue_enq_sum));

    check_result res = z3_optimizer->check();
    IndexedExample* base_eg = new IndexedExample();

    qset_t zero_queues;
    qset_t non_zero_queues;

    if (res == sat) {
        model m = z3_optimizer->get_model();

        populate_example_from_model(m, base_eg);
        DEBUG_MSG(*base_eg << endl);

        examples.push_back(base_eg);

        // populate zero queues
        for (unsigned int q = 0; q < in_queues.size(); q++) {
            expr total_cenq_val = m.eval(per_queue_enq_sum[q]);
            if (total_cenq_val.is_numeral() && total_cenq_val.get_numeral_int() == 0) {
                zero_queues.insert(q);
            } else {
                non_zero_queues.insert(q);
            }
        }
        DEBUG_MSG("zero_queues: " << zero_queues << endl);
        DEBUG_MSG("non_zero_queues: " << non_zero_queues << endl);

        target_queues = non_zero_queues;
    } else {
        cout << res << endl;
        z3_optimizer->pop();
        return;
    }

    // POP
    z3_optimizer->pop();

    DEBUG_MSG("base eg: " << (get_diff_millisec(start, noww()) / 1000.0) << endl);

    // PUSH
    z3_optimizer->push();

    z3_optimizer->add(base_wl_expr, "base_wl");
    z3_optimizer->add(query_expr, "query");

    char constr_name[100];
    for (qset_t::iterator it = zero_queues.begin(); it != zero_queues.end(); it++) {
        unsigned int queue = *it;
        Metric* cenq_metric = in_queues[queue]->get_metric(metric_t::CENQ);
        for (unsigned int t = 0; t < total_time; t++) {
            snprintf(constr_name, 100, "%d_is_zerod_queue[%d}", queue, t);
            expr constr_expr = cenq_metric->val(t).second == 0;
            z3_optimizer->add(constr_expr, constr_name);
        }
    }

    for (qset_t::iterator it = target_queues.begin(); it != target_queues.end(); it++) {
        unsigned int q = *it;
        snprintf(constr_name, 100, "%d_example_trimming", q);
        expr constr_expr = in_queues[q]->enq_cnt(total_time - 1) == 0 &&
                           in_queues[q]->enq_cnt(total_time - 2) <= 1;
        z3_optimizer->add(constr_expr, constr_name);
    }

    /*
    for (qset_t::iterator it = target_queues.begin();
         it != target_queues.end(); it++){
        unsigned int q = *it;
        for (unsigned int t = 0; t < total_time; t++){
            for (unsigned int p = 0; p < in_queues[q]->max_enq(); p++){
                snprintf(constr_name, 100, "%d_test_%d_%d", q, t, p);
                expr pkt = in_queues[q]->enqs(p)[t];
                expr constr_expr = implies(net_ctx.pkt2val(pkt),
                                           net_ctx.pkt2meta2(pkt) == 1);
                z3_optimizer->add(constr_expr, constr_name);
            }
        }
    }*/

    expr_vector smooth_flow_vec(net_ctx.z3_ctx());
    for (qset_t::iterator it = target_queues.begin(); it != target_queues.end(); it++) {
        unsigned int q = *it;
        for (unsigned int t = 0; t < total_time - 1; t++) {
            expr pkt1 = in_queues[q]->enqs(0)[t + 1];
            expr val1 = net_ctx.pkt2val(pkt1);
            expr meta11 = net_ctx.pkt2meta1(pkt1);
            expr meta12 = net_ctx.pkt2meta2(pkt1);

            expr prev_empty = net_ctx.bool_val(true);

            for (int t2 = t; t2 >= 0; t2--) {
                expr pkt2 = in_queues[q]->enqs(0)[t2];
                expr val2 = net_ctx.pkt2val(pkt2);
                expr meta21 = net_ctx.pkt2meta1(pkt2);
                expr meta22 = net_ctx.pkt2meta2(pkt2);

                expr smooth_flow_at_t = ite(val1 && prev_empty && val2 && meta11 != meta21,
                                            net_ctx.int_val(1),
                                            net_ctx.int_val(0));
                smooth_flow_vec.push_back(smooth_flow_at_t);

                smooth_flow_at_t = ite(val1 && prev_empty && val2 && meta12 != meta22,
                                       net_ctx.int_val(1),
                                       net_ctx.int_val(0));
                smooth_flow_vec.push_back(smooth_flow_at_t);

                prev_empty = prev_empty && !val2;
            }
        }
    }

    z3_optimizer->add(sum(smooth_flow_vec) == 0, "smooth");

    // expr_vector diff_from_base_vec(net_ctx.z3_ctx());
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        for (unsigned int t = 0; t < total_time; t++) {
            // expr diff_from_base = ite(in_queues[q]->enq_cnt(t) == (int)base_eg->enqs[q][t],
            //                           net_ctx.int_val(0),
            //                           net_ctx.int_val(1));
            // diff_from_base_vec.push_back(diff_from_base);
            z3_optimizer->add_soft(in_queues[q]->enq_cnt(t) == (int) base_eg->enqs[q][t], 1);
        }
    }

    for (unsigned int q = 0; q < in_queues.size(); q++) {
        for (unsigned int t = 0; t < total_time; t++) {
            expr pkt = in_queues[q]->enqs(0)[t];
            if (base_eg->enqs_meta1[q][t].size() > 0) {
                z3_optimizer->add_soft(net_ctx.pkt2meta1(pkt) == (int) base_eg->enqs_meta1[q][t][0],
                                       1);
                z3_optimizer->add_soft(net_ctx.pkt2meta2(pkt) == (int) base_eg->enqs_meta2[q][t][0],
                                       1);
            }
        }
    }

    IndexedExample* eg = base_eg;
    unsigned int cap = EG_RANDOM_MOD_START;
    unsigned int mod_thresh = min(cap,
                                  (unsigned int) ceil(EG_RANDOM_MOD_PERCENT *
                                                      non_zero_queues.size() * total_time));

    unsigned int moving_avg_window = 5;
    deque<unsigned int> past_thresh;
    past_thresh.push_back(mod_thresh);

    for (unsigned int i = 1; i < count; i++) {
        if ((i + 1) % 10 == 0) cout << "generated " << (i + 1) << " good examples." << endl;

        bool found = false;

        for (int j = mod_thresh; j >= 0 && !found; j--) {
            for (unsigned int k = 0; k < EG_RANDOM_MOD_MAX_TRIES; k++) {

                expr mod_expr = get_random_eg_mod(eg, j, non_zero_queues);
                z3_optimizer->push();
                z3_optimizer->add(mod_expr, "prev_eg_constr");

                // z3_optimizer->minimize(sum(smooth_flow_vec));
                // z3_optimizer->minimize(sum(diff_from_base_vec));

                res = z3_optimizer->check();

                if (res == sat) {
                    found = true;

                    // DEBUG_MSG("changes: " << mod_expr << endl);
                    model m = z3_optimizer->get_model();

                    IndexedExample* new_eg = new IndexedExample();
                    populate_example_from_model(m, new_eg);
                    examples.push_back(new_eg);

                    // cout << m.eval(sum(smooth_flow_vec)).get_numeral_int() << endl;

                    DEBUG_MSG(*new_eg << endl);
                    expr_vector eg_vec(net_ctx.z3_ctx());
                    for (unsigned int q = 0; q < new_eg->enqs.size(); q++) {
                        for (unsigned int t = 0; t < new_eg->total_time; t++) {
                            eg_vec.push_back(in_queues[q]->enq_cnt(t) == (int) new_eg->enqs[q][t]);
                        }
                    }

                    expr not_this_example = !mk_and(eg_vec);
                    eg = new_eg;

                    z3_optimizer->pop();
                    snprintf(constr_name, 100, "example_%d", i);
                    z3_optimizer->add(not_this_example, constr_name);

                    past_thresh.push_back(j);
                    if (past_thresh.size() > moving_avg_window) {
                        past_thresh.pop_front();
                    }

                    double sum = 0.0;
                    for (unsigned int h = 0; h < past_thresh.size(); h++) {
                        sum += past_thresh[h];
                    }
                    mod_thresh = ceil(sum / past_thresh.size());

                    break;

                } else {
                    DEBUG_MSG(j << " mods didn't work" << endl);
                    z3_optimizer->pop();
                }
            }
        }
        if (!found) {
            // TODO: did we find all the traces??? can this help with the search?
            break;
        }
    }

    // POP
    z3_optimizer->pop();
}

void ContentionPoint::generate_good_examples2(IndexedExample* base_eg,
                                              unsigned int count,
                                              deque<IndexedExample*>& examples) {
    if (!is_shared_config_set()) {
        cout << "ERROR: SharedConfig is not set." << endl;
        return;
    }

    // PUSH
    z3_optimizer->push();

    // add base workload and query
    z3_optimizer->add(base_wl_expr, "base_wl");
    z3_optimizer->add(query_expr, "query");

    // add constraints to ensure the queues that
    // are "zero" in the base example will stay
    // zero in the rest of the examples too.
    char constr_name[100];
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        if (target_queues.find(q) != target_queues.end()) continue;

        Metric* cenq_metric = in_queues[q]->get_metric(metric_t::CENQ);
        for (unsigned int t = 0; t < total_time; t++) {
            snprintf(constr_name, 100, "%d_is_zerod_queue[%d}", q, t);
            expr constr_expr = cenq_metric->val(t).second == 0;
            z3_optimizer->add(constr_expr, constr_name);
        }
    }

    // Packets enqueued in the last time step will not
    // have enough time to be processed. To avoid arbitrary
    // numbers, ensure enq_cnt in the last time step is zero
    // in all examples.
    // Similarly, do not allow more than one packet in the
    // second to last time step (TODO: the exact constant
    // may need to be generalized.
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        snprintf(constr_name, 100, "%d_example_trimming", q);
        expr constr_expr = in_queues[q]->enq_cnt(total_time - 1) == 0 &&
                           in_queues[q]->enq_cnt(total_time - 2) <= 1;
        z3_optimizer->add(constr_expr, constr_name);
    }

    expr_vector smooth_flow_vec(net_ctx.z3_ctx());
    for (qset_t::iterator it = target_queues.begin(); it != target_queues.end(); it++) {
        unsigned int q = *it;
        for (unsigned int t = 0; t < total_time - 1; t++) {
            expr pkt1 = in_queues[q]->enqs(0)[t + 1];
            expr val1 = net_ctx.pkt2val(pkt1);
            expr meta11 = net_ctx.pkt2meta1(pkt1);
            expr meta12 = net_ctx.pkt2meta2(pkt1);

            expr prev_empty = net_ctx.bool_val(true);

            for (int t2 = t; t2 >= 0; t2--) {
                expr pkt2 = in_queues[q]->enqs(0)[t2];
                expr val2 = net_ctx.pkt2val(pkt2);
                expr meta21 = net_ctx.pkt2meta1(pkt2);
                expr meta22 = net_ctx.pkt2meta2(pkt2);

                expr smooth_flow_at_t = ite(val1 && prev_empty && val2 && meta11 != meta21,
                                            net_ctx.int_val(1),
                                            net_ctx.int_val(0));
                smooth_flow_vec.push_back(smooth_flow_at_t);

                smooth_flow_at_t = ite(val1 && prev_empty && val2 && meta12 != meta22,
                                       net_ctx.int_val(1),
                                       net_ctx.int_val(0));
                smooth_flow_vec.push_back(smooth_flow_at_t);

                prev_empty = prev_empty && !val2;
            }
        }
    }

    z3_optimizer->add(sum(smooth_flow_vec) == 0, "smooth");

    // Add soft constraints to minimize the distance
    // between the base example and the rest of the
    // examples, so that they are not radically different.
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        for (unsigned int t = 0; t < total_time; t++) {
            z3_optimizer->add_soft(in_queues[q]->enq_cnt(t) == (int) base_eg->enqs[q][t], 1);
        }
    }

    for (unsigned int q = 0; q < in_queues.size(); q++) {
        for (unsigned int t = 0; t < total_time; t++) {
            expr pkt = in_queues[q]->enqs(0)[t];
            if (base_eg->enqs_meta1[q][t].size() > 0) {
                z3_optimizer->add_soft(net_ctx.pkt2meta1(pkt) == (int) base_eg->enqs_meta1[q][t][0],
                                       1);
                z3_optimizer->add_soft(net_ctx.pkt2meta2(pkt) == (int) base_eg->enqs_meta2[q][t][0],
                                       1);
            }
        }
    }

    IndexedExample* eg = base_eg;
    unsigned int cap = EG_RANDOM_MOD_START;
    unsigned int mod_thresh = min(cap,
                                  (unsigned int) ceil(EG_RANDOM_MOD_PERCENT * target_queues.size() *
                                                      total_time));
    // unsigned int mod_thresh = 2;

    unsigned int moving_avg_window = 5;
    deque<unsigned int> past_thresh;
    past_thresh.push_back(mod_thresh);

    for (unsigned int i = 1; i < count; i++) {
        if ((i + 1) % 10 == 0) cout << "generated " << (i + 1) << " good examples." << endl;

        bool found = false;

        for (int j = mod_thresh; j >= 0 && !found; j--) {
            for (unsigned int k = 0; k < EG_RANDOM_MOD_MAX_TRIES; k++) {

                expr mod_expr = get_random_eg_mod(eg, j, target_queues);
                z3_optimizer->push();
                z3_optimizer->add(mod_expr, "prev_eg_constr");

                // z3_optimizer->minimize(sum(smooth_flow_vec));
                // z3_optimizer->minimize(sum(diff_from_base_vec));

                check_result res = z3_optimizer->check();

                if (res == sat) {
                    found = true;

                    // DEBUG_MSG("changes: " << mod_expr << endl);
                    model m = z3_optimizer->get_model();

                    IndexedExample* new_eg = new IndexedExample();
                    populate_example_from_model(m, new_eg);
                    examples.push_back(new_eg);

                    // cout << m.eval(sum(smooth_flow_vec)).get_numeral_int() << endl;

                    DEBUG_MSG(*new_eg << endl);
                    expr_vector eg_vec(net_ctx.z3_ctx());
                    for (unsigned int q = 0; q < new_eg->enqs.size(); q++) {
                        for (unsigned int t = 0; t < new_eg->total_time; t++) {
                            eg_vec.push_back(in_queues[q]->enq_cnt(t) == (int) new_eg->enqs[q][t]);
                        }
                    }

                    expr not_this_example = !mk_and(eg_vec);
                    eg = new_eg;

                    z3_optimizer->pop();
                    snprintf(constr_name, 100, "example_%d", i);
                    z3_optimizer->add(not_this_example, constr_name);

                    past_thresh.push_back(j);
                    if (past_thresh.size() > moving_avg_window) {
                        past_thresh.pop_front();
                    }

                    double sum = 0.0;
                    for (unsigned int h = 0; h < past_thresh.size(); h++) {
                        sum += past_thresh[h];
                    }
                    mod_thresh = ceil(sum / past_thresh.size());

                    break;

                } else {
                    DEBUG_MSG(j << " mods didn't work" << endl);
                    z3_optimizer->pop();
                }
            }
        }
        if (!found) {
            // TODO: did we find all the traces??? can this help with the search?
            break;
        }
    }

    // POP
    z3_optimizer->pop();
}

IndexedExample* ContentionPoint::index_example(Example* eg) {
    IndexedExample* ieg = new IndexedExample();
    ieg->total_time = eg->total_time;
    ieg->query_qid = eg->query_qid;

    for (unsigned int i = 0; i < in_queues.size(); i++) {
        cid_t qid = in_queues[i]->get_id();
        if (eg->enqs.find(qid) == eg->enqs.end()) {
            cout << "ContentionPoint::workload_satisifes_example: Example does not include "
                    "all queues."
                 << endl;
        }
        ieg->enqs.push_back(eg->enqs[qid]);
    }

    for (unsigned int i = 0; i < in_queues.size(); i++) {
        cid_t qid = in_queues[i]->get_id();
        if (eg->enqs_meta1.find(qid) == eg->enqs_meta1.end()) {
            cout << "ContentionPoint::workload_satisifes_example: Example does not include "
                    "all queues."
                 << endl;
        }
        ieg->enqs_meta1.push_back(eg->enqs_meta1[qid]);
    }

    for (unsigned int i = 0; i < in_queues.size(); i++) {
        cid_t qid = in_queues[i]->get_id();
        if (eg->enqs_meta2.find(qid) == eg->enqs_meta2.end()) {
            cout << "ContentionPoint::workload_satisifes_example: Example does not include "
                    "all queues."
                 << endl;
        }
        ieg->enqs_meta2.push_back(eg->enqs_meta2[qid]);
    }

    for (unsigned int i = 0; i < in_queues.size(); i++) {
        cid_t qid = in_queues[i]->get_id();
        if (eg->deqs.find(qid) == eg->deqs.end()) {
            cout << "ContentionPoint::workload_satisifes_example: Example does not include "
                    "all queues."
                 << endl;
        }
        ieg->deqs.push_back(eg->deqs[qid]);
    }

    return ieg;
}

Example* ContentionPoint::unindex_example(IndexedExample* ieg) {
    Example* eg = new Example();
    eg->total_time = ieg->total_time;
    eg->query_qid = ieg->query_qid;

    for (unsigned int i = 0; i < in_queues.size(); i++) {
        cid_t qid = in_queues[i]->get_id();
        eg->enqs[qid] = ieg->enqs[i];
        eg->enqs_meta1[qid] = ieg->enqs_meta1[i];
        eg->enqs_meta2[qid] = ieg->enqs_meta2[i];
        eg->deqs[qid] = ieg->deqs[i];
    }

    return eg;
}

void ContentionPoint::add_constr(expr const& e, char const* p) {
    z3_solver->add(e, p);
    z3_optimizer->add(e, p);
}

void ContentionPoint::add_constr_from_map(map<string, expr> new_constr_map) {
    map<string, expr>::iterator it;
    for (it = new_constr_map.begin(); it != new_constr_map.end(); it++) {
        add_constr(it->second, it->first.c_str());
    }

    constr_map.insert(new_constr_map.begin(), new_constr_map.end());
}

string ContentionPoint::get_model_str(model& m) {
    stringstream ss;


    for (unsigned int t = 0; t < total_time; t++) {
        ss << "---------- T = " << t << " ----------" << endl;
        ss << "input queues: " << endl;
        for (unsigned int q = 0; q < in_queues.size(); q++) {
            Queue* queue = in_queues[q];
            ss << queue->get_model_str(m, net_ctx, t) << endl;
        }

        ss << "output queues: " << endl;
        for (unsigned int q = 0; q < out_queues.size(); q++) {
            Queue* queue = out_queues[q];
            ss << queue->get_model_str(m, net_ctx, t) << endl;
        }

        ss << "other info: " << endl;
        ss << cp_model_str(m, net_ctx, t) << endl;


        for (unsigned int n = 0; n < nodes.size(); n++) {
            ss << "------------- " << endl;
            QueuingModule* qm = id_to_qm[nodes[n]];
            ss << qm->get_id() << endl;
            ss << "input queues: " << endl;
            for (unsigned int q = 0; q < qm->in_queue_cnt(); q++) {
                Queue* queue = qm->get_in_queue(q);
                ss << queue->get_model_str(m, net_ctx, t) << endl;
            }

            ss << "output queues: " << endl;
            for (unsigned int q = 0; q < qm->out_queue_cnt(); q++) {
                Queue* queue = qm->get_out_queue(q);
                ss << queue->get_model_str(m, net_ctx, t) << endl;
            }
        }
    }

    return ss.str();
}

void ContentionPoint::populate_example_from_model(model& m, IndexedExample* eg) {
    eg->total_time = total_time;
    eg->query_qid = query.get_qid();

    // Adding enqs
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        Queue* queue = in_queues[q];
        vector<unsigned int> enqs;
        for (unsigned int t = 0; t < total_time; t++) {
            expr enq_q_t = queue->enq_cnt(t);
            unsigned int enqs_t = m.eval(enq_q_t).get_numeral_uint();
            enqs.push_back(enqs_t);
        }
        eg->enqs.push_back(enqs);
    }

    // Adding enqs_meta1 and enqs_meta2
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        Queue* queue = in_queues[q];
        vector<vector<int>> enqs_meta1;
        vector<vector<int>> enqs_meta2;
        for (unsigned int t = 0; t < total_time; t++) {
            vector<int> meta1;
            vector<int> meta2;

            for (unsigned int p = 0; p < queue->max_enq(); p++) {
                expr pkt = queue->enqs(p)[t];
                expr val = net_ctx.pkt2val(pkt);
                if (m.eval(val).is_true()) {
                    expr m1 = m.eval(net_ctx.pkt2meta1(pkt));
                    expr m2 = m.eval(net_ctx.pkt2meta2(pkt));

                    // TODO: FIXME
                    int meta1val = m1.is_numeral() ? m1.get_numeral_int() : 1000;
                    meta1.push_back(meta1val);
                    int meta2val = m2.is_numeral() ? m2.get_numeral_int() : 1000;
                    meta2.push_back(meta2val);
                }
            }

            enqs_meta1.push_back(meta1);
            enqs_meta2.push_back(meta2);
        }
        eg->enqs_meta1.push_back(enqs_meta1);
        eg->enqs_meta2.push_back(enqs_meta2);
    }

    // Adding deqs
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        Queue* queue = in_queues[q];
        vector<unsigned int> deqs;
        for (unsigned int t = 0; t < total_time; t++) {
            unsigned int deq_t = m.eval(queue->deq_cnt(t)).get_numeral_uint();
            deqs.push_back(deq_t);
        }
        eg->deqs.push_back(deqs);
    }
}

string ContentionPoint::stats_str() {
    stringstream ss;

    ss << "z3 statistics: " << endl;
    stats sts = z3_solver->statistics();
    for (unsigned int i = 0; i < sts.size(); i++) {
        ss << sts.key(i) << ": ";
        if (sts.is_uint(i)) {
            ss << sts.uint_value(i) << endl;
        } else if (sts.is_double(i)) {
            ss << sts.double_value(i) << endl;
        }
    }

    return ss.str();
}

ostream& operator<<(ostream& os, const ContentionPoint& p) {
    os << "nodes: ";
    for (unsigned int i = 0; i < p.nodes.size(); i++) {
        os << p.nodes.at(i) << " ";
    }
    os << endl;

    os << "in_queues: ";
    for (unsigned int i = 0; i < p.in_queues.size(); i++) {
        os << (*p.in_queues.at(i)) << " ";
    }
    os << endl;

    os << "out_queues: ";
    for (unsigned int i = 0; i < p.out_queues.size(); i++) {
        os << (*p.out_queues.at(i)) << " ";
    }
    os << endl;
    os << endl;

    return os;
}

expr ContentionPoint::mk_op(expr lhs, op_t op, expr rhs) {
    switch (op) {
        case (op_t::GT): return lhs > rhs;
        case (op_t::GE): return lhs >= rhs;
        case (op_t::LT): return lhs < rhs;
        case (op_t::LE): return lhs <= rhs;
        case (op_t::EQ): return lhs == rhs;
    }
    cout << "FPerfModel::mk_op: should not reach here" << endl;
    return lhs >= rhs;
}

unsigned int ContentionPoint::in_queue_cnt() {
    return (unsigned int) in_queues.size();
}

vector<Queue*> ContentionPoint::get_in_queues() {
    return in_queues;
}

unsigned int ContentionPoint::out_queue_cnt() {
    return (unsigned int) out_queues.size();
}

Queue* ContentionPoint::get_out_queue(unsigned int ind) {
    return out_queues[ind];
}


Query ContentionPoint::get_query() {
    return query;
}

bool ContentionPoint::is_query_set() {
    return query_is_set;
}

QueuingModule* ContentionPoint::get_qm(cid_t id) {
    return id_to_qm[id];
}

bool ContentionPoint::set_shared_config(SharedConfig* shared_config) {
    if (!is_shared_config_set()) {
        shared_config_is_set = true;
        this->shared_config = shared_config;
        target_queues = shared_config->get_target_queues();
        dists = shared_config->get_dists();
        if (total_time != shared_config->get_total_time()) {
            cout << "Total time in contention point and shared config are different" << endl;
            return false;
        }
        if (in_queues.size() != shared_config->get_in_queue_cnt()) {
            cout << "Number of input queues in contention point and shared config are different"
                 << endl;
            return false;
        }
        return true;
    }
    return false;
}

bool ContentionPoint::is_shared_config_set() {
    return shared_config_is_set;
}

unsigned int ContentionPoint::get_total_time() {
    return total_time;
}

solver_res_t ContentionPoint::get_solver_res_t(check_result z3_res) {
    switch (z3_res) {
        case unsat: {
            return solver_res_t::UNSAT;
        }
        case sat: {
            return solver_res_t::SAT;
        }
        case unknown: {
            return solver_res_t::UNKNOWN;
        }
    }
    cout << "get_solver_res_t:: Should not reach here" << endl;
    return solver_res_t::UNKNOWN;
}

/* *********** Generating Z3 Expressions ************ */

expr ContentionPoint::get_expr(Query& query) {
    expr query_expr(net_ctx.z3_ctx());
    metric_t m = query.get_metric();

    if (metrics.find(m) == metrics.end()) {
        cout << "ContentionPoint::get_expr(Query): Invalid Query" << endl;
        query_expr = net_ctx.bool_val(false);
        return query_expr;
    }

    query_lhs_t query_lhs = query.get_lhs();
    vector<Metric*> query_metrics;

    switch (query_lhs.index()) {
        // one queue
        case 0: {
            cid_t qid = get<cid_t>(query_lhs);
            if (metrics[m].find(qid) == metrics[m].end()) {
                cout << "ContentionPoint::get_expr(Query): Invalid Query" << endl;
                query_expr = net_ctx.bool_val(false);
                return query_expr;
            }
            query_metrics.push_back(metrics[m][qid]);
            break;
        }
        // qdiff
        case 1: {
            qdiff_t ids = get<qdiff_t>(query_lhs);
            cid_t qid1 = ids.first;
            cid_t qid2 = ids.second;

            if (metrics[m].find(qid1) == metrics[m].end() ||
                metrics[m].find(qid2) == metrics[m].end()) {
                cout << "ContentionPoint::get_expr(Query): Invalid Query" << endl;
                query_expr = net_ctx.bool_val(false);
                return query_expr;
            }
            query_metrics.push_back(metrics[m][qid1]);
            query_metrics.push_back(metrics[m][qid2]);
            break;
        }

        // qsum
        case 2: {
            qsum_t ids = get<qsum_t>(query_lhs);
            for (unsigned int i = 0; i < ids.size(); i++) {
                cid_t id = ids[i];
                if (metrics[m].find(id) == metrics[m].end()) {
                    cout << "ContentionPoint::get_expr(Query): Invalid Query" << endl;
                    query_expr = net_ctx.bool_val(false);
                    return query_expr;
                }
                query_metrics.push_back(metrics[m][id]);
            }
            break;
        }

        default: {
            cout << "ContentionPoint::get_expr(Query): Invalid Query" << endl;
            query_expr = net_ctx.bool_val(false);
            return query_expr;
        }
    }

    expr_vector query_vec(net_ctx.z3_ctx());
    time_range_t time_range = query.get_time_range();
    for (unsigned int t = time_range.first; t <= time_range.second; t++) {
        expr_vector valid(net_ctx.z3_ctx());
        expr rhs = net_ctx.int_val(query.get_thresh());
        expr lhs(net_ctx.z3_ctx());

        switch (query_lhs.index()) {
            // one queue
            case 0: {
                m_val_expr_t val_expr = query_metrics[0]->val(t);
                valid.push_back(val_expr.first);
                lhs = val_expr.second;
                break;
            }

            // qdiff
            case 1: {
                m_val_expr_t val_expr1 = query_metrics[0]->val(t);
                m_val_expr_t val_expr2 = query_metrics[1]->val(t);
                valid.push_back(val_expr1.first);
                valid.push_back(val_expr2.first);
                lhs = val_expr1.second - val_expr2.second;
                break;
            }

            // qsum
            case 2: {
                m_val_expr_t val_expr = query_metrics[0]->val(t);
                valid.push_back(val_expr.first);
                lhs = val_expr.second;
                for (unsigned int i = 1; i < query_metrics.size(); i++) {
                    val_expr = query_metrics[i]->val(t);
                    valid.push_back(val_expr.first);
                    lhs = lhs + val_expr.second;
                }
                break;
            }

            default: {
                cout << "ContentionPoint::get_expr(Query): Invalid Query" << endl;
                query_expr = net_ctx.bool_val(false);
                return query_expr;
            }
        }

        query_vec.push_back(mk_and(valid) && mk_op(lhs, query.get_op(), rhs));
    }

    switch (query.get_quant()) {
        case query_quant_t::FORALL: query_expr = mk_and(query_vec); break;
        case query_quant_t::EXISTS: query_expr = mk_or(query_vec); break;
        default: cout << "ContentionPoint::get_expr(Query): Invalid Query" << endl;
    }

    return query_expr;
}

expr ContentionPoint::get_expr(IndexedExample* eg, vector<metric_t>& metrics) {
    expr_vector res_vec(net_ctx.z3_ctx());

    for (unsigned int q = 0; q < in_queues.size(); q++) {
        Queue* queue = in_queues[q];
        for (unsigned int i = 0; i < metrics.size(); i++) {
            Metric* m = queue->get_metric(metrics[i]);
            for (unsigned int t = 0; t < total_time; t++) {
                m_val_expr_t m_q_t = m->val(t);
                metric_val eg_val;
                m->eval(eg, t, q, eg_val);
                if (eg_val.valid) {
                    res_vec.push_back(m_q_t.first && (m_q_t.second == (int) eg_val.value));
                } else {
                    res_vec.push_back(!m_q_t.first);
                }
            }
        }
    }

    return mk_and(res_vec);
}

expr ContentionPoint::get_expr(IndexedExample* eg) {
    expr_vector res_vec(net_ctx.z3_ctx());

    // enqs
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        Queue* queue = in_queues[q];
        for (unsigned int t = 0; t < total_time; t++) {
            expr enq_q_t = queue->enq_cnt(t);
            res_vec.push_back(enq_q_t == (int) eg->enqs[q][t]);
        }
    }

    // enqs_meta1 and enqs_meta2
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        Queue* queue = in_queues[q];
        for (unsigned int t = 0; t < total_time; t++) {
            for (unsigned int p = 0; p < eg->enqs_meta1[q][t].size(); p++) {
                expr pkt = queue->enqs(p)[t];
                expr val = net_ctx.pkt2val(pkt);
                expr m1 = net_ctx.pkt2meta1(pkt);
                expr m2 = net_ctx.pkt2meta2(pkt);

                res_vec.push_back(val);
                res_vec.push_back(m1 == (int) eg->enqs_meta1[q][t][p]);
                res_vec.push_back(m2 == (int) eg->enqs_meta2[q][t][p]);
            }
        }
    }

    return mk_and(res_vec);
}

expr ContentionPoint::get_expr(Workload wl) {
    if (wl.is_empty()) return net_ctx.bool_val(false);
    if (wl.is_all()) return net_ctx.bool_val(true);

    expr_vector res(net_ctx.z3_ctx());
    set<TimedSpec> specs = wl.get_all_specs();
    for (set<TimedSpec>::iterator it = specs.begin(); it != specs.end(); it++) {
        res.push_back(get_expr(*it));
    }
    return mk_and(res);
}

expr ContentionPoint::get_expr(TimedSpec spec) {
    time_range_t time_range = spec.get_time_range();
    WlSpec* wl_spec = spec.get_wl_spec();

    // Dynamic casting to determine the specific type of WlSpec
    if (auto compSpec = dynamic_cast<Comp*>(wl_spec)) {
        return get_expr(*compSpec, time_range);
    } else if (auto sameSpec = dynamic_cast<Same*>(wl_spec)) {
        return get_expr(*sameSpec, time_range);
    } else if (auto incrSpec = dynamic_cast<Incr*>(wl_spec)) {
        return get_expr(*incrSpec, time_range);
    } else if (auto decrSpec = dynamic_cast<Decr*>(wl_spec)) {
        return get_expr(*decrSpec, time_range);
    } else if (auto uniqSpec = dynamic_cast<Unique*>(wl_spec)) {
        return get_expr(*uniqSpec, time_range);
    } else {
        cout << "ContentionPoint::get_expr(TimedSpec): Invalid WlSpec" << endl;
        return net_ctx.bool_val(false);
    }
}

expr ContentionPoint::get_expr(Unique uniq, time_range_t time_range) {
    qset_t qset = uniq.get_qset();
    metric_t metric = uniq.get_metric();
    vector<unsigned int> queues(qset.begin(), qset.end());
    unsigned int start = time_range.first;
    unsigned int end = time_range.second;

    expr_vector unique_metric(net_ctx.z3_ctx());

    for (unsigned int t = start; t <= end; t++) {
        for (unsigned int i = 0; i < queues.size(); i++) {
            unsigned int q = queues[i];
            Queue* queue = in_queues[q];
            Metric* metric1 = queue->get_metric(metric);
            m_val_expr_t metric1_val_expr = metric1->val(t);
            expr m1_valid = metric1_val_expr.first;
            expr m1_value = metric1_val_expr.second;

            for (unsigned int j = i + 1; j < queues.size(); j++) {
                unsigned int q2 = queues[j];
                if (q2 == q) continue;
                Queue* queue2 = in_queues[q2];
                Metric* metric2 = queue2->get_metric(metric);
                m_val_expr_t metric2_val_expr = metric2->val(t);
                expr m2_valid = metric2_val_expr.first;
                expr m2_value = metric2_val_expr.second;
                unique_metric.push_back(implies(m1_valid && m2_valid, m1_value != m2_value));
            }
        }
    }
    expr constr = mk_and(unique_metric);
    return constr;
}

expr ContentionPoint::get_expr(Same same, time_range_t time_range) {
    Queue* queue = in_queues[same.get_queue()];
    Metric* metric = queue->get_metric(same.get_metric());
    m_val_expr_t initial_val_expr = metric->val(time_range.first);

    expr valid_expr = initial_val_expr.first;
    expr init_value_expr = initial_val_expr.second;

    expr_vector res(net_ctx.z3_ctx());
    res.push_back(valid_expr);
    expr_vector no_enq(net_ctx.z3_ctx());

    for (unsigned int t = time_range.first + 1; t <= time_range.second; t++) {
        m_val_expr_t val_expr = metric->val(t);
        res.push_back(val_expr.first);
        res.push_back(val_expr.second == init_value_expr);
        no_enq.push_back(queue->enq_cnt(t) == 0);
    }

    expr on_enq = mk_and(res);
    return ite(mk_and(no_enq), net_ctx.bool_val(true), on_enq);
}

expr ContentionPoint::get_expr(Incr incr, time_range_t time_range) {
    Queue* queue = in_queues[incr.get_queue()];
    Metric* metric = queue->get_metric(incr.get_metric());
    m_val_expr_t initial_val_expr = metric->val(time_range.first);

    expr valid_expr = initial_val_expr.first;
    expr value_expr = initial_val_expr.second;

    expr_vector res(net_ctx.z3_ctx());
    res.push_back(valid_expr);

    for (unsigned int t = time_range.first + 1; t <= time_range.second; t++) {
        m_val_expr_t val_expr = metric->val(t);
        res.push_back(val_expr.first);
        res.push_back(val_expr.second > value_expr);
        value_expr = val_expr.second;
    }

    return mk_and(res);
}

expr ContentionPoint::get_expr(Decr decr, time_range_t time_range) {
    Queue* queue = in_queues[decr.get_queue()];
    Metric* metric = queue->get_metric(decr.get_metric());
    m_val_expr_t initial_val_expr = metric->val(time_range.first);

    expr valid_expr = initial_val_expr.first;
    expr value_expr = initial_val_expr.second;

    expr_vector res(net_ctx.z3_ctx());
    res.push_back(valid_expr);

    for (unsigned int t = time_range.first + 1; t <= time_range.second; t++) {
        m_val_expr_t val_expr = metric->val(t);
        res.push_back(val_expr.first);
        res.push_back(val_expr.second < value_expr);
        value_expr = val_expr.second;
    }

    return mk_and(res);
}

expr ContentionPoint::get_expr(Comp comp, time_range_t time_range) {
    expr_vector res(net_ctx.z3_ctx());
    for (unsigned int t = time_range.first; t <= time_range.second; t++) {
        res.push_back(get_expr(comp, t));
    }
    return mk_and(res);
}

expr ContentionPoint::get_expr(Comp comp, unsigned int t) {
    m_val_expr_t lhs = get_expr(comp.get_lhs(), t);
    m_val_expr_t rhs = get_expr(comp.get_rhs(), t);
    return (lhs.first && rhs.first && mk_op(lhs.second, comp.get_op(), rhs.second));
}

m_val_expr_t ContentionPoint::get_expr(rhs_t rhs, unsigned int t) {
    return visit([t, this](auto const& sf) { return this->get_expr(sf, t); }, rhs);
}

m_val_expr_t ContentionPoint::get_expr(lhs_t lhs, unsigned int t) {
    return visit([t, this](auto const& sf) { return this->get_expr(sf, t); }, lhs);
}

m_val_expr_t ContentionPoint::get_expr(unsigned int c, unsigned int t) {
    (void) t;
    return m_val_expr_t(net_ctx.bool_val(true), net_ctx.int_val(c));
}

m_val_expr_t ContentionPoint::get_expr(Time time, unsigned int t) {
    return m_val_expr_t(net_ctx.bool_val(true), net_ctx.int_val(time.get_coeff() * (t + 1)));
}

m_val_expr_t ContentionPoint::get_expr(Indiv indiv, unsigned int t) {
    Queue* queue = in_queues[indiv.get_queue()];
    Metric* metric = queue->get_metric(indiv.get_metric());
    // TODO: This a temporary fix, after parametrizing search based on metrics we need to throw
    // exception and stop the search if metric not exists
    if (metric != nullptr)
        return metric->val(t);
    else
        return {net_ctx.bool_val(false), net_ctx.int_val(0)};
}

m_val_expr_t ContentionPoint::get_expr(QSum qsum, unsigned int t) {
    expr res = net_ctx.int_val(0);
    expr_vector valid(net_ctx.z3_ctx());

    qset_t qset = qsum.get_qset();
    for (qset_t::iterator it = qset.begin(); it != qset.end(); it++) {
        unsigned int q = *it;
        m_val_expr_t val_expr = in_queues[q]->get_metric(qsum.get_metric())->val(t);
        valid.push_back(val_expr.first);
        res = res + val_expr.second;
    }
    return m_val_expr_t(mk_and(valid), res);
}


/* *********** Workload Satisifes Example ************ */

bool ContentionPoint::workload_satisfies_example(Workload wl, IndexedExample* eg) {
    set<TimedSpec> specs = wl.get_all_specs();
    for (set<TimedSpec>::iterator it = specs.begin(); it != specs.end(); it++) {
        if (!timedspec_satisfies_example(*it, eg)) {
            return false;
        }
    }
    return true;
}


bool ContentionPoint::timedspec_satisfies_example(TimedSpec spec, IndexedExample* eg) {
    time_range_t time_range = spec.get_time_range();
    if (eg->total_time - 1 < time_range.first) return false;

    WlSpec* wlspec = spec.get_wl_spec();

    if(auto compSpec = dynamic_cast<Comp*>(wlspec)) {
        return eval_spec(*compSpec, eg, time_range);
    } else if(auto incrSpec = dynamic_cast<Incr*>(wlspec)) {
        return eval_spec(*incrSpec, eg, time_range);
    } else if(auto decrSpec = dynamic_cast<Decr*>(wlspec)) {
        return eval_spec(*decrSpec, eg, time_range);
    } else if(auto sameSpec = dynamic_cast<Same*>(wlspec)) {
        return eval_spec(*sameSpec, eg, time_range);
    } else if(auto uniqueSpec = dynamic_cast<Unique*>(wlspec)) {
        return eval_spec(*uniqueSpec, eg, time_range);
    } else {
        throw std::runtime_error("ContentionPoint::timedspec_satisfies_example: Invalid WlSpec");
    }
}

bool ContentionPoint::eval_spec(Unique uniq, IndexedExample* eg, time_range_t time_range) const {
    for (unsigned int t = time_range.first + 1; t <= time_range.second; t++) {
        set<int>::size_type vals_count = 0;
        set<int> unique_vals;
        for (unsigned int queue : uniq.get_qset()) {
            Metric* metric = in_queues[queue]->get_metric(uniq.get_metric());
            metric_val m_val;
            metric->eval(eg, t, queue, m_val);
            if (m_val.valid) {
                vals_count++;
                unique_vals.insert(m_val.value);
            }
        }
        if (vals_count != unique_vals.size()) return false;
    }
    return true;
}

bool ContentionPoint::eval_spec(Same same, IndexedExample* eg, time_range_t time_range) const {
    unsigned int queue = same.get_queue();
    Metric* metric = in_queues[queue]->get_metric(same.get_metric());
    metric_val m_val;
    metric->eval(eg, time_range.first, queue, m_val);
    if (!m_val.valid) return false;

    unsigned int first_value = m_val.value;

    for (unsigned int t = time_range.first + 1; t <= time_range.second; t++) {
        metric->eval(eg, t, queue, m_val);
        if (!m_val.valid) return false;
        if (m_val.value != first_value) return false;
    }
    return true;
}

bool ContentionPoint::eval_spec(Incr incr, IndexedExample* eg, time_range_t time_range) const {
    unsigned int queue = incr.get_queue();
    Metric* metric = in_queues[queue]->get_metric(incr.get_metric());
    metric_val m_val;
    metric->eval(eg, time_range.first, queue, m_val);
    if (!m_val.valid) return false;

    unsigned int m_value = m_val.value;

    for (unsigned int t = time_range.first + 1; t <= time_range.second; t++) {
        metric->eval(eg, t, queue, m_val);
        if (!m_val.valid) return false;
        if (m_val.value <= m_value) return false;
        m_value = m_val.value;
    }
    return true;
}

bool ContentionPoint::eval_spec(Decr decr, IndexedExample* eg, time_range_t time_range) const {
    unsigned int queue = decr.get_queue();
    Metric* metric = in_queues[queue]->get_metric(decr.get_metric());
    metric_val m_val;
    metric->eval(eg, time_range.first, queue, m_val);
    if (!m_val.valid) return false;

    unsigned int m_value = m_val.value;

    for (unsigned int t = time_range.first + 1; t <= time_range.second; t++) {
        metric->eval(eg, t, queue, m_val);
        if (!m_val.valid) return false;
        if (m_val.value >= m_value) return false;
        m_value = m_val.value;
    }
    return true;
}

bool ContentionPoint::eval_spec(Comp comp, IndexedExample* eg, time_range_t time_range) const {
    for (unsigned int t = time_range.first; t <= time_range.second; t++) {
        metric_val lhs_m_val, rhs_m_val;

        eval_lhs(comp.get_lhs(), eg, t, lhs_m_val);
        if (!lhs_m_val.valid) return false;

        eval_rhs(comp.get_rhs(), eg, t, rhs_m_val);
        if (!rhs_m_val.valid) return false;

        unsigned int lhs_val = lhs_m_val.value;
        unsigned int rhs_val = rhs_m_val.value;
        bool res = eval_op(lhs_val, comp.get_op(), rhs_val);
        if (!res) return false;
    }

    return true;
}

void ContentionPoint::eval_rhs(rhs_t rhs,
                               IndexedExample* eg,
                               unsigned int time,
                               metric_val& res) const {
    if (holds_alternative<m_expr_t>(rhs)) {
        eval_m_expr(get<m_expr_t>(rhs), eg, time, res);
    } else if (holds_alternative<Time>(rhs)) {
        eval_Time(get<Time>(rhs), eg, time, res);
    } else if (holds_alternative<unsigned int>(rhs)) {
        res.valid = true;
        res.value = get<unsigned int>(rhs);
    } else {
        cout << "ContentionPoint::eval_rhs: invalid variant." << endl;
    }
}

void ContentionPoint::eval_lhs(lhs_t lhs,
                               IndexedExample* eg,
                               unsigned int time,
                               metric_val& res) const {
    eval_m_expr(lhs, eg, time, res);
}

void ContentionPoint::eval_m_expr(m_expr_t m_expr,
                                  IndexedExample* eg,
                                  unsigned int time,
                                  metric_val& res) const {
    visit([&eg, time, &res, this](auto const& sf) { this->eval_m_expr(sf, eg, time, res); },
          m_expr);
}

void ContentionPoint::eval_m_expr(QSum qsum,
                                  IndexedExample* eg,
                                  unsigned int time,
                                  metric_val& res) const {
    res.value = 0;
    qset_t qset = qsum.get_qset();
    metric_t metric = qsum.get_metric();
    for (qset_t::iterator it = qset.begin(); it != qset.end(); it++) {
        unsigned int q = *it;
        metric_val m_val;
        in_queues[q]->get_metric(metric)->eval(eg, time, q, m_val);
        if (!m_val.valid) {
            res.valid = false;
            return;
        }
        res.value += m_val.value;
    }
    res.valid = true;
}

void ContentionPoint::eval_m_expr(Indiv indiv,
                                  IndexedExample* eg,
                                  unsigned int time,
                                  metric_val& res) const {
    unsigned int queue = indiv.get_queue();
    Metric* metric = in_queues[queue]->get_metric(indiv.get_metric());
    // TODO: This a temporary fix, after parametrizing search based on metrics we need to throw
    // exception and stop the search if metric not exists
    if (metric != nullptr)
        metric->eval(eg, time, queue, res);
    else
        res.valid = false;
}

void ContentionPoint::eval_Time(Time time,
                                IndexedExample* eg,
                                unsigned int t,
                                metric_val& res) const {
    (void) eg;
    unsigned int coeff = time.get_coeff();
    res.value = coeff * (t + 1);
    res.valid = true;
}
