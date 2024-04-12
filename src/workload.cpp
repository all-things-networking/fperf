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

//************************************* RHS *************************************//

bool Expr::applies_to_queue(unsigned int queue) const {
    // If this object is of type MExpr, then check if it applies to the queue
    const MExpr* m_expr = dynamic_cast<const MExpr*>(this);
    if (m_expr) {
        return m_expr->applies_to_queue(queue);
    }
    return false;
}

unsigned int Expr::ast_size() const {
    const MExpr* m_expr = dynamic_cast<const MExpr*>(this);
    if (m_expr) {
        return m_expr->ast_size();
    }
    return 0u;
}

ostream& operator<<(ostream& os, const Expr* rhs) {
    const MExpr* m_expr = dynamic_cast<const MExpr*>(rhs);
    if (m_expr) {
        os << *m_expr;
    }
    const Time* time = dynamic_cast<const Time*>(rhs);
    if (time) {
        os << *time;
    }
    const Constant* c = dynamic_cast<const Constant*>(rhs);
    if (c) {
        os << *c;
    }
    return os;
}

ostream& operator<<(ostream& os, const Expr& rhs) {
    const MExpr* m_expr = dynamic_cast<const MExpr*>(&rhs);
    if (m_expr) {
        os << *m_expr;
    }
    const Time* time = dynamic_cast<const Time*>(&rhs);
    if (time) {
        os << *time;
    }
    const Constant* c = dynamic_cast<const Constant*>(&rhs);
    if (c) {
        os << *c;
    }
    return os;
}

bool Expr::operator==(const Expr& other) const {
    const MExpr* m_expr = dynamic_cast<const MExpr*>(this);
    if (m_expr) {
        const MExpr* other_m_expr = dynamic_cast<const MExpr*>(&other);
        if (other_m_expr) {
            return *m_expr == *other_m_expr;
        }
    }
    const Time* time = dynamic_cast<const Time*>(this);
    if (time) {
        const Time* other_time = dynamic_cast<const Time*>(&other);
        if (other_time) {
            return *time == *other_time;
        }
    }
    const Constant* c = dynamic_cast<const Constant*>(this);
    if (c) {
        const Constant* other_c = dynamic_cast<const Constant*>(&other);
        if (other_c) {
            return *c == *other_c;
        }
    }
    cout << "Rhs::operator== should not reach here" << endl;
    return false;
}

bool Expr::operator<(const Expr& other) const {
    const MExpr* m_expr = dynamic_cast<const MExpr*>(this);
    if (m_expr) {
        const MExpr* other_m_expr = dynamic_cast<const MExpr*>(&other);
        if (other_m_expr) {
            return *m_expr < *other_m_expr;
        }
    }
    const Time* time = dynamic_cast<const Time*>(this);
    if (time) {
        const Time* other_time = dynamic_cast<const Time*>(&other);
        if (other_time) {
            return *time < *other_time;
        }
    }
    const Constant* c = dynamic_cast<const Constant*>(this);
    if (c) {
        const Constant* other_c = dynamic_cast<const Constant*>(&other);
        if (other_c) {
            return *c < *other_c;
        }
    }
    cout << "Rhs::operator< should not reach here" << endl;
    return false;
}

//************************************* MExpr *************************************//

bool MExpr::applies_to_queue(unsigned int queue) const {
    const QSum* qsum = dynamic_cast<const QSum*>(this);
    if (qsum) {
        return qsum->applies_to_queue(queue);
    }
    const Indiv* indiv = dynamic_cast<const Indiv*>(this);
    if (indiv) {
        return indiv->applies_to_queue(queue);
    }
    cout << "MExpr::applies_to_queue: should not reach here" << endl;
    return false;
}

unsigned int MExpr::ast_size() const {
    const QSum* qsum = dynamic_cast<const QSum*>(this);
    if (qsum) {
        return qsum->ast_size();
    }
    return 1u;
}

ostream& operator<<(ostream& os, const MExpr* m_expr) {
    const QSum* qsum = dynamic_cast<const QSum*>(m_expr);
    if (qsum) {
        os << *qsum;
    }
    const Indiv* indiv = dynamic_cast<const Indiv*>(m_expr);
    if (indiv) {
        os << *indiv;
    }
    return os;
}

