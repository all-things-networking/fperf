//
//  workload.cpp
//  AutoPerf
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

//************************************* TimedSpec *************************************//

TimedSpec SpecFactory::random_timed_spec() {
    WlSpec wl_spec = random_wl_spec();

    unsigned int time_range_lb = dists->timestep();
    unsigned int time_range_ub = dists->timestep();
    while (time_range_ub < time_range_lb) {
        time_range_lb = dists->timestep();
        time_range_ub = dists->timestep();
    }

    return TimedSpec(wl_spec, time_range_t(time_range_lb, time_range_ub), total_time);
}


void SpecFactory::pick_neighbors(TimedSpec& spec, vector<TimedSpec>& neighbors) {
    WlSpec wl_spec = spec.get_wl_spec();
    time_range_t time_range = spec.get_time_range();

    // Changing wl_spec
    vector<WlSpec> wl_spec_neighbors;
    pick_neighbors(wl_spec, wl_spec_neighbors);
    for (unsigned int i = 0; i < wl_spec_neighbors.size(); i++) {
        TimedSpec nei = TimedSpec(wl_spec_neighbors[i], time_range, total_time);
        if (!nei.spec_is_empty() && !nei.spec_is_all()) {
            neighbors.push_back(nei);
        }
    }

    // changing time

    std::mt19937& gen = dists->get_gen();
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

WlSpec SpecFactory::random_wl_spec() {
    lhs_t lhs = random_lhs();
    rhs_t rhs = random_rhs();
    if (holds_alternative<TONE>(lhs)) {
        metric_t metric = get<TONE>(lhs).get_metric();
        if (metric == metric_t::META1) {
            rhs = random_rhs(false, dists->get_pkt_meta1_val_dist());
        } else if (metric == metric_t::META2) {
            rhs = random_rhs(false, dists->get_pkt_meta2_val_dist());
        }
    }

    comp_t comp = random_comp();
    WlSpec res = WlSpec(lhs, comp, rhs);
    while (res.spec_is_empty() || res.spec_is_all()) {
        lhs = random_lhs();
        rhs = random_rhs();
        if (holds_alternative<TONE>(lhs)) {
            metric_t metric = get<TONE>(lhs).get_metric();
            if (metric == metric_t::META1) {
                rhs = random_rhs(false, dists->get_pkt_meta1_val_dist());
            } else if (metric == metric_t::META2) {
                rhs = random_rhs(false, dists->get_pkt_meta2_val_dist());
            }
        }
        comp = random_comp();
        res = WlSpec(lhs, comp, rhs);
    }
    return res;
}

void SpecFactory::pick_neighbors(WlSpec& spec, vector<WlSpec>& neighbors) {
    lhs_t lhs = spec.get_lhs();
    comp_t comp = spec.get_comp();
    rhs_t rhs = spec.get_rhs();

    // Changing lhs
    vector<lhs_t> lhs_neighbors;
    pick_lhs_neighbors(lhs, lhs_neighbors);
    for (unsigned int i = 0; i < lhs_neighbors.size(); i++) {
        WlSpec nei = WlSpec(lhs_neighbors[i], comp, rhs);
        if (!nei.spec_is_empty() && !nei.spec_is_all()) {
            neighbors.push_back(nei);
        }
    }

    // Changing comp
    comp_t new_comp = random_comp();
    while (new_comp == comp) {
        new_comp = random_comp();
    }
    WlSpec nei = WlSpec(lhs, new_comp, rhs);
    if (!nei.spec_is_empty() && !nei.spec_is_all()) {
        neighbors.push_back(nei);
    }

    // Changing rhs
    vector<rhs_t> rhs_neighbors;
    if (holds_alternative<TONE>(lhs)) {
        metric_t metric = get<TONE>(lhs).get_metric();
        if (metric == metric_t::META1) {
            pick_rhs_neighbors(rhs, rhs_neighbors, false, dists->get_pkt_meta1_val_dist());
        } else if (metric == metric_t::META2) {
            pick_rhs_neighbors(rhs, rhs_neighbors, false, dists->get_pkt_meta2_val_dist());
        } else {
            pick_rhs_neighbors(rhs, rhs_neighbors);
        }
    } else {
        pick_rhs_neighbors(rhs, rhs_neighbors);
    }
    for (unsigned int i = 0; i < rhs_neighbors.size(); i++) {
        WlSpec nei = WlSpec(lhs, comp, rhs_neighbors[i]);
        if (!nei.spec_is_empty() && !nei.spec_is_all()) {
            neighbors.push_back(nei);
        }
    }
}

//************************************* RHS *************************************//

rhs_t SpecFactory::random_rhs() {
    unsigned int rhs_type = dists->rhs();
    while (rhs_type == 0) {
        rhs_type = dists->rhs();
    }
    switch (rhs_type) {
    case 0: return random_trf();
    case 1: return random_time();
    default: return dists->rhs_const();
    }
    return 0;
}

rhs_t SpecFactory::random_rhs(bool time_valid,
                              std::uniform_int_distribution<unsigned int> const_dist) {
    unsigned int rhs_type = dists->rhs();
    while (rhs_type == 0) {
        rhs_type = dists->rhs();
    }

    if (time_valid && rhs_type == 1) return random_time();

    std::mt19937& gen = dists->get_gen();
    return const_dist(gen);
}

void SpecFactory::pick_rhs_neighbors(rhs_t rhs, vector<rhs_t>& neighbors) {
    switch (rhs.index()) {
    // trf
    case 0: {
        trf_t trf = get<trf_t>(rhs);
        vector<trf_t> trf_neighbors;
        pick_trf_neighbors(trf, trf_neighbors);
        neighbors.insert(neighbors.end(), trf_neighbors.begin(), trf_neighbors.end());
        break;
    }
    // TIME
    case 1: {
        TIME time = get<TIME>(rhs);
        TIME time_neighbor = random_time();
        while (time_neighbor == time) {
            time_neighbor = random_time();
        }
        neighbors.push_back(time_neighbor);
        neighbors.push_back(time.get_coeff());
        break;
    }
    // C
    case 2: {
        unsigned int c = get<unsigned int>(rhs);
        unsigned int c_neighbor = dists->rhs_const();
        while (c_neighbor == c) {
            c_neighbor = dists->rhs_const();
        }
        neighbors.push_back(c_neighbor);

        neighbors.push_back(TIME(1u));
        break;
    }
    default: break;
    }
}

void SpecFactory::pick_rhs_neighbors(rhs_t rhs,
                                     vector<rhs_t>& neighbors,
                                     bool time_valid,
                                     std::uniform_int_distribution<unsigned int> const_dist) {
    switch (rhs.index()) {
    // trf
    case 0: {
        trf_t trf = get<trf_t>(rhs);
        vector<trf_t> trf_neighbors;
        pick_trf_neighbors(trf, trf_neighbors);
        neighbors.insert(neighbors.end(), trf_neighbors.begin(), trf_neighbors.end());
        break;
    }
    // TIME
    case 1: {
        TIME time = get<TIME>(rhs);
        TIME time_neighbor = random_time();
        while (time_neighbor == time) {
            time_neighbor = random_time();
        }
        neighbors.push_back(time_neighbor);

        unsigned int c = time.get_coeff();
        if (c > const_dist.max()) c = const_dist.max();
        if (c < const_dist.min()) c = const_dist.min();
        neighbors.push_back(c);
        break;
    }
    // C
    case 2: {
        std::mt19937& gen = dists->get_gen();
        unsigned int c = get<unsigned int>(rhs);
        unsigned int c_neighbor = const_dist(gen);
        while (c_neighbor == c) {
            c_neighbor = const_dist(gen);
        }
        neighbors.push_back(c_neighbor);

        if (time_valid) {
            neighbors.push_back(TIME(1u));
        }

        break;
    }
    default: break;
    }
}

//************************************* LHS *************************************//

lhs_t SpecFactory::random_lhs() {
    return random_trf();
}

void SpecFactory::pick_lhs_neighbors(lhs_t lhs, vector<lhs_t>& neighbors) {
    pick_trf_neighbors(lhs, neighbors);
}

//************************************* TRF *************************************//

trf_t SpecFactory::random_trf() {
    unsigned int trf_type = dists->trf();
    switch (trf_type) {
    case 0: return random_tsum();
    default: return random_tone();
    }
}

void SpecFactory::pick_trf_neighbors(trf_t trf, vector<trf_t>& neighbors) {
    switch (trf.index()) {
    // TSUM
    case 0: {
        pick_neighbors(get<TSUM>(trf), neighbors);
        break;
    }
    // TONE
    case 1: {
        pick_neighbors(get<TONE>(trf), neighbors);
        break;
    }
    default: break;
    }
}

//************************************* TSUM *************************************//

qset_t SpecFactory::random_tsum_qset() {
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

TSUM SpecFactory::random_tsum() {
    // TODO: generalize this to aggregatable metrics
    metric_t metric = metric_t::CENQ;
    qset_t qset = random_tsum_qset();
    return TSUM(qset, metric);
}


void SpecFactory::pick_neighbors(TSUM& tsum, vector<trf_t>& neighbors) {
    // TODO: change metric, when there is more than one aggregatable metric

    // changing qset
    qset_t qset = tsum.qset;
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
        neighbors.push_back(TSUM(qset_neighbor, tsum.metric));
    }

    // remove one from qset
    uniform_int_distribution<unsigned int> dist(0, (unsigned int) qset.size() - 1);
    qset_t::iterator it = qset.begin();
    std::mt19937& gen = dists->get_gen();
    advance(it, dist(gen));
    if (qset.size() == 2) {
        neighbors.push_back(TONE(tsum.metric, *it));
    } else {
        qset_t qset_neighbor = qset;
        qset_neighbor.erase(*it);
        neighbors.push_back(TSUM(qset_neighbor, tsum.metric));
    }
}

//************************************* TONE *************************************//

TONE SpecFactory::random_tone() {
    metric_t metric = dists->wl_metric();
    unsigned int queue = dists->input_queue();

    while (target_queues.find(queue) == target_queues.end()) {
        queue = dists->input_queue();
    }
    return TONE(metric, queue);
}

void SpecFactory::pick_neighbors(TONE& tone, vector<trf_t>& neighbors) {
    // TODO: change metric?

    // changing queue
    unsigned int queue_neighbor = dists->input_queue();
    while (queue_neighbor == tone.get_queue() ||
           target_queues.find(queue_neighbor) == target_queues.end()) {
        queue_neighbor = dists->input_queue();
    }
    neighbors.push_back(TONE(tone.get_metric(), queue_neighbor));
}

//************************************* TIME *************************************//

TIME SpecFactory::random_time() {
    unsigned int random_coeff = dists->rhs_time_coeff();
    return TIME(random_coeff);
}

//************************************* COMP *************************************//
comp_t SpecFactory::random_comp() {
    return dists->comp();
}
