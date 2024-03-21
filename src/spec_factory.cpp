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
#include <random>

#include "util.hpp"

SpecFactory::SpecFactory(SharedConfig* shared_config):
shared_config(shared_config),
dists(shared_config->get_dists()) {
    total_time = shared_config->get_total_time();
    in_queue_cnt = shared_config->get_in_queue_cnt();
    target_queues = shared_config->get_target_queues();

    initializeSpecs();
}

RandomSpecGenerationParameters SpecFactory::get_metric_params(metric_t metric_type) {
    switch (metric_type) {
        case metric_t::DST: return {false, dists->get_pkt_meta1_val_dist()};
        case metric_t::ECMP: return {false, dists->get_pkt_meta2_val_dist()};
        default: return {true, dists->get_rhs_const_dist()};
    }
}

void SpecFactory::initializeSpecs() {
    SpecFactory::spec_generators.clear();

    SpecGeneratorFuncPtr comp_ptr = &SpecFactory::random_comp;
    SpecFactory::spec_generators.push_back(comp_ptr);
}

//************************************* TimedSpec *************************************//

TimedSpec SpecFactory::random_timed_spec() {
    WlSpec* wl_spec = random_wl_spec();

    unsigned int time_range_lb = dists->timestep();
    unsigned int time_range_ub = dists->timestep();
    while (time_range_ub < time_range_lb) {
        time_range_lb = dists->timestep();
        time_range_ub = dists->timestep();
    }

    return TimedSpec(wl_spec, time_range_t(time_range_lb, time_range_ub), total_time);
}