ostream& operator<<(ostream& os, const MExpr& m_expr) {
    const QSum* qsum = dynamic_cast<const QSum*>(&m_expr);
    if (qsum) {
        os << *qsum;
    }
    const Indiv* indiv = dynamic_cast<const Indiv*>(&m_expr);
    if (indiv) {
        os << *indiv;
    }
    return os;
}

bool MExpr::operator==(const MExpr& other) const {
    if (dynamic_cast<const QSum*>(this) && dynamic_cast<const QSum*>(&other)) {
        return *dynamic_cast<const QSum*>(this) == *dynamic_cast<const QSum*>(&other);
    }
    if (dynamic_cast<const Indiv*>(this) && dynamic_cast<const Indiv*>(&other)) {
        return *dynamic_cast<const Indiv*>(this) == *dynamic_cast<const Indiv*>(&other);
    }
    return false;
}

bool MExpr::operator<(const MExpr& other) const {
    if (dynamic_cast<const QSum*>(this) && dynamic_cast<const QSum*>(&other)) {
        return *dynamic_cast<const QSum*>(this) < *dynamic_cast<const QSum*>(&other);
    }
    if (dynamic_cast<const Indiv*>(this) && dynamic_cast<const Indiv*>(&other)) {
        return *dynamic_cast<const Indiv*>(this) < *dynamic_cast<const Indiv*>(&other);
    }
    return false;
}

//************************************* Time *************************************//

Time::Time(unsigned int coeff): coeff(coeff) {
}

unsigned int Time::get_coeff() const {
    return coeff;
}

