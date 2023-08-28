//
//  workload.cpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/19/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "workload.hpp"

#include <algorithm>
#include <set>
#include <sstream>

#include "util.hpp"

//************************************* TSUM *************************************//

TSUM::TSUM(qset_t qset, metric_t metric): qset(qset), metric(metric) {
}

bool TSUM::applies_to_queue(unsigned int queue) const {
    return (qset.find(queue) != qset.end());
}

unsigned int TSUM::ast_size() const {
    return (unsigned int) qset.size();
}

qset_t TSUM::get_qset() const {
    return qset;
}

metric_t TSUM::get_metric() const {
    return metric;
}

std::ostream& operator<<(std::ostream& os, const TSUM& tsum) {
    os << "SUM_[q in " << tsum.qset << "] " << tsum.metric << "(q ,t)";
    return os;
}

bool operator==(const TSUM& s1, const TSUM& s2) {
    return (s1.qset == s2.qset && s1.metric == s2.metric);
}

bool operator<(const TSUM& s1, const TSUM& s2) {
    return (s1.qset < s2.qset || (s1.qset == s2.qset && s1.metric < s2.metric));
}

//************************************* TONE *************************************//

TONE::TONE(metric_t metric, unsigned int queue): metric(metric), queue(queue) {
}


bool TONE::applies_to_queue(unsigned int queue) const {
    return this->queue == queue;
}

unsigned int TONE::get_queue() const {
    return queue;
}

metric_t TONE::get_metric() const {
    return metric;
}

std::ostream& operator<<(std::ostream& os, const TONE& tone) {
    os << tone.metric << "(" << tone.queue << ", t)";
    return os;
}

bool operator==(const TONE& s1, const TONE& s2) {
    return (s1.metric == s2.metric && s1.queue == s2.queue);
}

bool operator<(const TONE& s1, const TONE& s2) {
    return (s1.metric < s2.metric || (s1.metric == s2.metric && s1.queue < s2.queue));
}

//************************************* TIME *************************************//

TIME::TIME(unsigned int coeff): coeff(coeff) {
}

unsigned int TIME::get_coeff() const {
    return coeff;
}

std::ostream& operator<<(std::ostream& os, const TIME& time) {
    if (time.coeff != 1) os << time.coeff;
    os << "t";
    return os;
}

bool operator==(const TIME& t1, const TIME& t2) {
    return t1.coeff == t2.coeff;
}

bool operator<(const TIME& t1, const TIME& t2) {
    return t1.coeff < t2.coeff;
}

//************************************* TRF *************************************//

bool trf_applies_to_queue(const trf_t trf, unsigned int queue) {
    switch (trf.index()) {
    // TSUM
    case 0: {
        return get<TSUM>(trf).applies_to_queue(queue);
    }
    // TONE
    case 1: {
        return get<TONE>(trf).applies_to_queue(queue);
    }
    default: break;
    }
    cout << "trf_applies_to_queue: should not reach here" << endl;
    return false;
}

unsigned int trf_ast_size(const trf_t trf) {
    if (holds_alternative<TSUM>(trf)) {
        return get<TSUM>(trf).ast_size();
    } else
        return 1u;
}

std::ostream& operator<<(std::ostream& os, const trf_t& trf) {
    switch (trf.index()) {
    // TSUM
    case 0: os << get<TSUM>(trf); break;
    // TONE
    case 1: os << get<TONE>(trf); break;
    default: break;
    }
    return os;
}

bool operator==(const trf_t& trf1, const trf_t& trf2) {
    if (std::holds_alternative<TSUM>(trf1) && std::holds_alternative<TSUM>(trf2)) {
        return std::get<TSUM>(trf1) == std::get<TSUM>(trf2);
    }
    if (std::holds_alternative<TONE>(trf1) && std::holds_alternative<TONE>(trf2)) {
        return std::get<TONE>(trf1) == std::get<TONE>(trf2);
    }
    return false;
}

bool operator<(const trf_t& trf1, const trf_t& trf2) {
    // TSUM < TONE

    if (std::holds_alternative<TSUM>(trf1)) {
        if (std::holds_alternative<TSUM>(trf2)) {
            return std::get<TSUM>(trf1) < std::get<TSUM>(trf2);
        } else {
            return true;
        }
    }

    else if (std::holds_alternative<TONE>(trf1)) {
        if (std::holds_alternative<TONE>(trf2)) {
            return std::get<TONE>(trf1) < std::get<TONE>(trf2);
        } else {
            return false;
        }
    }

    std::cout << "operator < for trf: should not reach here" << endl;
    return false;
}

//************************************* LHS *************************************//

bool lhs_applies_to_queue(const lhs_t lhs, unsigned int queue) {
    return trf_applies_to_queue(lhs, queue);
}

unsigned int lhs_ast_size(const lhs_t lhs) {
    return trf_ast_size(lhs);
}


//************************************* RHS *************************************//

bool rhs_applies_to_queue(const rhs_t rhs, unsigned int queue) {
    if (holds_alternative<trf_t>(rhs)) {
        trf_t rhs_trf = get<trf_t>(rhs);
        return trf_applies_to_queue(rhs_trf, queue);
    }
    return false;
}

