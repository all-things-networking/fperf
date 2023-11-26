//
//  workload.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/19/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "workload.hpp"

#include <algorithm>
#include <set>
#include <sstream>

#include "util.hpp"

//************************************* QSum *************************************//

QSum::QSum(qset_t qset, metric_t metric): qset(qset), metric(metric) {
}

bool QSum::applies_to_queue(unsigned int queue) const {
    return (qset.find(queue) != qset.end());
}

unsigned int QSum::ast_size() const {
    return (unsigned int) qset.size();
}

qset_t QSum::get_qset() const {
    return qset;
}

metric_t QSum::get_metric() const {
    return metric;
}

ostream& operator<<(ostream& os, const QSum& qsum) {
    os << "SUM_[q in " << qsum.qset << "] " << qsum.metric << "(q ,t)";
    return os;
}

bool operator==(const QSum& s1, const QSum& s2) {
    return (s1.qset == s2.qset && s1.metric == s2.metric);
}

bool operator<(const QSum& s1, const QSum& s2) {
    return (s1.qset < s2.qset || (s1.qset == s2.qset && s1.metric < s2.metric));
}

//************************************* Indiv *************************************//

Indiv::Indiv(metric_t metric, unsigned int queue): metric(metric), queue(queue) {
}


bool Indiv::applies_to_queue(unsigned int queue) const {
    return this->queue == queue;
}

unsigned int Indiv::get_queue() const {
    return queue;
}

metric_t Indiv::get_metric() const {
    return metric;
}

ostream& operator<<(ostream& os, const Indiv& indiv) {
    os << indiv.metric << "(" << indiv.queue << ", t)";
    return os;
}

bool operator==(const Indiv& s1, const Indiv& s2) {
    return (s1.metric == s2.metric && s1.queue == s2.queue);
}

bool operator<(const Indiv& s1, const Indiv& s2) {
    return (s1.metric < s2.metric || (s1.metric == s2.metric && s1.queue < s2.queue));
}

//************************************* Time *************************************//

Time::Time(unsigned int coeff): coeff(coeff) {
}

unsigned int Time::get_coeff() const {
    return coeff;
}

ostream& operator<<(ostream& os, const Time& time) {
    if (time.coeff != 1) os << time.coeff;
    os << "t";
    return os;
}

bool operator==(const Time& t1, const Time& t2) {
    return t1.coeff == t2.coeff;
}

bool operator<(const Time& t1, const Time& t2) {
    return t1.coeff < t2.coeff;
}

//************************************* MTRC_EXPR *************************************//

bool m_expr_applies_to_queue(const m_expr_t m_expr, unsigned int queue) {
    switch (m_expr.index()) {
        // QSum
        case 0: {
            return get<QSum>(m_expr).applies_to_queue(queue);
        }
        // Indiv
        case 1: {
            return get<Indiv>(m_expr).applies_to_queue(queue);
        }
        default: break;
    }
    cout << "m_expr_applies_to_queue: should not reach here" << endl;
    return false;
}

unsigned int m_expr_ast_size(const m_expr_t m_expr) {
    if (holds_alternative<QSum>(m_expr)) {
        return get<QSum>(m_expr).ast_size();
    } else
        return 1u;
}

ostream& operator<<(ostream& os, const m_expr_t& m_expr) {
    switch (m_expr.index()) {
        // QSum
        case 0: os << get<QSum>(m_expr); break;
        // Indiv
        case 1: os << get<Indiv>(m_expr); break;
        default: break;
    }
    return os;
}

bool operator==(const m_expr_t& m_expr1, const m_expr_t& m_expr2) {
    if (holds_alternative<QSum>(m_expr1) && holds_alternative<QSum>(m_expr2)) {
        return get<QSum>(m_expr1) == get<QSum>(m_expr2);
    }
    if (holds_alternative<Indiv>(m_expr1) && holds_alternative<Indiv>(m_expr2)) {
        return get<Indiv>(m_expr1) == get<Indiv>(m_expr2);
    }
    return false;
}

bool operator<(const m_expr_t& m_expr1, const m_expr_t& m_expr2) {
    // QSum < Indiv

    if (holds_alternative<QSum>(m_expr1)) {
        if (holds_alternative<QSum>(m_expr2)) {
            return get<QSum>(m_expr1) < get<QSum>(m_expr2);
        } else {
            return true;
        }
    }

    else if (holds_alternative<Indiv>(m_expr1)) {
        if (holds_alternative<Indiv>(m_expr2)) {
            return get<Indiv>(m_expr1) < get<Indiv>(m_expr2);
        } else {
            return false;
        }
    }

    cout << "operator < for m_expr: should not reach here" << endl;
    return false;
}

//************************************* LHS *************************************//

bool lhs_applies_to_queue(const lhs_t lhs, unsigned int queue) {
    return m_expr_applies_to_queue(lhs, queue);
}

