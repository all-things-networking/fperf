//
//  workload.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/19/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "spec_factory.hpp"

#include <algorithm>
#include <set>
#include <sstream>

#include "util.hpp"

SpecFactory::SpecFactory(SharedConfig* shared_config):
shared_config(shared_config),
dists(shared_config->get_dists()) {
    total_time = shared_config->get_total_time();
    in_queue_cnt = shared_config->get_in_queue_cnt();
    target_queues = shared_config->get_target_queues();
}

RandomSpecGenerationParameters SpecFactory::get_metric_params(metric_t metric_type) {
    switch (metric_type) {
        case metric_t::DST: return {false, dists->get_pkt_meta1_val_dist()};
        case metric_t::ECMP: return {false, dists->get_pkt_meta2_val_dist()};
        default: return {true, dists->get_rhs_const_dist()};
    }
}

//************************************* TimedSpec *************************************//

TimedSpec SpecFactory::random_timed_spec() {
    wl_spec_t wl_spec = random_wl_spec();

    unsigned int time_range_lb = dists->timestep();
    unsigned int time_range_ub = dists->timestep();
    while (time_range_ub < time_range_lb) {
        time_range_lb = dists->timestep();
        time_range_ub = dists->timestep();
    }

    return TimedSpec(wl_spec, time_range_t(time_range_lb, time_range_ub), total_time);
}


void SpecFactory::pick_neighbors(TimedSpec& spec, vector<TimedSpec>& neighbors) {
    wl_spec_t wl_spec = spec.get_wl_spec();
    time_range_t time_range = spec.get_time_range();

    // Changing wl_spec
    vector<wl_spec_t> wl_spec_neighbors;
    pick_neighbors(wl_spec, wl_spec_neighbors);
    for (unsigned int i = 0; i < wl_spec_neighbors.size(); i++) {
        TimedSpec nei = TimedSpec(wl_spec_neighbors[i], time_range, total_time);
        if (!nei.spec_is_empty() && !nei.spec_is_all()) {
            neighbors.push_back(nei);
        }
    }

    // changing time

    mt19937& gen = dists->get_gen();
    // change lb
    if (time_range.second > 0) {
        uniform_int_distribution<unsigned int> lb_dist(0, time_range.second);
        time_range_t time_range_neighbor(lb_dist(gen), time_range.second);
        while (time_range_neighbor == time_range) {
            time_range_neighbor = time_range_t(lb_dist(gen), time_range.second);
        }
        TimedSpec nei = TimedSpec(wl_spec, time_range_neighbor, total_time);
        neighbors.push_back(nei);
    }

    if (time_range.first < total_time - 1) {
        uniform_int_distribution<unsigned int> ub_dist(time_range.first, total_time - 1);
        time_range_t time_range_neighbor(time_range.first, ub_dist(gen));
        while (time_range_neighbor == time_range) {
            time_range_neighbor = time_range_t(time_range.first, ub_dist(gen));
        }
        TimedSpec nei = TimedSpec(wl_spec, time_range_neighbor, total_time);
        neighbors.push_back(nei);
    }
}

//************************************* WlSpec *************************************//

wl_spec_t SpecFactory::random_wl_spec() {
    return random_comp();
}

void SpecFactory::pick_neighbors(wl_spec_t& spec, vector<wl_spec_t>& neighbors) {
    if (holds_alternative<Comp>(spec)) {
        vector<Comp> comp_neighbors;
        pick_neighbors(get<Comp>(spec), comp_neighbors);
        neighbors.insert(neighbors.end(), comp_neighbors.begin(), comp_neighbors.end());
    }
}

//************************************* COMP *************************************//

Comp SpecFactory::random_comp() {
    Comp* res;

    do {
        lhs_t lhs = random_lhs();
        rhs_t rhs = random_rhs();
        if (holds_alternative<Indiv>(lhs)) {
            metric_t metric = get<Indiv>(lhs).get_metric();
            rhs = random_rhs(get_metric_params(metric));
        }
        op_t op = random_op();
        res = new Comp(lhs, op, rhs);
    } while (res->spec_is_empty() || res->spec_is_all());

    return *res;
}