unsigned int rhs_ast_size(const rhs_t rhs) {
    if (holds_alternative<trf_t>(rhs)) {
        trf_t rhs_trf = get<trf_t>(rhs);
        return trf_ast_size(rhs_trf);
    } else
        return 0u;
}

std::ostream& operator<<(std::ostream& os, const rhs_t& rhs) {
    switch (rhs.index()) {
    // trf
    case 0: {
        os << get<trf_t>(rhs);
        break;
    }
    // TIME
    case 1: {
        os << get<TIME>(rhs);
        break;
    }
    // C
    case 2: {
        os << get<unsigned int>(rhs);
        break;
    }
    default: break;
    }
    return os;
}

bool operator==(const rhs_t& rhs1, const rhs_t& rhs2) {
    if (std::holds_alternative<trf_t>(rhs1) && std::holds_alternative<trf_t>(rhs2)) {
        return std::get<trf_t>(rhs1) == std::get<trf_t>(rhs2);
    }
    if (std::holds_alternative<TIME>(rhs1) && std::holds_alternative<TIME>(rhs2)) {
        return std::get<TIME>(rhs1) == std::get<TIME>(rhs2);
    }
    if (std::holds_alternative<unsigned int>(rhs1) && std::holds_alternative<unsigned int>(rhs2)) {
        return std::get<unsigned int>(rhs1) == std::get<unsigned int>(rhs2);
    }
    return false;
}

bool operator<(const rhs_t& rhs1, const rhs_t& rhs2) {
    // trf_t < TIME < unsigned int

    if (std::holds_alternative<trf_t>(rhs1)) {
        if (std::holds_alternative<trf_t>(rhs2)) {
            return std::get<trf_t>(rhs1) < std::get<trf_t>(rhs2);
        } else {
            return true;
        }
    }

    else if (std::holds_alternative<TIME>(rhs1)) {
        if (std::holds_alternative<TIME>(rhs2)) {
            return std::get<TIME>(rhs1) < std::get<TIME>(rhs2);
        } else if (std::holds_alternative<trf_t>(rhs2)) {
            return false;
        } else {
            return true;
        }
    }

    if (std::holds_alternative<unsigned int>(rhs1)) {
        if (std::holds_alternative<unsigned int>(rhs2)) {
            return std::get<unsigned int>(rhs1) < std::get<unsigned int>(rhs2);
        } else {
            return false;
        }
    }
    std::cout << "operator < for rhs: should not reach here" << endl;
    return false;
}

//************************************* Same *************************************//

Same::Same(metric_t metric, unsigned int queue): metric(metric), queue(queue) {
}


bool Same::applies_to_queue(unsigned int queue) const {
    return this->queue == queue;
}

unsigned int Same::get_queue() const {
    return queue;
}

metric_t Same::get_metric() const {
    return metric;
}

std::ostream& operator<<(std::ostream& os, const Same& tone) {
    os << "Same[" << tone.metric << "(" << tone.queue << ", t)]";
    return os;
}

bool operator==(const Same& s1, const Same& s2) {
    return (s1.metric == s2.metric && s1.queue == s2.queue);
}

bool operator<(const Same& s1, const Same& s2) {
    return (s1.metric < s2.metric || (s1.metric == s2.metric && s1.queue < s2.queue));
}


//************************************* Unique *************************************//

Unique::Unique(qset_t qset, metric_t metric): qset(qset), metric(metric) {
}

bool Unique::applies_to_queue(unsigned int queue) const {
    return (qset.find(queue) != qset.end());
}

unsigned int Unique::ast_size() const {
    return (unsigned int) qset.size();
}

qset_t Unique::get_qset() const {
    return qset;
}

metric_t Unique::get_metric() const {
    return metric;
}

std::ostream& operator<<(std::ostream& os, const Unique& tsum) {
    os << "Unique_[q in " << tsum.qset << "] " << tsum.metric << "(q ,t)";
    return os;
}

bool operator==(const Unique& s1, const Unique& s2) {
    return (s1.qset == s2.qset && s1.metric == s2.metric);
}

bool operator<(const Unique& s1, const Unique& s2) {
    return (s1.qset < s2.qset || (s1.qset == s2.qset && s1.metric < s2.metric));
}


//************************************* WlSpec *************************************//

// Invariant: An object of this class
//            is always normalized independent of the operation
//            running on it.

WlSpec::WlSpec(lhs_t lhs, comp_t comp, rhs_t rhs): lhs(lhs), comp(comp), rhs(rhs) {
    normalize();
}

bool WlSpec::applies_to_queue(unsigned int queue) const {
    bool lhs_applies = lhs_applies_to_queue(lhs, queue);
    bool rhs_applies = rhs_applies_to_queue(rhs, queue);
    return lhs_applies || rhs_applies;
}

