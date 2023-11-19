//
//  workload.hpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/19/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef workload_hpp
#define workload_hpp

#include <iostream>
#include <map>
#include <set>
#include <tuple>
#include <variant>
#include <vector>

#include "cost.hpp"
#include "example.hpp"
#include "metric.hpp"
#include "util.hpp"

/* --------------------------------------------------------------
The Grammar:
 Workload    := /\ (TimedSpec)
 TimedSpec   := forall t in [CONST, CONST] WlSpec
 WlSpec      := SAME | INCR | DECR | COMP | UNIQ
 SAME        := same[MTRC(Q, t)]
 INCR        := incr[MTRC(Q, t)]
 DECR        := decr[MTRC(Q, t)]
 UNIQ        := uniq[MTRC(QSET, t)]
 COMP        := LHS OP RHS
 LHS         := MTRC_EXPR
 RHS         := MTRC_EXPR | TIME | C
 MTRC_EXPR   := QSUM | INDIV
 QSUM        := SUM_[q in QSET] MTRC(q, t)
 INDIV       := MTRC(Q, t)
 TIME        := C.t
----------------------------------------------------------------- */

//************************************* QSUM *************************************//

class QSum {

public:
    QSum(qset_t qset, metric_t metric);

    unsigned int ast_size() const;
    bool applies_to_queue(unsigned int queue) const;

    qset_t get_qset() const;
    metric_t get_metric() const;

private:
    qset_t qset;
    metric_t metric;

    friend std::ostream& operator<<(std::ostream& os, const QSum& tsum);
    friend bool operator==(const QSum& s1, const QSum& s2);
    friend bool operator<(const QSum& s1, const QSum& s2);
    friend class SpecFactory;
};

//************************************* INDIV *************************************//

class Indiv {

public:
    Indiv(metric_t metric, unsigned int queue);

    bool applies_to_queue(unsigned int queue) const;

    unsigned int get_queue() const;
    metric_t get_metric() const;

private:
    metric_t metric;
    unsigned int queue;

    friend std::ostream& operator<<(std::ostream& os, const Indiv& tone);
    friend bool operator==(const Indiv& s1, const Indiv& s2);
    friend bool operator<(const Indiv& s1, const Indiv& s2);
};

//************************************* TIME *************************************//

class Time {

public:
    Time(unsigned int coeff);
    unsigned int get_coeff() const;

private:
    unsigned int coeff;

    friend std::ostream& operator<<(std::ostream& os, const Time& time);
    friend bool operator==(const Time& t1, const Time& t2);
    friend bool operator<(const Time& t1, const Time& t2);
};

//************************************* MTRC_EXPR/LHS *************************************//

typedef std::variant<QSum, Indiv> m_expr_t;
typedef m_expr_t lhs_t;

unsigned int lhs_ast_size(const lhs_t lhs);
unsigned int m_expr_ast_size(const m_expr_t m_expr);

bool lhs_applies_to_queue(const lhs_t lhs, unsigned int queue);
bool m_expr_applies_to_queue(const m_expr_t m_expr, unsigned int queue);

std::ostream& operator<<(std::ostream& os, const m_expr_t& m_expr);

bool operator==(const m_expr_t& m_expr1, const m_expr_t& m_expr2);
bool operator<(const m_expr_t& m_expr1, const m_expr_t& m_expr2);

//************************************* RHS *************************************//

typedef std::variant<m_expr_t, Time, unsigned int> rhs_t;

unsigned int rhs_ast_size(const rhs_t rhs);
bool rhs_applies_to_queue(const rhs_t rhs, unsigned int queue);

std::ostream& operator<<(std::ostream& os, const rhs_t& rhs);

bool operator==(const rhs_t& rhs1, const rhs_t& rhs2);
bool operator<(const rhs_t& rhs1, const rhs_t& rhs2);

//************************************* UNIQ *************************************//
class Unique {

public:
    Unique(metric_t metric, qset_t qset);

    bool applies_to_queue(unsigned int queue) const;

    qset_t get_qset() const;
    metric_t get_metric() const;

private:
    metric_t metric;
    qset_t qset;

    friend std::ostream& operator<<(std::ostream& os, const Unique& u);
    friend bool operator==(const Unique& u1, const Unique& u2);
    friend bool operator<(const Unique& u1, const Unique& u2);
};

//************************************* SAME *************************************//
class Same {

public:
    Same(metric_t metric, unsigned int queue);

    bool applies_to_queue(unsigned int queue) const;

    unsigned int get_queue() const;
    metric_t get_metric() const;

private:
    metric_t metric;
    unsigned int queue;

    friend std::ostream& operator<<(std::ostream& os, const Same& s);
    friend bool operator==(const Same& s1, const Same& s2);
    friend bool operator<(const Same& s1, const Same& s2);
};

//************************************* INCR *************************************//
class Incr {

public:
    Incr(metric_t metric, unsigned int queue);

    bool applies_to_queue(unsigned int queue) const;

    unsigned int get_queue() const;
    metric_t get_metric() const;

private:
    metric_t metric;
    unsigned int queue;