ostream& operator<<(ostream& os, const Time* time) {
    if (time->coeff != 1) os << time->coeff;
    os << "t";
    return os;
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

//************************************* Constant *************************************//

Constant::Constant(unsigned int coeff): coeff(coeff) {}

unsigned int Constant::get_coeff() const {
    return coeff;
}

ostream& operator<<(ostream& os, const Constant* c) {
    os << c->coeff;
    return os;
}

ostream& operator<<(ostream& os, const Constant& c) {
    os << c.coeff;
    return os;
}

bool operator==(const Constant& c1, const Constant& c2) {
    return c1.coeff == c2.coeff;
}

bool operator<(const Constant& c1, const Constant& c2) {
    return c1.coeff < c2.coeff;
}

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

ostream& operator<<(ostream& os, const QSum* qsum) {
    os << "SUM_[q in " << qsum->qset << "] " << qsum->metric << "(q ,t)";
    return os;
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

ostream& operator<<(ostream& os, const Indiv* indiv) {
    os << indiv->metric << "(" << indiv->queue << ", t)";
    return os;
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

bool Unique::operator==(const WlSpec& other) const {
    if (dynamic_cast<const Unique*>(&other) == NULL) return false;
    const Unique* other_unique = dynamic_cast<const Unique*>(&other);
    return (this->metric == other_unique->metric && this->qset == other_unique->qset);
}

bool Unique::less_than(const WlSpec& other) const {
    const Unique* other_unique = dynamic_cast<const Unique*>(&other);
    return (this->metric < other_unique->metric ||
            (this->metric == other_unique->metric && this->qset < other_unique->qset));
}

int Unique::type_id() const {
    return 5;
}

std::string Unique::to_string() const {
    stringstream ss;
    ss << "Unique[" << metric << "(" << qset << ", t)]";
    return ss.str();
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

bool Same::operator==(const WlSpec& other) const {
    if (dynamic_cast<const Same*>(&other) == NULL) return false;
    const Same* other_same = dynamic_cast<const Same*>(&other);
    return (this->metric == other_same->metric && this->queue == other_same->queue);
}

bool Same::less_than(const WlSpec& other) const {
    const Same* other_same = dynamic_cast<const Same*>(&other);
    return (this->metric < other_same->metric ||
            (this->metric == other_same->metric && this->queue < other_same->queue));
}

int Same::type_id() const {
    return 2;
}

std::string Same::to_string() const {
    stringstream ss;
    ss << "Same[" << metric << "(" << queue << ", t)]";
    return ss.str();
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

bool Incr::operator==(const WlSpec& other) const {
    if (dynamic_cast<const Incr*>(&other) == NULL) return false;
    const Incr* other_incr = dynamic_cast<const Incr*>(&other);
    return (this->metric == other_incr->metric && this->queue == other_incr->queue);
}

bool Incr::less_than(const WlSpec &other) const {
    const Incr* other_incr = dynamic_cast<const Incr*>(&other);
    return (this->metric < other_incr->metric ||
            (this->metric == other_incr->metric && this->queue < other_incr->queue));
}

int Incr::type_id() const {
    return 3;
}

std::string Incr::to_string() const {
    stringstream ss;
    ss << "Incr[" << metric << "(" << queue << ", t)]";
    return ss.str();
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

bool Decr::operator==(const WlSpec& other) const {
    if (dynamic_cast<const Decr*>(&other) == NULL) return false;
    const Decr* other_decr = dynamic_cast<const Decr*>(&other);
    return (this->metric == other_decr->metric && this->queue == other_decr->queue);
}

bool Decr::less_than(const WlSpec &other) const {
    const Decr* other_decr = dynamic_cast<const Decr*>(&other);
    return (this->metric < other_decr->metric ||
            (this->metric == other_decr->metric && this->queue < other_decr->queue));
}

int Decr::type_id() const {
    return 4;
}

std::string Decr::to_string() const {
    stringstream ss;
    ss << "Decr[" << metric << "(" << queue << ", t)]";
    return ss.str();
}

//************************************* Comp *************************************//

// Invariant: An object of this class
//            is always normalized independent of the operation
//            running on it.

Comp::Comp(MExpr* lhs, Op op, Expr* rhs): lhs(lhs), op(op), rhs(rhs) {
    if(lhs==nullptr || rhs==nullptr) {
        throw std::invalid_argument("lhs and rhs cannot be null");
    }
    normalize();
}

bool Comp::applies_to_queue(unsigned int queue) const {
    bool lhs_applies = lhs->applies_to_queue(queue);
    bool rhs_applies = rhs->applies_to_queue(queue);
    return lhs_applies || rhs_applies;
}

void Comp::normalize() {
    // If the right hand side is TRF, we might be able to normalize
    // Cast Rhs* rhs to MExpr
    const MExpr* m_expr = dynamic_cast<const MExpr*>(rhs);
    if (m_expr) {

        MExpr* lhs_m_expr = dynamic_cast<MExpr*>(lhs);
        MExpr* rhs_m_expr = dynamic_cast<MExpr*>(rhs);

        const QSum* lhs_qsum = dynamic_cast<const QSum*>(lhs_m_expr);
        bool lhs_is_qsum = (lhs_qsum != NULL);
        const Indiv* lhs_indiv = dynamic_cast<const Indiv*>(lhs_m_expr);
        bool lhs_is_indiv = (lhs_indiv != NULL);

        const QSum* rhs_qsum = dynamic_cast<const QSum*>(rhs_m_expr);
        bool rhs_is_qsum = (rhs_qsum != NULL);
        const Indiv* rhs_indiv = dynamic_cast<const Indiv*>(rhs_m_expr);
        bool rhs_is_indiv = (rhs_indiv != NULL);

        // If both are QSum
        if (lhs_is_qsum && rhs_is_qsum) {

            if (lhs_qsum->get_metric() == rhs_qsum->get_metric()) {
                qset_t lhs_qset = lhs_qsum->get_qset();
                qset_t rhs_qset = rhs_qsum->get_qset();

                set<unsigned int> inters;
                set_intersection(lhs_qset.begin(),
                                 lhs_qset.end(),
                                 rhs_qset.begin(),
                                 rhs_qset.end(),
                                 inserter(inters, inters.begin()));
                bool intersect = inters.size() > 0;

                if (lhs_qset == rhs_qset) {
                    if (op.get_type() == Op::Type::GE || op.get_type() == Op::Type::LE) is_all = true;
                    if (op.get_type() == Op::Type::GT || op.get_type() == Op::Type::LT) is_empty = true;
                } else if (is_superset(rhs_qset, lhs_qset)) {
                    set<unsigned int> diff;
                    set_difference(rhs_qset.begin(),
                                   rhs_qset.end(),
                                   lhs_qset.begin(),
                                   lhs_qset.end(),
                                   inserter(diff, diff.begin()));

                    if (op.get_type() == Op::Type::LE)
                        is_all = true;
                    else if (op.get_type() == Op::Type::GT)
                        is_empty = true;
                    else if (diff.size() == 1) {
                        lhs = new Indiv(rhs_qsum->get_metric(), *(diff.begin()));
                        if (op.get_type() == Op::Type::EQ)
                            op = Op(Op::Type::LE);
                        else
                            op.neg();
                        rhs = new Constant(0u);
                    } else {
                        lhs = new QSum(qset_t(diff.begin(), diff.end()), rhs_qsum->get_metric());
                        if (op.get_type() == Op::Type::EQ)
                            op = Op(Op::Type::LE);
                        else
                            op.neg();
                        rhs = new Constant(0u);
                    }
                } else if (is_superset(lhs_qset, rhs_qset)) {
                    set<unsigned int> diff;
                    set_difference(lhs_qset.begin(),
                                   lhs_qset.end(),
                                   rhs_qset.begin(),
                                   rhs_qset.end(),
                                   inserter(diff, diff.begin()));

                    if (op.get_type() == Op::Type::LT)
                        is_empty = true;
                    else if (op.get_type() == Op::Type::GE)
                        is_all = true;
                    else if (diff.size() == 1) {
                        lhs = new Indiv(rhs_qsum->get_metric(), *(diff.begin()));
                        rhs = new Constant(0u);
                    } else {
                        lhs = new QSum(qset_t(diff.begin(), diff.end()), rhs_qsum->get_metric());
                        rhs = new Constant(0u);
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
                        lhs = new Indiv(lhs_qsum->get_metric(), *(lhs_diff.begin()));
                    } else {
                        lhs = new QSum(qset_t(lhs_diff.begin(), lhs_diff.end()), lhs_qsum->get_metric());
                    }

                    // set rhs
                    if (rhs_diff.size() == 1) {
                        rhs = new Indiv(rhs_qsum->get_metric(), *(rhs_diff.begin()));
                    } else {
                        rhs = new QSum(qset_t(rhs_diff.begin(), rhs_diff.end()), rhs_qsum->get_metric());
                    }
                }
            }
        }
        // if both are Indiv
        else if (lhs_is_indiv && rhs_is_indiv) {
            if (lhs_indiv == rhs_indiv) {
                if (op.get_type() == Op::Type::LE || op.get_type() == Op::Type::GE)
                    is_all = true;
                else if (op.get_type() == Op::Type::LT || op.get_type() == Op::Type::GT)
                    is_empty = true;
            }
            // TODO: generalize this to oparable metrics
            if (lhs_indiv->get_metric() != rhs_indiv->get_metric()) {
                is_empty = true;
            }
        }
        // If lhs is Indiv and rhs is QSum, swap
        // Notice here we only have LT or LE as op
        else if (lhs_is_indiv && rhs_is_qsum) {
            MExpr* tmp = lhs_m_expr;

            lhs = rhs_m_expr;
            lhs_m_expr = rhs_m_expr;

            rhs = tmp;
            rhs_m_expr = tmp;

            op.neg();

            lhs_is_qsum = true;
            lhs_is_indiv = false;

            rhs_is_qsum = false;
            rhs_is_indiv = true;
        }
        // Both cases of one Indiv and one QSum
        // should end up here. Note that op
        // can be all of LT, LE, GT, GE now
        if (lhs_is_qsum && rhs_is_indiv) {

            qset_t lhs_qset = lhs_qsum->get_qset();
            unsigned int rhs_queue = rhs_indiv->get_queue();

            if (lhs_qsum->get_metric() == rhs_indiv->get_metric()) {
                qset_t::iterator it = lhs_qset.find(rhs_queue);
                if (it != lhs_qset.end()) {
                    if (op.get_type() == Op::Type::GE)
                        is_all = true;
                    else if (op.get_type() == Op::Type::LT)
                        is_empty = true;
                    else {
                        lhs_qset.erase(it);
                        rhs = new Constant(0u);
                        if (lhs_qset.size() == 1) {
                            lhs = new Indiv(lhs_qsum->get_metric(), *(lhs_qset.begin()));
                        } else {
                            lhs = new QSum(lhs_qset, lhs_qsum->get_metric());
                        }
                    }
                }
            }
        }
    }

    // If the right hand side is constant, op should either be
    // GE or LE
    const Constant* constant = dynamic_cast<const Constant*>(rhs);
    if (constant) {
        if (op.get_type() == Op::Type::GT) {
            op = Op(Op::Type::GE);
            rhs = new Constant(constant->get_coeff() + 1);
        } else if (op.get_type() == Op::Type::LT) {
            if (constant == 0)
                is_empty = true;
            else {
                op = Op(Op::Type::LE);
                rhs = new Constant(constant->get_coeff() - 1);
            }
        }
    }

    const Time* t = dynamic_cast<const Time*>(rhs);
    if (t) {
        unsigned int c = t->get_coeff();
        if (c == 0) {
            rhs = new Constant(c);
        } else {
            const Indiv* indiv = dynamic_cast<const Indiv*>(lhs);
            if (indiv) {
                if (op.get_type() == Op::Type::GE && c > MAX_ENQ) is_empty = true;
                if (op.get_type() == Op::Type::EQ && c > MAX_ENQ) is_empty = true;
                if (op.get_type() == Op::Type::GT && c >= MAX_ENQ) is_empty = true;
            }
            const QSum* qsum = dynamic_cast<const QSum*>(lhs);
            if (qsum) {
                if (op.get_type() == Op::Type::GE && c > (qsum->get_qset().size() - 1) * MAX_ENQ) is_empty = true;
                if (op.get_type() == Op::Type::EQ && c > (qsum->get_qset().size() - 1) * MAX_ENQ) is_empty = true;
                if (op.get_type() == Op::Type::GT && c >= (qsum->get_qset().size() - 1) * MAX_ENQ) is_empty = true;
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
    return lhs->ast_size() + rhs->ast_size();
}

pair<metric_t, qset_t> Comp::get_zero_queues() const {
    qset_t qset;
    metric_t metric;

    const Constant* c = dynamic_cast<const Constant*>(rhs);

    if (op.get_type() == Op::Type::LE && c && c->get_coeff() == 0) {
        const Indiv* indiv = dynamic_cast<const Indiv*>(lhs);
        const QSum* qsum = dynamic_cast<const QSum*>(lhs);
        if (indiv) {
            metric = indiv->get_metric();
            if (Metric::properties.at(metric).non_negative) {
                qset.insert(indiv->get_queue());
            }
        } else if (qsum) {
            metric = qsum->get_metric();
            if (Metric::properties.at(metric).non_negative) {
                qset_t s_qset = qsum->get_qset();
                for (qset_t::iterator it = s_qset.begin(); it != s_qset.end(); it++) {
                    qset.insert(*it);
                }
            }
        }
    }
    return make_pair(metric, qset);
}

MExpr* Comp::get_lhs() const {
    return lhs;
}

Op Comp::get_op() const {
    return op;
}

Expr* Comp::get_rhs() const {
    return rhs;
}

bool Comp::operator==(const WlSpec& other) const {
    if (dynamic_cast<const Comp*>(&other) == NULL) return false;
    const Comp* other_comp = dynamic_cast<const Comp*>(&other);
    return (this->lhs == other_comp->lhs && this->op == other_comp->op && this->rhs == other_comp->rhs);
}

bool Comp::less_than(const WlSpec& other) const {
    const Comp* other_comp = dynamic_cast<const Comp*>(&other);
    return (this->lhs < other_comp->lhs ||
            (this->lhs == other_comp->lhs && this->op < other_comp->op) ||
            (this->lhs == other_comp->lhs && this->op == other_comp->op && this->rhs < other_comp->rhs));
}

int Comp::type_id() const {
    return 1;
}

std::string Comp::to_string() const {
    std::ostringstream os;
    if (is_all)
        os << "*";
    else if (is_empty)
        os << "FALSE";
    else
        os << lhs << " " << op << " " << rhs;
    return os.str();
}

//************************************* WlSpec *************************************//
bool WlSpec::spec_is_empty() const {
    return false;
}

bool WlSpec::spec_is_all() const {
    return false;
}

unsigned int WlSpec::ast_size() const {
    return 1u;
}

bool WlSpec::operator<(const WlSpec& other) const {
    if (type_id() != other.type_id()) return type_id() < other.type_id();

    return less_than(other);
}

ostream& operator<<(ostream& os, const WlSpec* wl_spec) {
    os << wl_spec->to_string();
    return os;
}

//************************************* TimedSpec *************************************//

// Invariant: An object of this class
//            is always normalized independent of the operation
//            running on it.

TimedSpec::TimedSpec(WlSpec* wl_spec, time_range_t time_range, unsigned int total_time):
wl_spec(wl_spec),
time_range(time_range),
total_time(total_time) {
    normalize();
}

TimedSpec::TimedSpec(WlSpec* wl_spec, unsigned int until_time, unsigned int total_time):
wl_spec(wl_spec),
time_range(time_range_t(0, until_time - 1)),
total_time(total_time) {
    if (until_time == 0) time_range = time_range_t(1, 0);
    normalize();
}


bool TimedSpec::applies_to_queue(unsigned int queue) const {
    return wl_spec->applies_to_queue(queue);
}

void TimedSpec::set_time_range_ub(unsigned int ub) {
    time_range.second = ub;
    normalize();
}

void TimedSpec::normalize() {
    if (wl_spec->spec_is_empty()) {
        is_empty = true;
    } else if (wl_spec->spec_is_all() || time_range.first > time_range.second) {
        is_all = true;
    } else {
        auto compSpec = dynamic_cast<Comp*>(wl_spec);
        if (compSpec) {
            MExpr* lhs = compSpec->get_lhs();
            Op op = compSpec->get_op();
            Expr* rhs = compSpec->get_rhs();

            const Constant* c = dynamic_cast<const Constant*>(rhs);
            if (c) {
                metric_t metric;
                const Indiv* indiv = dynamic_cast<const Indiv*>(lhs);
                const QSum* qsum = dynamic_cast<const QSum*>(lhs);
                if (indiv) {
                    metric = indiv->get_metric();
                } else {
                    metric = qsum->get_metric();
                }
                if (Metric::properties.at(metric).non_decreasing) {
                    if (op.get_type() == Op::Type::GE || op.get_type() == Op::Type::GT) {
                        time_range.second = total_time - 1;
                    } else if (op.get_type() == Op::Type::LE || op.get_type() == Op::Type::LT) {
                        time_range.first = 0;
                    }
                }
            }
        } else {
            // TODO: handle other types of WlSpec
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

WlSpec* TimedSpec::get_wl_spec() const {
    return wl_spec;
}

ostream& operator<<(ostream& os, const TimedSpec& spec) {
    os << spec.time_range << ": ";
    os << spec.wl_spec;
    return os;
}
ostream& operator<<(ostream& os, const TimedSpec* spec) {
    if (spec) {
        os << *spec;
    }
    return os;
}

bool operator==(const TimedSpec& spec1, const TimedSpec& spec2) {
    WlSpec* wl_spec1 = spec1.get_wl_spec();
    WlSpec* wl_spec2 = spec2.get_wl_spec();
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
    WlSpec* wl_spec = spec.get_wl_spec();
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
        WlSpec* spec = it->get_wl_spec();
        ast_val += spec->ast_size();
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
        typedef pair<time_range_t, set<WlSpec*>> timeline_entry;
        vector<timeline_entry> to_add;

        bool valid_last_new_entry = false;
        timeline_entry last_new_entry;

        // Merging consecutive timeline entries
        // with the same set of specs.
        for (timeline_t::iterator it = timeline.begin(); it != prev(timeline.end()); it++) {

            timeline_t::iterator next_it = next(it);

            set<WlSpec*> specs1 = it->second;
            set<WlSpec*> specs2 = next_it->second;

            if (specs1 == specs2) {
                time_range_t time_range_1 = it->first;
                time_range_t time_range_2 = next_it->first;
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
    set<WlSpec*> specs = timeline[time_range];

    // if one is empty, the entire thing is empty
    for (set<WlSpec*>::iterator it = specs.begin(); it != specs.end(); it++) {
        WlSpec* spec = *it;
        if (spec->spec_is_empty()) {
            WlSpec* spec = *it;
            timeline[time_range].clear();
            timeline[time_range].insert(spec);
            return;
        }
    }

    // Filter:
    // - Remove the "alls".
    // TODO: implement with the std utilities like sort and remove_if

    set<WlSpec*> filtered_specs;
    for (set<WlSpec*>::iterator it = specs.begin(); it != specs.end(); it++) {

        // Check if all
        if ((*it)->spec_is_all()) { continue; }

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
    vector<WlSpec*> non_op_specs;

    for (auto it = specs.begin(); it != specs.end(); it++) {
        auto spec = *it;

        // Attempt to cast spec to Comp.
        auto compSpec = dynamic_cast<Comp*>(spec);

        if (compSpec) {
            // If spec is of Comp type, process it.
            auto zero_queues = compSpec->get_zero_queues();
            qset_t zero_queue_set = zero_queues.second;
            bool has_zero = zero_queue_set.size() > 0;

            if (has_zero) {
                metric_t metric = zero_queues.first;

                for (auto z_it = zero_queue_set.begin(); z_it != zero_queue_set.end(); z_it++) {
                    unsigned int q = *z_it;
                    zero_pair p(metric, q);
                    zeros.push_back(p);
                }

                zero_comps.push_back(*compSpec);
            } else {
                non_zero_comps.push_back(*compSpec);
            }
        } else {
            // If spec is not of Comp type, handle accordingly.
            non_op_specs.push_back(spec);
        }
    }

    bool changed = true;
    vector<Comp> new_comps;

    unsigned int round = 0;
    while (changed) {
        round++;

        changed = false;
        new_comps.clear();

        for (unsigned int i = 0; i < non_zero_comps.size(); i++) {
            bool spec_changed = false;
            bool empty_lhs = false;

            Comp comp = non_zero_comps[i];

            MExpr* lhs = comp.get_lhs();
            Op op = comp.get_op();
            Expr* rhs = comp.get_rhs();


            for (unsigned int j = 0; j < zeros.size(); j++) {
                metric_t z_metric = zeros[j].first;
                unsigned int z_q = zeros[j].second;

                if (lhs->applies_to_queue(z_q)) {
                    const MExpr* m_expr = dynamic_cast<const MExpr*>(rhs);
                    const Indiv* indiv = dynamic_cast<const Indiv*>(m_expr);
                    const QSum* qsum = dynamic_cast<const QSum*>(m_expr);
                    if (indiv) {
                        if (indiv->get_metric() == z_metric) {
                            rhs = 0u;
                            spec_changed = true;
                        }
                    } else if (qsum) {
                        if (qsum->get_metric() == z_metric) {
                            qset_t qset = qsum->get_qset();
                            qset.erase(z_q);
                            if (qset.size() == 1) {
                                unsigned int q = *(qset.begin());
                                rhs = new Indiv(qsum->get_metric(), q);
                            } else {
                                rhs = new QSum(qset, qsum->get_metric());
                            }
                            spec_changed = true;
                        }
                    }
                }

                if (lhs->applies_to_queue(z_q)) {
                    const Indiv* indiv = dynamic_cast<const Indiv*>(lhs);
                    const QSum* qsum = dynamic_cast<const QSum*>(lhs);
                    if (indiv) {
                        if (indiv->get_metric() == z_metric) {
                            empty_lhs = true;
                            spec_changed = true;
                        }
                    } else if (qsum) {
                        if (qsum->get_metric() == z_metric) {
                            qset_t qset = qsum->get_qset();
                            qset.erase(z_q);
                            if (qset.size() == 1) {
                                lhs = new Indiv(qsum->get_metric(), *(qset.begin()));
                            } else {
                                lhs = new QSum(qset, qsum->get_metric());
                            }
                            spec_changed = true;
                        }
                    }
                }
            }

            if (spec_changed) {
                changed = true;

                if (empty_lhs) {
                    const Constant* c = dynamic_cast<const Constant*>(rhs);
                    const Time* time = dynamic_cast<const Time*>(rhs);
                    if (c) {
                        if (!Op::eval(0, op, c->get_coeff())) {
                            empty = true;
                        }
                        // If not, this spec is all and will not
                        // be added
                    } else if (time) {
                        bool is_false = false;
                        for (unsigned int t = time_range.first; t <= time_range.second; t++) {
                            unsigned int t_eval = time->get_coeff() * (t + 1);
                            if (!Op::eval(0, op, t_eval)) {
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
                        lhs = dynamic_cast<MExpr*>(rhs);
                        rhs = 0u;
                        op.neg();

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


    set<WlSpec*> final_specs;

    for (const auto& comp : zero_comps) {
        final_specs.insert(new Comp(comp));
    }
    for (const auto& comp : non_zero_comps) {
        final_specs.insert(new Comp(comp));
    }
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
        set<WlSpec*> specs = it->second;

        for (set<WlSpec*>::iterator s_it = specs.begin(); s_it != specs.end();
             s_it++) {

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
        set<WlSpec*> specs = it->second;

        if (specs.size() == 0) res++;
        for (set<WlSpec*>::const_iterator s_it = specs.cbegin(); s_it != specs.cend(); s_it++) {
            res += (*s_it)->ast_size();
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
    set<WlSpec*> beginning_set = beginning->second;

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
    set<WlSpec*> end_set = end->second;

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
            set<WlSpec*> specs = it->second;
            if (specs.size() == 0) {
                ss << "*" << endl;
                continue;
            }

            bool is_first = true;
            for (set<WlSpec*>::iterator it2 = specs.begin(); it2 != specs.end();
                 it2++) {
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