void WlSpec::normalize() {
    // If the right hand side is TRF, we might be able to normalize
    if (holds_alternative<trf_t>(rhs)) {

        trf_t lhs_trf = lhs;
        trf_t rhs_trf = get<trf_t>(rhs);

        bool lhs_is_tsum = holds_alternative<TSUM>(lhs_trf);
        bool lhs_is_tone = holds_alternative<TONE>(lhs_trf);

        bool rhs_is_tsum = holds_alternative<TSUM>(rhs_trf);
        bool rhs_is_tone = holds_alternative<TONE>(rhs_trf);

        // If both are TSUM
        if (lhs_is_tsum && rhs_is_tsum) {

            TSUM lhs_tsum = get<TSUM>(lhs_trf);
            TSUM rhs_tsum = get<TSUM>(rhs_trf);

            if (lhs_tsum.get_metric() == rhs_tsum.get_metric()) {
                qset_t lhs_qset = lhs_tsum.get_qset();
                qset_t rhs_qset = rhs_tsum.get_qset();

                set<unsigned int> inters;
                set_intersection(lhs_qset.begin(),
                                 lhs_qset.end(),
                                 rhs_qset.begin(),
                                 rhs_qset.end(),
                                 std::inserter(inters, inters.begin()));
                bool intersect = inters.size() > 0;

                if (lhs_qset == rhs_qset) {
                    if (comp == comp_t::GE || comp == comp_t::LE) is_all = true;
                    if (comp == comp_t::GT || comp == comp_t::LT) is_empty = true;
                } else if (is_superset(rhs_qset, lhs_qset)) {
                    set<unsigned int> diff;
                    set_difference(rhs_qset.begin(),
                                   rhs_qset.end(),
                                   lhs_qset.begin(),
                                   lhs_qset.end(),
                                   std::inserter(diff, diff.begin()));

                    if (comp == comp_t::LE)
                        is_all = true;
                    else if (comp == comp_t::GT)
                        is_empty = true;
                    else if (diff.size() == 1) {
                        lhs = TONE(rhs_tsum.get_metric(), *(diff.begin()));
                        if (comp == comp_t::EQ)
                            comp = comp_t::LE;
                        else
                            comp = neg_comp(comp);
                        rhs = 0u;
                    } else {
                        lhs = TSUM(qset_t(diff.begin(), diff.end()), rhs_tsum.get_metric());
                        if (comp == comp_t::EQ)
                            comp = comp_t::LE;
                        else
                            comp = neg_comp(comp);
                        rhs = 0u;
                    }
                } else if (is_superset(lhs_qset, rhs_qset)) {
                    set<unsigned int> diff;
                    set_difference(lhs_qset.begin(),
                                   lhs_qset.end(),
                                   rhs_qset.begin(),
                                   rhs_qset.end(),
                                   std::inserter(diff, diff.begin()));

                    if (comp == comp_t::LT)
                        is_empty = true;
                    else if (comp == comp_t::GE)
                        is_all = true;
                    else if (diff.size() == 1) {
                        lhs = TONE(rhs_tsum.get_metric(), *(diff.begin()));
                        rhs = 0u;
                    } else {
                        lhs = TSUM(qset_t(diff.begin(), diff.end()), rhs_tsum.get_metric());
                        rhs = 0u;
                    }
                } else if (intersect) {
                    set<unsigned int> lhs_diff;
                    set_difference(lhs_qset.begin(),
                                   lhs_qset.end(),
                                   rhs_qset.begin(),
                                   rhs_qset.end(),
                                   std::inserter(lhs_diff, lhs_diff.begin()));

                    set<unsigned int> rhs_diff;
                    set_difference(rhs_qset.begin(),
                                   rhs_qset.end(),
                                   lhs_qset.begin(),
                                   lhs_qset.end(),
                                   std::inserter(rhs_diff, rhs_diff.begin()));

                    // set lhs
                    if (lhs_diff.size() == 1) {
                        lhs = TONE(lhs_tsum.get_metric(), *(lhs_diff.begin()));
                    } else {
                        lhs = TSUM(qset_t(lhs_diff.begin(), lhs_diff.end()), lhs_tsum.get_metric());
                    }

                    // set rhs
                    if (rhs_diff.size() == 1) {
                        rhs = TONE(rhs_tsum.get_metric(), *(rhs_diff.begin()));
                    } else {
                        rhs = TSUM(qset_t(rhs_diff.begin(), rhs_diff.end()), rhs_tsum.get_metric());
                    }
                }
            }
        }
        // if both are TONE
        else if (lhs_is_tone && rhs_is_tone) {
            TONE lhs_tone = get<TONE>(lhs_trf);
            TONE rhs_tone = get<TONE>(rhs_trf);
            if (lhs_tone == rhs_tone) {
                if (comp == comp_t::LE || comp == comp_t::GE)
                    is_all = true;
                else if (comp == comp_t::LT || comp == comp_t::GT)
                    is_empty = true;
            }
            // TODO: generalize this to comparable metrics
            if (lhs_tone.get_metric() != rhs_tone.get_metric()) {
                is_empty = true;
            }
        }
        // If lhs is TONE and rhs is TSUM, swap
        // Notice here we only have LT or LE as comp
        else if (lhs_is_tone && rhs_is_tsum) {
            trf_t tmp = lhs_trf;

            lhs = rhs_trf;
            lhs_trf = rhs_trf;

            rhs = tmp;
            rhs_trf = tmp;

            comp = neg_comp(comp);

            lhs_is_tsum = true;
            lhs_is_tone = false;

            rhs_is_tsum = false;
            rhs_is_tone = true;
        }
        // Both cases of one TONE and one TSUM
        // should end up here. Note that comp
        // can be all of LT, LE, GT, GE now
        if (lhs_is_tsum && rhs_is_tone) {
            TSUM lhs_tsum = get<TSUM>(lhs_trf);
            TONE rhs_tone = get<TONE>(rhs_trf);

            qset_t lhs_qset = lhs_tsum.get_qset();
            unsigned int rhs_queue = rhs_tone.get_queue();

            if (lhs_tsum.get_metric() == rhs_tone.get_metric()) {
                qset_t::iterator it = lhs_qset.find(rhs_queue);
                if (it != lhs_qset.end()) {
                    if (comp == comp_t::GE)
                        is_all = true;
                    else if (comp == comp_t::LT)
                        is_empty = true;
                    else {
                        lhs_qset.erase(it);
                        rhs = 0u;
                        if (lhs_qset.size() == 1) {
                            lhs = TONE(lhs_tsum.get_metric(), *(lhs_qset.begin()));
                        } else {
                            lhs = TSUM(lhs_qset, lhs_tsum.get_metric());
                        }
                    }
                }
            }
        }
    }

    // If the right hand side is constant, comp should either be
    // GE or LE
    if (holds_alternative<unsigned int>(rhs)) {
        unsigned int c = get<unsigned int>(rhs);
        if (comp == comp_t::GT) {
            comp = comp_t::GE;
            rhs = c + 1;
        } else if (comp == comp_t::LT) {
            if (c == 0)
                is_empty = true;
            else {
                comp = comp_t::LE;
                rhs = c - 1;
            }
        }
    }

    if (holds_alternative<TIME>(rhs)) {
        unsigned int c = get<TIME>(rhs).get_coeff();
        if (c == 0) {
            rhs = c;
        } else {
            if (holds_alternative<TONE>(lhs)) {
                if (comp == comp_t::GE && c > MAX_ENQ) is_empty = true;
                if (comp == comp_t::EQ && c > MAX_ENQ) is_empty = true;
                if (comp == comp_t::GT && c >= MAX_ENQ) is_empty = true;
            }
            if (holds_alternative<TSUM>(lhs)) {
                TSUM tsum = get<TSUM>(lhs);
                if (comp == comp_t::GE && c > (tsum.get_qset().size() - 1) * MAX_ENQ)
                    is_empty = true;
                if (comp == comp_t::EQ && c > (tsum.get_qset().size() - 1) * MAX_ENQ)
                    is_empty = true;
                if (comp == comp_t::GT && c >= (tsum.get_qset().size() - 1) * MAX_ENQ)
                    is_empty = true;
            }
        }
    }
}