    friend std::ostream& operator<<(std::ostream& os, const Incr& incr);
    friend bool operator==(const Incr& incr1, const Incr& incr2);
    friend bool operator<(const Incr& incr1, const Incr& incr2);
};

//************************************* DECR *************************************//
class Decr {

public:
    Decr(metric_t metric, unsigned int queue);

    bool applies_to_queue(unsigned int queue) const;

    unsigned int get_queue() const;
    metric_t get_metric() const;

private:
    metric_t metric;
    unsigned int queue;

    friend std::ostream& operator<<(std::ostream& os, const Decr& decr);
    friend bool operator==(const Decr& decr1, const Decr& decr2);
    friend bool operator<(const Decr& decr1, const Decr& decr2);
};

//************************************* COMP *************************************//

class Comp {
public:
    Comp(lhs_t lhs, op_t op, rhs_t rhs);

    bool spec_is_empty() const;
    bool spec_is_all() const;
    unsigned int ast_size() const;
    bool applies_to_queue(unsigned int queue) const;
    pair<metric_t, qset_t> get_zero_queues() const;

    lhs_t get_lhs() const;
    op_t get_op() const;
    rhs_t get_rhs() const;

private:
    lhs_t lhs;
    op_t op;
    rhs_t rhs;

    bool is_empty = false;
    bool is_all = false;

    void normalize();

    friend std::ostream& operator<<(std::ostream& os, const Comp& spec);
    friend std::ostream& operator<<(std::ostream& os, const Comp* spec);
    friend bool operator==(const Comp& spec1, const Comp& spec2);
    friend bool operator!=(const Comp& spec1, const Comp& spec2);
    friend bool operator<(const Comp& spec1, const Comp& spec2);
};

//************************************* WlSpec *************************************//
typedef std::variant<Comp, Same, Incr, Decr, Unique> wl_spec_t;

bool wl_spec_applies_to_queue(wl_spec_t spec, unsigned int queue);

bool wl_spec_is_empty(wl_spec_t spec);

bool wl_spec_is_all(wl_spec_t spec);

unsigned int wl_spec_ast_size(const wl_spec_t wl_spec);

std::ostream& operator<<(std::ostream& os, const wl_spec_t& wl_spec);

bool operator==(const wl_spec_t& wl_spec1, const wl_spec_t& wl_spec2);
bool operator<(const wl_spec_t& wl_spec1, const wl_spec_t& wl_spec2);

//************************************* Timed WlSpec *************************************//

class TimedSpec {

public:
    TimedSpec(wl_spec_t wl_spec, time_range_t time_range, unsigned int total_time);
    TimedSpec(wl_spec_t wl_spec, unsigned int until_time, unsigned int total_time);

    bool spec_is_empty() const;
    bool spec_is_all() const;
    bool applies_to_queue(unsigned int queue) const;

    time_range_t get_time_range() const;
    wl_spec_t get_wl_spec() const;

    void set_time_range_ub(unsigned int ub);

private:
    wl_spec_t wl_spec;
    time_range_t time_range;
    unsigned int total_time;

    bool is_empty = false;
    bool is_all = false;

    void normalize();

    friend std::ostream& operator<<(std::ostream& os, const TimedSpec& spec);
    friend std::ostream& operator<<(std::ostream& os, const TimedSpec* spec);
    friend bool operator==(const TimedSpec& spec1, const TimedSpec& spec2);
    friend bool operator!=(const TimedSpec& spec1, const TimedSpec& spec2);
    friend bool operator<(const TimedSpec& spec1, const TimedSpec& spec2);
};


//************************************* Workload *************************************//
typedef map<time_range_t, std::set<wl_spec_t>> timeline_t;

class Workload {
public:
    Workload(unsigned int max_size, unsigned int queue_cnt, unsigned int total_time);

    void clear();
    void add_spec(TimedSpec spec);
    void rm_spec(TimedSpec spec);
    void mod_spec(TimedSpec old_spec, TimedSpec new_spec);

    unsigned long size() const;

    unsigned int get_max_size() const;
    unsigned int get_queue_cnt() const;
    unsigned int get_total_time() const;
    timeline_t get_timeline() const;
    set<TimedSpec> get_all_specs() const;

    bool is_empty() const;
    bool is_all() const;

    wl_cost_t cost() const;

    friend std::ostream& operator<<(std::ostream& os, const Workload& wl);
    friend std::ostream& operator<<(std::ostream& os, const Workload* wl);

private:
    unsigned int max_size;
    unsigned int queue_cnt;
    unsigned int total_time;
    std::set<wl_spec_t> empty_set;

    set<TimedSpec> all_specs;
    timeline_t timeline;

    bool empty = false;
    bool all = false;

    void normalize();
    void normalize(time_range_t time_range);
    void regenerate_spec_set();

    unsigned int ast_size() const;
    string get_timeline_str();

    set<time_range_t> add_time_range(time_range_t time_range);
};

bool operator==(const Workload& wl1, const Workload& wl2);

#endif /* workload_hpp */