void SpecFactory::pick_neighbors(TimedSpec& spec, vector<TimedSpec>& neighbors) {
    WlSpec* wl_spec = spec.get_wl_spec();
    time_range_t time_range = spec.get_time_range();

    // Changing wl_spec
    vector<WlSpec*> wl_spec_neighbors;
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

WlSpec* SpecFactory::random_wl_spec() {
    mt19937& gen = dists->get_gen();
    uniform_int_distribution<> dist(0, spec_generators.size() - 1);
    int index = dist(gen);
    return (this->*spec_generators[index])();
}

void SpecFactory::pick_neighbors(WlSpec* spec, std::vector<WlSpec*>& neighbors) {
    Comp* compSpec = dynamic_cast<Comp*>(spec);
    if (compSpec) {
        vector<Comp> comp_neighbors;
        pick_neighbors(*compSpec, comp_neighbors);
        for (const Comp& neighbor : comp_neighbors) {
            neighbors.push_back(new Comp(neighbor));
        }
    }
}

//************************************* COMP *************************************//

WlSpec* SpecFactory::random_comp() {
    Comp* res;

    do {
        Lhs* lhs = random_lhs();
        Rhs* rhs = random_rhs();
        const Indiv* indiv = dynamic_cast<Indiv*>(lhs);
        if (indiv) {
            metric_t metric = indiv->get_metric();
            rhs = random_rhs(get_metric_params(metric));
        }
        Op op = random_op();
        res = new Comp(lhs, op, rhs);
    } while (res->spec_is_empty() || res->spec_is_all());

    return res;
}

void SpecFactory::pick_neighbors(Comp& spec, vector<Comp>& neighbors) {
    Lhs* lhs = spec.get_lhs();
    Op op = spec.get_op();
    Rhs* rhs = spec.get_rhs();

    // Changing lhs
    vector<Lhs*> lhs_neighbors;
    pick_lhs_neighbors(lhs, lhs_neighbors);
    for (unsigned int i = 0; i < lhs_neighbors.size(); i++) {
        Comp nei = Comp(lhs_neighbors[i], op, rhs);
        if (!nei.spec_is_empty() && !nei.spec_is_all()) {
            neighbors.push_back(nei);
        }
    }

    // Changing op
    Op new_op = random_op();
    while (new_op == op) {
        new_op = random_op();
    }
    Comp nei = Comp(lhs, new_op, rhs);
    if (!nei.spec_is_empty() && !nei.spec_is_all()) {
        neighbors.push_back(nei);
    }

    // Changing rhs
    vector<Rhs*> rhs_neighbors;
    const Indiv* indiv = dynamic_cast<Indiv*>(lhs);
    if (indiv) {
        metric_t metric = indiv->get_metric();
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

Rhs* SpecFactory::random_rhs() {
    return random_rhs({true, dists->get_rhs_const_dist()});
}

Rhs* SpecFactory::random_rhs(RandomSpecGenerationParameters params) {
    unsigned int rhs_type = dists->rhs();
    while (rhs_type == 0) {
        rhs_type = dists->rhs();
    }

    if (params.time_valid && rhs_type == 1){
        Time time = random_time();
        return dynamic_cast<Rhs*>(new Time(time));
    }

    mt19937& gen = dists->get_gen();
    unsigned int c = params.const_dist(gen);
    return new Constant(c);
}

void SpecFactory::pick_rhs_neighbors(Rhs* rhs, vector<Rhs*>& neighbors) {
    pick_rhs_neighbors(rhs, neighbors, {true, dists->get_rhs_const_dist(), false});
}

void SpecFactory::pick_rhs_neighbors(Rhs* rhs,
                                     vector<Rhs*>& neighbors,
                                     RandomSpecGenerationParameters params) {
    MExpr* trf = dynamic_cast<MExpr*>(rhs);
    if (trf) {
        vector<MExpr*> trf_neighbors;
        pick_m_expr_neighbors(trf, trf_neighbors);
        for (const MExpr* neighbor : trf_neighbors) {
            neighbors.push_back(new MExpr(*neighbor));
        }
    }
    Time* time = dynamic_cast<Time*>(rhs);
    if (time) {
        Time time_neighbor = random_time();
        while (time_neighbor == *time) {
            time_neighbor = random_time();
        }
        neighbors.push_back(new Time(time_neighbor));

        unsigned int c = time->get_coeff();
        if (params.bound_with_dist) {
            if (c > params.const_dist.max()) c = params.const_dist.max();
            if (c < params.const_dist.min()) c = params.const_dist.min();
        }
        neighbors.push_back(new Constant(c));
    }
    Constant* c = dynamic_cast<Constant*>(rhs);
    if (c) {
        mt19937& gen = dists->get_gen();
        unsigned int c_val = c->get_coeff();
        unsigned int c_neighbor = params.const_dist(gen);
        while (c_neighbor == c_val) {
            c_neighbor = params.const_dist(gen);
        }
        neighbors.push_back(new Constant(c_neighbor));

        if (params.time_valid) {
            neighbors.push_back(new Time(1u));
        }
    }
}

//************************************* LHS *************************************//

Lhs* SpecFactory::random_lhs() {
    return random_m_expr();
}

void SpecFactory::pick_lhs_neighbors(Lhs* lhs, vector<Lhs*>& neighbors) {
    MExpr* trf = dynamic_cast<MExpr*>(lhs);
    // Turn neighbors into vector<MExpr*>
    vector<MExpr*> m_expr_neighbors;
    for(Lhs* neighbor : neighbors) {
        m_expr_neighbors.push_back(dynamic_cast<MExpr*>(neighbor));
    }
    pick_m_expr_neighbors(trf, m_expr_neighbors);
}

//************************************* TRF *************************************//

MExpr* SpecFactory::random_m_expr() {
    unsigned int trf_type = dists->trf();
    if (target_queues.size() < 2) trf_type = 1;
    switch (trf_type) {
        case 0: return new QSum(random_qsum());
        default: return new Indiv(random_indiv());
    }
}

void SpecFactory::pick_m_expr_neighbors(MExpr* m_expr, vector<MExpr*>& neighbors) {
    QSum* qsum = dynamic_cast<QSum*>(m_expr);
    if (qsum) {
        pick_neighbors(*qsum, neighbors);
    }
    Indiv* indiv = dynamic_cast<Indiv*>(m_expr);
    if (indiv) {
        pick_neighbors(*indiv, neighbors);
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


void SpecFactory::pick_neighbors(QSum& qsum, vector<MExpr*>& neighbors) {
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
        neighbors.push_back(new QSum(qset_neighbor, qsum.metric));
    }

    // remove one from qset
    uniform_int_distribution<unsigned int> dist(0, (unsigned int) qset.size() - 1);
    qset_t::iterator it = qset.begin();
    mt19937& gen = dists->get_gen();
    advance(it, dist(gen));
    if (qset.size() == 2) {
        neighbors.push_back(new Indiv(qsum.metric, *it));
    } else {
        qset_t qset_neighbor = qset;
        qset_neighbor.erase(*it);
        neighbors.push_back(new QSum(qset_neighbor, qsum.metric));
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

void SpecFactory::pick_neighbors(Indiv& indiv, vector<MExpr*>& neighbors) {
    // TODO: change metric?

    // changing queue
    unsigned int queue_neighbor = dists->input_queue();
    while ((queue_neighbor == indiv.get_queue() && target_queues.size() > 1) ||
           target_queues.find(queue_neighbor) == target_queues.end()) {
        queue_neighbor = dists->input_queue();
    }
    neighbors.push_back(new Indiv(indiv.get_metric(), queue_neighbor));
}

//************************************* TIME *************************************//

Time SpecFactory::random_time() {
    unsigned int random_coeff = dists->rhs_time_coeff();
    return Time(random_coeff);
}

//************************************* COMP *************************************//
Op SpecFactory::random_op() {
    return dists->op();
}