bool WlSpec::spec_is_empty() const {
    return is_empty;
}

bool WlSpec::spec_is_all() const {
    return is_all;
}

unsigned int WlSpec::ast_size() const {
    if (is_all) return 1u;
    if (is_empty) return 0u;
    return lhs_ast_size(lhs) + rhs_ast_size(rhs);
}

pair<metric_t, qset_t> WlSpec::get_zero_queues() const {
    qset_t qset;
    metric_t metric;

    if (comp == comp_t::LE && holds_alternative<unsigned int>(rhs) && get<unsigned int>(rhs) == 0) {
        if (holds_alternative<TONE>(lhs)) {
            TONE tone = get<TONE>(lhs);
            metric = tone.get_metric();
            if (Metric::properties.at(metric).non_negative) {
                qset.insert(tone.get_queue());
            }
        } else if (holds_alternative<TSUM>(lhs)) {
            TSUM tsum = get<TSUM>(lhs);
            metric = tsum.get_metric();
            if (Metric::properties.at(metric).non_negative) {
                qset_t s_qset = tsum.get_qset();
                for (qset_t::iterator it = s_qset.begin(); it != s_qset.end(); it++) {
                    qset.insert(*it);
                }
            }
        }
    }
    return make_pair(metric, qset);
}

lhs_t WlSpec::get_lhs() const {
    return lhs;
}

comp_t WlSpec::get_comp() const {
    return comp;
}

rhs_t WlSpec::get_rhs() const {
    return rhs;
}

std::ostream& operator<<(std::ostream& os, const WlSpec& spec) {
    if (spec.is_all)
        os << "*";
    else if (spec.is_empty)
        os << "FALSE";
    else
        os << spec.lhs << " " << spec.comp << " " << spec.rhs;
    return os;
}

std::ostream& operator<<(std::ostream& os, const WlSpec* spec) {
    os << spec->lhs << " " << spec->comp << " " << spec->rhs;
    return os;
}

bool operator==(const WlSpec& spec1, const WlSpec& spec2) {
    lhs_t lhs1 = spec1.lhs;
    comp_t comp1 = spec1.comp;
    rhs_t rhs1 = spec1.rhs;

    lhs_t lhs2 = spec2.lhs;
    comp_t comp2 = spec2.comp;
    rhs_t rhs2 = spec2.rhs;

    bool res = (lhs1 == lhs2 && comp1 == comp2 && rhs1 == rhs2);
    return res;
}