void SpecFactory::pick_neighbors(Comp& spec, vector<Comp>& neighbors) {
    lhs_t lhs = spec.get_lhs();
    op_t op = spec.get_op();
    rhs_t rhs = spec.get_rhs();

    // Changing lhs
    vector<lhs_t> lhs_neighbors;
    pick_lhs_neighbors(lhs, lhs_neighbors);
    for (unsigned int i = 0; i < lhs_neighbors.size(); i++) {
        Comp nei = Comp(lhs_neighbors[i], op, rhs);
        if (!nei.spec_is_empty() && !nei.spec_is_all()) {
            neighbors.push_back(nei);
        }
    }

    // Changing op
    op_t new_op = random_op();
    while (new_op == op) {
        new_op = random_op();
    }
    Comp nei = Comp(lhs, new_op, rhs);
    if (!nei.spec_is_empty() && !nei.spec_is_all()) {
        neighbors.push_back(nei);
    }

    // Changing rhs
    vector<rhs_t> rhs_neighbors;
    if (holds_alternative<Indiv>(lhs)) {
        metric_t metric = get<Indiv>(lhs).get_metric();
        pick_rhs_neighbors(rhs, rhs_neighbors, get_metric_params(metric));
    } else {
        pick_rhs_neighbors(rhs, rhs_neighbors);
    }
    for (unsigned int i = 0; i < rhs_neighbors.size(); i++) {
        Comp nei = Comp(lhs, op, rhs_neighbors[i]);
        if (!nei.spec_is_empty() && !nei.spec_is_all()) {
            neighbors.push_back(nei);
        }
    }
}

//************************************* RHS *************************************//

rhs_t SpecFactory::random_rhs() {
    return random_rhs({true, dists->get_rhs_const_dist()});
}

rhs_t SpecFactory::random_rhs(RandomSpecGenerationParameters params) {
    unsigned int rhs_type = dists->rhs();
    while (rhs_type == 0) {
        rhs_type = dists->rhs();
    }

    if (params.time_valid && rhs_type == 1) return random_time();

    mt19937& gen = dists->get_gen();
    return params.const_dist(gen);
}

void SpecFactory::pick_rhs_neighbors(rhs_t rhs, vector<rhs_t>& neighbors) {
    pick_rhs_neighbors(rhs, neighbors, {true, dists->get_rhs_const_dist(), false});
}

void SpecFactory::pick_rhs_neighbors(rhs_t rhs,
                                     vector<rhs_t>& neighbors,
                                     RandomSpecGenerationParameters params) {
    switch (rhs.index()) {
        // trf
        case 0: {
            m_expr_t trf = get<m_expr_t>(rhs);
            vector<m_expr_t> trf_neighbors;
            pick_m_expr_neighbors(trf, trf_neighbors);
            neighbors.insert(neighbors.end(), trf_neighbors.begin(), trf_neighbors.end());
            break;
        }
        // TIME
        case 1: {
            Time time = get<Time>(rhs);
            Time time_neighbor = random_time();
            while (time_neighbor == time) {
                time_neighbor = random_time();
            }
            neighbors.push_back(time_neighbor);

            unsigned int c = time.get_coeff();
            if (params.bound_with_dist) {
                if (c > params.const_dist.max()) c = params.const_dist.max();
                if (c < params.const_dist.min()) c = params.const_dist.min();
            }
            neighbors.push_back(c);
            break;
        }
        // C
        case 2: {
            mt19937& gen = dists->get_gen();
            unsigned int c = get<unsigned int>(rhs);
            unsigned int c_neighbor = params.const_dist(gen);
            while (c_neighbor == c) {
                c_neighbor = params.const_dist(gen);
            }
            neighbors.push_back(c_neighbor);

            if (params.time_valid) {
                neighbors.push_back(Time(1u));
            }

            break;
        }
        default: break;
    }
}

//************************************* LHS *************************************//