unsigned int lhs_ast_size(const lhs_t lhs) {
    return m_expr_ast_size(lhs);
}


//************************************* RHS *************************************//

bool rhs_applies_to_queue(const rhs_t rhs, unsigned int queue) {
    if (holds_alternative<m_expr_t>(rhs)) {
        m_expr_t rhs_m_expr = get<m_expr_t>(rhs);
        return m_expr_applies_to_queue(rhs_m_expr, queue);
    }
    return false;
}

unsigned int rhs_ast_size(const rhs_t rhs) {
    if (holds_alternative<m_expr_t>(rhs)) {
        m_expr_t rhs_m_expr = get<m_expr_t>(rhs);
        return m_expr_ast_size(rhs_m_expr);
    } else
        return 0u;
}

ostream& operator<<(ostream& os, const rhs_t& rhs) {
    switch (rhs.index()) {
        // m_expr
        case 0: {
            os << get<m_expr_t>(rhs);
            break;
        }
        // Time
        case 1: {
            os << get<Time>(rhs);
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
    if (holds_alternative<m_expr_t>(rhs1) && holds_alternative<m_expr_t>(rhs2)) {
        return get<m_expr_t>(rhs1) == get<m_expr_t>(rhs2);
    }
    if (holds_alternative<Time>(rhs1) && holds_alternative<Time>(rhs2)) {
        return get<Time>(rhs1) == get<Time>(rhs2);
    }
    if (holds_alternative<unsigned int>(rhs1) && holds_alternative<unsigned int>(rhs2)) {
        return get<unsigned int>(rhs1) == get<unsigned int>(rhs2);
    }
    return false;
}

bool operator<(const rhs_t& rhs1, const rhs_t& rhs2) {
    // m_expr_t < Time < unsigned int

    if (holds_alternative<m_expr_t>(rhs1)) {
        if (holds_alternative<m_expr_t>(rhs2)) {
            return get<m_expr_t>(rhs1) < get<m_expr_t>(rhs2);
        } else {
            return true;
        }
    }

    else if (holds_alternative<Time>(rhs1)) {
        if (holds_alternative<Time>(rhs2)) {
            return get<Time>(rhs1) < get<Time>(rhs2);
        } else if (holds_alternative<m_expr_t>(rhs2)) {
            return false;
        } else {
            return true;
        }
    }

    if (holds_alternative<unsigned int>(rhs1)) {
        if (holds_alternative<unsigned int>(rhs2)) {
            return get<unsigned int>(rhs1) < get<unsigned int>(rhs2);
        } else {
            return false;
        }
    }
    cout << "operator < for rhs: should not reach here" << endl;
    return false;
}

//************************************* UNIQ *************************************//

Unique::Unique(metric_t metric, qset_t qset): metric(metric), qset(qset) {
}


bool Unique::applies_to_queue(unsigned int queue) const {
    return (qset.find(queue) != qset.end());
}

qset_t Unique::get_qset() const {
    return qset;
}

metric_t Unique::get_metric() const {
    return metric;
}

ostream& operator<<(ostream& os, const Unique& u) {
    os << "Uniqe[" << u.metric << "(" << u.qset << ", t)]";
    return os;
}

bool operator==(const Unique& u1, const Unique& u2) {
    return (u1.metric == u2.metric && u1.qset == u2.qset);
}

bool operator<(const Unique& u1, const Unique& u2) {
    return (u1.metric < u2.metric || (u1.metric == u2.metric && u1.qset < u2.qset));
}

//************************************* SAME *************************************//

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

ostream& operator<<(ostream& os, const Same& s) {
    os << "Same[" << s.metric << "(" << s.queue << ", t)]";
    return os;
}

bool operator==(const Same& s1, const Same& s2) {
    return (s1.metric == s2.metric && s1.queue == s2.queue);
}

bool operator<(const Same& s1, const Same& s2) {
    return (s1.metric < s2.metric || (s1.metric == s2.metric && s1.queue < s2.queue));
}

//************************************* INCR *************************************//

Incr::Incr(metric_t metric, unsigned int queue): metric(metric), queue(queue) {
}


bool Incr::applies_to_queue(unsigned int queue) const {
    return this->queue == queue;
}

unsigned int Incr::get_queue() const {
    return queue;
}

metric_t Incr::get_metric() const {
    return metric;
}

ostream& operator<<(ostream& os, const Incr& incr) {
    os << "Incr[" << incr.metric << "(" << incr.queue << ", t)]";
    return os;
}

bool operator==(const Incr& incr1, const Incr& incr2) {
    return (incr1.metric == incr2.metric && incr1.queue == incr2.queue);
}

bool operator<(const Incr& incr1, const Incr& incr2) {
    return (incr1.metric < incr2.metric ||
            (incr1.metric == incr2.metric && incr1.queue < incr2.queue));
}

//************************************* DECR *************************************//

Decr::Decr(metric_t metric, unsigned int queue): metric(metric), queue(queue) {
}


bool Decr::applies_to_queue(unsigned int queue) const {
    return this->queue == queue;
}

unsigned int Decr::get_queue() const {
    return queue;
}

metric_t Decr::get_metric() const {
    return metric;
}

ostream& operator<<(ostream& os, const Decr& decr) {
    os << "Decr[" << decr.metric << "(" << decr.queue << ", t)]";
    return os;
}

bool operator==(const Decr& decr1, const Decr& decr2) {
    return (decr1.metric == decr2.metric && decr1.queue == decr2.queue);
}

bool operator<(const Decr& decr1, const Decr& decr2) {
    return (decr1.metric < decr2.metric ||
            (decr1.metric == decr2.metric && decr1.queue < decr2.queue));
}

//************************************* Comp *************************************//

// Invariant: An object of this class
//            is always normalized independent of the operation
//            running on it.

Comp::Comp(lhs_t lhs, op_t op, rhs_t rhs): lhs(lhs), op(op), rhs(rhs) {
    normalize();
}

bool Comp::applies_to_queue(unsigned int queue) const {
    bool lhs_applies = lhs_applies_to_queue(lhs, queue);
    bool rhs_applies = rhs_applies_to_queue(rhs, queue);
    return lhs_applies || rhs_applies;
}

void Comp::normalize() {
    // If the right hand side is TRF, we might be able to normalize
    if (holds_alternative<m_expr_t>(rhs)) {

        m_expr_t lhs_m_expr = lhs;
        m_expr_t rhs_m_expr = get<m_expr_t>(rhs);

        bool lhs_is_qsum = holds_alternative<QSum>(lhs_m_expr);
        bool lhs_is_indiv = holds_alternative<Indiv>(lhs_m_expr);

        bool rhs_is_qsum = holds_alternative<QSum>(rhs_m_expr);
        bool rhs_is_indiv = holds_alternative<Indiv>(rhs_m_expr);

        // If both are QSum
        if (lhs_is_qsum && rhs_is_qsum) {

            QSum lhs_qsum = get<QSum>(lhs_m_expr);
            QSum rhs_qsum = get<QSum>(rhs_m_expr);

            if (lhs_qsum.get_metric() == rhs_qsum.get_metric()) {
                qset_t lhs_qset = lhs_qsum.get_qset();
                qset_t rhs_qset = rhs_qsum.get_qset();

                set<unsigned int> inters;
                set_intersection(lhs_qset.begin(),
                                 lhs_qset.end(),
                                 rhs_qset.begin(),
                                 rhs_qset.end(),
                                 inserter(inters, inters.begin()));
                bool intersect = inters.size() > 0;

                if (lhs_qset == rhs_qset) {
                    if (op == op_t::GE || op == op_t::LE) is_all = true;
                    if (op == op_t::GT || op == op_t::LT) is_empty = true;
                } else if (is_superset(rhs_qset, lhs_qset)) {
                    set<unsigned int> diff;
                    set_difference(rhs_qset.begin(),
                                   rhs_qset.end(),
                                   lhs_qset.begin(),
                                   lhs_qset.end(),
                                   inserter(diff, diff.begin()));

                    if (op == op_t::LE)
                        is_all = true;
                    else if (op == op_t::GT)
                        is_empty = true;
                    else if (diff.size() == 1) {
                        lhs = Indiv(rhs_qsum.get_metric(), *(diff.begin()));
                        if (op == op_t::EQ)
                            op = op_t::LE;
                        else
                            op = neg_op(op);
                        rhs = 0u;
                    } else {
                        lhs = QSum(qset_t(diff.begin(), diff.end()), rhs_qsum.get_metric());
                        if (op == op_t::EQ)
                            op = op_t::LE;
                        else
                            op = neg_op(op);
                        rhs = 0u;
                    }
                } else if (is_superset(lhs_qset, rhs_qset)) {
                    set<unsigned int> diff;
                    set_difference(lhs_qset.begin(),
                                   lhs_qset.end(),
                                   rhs_qset.begin(),
                                   rhs_qset.end(),
                                   inserter(diff, diff.begin()));

                    if (op == op_t::LT)
                        is_empty = true;
                    else if (op == op_t::GE)
                        is_all = true;
                    else if (diff.size() == 1) {
                        lhs = Indiv(rhs_qsum.get_metric(), *(diff.begin()));
                        rhs = 0u;
                    } else {
                        lhs = QSum(qset_t(diff.begin(), diff.end()), rhs_qsum.get_metric());
                        rhs = 0u;
                    }
                } else if (intersect) {
                    set<unsigned int> lhs_diff;
                    set_difference(lhs_qset.begin(),
                                   lhs_qset.end(),
                                   rhs_qset.begin(),
                                   rhs_qset.end(),
                                   inserter(lhs_diff, lhs_diff.begin()));

                    set<unsigned int> rhs_diff;
                    set_difference(rhs_qset.begin(),
                                   rhs_qset.end(),
                                   lhs_qset.begin(),
                                   lhs_qset.end(),
                                   inserter(rhs_diff, rhs_diff.begin()));

                    // set lhs
                    if (lhs_diff.size() == 1) {
                        lhs = Indiv(lhs_qsum.get_metric(), *(lhs_diff.begin()));
                    } else {
                        lhs = QSum(qset_t(lhs_diff.begin(), lhs_diff.end()), lhs_qsum.get_metric());
                    }

                    // set rhs
                    if (rhs_diff.size() == 1) {
                        rhs = Indiv(rhs_qsum.get_metric(), *(rhs_diff.begin()));
                    } else {
                        rhs = QSum(qset_t(rhs_diff.begin(), rhs_diff.end()), rhs_qsum.get_metric());
                    }
                }
            }
        }
        // if both are Indiv
        else if (lhs_is_indiv && rhs_is_indiv) {
            Indiv lhs_indiv = get<Indiv>(lhs_m_expr);
            Indiv rhs_indiv = get<Indiv>(rhs_m_expr);
            if (lhs_indiv == rhs_indiv) {
                if (op == op_t::LE || op == op_t::GE)
                    is_all = true;
                else if (op == op_t::LT || op == op_t::GT)
                    is_empty = true;
            }
            // TODO: generalize this to oparable metrics
            if (lhs_indiv.get_metric() != rhs_indiv.get_metric()) {
                is_empty = true;
            }
        }
        // If lhs is Indiv and rhs is QSum, swap
        // Notice here we only have LT or LE as op
        else if (lhs_is_indiv && rhs_is_qsum) {
            m_expr_t tmp = lhs_m_expr;

            lhs = rhs_m_expr;
            lhs_m_expr = rhs_m_expr;

            rhs = tmp;
            rhs_m_expr = tmp;

            op = neg_op(op);

            lhs_is_qsum = true;
            lhs_is_indiv = false;

            rhs_is_qsum = false;
            rhs_is_indiv = true;
        }
        // Both cases of one Indiv and one QSum
        // should end up here. Note that op
        // can be all of LT, LE, GT, GE now
        if (lhs_is_qsum && rhs_is_indiv) {
            QSum lhs_qsum = get<QSum>(lhs_m_expr);
            Indiv rhs_indiv = get<Indiv>(rhs_m_expr);

            qset_t lhs_qset = lhs_qsum.get_qset();
            unsigned int rhs_queue = rhs_indiv.get_queue();

            if (lhs_qsum.get_metric() == rhs_indiv.get_metric()) {
                qset_t::iterator it = lhs_qset.find(rhs_queue);
                if (it != lhs_qset.end()) {
                    if (op == op_t::GE)
                        is_all = true;
                    else if (op == op_t::LT)
                        is_empty = true;
                    else {
                        lhs_qset.erase(it);
                        rhs = 0u;
                        if (lhs_qset.size() == 1) {
                            lhs = Indiv(lhs_qsum.get_metric(), *(lhs_qset.begin()));
                        } else {
                            lhs = QSum(lhs_qset, lhs_qsum.get_metric());
                        }
                    }
                }
            }
        }
    }

    // If the right hand side is constant, op should either be
    // GE or LE
    if (holds_alternative<unsigned int>(rhs)) {
        unsigned int c = get<unsigned int>(rhs);
        if (op == op_t::GT) {
            op = op_t::GE;
            rhs = c + 1;
        } else if (op == op_t::LT) {
            if (c == 0)
                is_empty = true;
            else {
                op = op_t::LE;
                rhs = c - 1;
            }
        }
    }

    if (holds_alternative<Time>(rhs)) {
        unsigned int c = get<Time>(rhs).get_coeff();
        if (c == 0) {
            rhs = c;
        } else {
            if (holds_alternative<Indiv>(lhs)) {
                if (op == op_t::GE && c > MAX_ENQ) is_empty = true;
                if (op == op_t::EQ && c > MAX_ENQ) is_empty = true;
                if (op == op_t::GT && c >= MAX_ENQ) is_empty = true;
            }
            if (holds_alternative<QSum>(lhs)) {
                QSum qsum = get<QSum>(lhs);
                if (op == op_t::GE && c > (qsum.get_qset().size() - 1) * MAX_ENQ) is_empty = true;
                if (op == op_t::EQ && c > (qsum.get_qset().size() - 1) * MAX_ENQ) is_empty = true;
                if (op == op_t::GT && c >= (qsum.get_qset().size() - 1) * MAX_ENQ) is_empty = true;
            }
        }
    }
}

bool Comp::spec_is_empty() const {
    return is_empty;
}

bool Comp::spec_is_all() const {
    return is_all;
}

unsigned int Comp::ast_size() const {
    if (is_all) return 1u;
    if (is_empty) return 0u;
    return lhs_ast_size(lhs) + rhs_ast_size(rhs);
}

pair<metric_t, qset_t> Comp::get_zero_queues() const {
    qset_t qset;
    metric_t metric;

    if (op == op_t::LE && holds_alternative<unsigned int>(rhs) && get<unsigned int>(rhs) == 0) {
        if (holds_alternative<Indiv>(lhs)) {
            Indiv indiv = get<Indiv>(lhs);
            metric = indiv.get_metric();
            if (Metric::properties.at(metric).non_negative) {
                qset.insert(indiv.get_queue());
            }
        } else if (holds_alternative<QSum>(lhs)) {
            QSum qsum = get<QSum>(lhs);
            metric = qsum.get_metric();
            if (Metric::properties.at(metric).non_negative) {
                qset_t s_qset = qsum.get_qset();
                for (qset_t::iterator it = s_qset.begin(); it != s_qset.end(); it++) {
                    qset.insert(*it);
                }
            }
        }
    }
    return make_pair(metric, qset);
}

lhs_t Comp::get_lhs() const {
    return lhs;
}

op_t Comp::get_op() const {
    return op;
}

rhs_t Comp::get_rhs() const {
    return rhs;
}

ostream& operator<<(ostream& os, const Comp& comp) {
    if (comp.is_all)
        os << "*";
    else if (comp.is_empty)
        os << "FALSE";
    else
        os << comp.lhs << " " << comp.op << " " << comp.rhs;
    return os;
}

ostream& operator<<(ostream& os, const Comp* comp) {
    os << comp->lhs << " " << comp->op << " " << comp->rhs;
    return os;
}

bool operator==(const Comp& comp1, const Comp& comp2) {
    lhs_t lhs1 = comp1.lhs;
    op_t op1 = comp1.op;
    rhs_t rhs1 = comp1.rhs;

    lhs_t lhs2 = comp2.lhs;
    op_t op2 = comp2.op;
    rhs_t rhs2 = comp2.rhs;

    bool res = (lhs1 == lhs2 && op1 == op2 && rhs1 == rhs2);
    return res;
}

bool operator!=(const Comp& comp1, const Comp& comp2) {
    return !(comp1 == comp2);
}

bool operator<(const Comp& comp1, const Comp& comp2) {
    lhs_t lhs1 = comp1.lhs;
    op_t op1 = comp1.op;
    rhs_t rhs1 = comp1.rhs;

    lhs_t lhs2 = comp2.lhs;
    op_t op2 = comp2.op;
    rhs_t rhs2 = comp2.rhs;

    return (lhs1 < lhs2 || (lhs1 == lhs2 && op1 < op2) ||
            (lhs1 == lhs2 && op1 == op2 && rhs1 < rhs2));
}

//************************************* WlSpec *************************************//
// TODO: Only checks COMP
bool wl_spec_is_all(wl_spec_t spec) {
    if (holds_alternative<Comp>(spec)) return get<Comp>(spec).spec_is_all();
    return false;
}

// TODO: Only checks COMP
bool wl_spec_is_empty(wl_spec_t spec) {
    if (holds_alternative<Comp>(spec)) return get<Comp>(spec).spec_is_empty();
    return false;
}

unsigned int wl_spec_ast_size(const wl_spec_t wl_spec) {
    if (holds_alternative<Comp>(wl_spec)) {
        return get<Comp>(wl_spec).ast_size();
    } else
        return 1u;
}

bool wl_spec_applies_to_queue(wl_spec_t spec, unsigned int queue) {
    switch (spec.index()) {
        case 0: return get<Comp>(spec).applies_to_queue(queue);
        case 1: return get<Same>(spec).applies_to_queue(queue);
        case 2: return get<Incr>(spec).applies_to_queue(queue);
        case 3: return get<Decr>(spec).applies_to_queue(queue);
        case 4: return get<Unique>(spec).applies_to_queue(queue);
        default: return false;
    }
}

ostream& operator<<(ostream& os, const wl_spec_t& wl_spec) {
    switch (wl_spec.index()) {
        // Comp
        case 0: {
            os << get<Comp>(wl_spec);
            break;
        }
        // Same
        case 1: {
            os << get<Same>(wl_spec);
            break;
        }
        // Incr
        case 2: {
            os << get<Incr>(wl_spec);
            break;
        }
        // Decr
        case 3: {
            os << get<Decr>(wl_spec);
            break;
        }
        // Uniq
        case 4: {
            os << get<Unique>(wl_spec);
            break;
        }
        default: break;
    }
    return os;
}

bool operator==(const wl_spec_t& spec1, const wl_spec_t& spec2) {
    if (holds_alternative<Comp>(spec1) && holds_alternative<Comp>(spec2)) {
        return get<Comp>(spec1) == get<Comp>(spec2);
    }
    if (holds_alternative<Same>(spec1) && holds_alternative<Same>(spec2)) {
        return get<Same>(spec1) == get<Same>(spec2);
    }
    if (holds_alternative<Incr>(spec1) && holds_alternative<Incr>(spec2)) {
        return get<Incr>(spec1) == get<Incr>(spec2);
    }
    if (holds_alternative<Decr>(spec1) && holds_alternative<Decr>(spec2)) {
        return get<Decr>(spec1) == get<Decr>(spec2);
    }
    return false;
}

bool operator<(const wl_spec_t& spec1, const wl_spec_t& spec2) {
    // Comp < Same < Incr < Decr < Uniq

    if (holds_alternative<Comp>(spec1)) {
        if (holds_alternative<Comp>(spec2)) {
            return get<Comp>(spec1) < get<Comp>(spec2);
        } else {
            return true;
        }
    }

    else if (holds_alternative<Same>(spec1)) {
        if (holds_alternative<Same>(spec2)) {
            return get<Same>(spec1) < get<Same>(spec2);
        } else if (holds_alternative<Comp>(spec2)) {
            return false;
        } else {
            return true;
        }
    }

    else if (holds_alternative<Incr>(spec1)) {
        if (holds_alternative<Incr>(spec2)) {
            return get<Incr>(spec1) < get<Incr>(spec2);
        } else if (holds_alternative<Comp>(spec2) || holds_alternative<Same>(spec2)) {
            return false;
        } else {
            return true;
        }
    }

    else if (holds_alternative<Decr>(spec1)) {
        if (holds_alternative<Decr>(spec2)) {
            return get<Decr>(spec1) < get<Decr>(spec2);
        } else {
            return false;
        }
    }

    else if (holds_alternative<Unique>(spec1)) {
        if (holds_alternative<Unique>(spec2)) {
            return get<Unique>(spec1) < get<Unique>(spec2);
        } else if (holds_alternative<Unique>(spec2)) {
            return false;
        } else {
            return true;
        }
    }
    cout << "operator < for wl_spec_t: should not reach here" << endl;
    return false;
}

//************************************* TimedSpec *************************************//

// Invariant: An object of this class
//            is always normalized independent of the operation
//            running on it.

TimedSpec::TimedSpec(wl_spec_t wl_spec, time_range_t time_range, unsigned int total_time):
wl_spec(wl_spec),
time_range(time_range),
total_time(total_time) {
    normalize();
}

TimedSpec::TimedSpec(wl_spec_t wl_spec, unsigned int until_time, unsigned int total_time):
wl_spec(wl_spec),
time_range(time_range_t(0, until_time - 1)),
total_time(total_time) {
    if (until_time == 0) time_range = time_range_t(1, 0);
    normalize();
}


bool TimedSpec::applies_to_queue(unsigned int queue) const {
    return wl_spec_applies_to_queue(wl_spec, queue);
}

void TimedSpec::set_time_range_ub(unsigned int ub) {
    time_range.second = ub;
    normalize();
}

void TimedSpec::normalize() {
    if (wl_spec_is_empty(wl_spec))
        is_empty = true;
    else if (wl_spec_is_all(wl_spec) || time_range.first > time_range.second)
        is_all = true;
    else {
        if (!holds_alternative<Comp>(wl_spec)) return;

        Comp comp = get<Comp>(wl_spec);

        // When rhs is constant
        lhs_t lhs = comp.get_lhs();
        op_t op = comp.get_op();
        rhs_t rhs = comp.get_rhs();

        if (holds_alternative<unsigned int>(rhs)) {
            metric_t metric;
            if (holds_alternative<Indiv>(lhs)) {
                metric = get<Indiv>(lhs).get_metric();
            } else {
                metric = get<QSum>(lhs).get_metric();
            }
            if (Metric::properties.at(metric).non_decreasing) {
                if (op == op_t::GE || op == op_t::GT) {
                    time_range.second = total_time - 1;
                } else if (op == op_t::LE || op == op_t::LT) {
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

wl_spec_t TimedSpec::get_wl_spec() const {
    return wl_spec;
}

ostream& operator<<(ostream& os, const TimedSpec& spec) {
    os << spec.time_range << ": ";
    os << spec.wl_spec;

    return os;
}
ostream& operator<<(ostream& os, const TimedSpec* spec) {
    os << spec->time_range << ": ";
    os << spec->wl_spec;

    return os;
}

bool operator==(const TimedSpec& spec1, const TimedSpec& spec2) {
    wl_spec_t wl_spec1 = spec1.wl_spec;
    wl_spec_t wl_spec2 = spec2.wl_spec;

    return (wl_spec1 == wl_spec2 && spec1.time_range == spec2.time_range);
}

bool operator!=(const TimedSpec& spec1, const TimedSpec& spec2) {
    return !(spec1 == spec2);
}

bool operator<(const TimedSpec& spec1, const TimedSpec& spec2) {
    return (spec1.time_range < spec2.time_range ||
            ((spec1.time_range == spec2.time_range) && (spec1.wl_spec < spec2.wl_spec)));
}

//************************************* Workload *************************************//

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

void Workload::add_spec(TimedSpec spec) {
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

void Workload::rm_spec(TimedSpec spec) {
    auto erase_res = all_specs.erase(spec);
    time_range_t spec_time_range = spec.get_time_range();
    wl_spec_t wl_spec = spec.get_wl_spec();
    if (erase_res > 0) {
        for (timeline_t::iterator it = timeline.begin(); it != timeline.end(); it++) {
            if (is_superset(spec_time_range, it->first)) {
                (it->second).erase(wl_spec);
            }
        }

        normalize();
    }
}

void Workload::mod_spec(TimedSpec old_spec, TimedSpec new_spec) {
    if (all_specs.find(old_spec) != all_specs.end()) {
        rm_spec(old_spec);
        add_spec(new_spec);
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
        ast_val += wl_spec_ast_size(it->get_wl_spec());
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

        typedef pair<time_range_t, set<wl_spec_t>> timeline_entry;
        vector<timeline_entry> to_add;

        bool valid_last_new_entry = false;
        timeline_entry last_new_entry;

        // Merging consecutive timeline entries
        // with the same set of specs.
        for (timeline_t::iterator it = timeline.begin(); it != prev(timeline.end()); it++) {

            timeline_t::iterator next_it = next(it);

            set<wl_spec_t> specs1 = it->second;
            set<wl_spec_t> specs2 = next_it->second;

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

    // TODO: move this to as a check in Search::pick_neighbors
    if (all_specs.size() > max_size) {
        empty = true;
        return;
    }
}

void Workload::normalize(time_range_t time_range) {
    set<wl_spec_t> specs = timeline[time_range];

    // if one is empty, the entire thing is empty
    for (set<wl_spec_t>::iterator it = specs.begin(); it != specs.end(); it++) {
        if (wl_spec_is_empty(*it)) {
            wl_spec_t spec = *it;
            timeline[time_range].clear();
            timeline[time_range].insert(spec);
            return;
        }
    }

    // Filter:
    // - Remove the "alls".
    // TODO: implement with the std utilities like sort and remove_if

    set<wl_spec_t> filtered_specs;
    for (set<wl_spec_t>::iterator it = specs.begin(); it != specs.end(); it++) {

        if (wl_spec_is_all(*it)) continue;

        filtered_specs.insert(*it);
    }
    specs = filtered_specs;


    // ** Zero Propagation ** //

    typedef pair<metric_t, unsigned int> zero_pair;
    vector<zero_pair> zeros;


    // classify Comp specs into zero and non_zero specs
    // TODO: maybe simplify same, incr, and decr?

    vector<Comp> zero_comps;
    vector<Comp> non_zero_comps;
    vector<wl_spec_t> non_op_specs;

    for (set<wl_spec_t>::iterator it = specs.begin(); it != specs.end(); it++) {
        wl_spec_t spec = *it;

        if (!holds_alternative<Comp>(spec)) {
            non_op_specs.push_back(spec);
            continue;
        }

        Comp comp = get<Comp>(spec);

        auto zero_queues = comp.get_zero_queues();
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

            zero_comps.push_back(get<Comp>(spec));
        }

        else
            non_zero_comps.push_back(get<Comp>(spec));
    }

    bool changed = true;
    vector<Comp> new_comps;

    while (changed) {
        changed = false;
        new_comps.clear();

        for (unsigned int i = 0; i < non_zero_comps.size(); i++) {
            bool spec_changed = false;
            bool empty_lhs = false;

            Comp comp = non_zero_comps[i];

            lhs_t lhs = comp.get_lhs();
            op_t op = comp.get_op();
            rhs_t rhs = comp.get_rhs();


            for (unsigned int j = 0; j < zeros.size(); j++) {
                metric_t z_metric = zeros[j].first;
                unsigned int z_q = zeros[j].second;

                if (rhs_applies_to_queue(rhs, z_q)) {
                    m_expr_t m_expr = get<m_expr_t>(rhs);
                    if (holds_alternative<Indiv>(m_expr)) {
                        Indiv indiv = get<Indiv>(m_expr);
                        if (indiv.get_metric() == z_metric) {
                            rhs = 0u;
                            spec_changed = true;
                        }
                    } else if (holds_alternative<QSum>(m_expr)) {
                        QSum qsum = get<QSum>(m_expr);
                        if (qsum.get_metric() == z_metric) {
                            qset_t qset = qsum.get_qset();
                            qset.erase(z_q);
                            if (qset.size() == 1) {
                                unsigned int q = *(qset.begin());
                                rhs = Indiv(qsum.get_metric(), q);
                            } else {
                                rhs = QSum(qset, qsum.get_metric());
                            }
                            spec_changed = true;
                        }
                    }
                }

                if (lhs_applies_to_queue(lhs, z_q)) {
                    if (holds_alternative<Indiv>(lhs)) {
                        Indiv indiv = get<Indiv>(lhs);
                        if (indiv.get_metric() == z_metric) {
                            empty_lhs = true;
                            spec_changed = true;
                        }
                    } else if (holds_alternative<QSum>(lhs)) {
                        QSum qsum = get<QSum>(lhs);
                        if (qsum.get_metric() == z_metric) {
                            qset_t qset = qsum.get_qset();
                            qset.erase(z_q);
                            if (qset.size() == 1) {
                                lhs = Indiv(qsum.get_metric(), *(qset.begin()));
                            } else {
                                lhs = QSum(qset, qsum.get_metric());
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
                        if (!eval_op(0, op, c)) {
                            empty = true;
                        }
                        // If not, this spec is all and will not
                        // be added
                    } else if (holds_alternative<Time>(rhs)) {
                        Time time = get<Time>(rhs);
                        bool is_false = false;
                        for (unsigned int t = time_range.first; t <= time_range.second; t++) {
                            unsigned int t_eval = time.get_coeff() * (t + 1);
                            if (!eval_op(0, op, t_eval)) {
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
                        lhs = get<m_expr_t>(rhs);
                        rhs = 0u;
                        op = neg_op(op);

                        Comp new_comp = Comp(lhs, op, rhs);

                        auto zero_queues = new_comp.get_zero_queues();
                        bool is_zero = zero_queues.second.size() > 0;
                        if (is_zero) {
                            zero_comps.push_back(new_comp);
                            metric_t metric = zero_queues.first;
                            for (qset_t::iterator it = zero_queues.second.begin();
                                 it != zero_queues.second.end();
                                 it++) {
                                zero_pair p(metric, *it);
                                zeros.push_back(p);
                            }
                        } else {
                            new_comps.push_back(new_comp);
                        }
                    }
                } else {
                    Comp new_comp = Comp(lhs, op, rhs);

                    auto zero_queues = new_comp.get_zero_queues();
                    bool is_zero = zero_queues.second.size() > 0;
                    if (is_zero) {
                        zero_comps.push_back(new_comp);
                        metric_t metric = zero_queues.first;
                        for (qset_t::iterator it = zero_queues.second.begin();
                             it != zero_queues.second.end();
                             it++) {
                            zero_pair p(metric, *it);
                            zeros.push_back(p);
                        }
                    } else {
                        new_comps.push_back(new_comp);
                    }
                }
            } else {
                new_comps.push_back(comp);
            }
        }

        if (changed) {
            non_zero_comps.assign(new_comps.begin(), new_comps.end());
        }
    }


    set<wl_spec_t> final_specs;

    final_specs.insert(zero_comps.begin(), zero_comps.end());
    final_specs.insert(non_zero_comps.begin(), non_zero_comps.end());
    final_specs.insert(non_op_specs.begin(), non_op_specs.end());

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
        set<wl_spec_t> specs = it->second;

        for (set<wl_spec_t>::iterator s_it = specs.begin(); s_it != specs.end(); s_it++) {

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

    all_specs = set<TimedSpec>(new_all_specs.begin(), new_all_specs.end());
}

unsigned int Workload::ast_size() const {
    unsigned int res = 0;
    for (timeline_t::const_iterator it = timeline.cbegin(); it != timeline.cend(); it++) {
        set<wl_spec_t> specs = it->second;

        if (specs.size() == 0) res++;
        for (set<wl_spec_t>::iterator s_it = specs.begin(); s_it != specs.end(); s_it++) {
            res += wl_spec_ast_size(*s_it);
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
    set<wl_spec_t> beginning_set = beginning->second;

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
    set<wl_spec_t> end_set = end->second;

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
        ss << "FALSE" << endl;
    } else if (is_all()) {
        ss << "*" << endl;
    } else {
        for (timeline_t::iterator it = timeline.begin(); it != timeline.end(); it++) {
            ss << it->first << ": ";
            set<wl_spec_t> specs = it->second;
            if (specs.size() == 0) {
                ss << "*" << endl;
                continue;
            }

            bool is_first = true;
            for (set<wl_spec_t>::iterator it2 = specs.begin(); it2 != specs.end(); it2++) {
                if (!is_first) {
                    ss << "        ";
                }
                ss << *it2 << endl;
                is_first = false;
            }
        }
        ss << endl;
    }
    return ss.str();
}

ostream& operator<<(ostream& os, const Workload& wl) {
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


ostream& operator<<(ostream& os, const Workload* wl) {
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