bool operator!=(const WlSpec& spec1, const WlSpec& spec2) {
    return !(spec1 == spec2);
}

bool operator<(const WlSpec& spec1, const WlSpec& spec2) {
    lhs_t lhs1 = spec1.lhs;
    comp_t comp1 = spec1.comp;
    rhs_t rhs1 = spec1.rhs;

    lhs_t lhs2 = spec2.lhs;
    comp_t comp2 = spec2.comp;
    rhs_t rhs2 = spec2.rhs;

    return (lhs1 < lhs2 || (lhs1 == lhs2 && comp1 < comp2) ||
            (lhs1 == lhs2 && comp1 == comp2 && rhs1 < rhs2));
}


//************************************* TimedSpec *************************************//

// Invariant: An object of this class
//            is always normalized independent of the operation
//            running on it.

TimedSpec::TimedSpec(WlSpec wl_spec, time_range_t time_range, unsigned int total_time):
wl_spec(wl_spec),
time_range(time_range),
total_time(total_time) {
    normalize();
}

TimedSpec::TimedSpec(WlSpec wl_spec, unsigned int until_time, unsigned int total_time):
wl_spec(wl_spec),
time_range(time_range_t(0, until_time - 1)),
total_time(total_time) {
    if (until_time == 0) time_range = time_range_t(1, 0);
    normalize();
}


bool TimedSpec::applies_to_queue(unsigned int queue) const {
    return wl_spec.applies_to_queue(queue);
}

void TimedSpec::set_time_range_ub(unsigned int ub) {
    time_range.second = ub;
    normalize();
}

void TimedSpec::normalize() {
    if (wl_spec.spec_is_empty())
        is_empty = true;
    else if (wl_spec.spec_is_all() || time_range.first > time_range.second)
        is_all = true;
    else {
        // When rhs is constant
        lhs_t lhs = wl_spec.get_lhs();
        comp_t comp = wl_spec.get_comp();
        rhs_t rhs = wl_spec.get_rhs();

        if (holds_alternative<unsigned int>(rhs)) {
            metric_t metric;
            if (holds_alternative<TONE>(lhs)) {
                metric = get<TONE>(lhs).get_metric();
            } else {
                metric = get<TSUM>(lhs).get_metric();
            }
            if (Metric::properties.at(metric).non_decreasing) {
                if (comp == comp_t::GE || comp == comp_t::GT) {
                    time_range.second = total_time - 1;
                } else if (comp == comp_t::LE || comp == comp_t::LT) {
                    time_range.first = 0;
                }
            }
        }
    }
}

bool TimedSpec::spec_is_empty() const {
    return is_empty;
}

bool TimedSpec::spec_is_all() const {
    return is_all;
}

time_range_t TimedSpec::get_time_range() const {
    return time_range;
}

WlSpec TimedSpec::get_wl_spec() const {
    return wl_spec;
}

std::ostream& operator<<(std::ostream& os, const TimedSpec& spec) {
    os << spec.time_range << ": ";
    os << spec.wl_spec;

    return os;
}
std::ostream& operator<<(std::ostream& os, const TimedSpec* spec) {
    os << spec->time_range << ": ";
    os << spec->wl_spec;

    return os;
}

bool operator==(const TimedSpec& spec1, const TimedSpec& spec2) {
    WlSpec wl_spec1 = spec1.wl_spec;
    WlSpec wl_spec2 = spec2.wl_spec;

    return (wl_spec1 == wl_spec2 && spec1.time_range == spec2.time_range);
}

bool operator!=(const TimedSpec& spec1, const TimedSpec& spec2) {
    return !(spec1 == spec2);
}

bool operator<(const TimedSpec& spec1, const TimedSpec& spec2) {
    return (spec1.time_range < spec2.time_range ||
            ((spec1.time_range == spec2.time_range) && (spec1.wl_spec < spec2.wl_spec)));
}

//************************************* Workkload *************************************//

Workload::Workload(unsigned int max_size, unsigned int queue_cnt, unsigned int total_time):
max_size(max_size),
queue_cnt(queue_cnt),
total_time(total_time) {
    timeline[time_range_t(0, total_time - 1)] = empty_set;
}

void Workload::clear() {
    all_specs.clear();
    timeline.clear();
    timeline[time_range_t(0, total_time - 1)] = empty_set;
    empty = false;
    all = true;
}

void Workload::add_wl_spec(TimedSpec spec) {
    auto insert_res = all_specs.insert(spec);
    if (insert_res.second) {
        time_range_t time_range = spec.get_time_range();
        set<time_range_t> affected_time_ranges = add_time_range(time_range);

        for (set<time_range_t>::iterator it = affected_time_ranges.begin();
             it != affected_time_ranges.end();
             it++) {
            timeline[*it].insert(spec.get_wl_spec());
        }
        normalize();
    }
}