lhs_t SpecFactory::random_lhs() {
    return random_m_expr();
}

void SpecFactory::pick_lhs_neighbors(lhs_t lhs, vector<lhs_t>& neighbors) {
    pick_m_expr_neighbors(lhs, neighbors);
}

//************************************* TRF *************************************//

m_expr_t SpecFactory::random_m_expr() {
    unsigned int trf_type = dists->trf();
    if (target_queues.size() < 2) trf_type = 1;
    switch (trf_type) {
        case 0: return random_qsum();
        default: return random_indiv();
    }
}

void SpecFactory::pick_m_expr_neighbors(m_expr_t m_expr, vector<m_expr_t>& neighbors) {
    switch (m_expr.index()) {
        // QSUM
        case 0: {
            pick_neighbors(get<QSum>(m_expr), neighbors);
            break;
        }
        // INDIV
        case 1: {
            pick_neighbors(get<Indiv>(m_expr), neighbors);
            break;
        }
        default: break;
    }
}

//************************************* QSUM *************************************//

qset_t SpecFactory::random_qsum_qset() {
    qset_t res;
    unsigned int cnt = dists->input_queue_cnt();
    while (cnt < 2 || cnt > target_queues.size()) {
        cnt = dists->input_queue_cnt();
    }

    while (res.size() < cnt) {
        unsigned int q = dists->input_queue();

        while (target_queues.find(q) == target_queues.end()) {
            q = dists->input_queue();
        }

        res.insert(q);
    }
    return res;
}

QSum SpecFactory::random_qsum() {
    // TODO: generalize this to aggregatable metrics
    metric_t metric = metric_t::CENQ;
    qset_t qset = random_qsum_qset();
    return QSum(qset, metric);
}


void SpecFactory::pick_neighbors(QSum& qsum, vector<m_expr_t>& neighbors) {
    // TODO: change metric, when there is more than one aggregatable metric

    // changing qset
    qset_t qset = qsum.qset;
    // add one to qset
    unsigned int max_size = target_queues.size();
    if (qset.size() < max_size) {
        qset_t qset_neighbor = qset;
        while (qset_neighbor.size() == qset.size()) {
            unsigned int q = dists->input_queue();

            while (target_queues.find(q) == target_queues.end()) {
                q = dists->input_queue();
            }

            qset_neighbor.insert(q);
        }
        neighbors.push_back(QSum(qset_neighbor, qsum.metric));
    }

    // remove one from qset
    uniform_int_distribution<unsigned int> dist(0, (unsigned int) qset.size() - 1);
    qset_t::iterator it = qset.begin();
    mt19937& gen = dists->get_gen();
    advance(it, dist(gen));
    if (qset.size() == 2) {
        neighbors.push_back(Indiv(qsum.metric, *it));
    } else {
        qset_t qset_neighbor = qset;
        qset_neighbor.erase(*it);
        neighbors.push_back(QSum(qset_neighbor, qsum.metric));
    }
}

//************************************* INDIV *************************************//

Indiv SpecFactory::random_indiv() {
    metric_t metric = dists->wl_metric();
    unsigned int queue = dists->input_queue();

    while (target_queues.find(queue) == target_queues.end()) {
        queue = dists->input_queue();
    }
    return Indiv(metric, queue);
}

void SpecFactory::pick_neighbors(Indiv& indiv, vector<m_expr_t>& neighbors) {
    // TODO: change metric?

    // changing queue
    unsigned int queue_neighbor = dists->input_queue();
    while ((queue_neighbor == indiv.get_queue() && target_queues.size() > 1) ||
           target_queues.find(queue_neighbor) == target_queues.end()) {
        queue_neighbor = dists->input_queue();
    }
    neighbors.push_back(Indiv(indiv.get_metric(), queue_neighbor));
}

//************************************* TIME *************************************//

Time SpecFactory::random_time() {
    unsigned int random_coeff = dists->rhs_time_coeff();
    return Time(random_coeff);
}

//************************************* COMP *************************************//
op_t SpecFactory::random_op() {
    return dists->op();
}