void Workload::rm_wl_spec(TimedSpec spec) {
    auto erase_res = all_specs.erase(spec);
    time_range_t spec_time_range = spec.get_time_range();
    WlSpec wl_spec = spec.get_wl_spec();
    if (erase_res > 0) {
        for (timeline_t::iterator it = timeline.begin(); it != timeline.end(); it++) {
            if (is_superset(spec_time_range, it->first)) {
                (it->second).erase(wl_spec);
            }
        }

        normalize();
    }
}

void Workload::mod_wl_spec(TimedSpec old_spec, TimedSpec new_spec) {
    if (all_specs.find(old_spec) != all_specs.end()) {
        rm_wl_spec(old_spec);
        add_wl_spec(new_spec);
    }
}

// TODO: anything different for empty or all?
unsigned long Workload::size() const {
    return all_specs.size();
}

unsigned int Workload::get_max_size() const {
    return max_size;
}

unsigned int Workload::get_queue_cnt() const {
    return queue_cnt;
}

unsigned int Workload::get_total_time() const {
    return total_time;
}

timeline_t Workload::get_timeline() const {
    return timeline;
}

set<TimedSpec> Workload::get_all_specs() const {
    return all_specs;
}

wl_cost_t Workload::cost() const {
    unsigned int ast_val = 0;
    for (set<TimedSpec>::iterator it = all_specs.begin(); it != all_specs.end(); it++) {
        ast_val += (it->get_wl_spec()).ast_size();
    }

    unsigned int timeline_val = (unsigned int) timeline.size();

    return ast_val + timeline_val;
}

bool Workload::is_empty() const {
    if (empty) return true;

    for (set<TimedSpec>::iterator it = all_specs.begin(); it != all_specs.end(); it++) {
        if (it->spec_is_empty()) {
            return true;
        }
    }

    return false;
}

bool Workload::is_all() const {
    return all_specs.size() == 0;
}

void Workload::normalize() {
    empty = false;

    // If total time is zero, we are not
    // putting any constraints so it is "all"
    if (total_time == 0) clear();


    // Normalize each timeline entry
    for (timeline_t::iterator it = timeline.begin(); it != timeline.end(); it++) {
        normalize(it->first);
    }

    // TODO: what happens if an entry is "empty"?
    // do we just continue to this loop? or should we
    // check that first?
    if (timeline.size() > 1) {
        vector<time_range_t> to_erase;

        typedef pair<time_range_t, set<WlSpec>> timeline_entry;
        vector<timeline_entry> to_add;

        bool valid_last_new_entry = false;
        timeline_entry last_new_entry;

        // Merging consecutive timeline entries
        // with the same set of specs.
        for (timeline_t::iterator it = timeline.begin(); it != prev(timeline.end()); it++) {

            timeline_t::iterator next_it = next(it);

            set<WlSpec> specs1 = it->second;
            set<WlSpec> specs2 = next_it->second;

            if (specs1 == specs2) {
                time_range_t time_range_1 = it->first;

                time_range_t time_range_2 = next(it)->first;

                to_erase.push_back(time_range_1);
                to_erase.push_back(time_range_2);

                time_range_t to_add_range(time_range_1.first, time_range_2.second);

                if (valid_last_new_entry) {
                    last_new_entry.first.second = to_add_range.second;
                } else {
                    last_new_entry.first = to_add_range;
                    last_new_entry.second = specs1;
                    valid_last_new_entry = true;
                }
            } else {
                if (valid_last_new_entry) {
                    to_add.push_back(timeline_entry(last_new_entry));
                }
                valid_last_new_entry = false;
            }
        }

        if (valid_last_new_entry) {
            to_add.push_back(timeline_entry(last_new_entry));
        }

        for (unsigned int i = 0; i < to_erase.size(); i++) {
            timeline.erase(to_erase[i]);
        }

        for (unsigned int i = 0; i < to_add.size(); i++) {
            timeline.insert(to_add[i]);
        }
    }

    regenerate_spec_set();

    if (all_specs.size() > max_size) {
        empty = true;
        return;
    }
}

void Workload::normalize(time_range_t time_range) {
    set<WlSpec> specs = timeline[time_range];

    // if one is empty, the entire thing is empty
    for (set<WlSpec>::iterator it = specs.begin(); it != specs.end(); it++) {
        if (it->spec_is_empty()) {
            WlSpec spec = *it;
            timeline[time_range].clear();
            timeline[time_range].insert(spec);
            return;
        }
    }

    // Filter:
    // - Remove the "alls".
    // TODO: implement with the std utilities like sort and remove_if

    set<WlSpec> filtered_specs;
    for (set<WlSpec>::iterator it = specs.begin(); it != specs.end(); it++) {

        if (it->spec_is_all()) continue;

        filtered_specs.insert(*it);
    }
    specs = filtered_specs;


    // ** Zero Propagation ** //

    typedef pair<metric_t, unsigned int> zero_pair;
    vector<zero_pair> zeros;


    // classify into zero and non_zero specs

    vector<WlSpec> zero_specs;
    vector<WlSpec> non_zero_specs;

    for (set<WlSpec>::iterator it = specs.begin(); it != specs.end(); it++) {
        WlSpec spec = *it;

        auto zero_queues = spec.get_zero_queues();
        qset_t zero_queue_set = zero_queues.second;
        bool has_zero = zero_queue_set.size() > 0;


        if (has_zero) {
            metric_t metric = zero_queues.first;

            for (qset_t::iterator z_it = zero_queue_set.begin(); z_it != zero_queue_set.end();
                 z_it++) {
                unsigned int q = *z_it;
                zero_pair p(metric, q);
                zeros.push_back(p);
            }

            zero_specs.push_back(spec);
        }

        else
            non_zero_specs.push_back(spec);
    }

    bool changed = true;
    vector<WlSpec> new_specs;

    unsigned int round = 0;
    while (changed) {
        round++;

        changed = false;
        new_specs.clear();

        for (unsigned int i = 0; i < non_zero_specs.size(); i++) {
            bool spec_changed = false;
            bool empty_lhs = false;

            WlSpec spec = non_zero_specs[i];

            lhs_t lhs = spec.get_lhs();
            comp_t comp = spec.get_comp();
            rhs_t rhs = spec.get_rhs();


            for (unsigned int j = 0; j < zeros.size(); j++) {
                metric_t z_metric = zeros[j].first;
                unsigned int z_q = zeros[j].second;

                if (rhs_applies_to_queue(rhs, z_q)) {
                    trf_t trf = get<trf_t>(rhs);
                    if (holds_alternative<TONE>(trf)) {
                        TONE tone = get<TONE>(trf);
                        if (tone.get_metric() == z_metric) {
                            rhs = 0u;
                            spec_changed = true;
                        }
                    } else if (holds_alternative<TSUM>(trf)) {
                        TSUM tsum = get<TSUM>(trf);
                        if (tsum.get_metric() == z_metric) {
                            qset_t qset = tsum.get_qset();
                            qset.erase(z_q);
                            if (qset.size() == 1) {
                                unsigned int q = *(qset.begin());
                                rhs = TONE(tsum.get_metric(), q);
                            } else {
                                rhs = TSUM(qset, tsum.get_metric());
                            }
                            spec_changed = true;
                        }
                    }
                }

                if (lhs_applies_to_queue(lhs, z_q)) {
                    if (holds_alternative<TONE>(lhs)) {
                        TONE tone = get<TONE>(lhs);
                        if (tone.get_metric() == z_metric) {
                            empty_lhs = true;
                            spec_changed = true;
                        }
                    } else if (holds_alternative<TSUM>(lhs)) {
                        TSUM tsum = get<TSUM>(lhs);
                        if (tsum.get_metric() == z_metric) {
                            qset_t qset = tsum.get_qset();
                            qset.erase(z_q);
                            if (qset.size() == 1) {
                                lhs = TONE(tsum.get_metric(), *(qset.begin()));
                            } else {
                                lhs = TSUM(qset, tsum.get_metric());
                            }
                            spec_changed = true;
                        }
                    }
                }
            }

            if (spec_changed) {
                changed = true;

                if (empty_lhs) {
                    if (holds_alternative<unsigned int>(rhs)) {
                        unsigned int c = get<unsigned int>(rhs);
                        if (!eval_comp(0, comp, c)) {
                            empty = true;
                        }
                        // If not, this spec is all and will not
                        // be added
                    } else if (holds_alternative<TIME>(rhs)) {
                        TIME time = get<TIME>(rhs);
                        bool is_false = false;
                        for (unsigned int t = time_range.first; t <= time_range.second; t++) {
                            unsigned int t_eval = time.get_coeff() * (t + 1);
                            if (!eval_comp(0, comp, t_eval)) {
                                is_false = true;
                                break;
                            }
                        }
                        if (is_false) {
                            empty = true;
                        }
                        // If not, this spec is all and will not
                        // be added
                    } else {
                        lhs = get<trf_t>(rhs);
                        rhs = 0u;
                        comp = neg_comp(comp);

                        WlSpec new_spec = WlSpec(lhs, comp, rhs);

                        auto zero_queues = new_spec.get_zero_queues();
                        bool is_zero = zero_queues.second.size() > 0;
                        if (is_zero) {
                            zero_specs.push_back(new_spec);
                            metric_t metric = zero_queues.first;
                            for (qset_t::iterator it = zero_queues.second.begin();
                                 it != zero_queues.second.end();
                                 it++) {
                                zero_pair p(metric, *it);
                                zeros.push_back(p);
                            }
                        } else {
                            new_specs.push_back(new_spec);
                        }
                    }
                } else {
                    WlSpec new_spec = WlSpec(lhs, comp, rhs);

                    auto zero_queues = new_spec.get_zero_queues();
                    bool is_zero = zero_queues.second.size() > 0;
                    if (is_zero) {
                        zero_specs.push_back(new_spec);
                        metric_t metric = zero_queues.first;
                        for (qset_t::iterator it = zero_queues.second.begin();
                             it != zero_queues.second.end();
                             it++) {
                            zero_pair p(metric, *it);
                            zeros.push_back(p);
                        }
                    } else {
                        new_specs.push_back(new_spec);
                    }
                }
            } else {
                new_specs.push_back(spec);
            }
        }

        if (changed) {
            non_zero_specs.assign(new_specs.begin(), new_specs.end());
        }
    }


    set<WlSpec> final_specs;

    final_specs.insert(zero_specs.begin(), zero_specs.end());
    final_specs.insert(non_zero_specs.begin(), non_zero_specs.end());

    timeline[time_range] = final_specs;
}

void Workload::regenerate_spec_set() {
    if (timeline.size() == 0) {
        clear();
        return;
    }

    vector<TimedSpec> new_all_specs;

    for (timeline_t::iterator it = timeline.begin(); it != timeline.end(); it++) {
        time_range_t time_range = it->first;
        set<WlSpec> specs = it->second;

        for (set<WlSpec>::iterator s_it = specs.begin(); s_it != specs.end(); s_it++) {

            bool already_exists = false;
            for (unsigned int i = 0; i < new_all_specs.size(); i++) {
                if (new_all_specs[i].get_wl_spec() == *s_it &&
                    new_all_specs[i].get_time_range().second + 1 >= time_range.first) {
                    already_exists = true;
                    new_all_specs[i].set_time_range_ub(time_range.second);
                    break;
                }
            }
            if (!already_exists) {
                // Here is where we make new TimedSpec
                TimedSpec tspec = TimedSpec(*s_it, time_range, total_time);
                new_all_specs.push_back(tspec);
            }
        }
    }

    all_specs = std::set<TimedSpec>(new_all_specs.begin(), new_all_specs.end());
}

unsigned int Workload::ast_size() const {
    unsigned int res = 0;
    for (timeline_t::const_iterator it = timeline.cbegin(); it != timeline.cend(); it++) {
        set<WlSpec> specs = it->second;

        if (specs.size() == 0) res++;
        for (set<WlSpec>::iterator s_it = specs.begin(); s_it != specs.end(); s_it++) {
            res += s_it->ast_size();
        }
    }
    return res;
}

set<time_range_t> Workload::add_time_range(time_range_t time_range) {
    set<time_range_t> res;

    timeline_t::iterator beginning;
    timeline_t::iterator end;

    for (timeline_t::iterator it = timeline.begin(); it != timeline.end(); it++) {
        time_range_t entry_time_range = it->first;
        if (includes(entry_time_range, time_range.first)) {
            beginning = it;
            break;
        }
    }

    time_range_t beginning_time_range = beginning->first;
    set<WlSpec> beginning_set = beginning->second;

    if (time_range.first > beginning_time_range.first) {
        timeline[time_range_t(beginning_time_range.first, time_range.first - 1)] = beginning_set;

        timeline[time_range_t(time_range.first, beginning_time_range.second)] = beginning_set;

        timeline.erase(beginning_time_range);
    }

    for (timeline_t::iterator it = timeline.begin(); it != timeline.end(); it++) {
        time_range_t entry_time_range = it->first;
        if (includes(entry_time_range, time_range.second)) {
            end = it;
            break;
        }
    }

    time_range_t end_time_range = end->first;
    set<WlSpec> end_set = end->second;

    if (time_range.second < end_time_range.second) {
        timeline[time_range_t(end_time_range.first, time_range.second)] = end_set;

        timeline[time_range_t(time_range.second + 1, end_time_range.second)] = end_set;

        timeline.erase(end_time_range);
    }

    for (timeline_t::iterator it = timeline.begin(); it != timeline.end(); it++) {
        time_range_t entry_time_range = it->first;
        if (is_superset(time_range, entry_time_range)) {
            res.insert(entry_time_range);
        }
    }

    return res;
}

string Workload::get_timeline_str() {
    stringstream ss;
    if (is_empty()) {
        ss << "FALSE" << std::endl;
    } else if (is_all()) {
        ss << "*" << std::endl;
    } else {
        for (timeline_t::iterator it = timeline.begin(); it != timeline.end(); it++) {
            ss << it->first << ": ";
            set<WlSpec> specs = it->second;
            if (specs.size() == 0) {
                ss << "*" << endl;
                continue;
            }

            bool is_first = true;
            for (set<WlSpec>::iterator it2 = specs.begin(); it2 != specs.end(); it2++) {
                if (!is_first) {
                    ss << "        ";
                }
                ss << *it2 << std::endl;
                is_first = false;
            }
        }
        ss << std::endl;
    }
    return ss.str();
}

std::ostream& operator<<(std::ostream& os, const Workload& wl) {
    if (wl.is_empty())
        os << "empty";
    else if (wl.is_all())
        os << "*";
    else {
        set<TimedSpec> all_specs = wl.get_all_specs();
        for (set<TimedSpec>::iterator it = all_specs.begin(); it != all_specs.end(); it++) {
            os << *it << endl;
        }
    }
    return os;
}


std::ostream& operator<<(std::ostream& os, const Workload* wl) {
    if (wl->is_empty())
        os << "empty";
    else if (wl->is_all())
        os << "*";
    else {
        set<TimedSpec> all_specs = wl->get_all_specs();
        for (set<TimedSpec>::iterator it = all_specs.begin(); it != all_specs.end(); it++) {
            os << *it << endl;
        }
    }
    return os;
}


bool operator==(const Workload& wl1, const Workload& wl2) {
    return wl1.get_timeline() == wl2.get_timeline();
}
