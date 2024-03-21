//
//  workload.hpp
//  FPerf
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
#include <string>
#include <memory>

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

//************************************* RHS *************************************//
class Rhs {
public:
    virtual ~Rhs() = default;
    virtual unsigned int ast_size() const;
    virtual bool applies_to_queue(unsigned int queue) const;

    virtual bool operator==(const Rhs& other) const;
    virtual bool operator<(const Rhs& other) const;
};

ostream& operator<<(ostream& os, const Rhs* rhs);
ostream& operator<<(ostream& os, const Rhs& rhs);

//************************************* LHS / MExpr *************************************//

class Lhs : public Rhs {
public:
    virtual ~Lhs() = default;
    virtual unsigned int ast_size() const override;
    virtual bool applies_to_queue(unsigned int queue) const override;
};

class MExpr : public Lhs {
public:
    virtual ~MExpr() = default;
    unsigned int ast_size() const override;
    bool applies_to_queue(unsigned int queue) const override;

    virtual bool operator==(const MExpr& other) const;
    virtual bool operator<(const MExpr& other) const;
};

ostream& operator<<(ostream& os, const MExpr* m_expr);
ostream& operator<<(ostream& os, const MExpr& m_expr);

//************************************* TIME *************************************//

class Time : public Rhs {

public:
    Time(unsigned int coeff);
    unsigned int get_coeff() const;

private:
    unsigned int coeff;

    friend ostream& operator<<(ostream& os, const Time* time);
    friend ostream& operator<<(ostream& os, const Time& time);
    friend bool operator==(const Time& t1, const Time& t2);
    friend bool operator<(const Time& t1, const Time& t2);
};

//************************************* Constant *************************************//

class Constant : public Rhs {

public:
    Constant(unsigned int coeff);
    unsigned int get_coeff() const;

private:
    unsigned int coeff;

    friend ostream& operator<<(ostream& os, const Constant* c);
    friend ostream& operator<<(ostream& os, const Constant& c);
    friend bool operator==(const Constant& c1, const Constant& c2);
    friend bool operator<(const Constant& c1, const Constant& c2);
};

//************************************* QSUM *************************************//

class QSum : public MExpr {

public:
    QSum(qset_t qset, metric_t metric);

    unsigned int ast_size() const;
    bool applies_to_queue(unsigned int queue) const;

    qset_t get_qset() const;
    metric_t get_metric() const;

private:
    qset_t qset;
    metric_t metric;

    friend ostream& operator<<(ostream& os, const QSum* tsum);
    friend ostream& operator<<(ostream& os, const QSum& tsum);
    friend bool operator==(const QSum& s1, const QSum& s2);
    friend bool operator<(const QSum& s1, const QSum& s2);
    friend class SpecFactory;
};

//************************************* INDIV *************************************//

class Indiv : public MExpr {

public:
    Indiv(metric_t metric, unsigned int queue);

    bool applies_to_queue(unsigned int queue) const;

    unsigned int get_queue() const;
    metric_t get_metric() const;

private:
    metric_t metric;
    unsigned int queue;

    friend ostream& operator<<(ostream& os, const Indiv* tone);
    friend ostream& operator<<(ostream& os, const Indiv& tone);
    friend bool operator==(const Indiv& s1, const Indiv& s2);
    friend bool operator<(const Indiv& s1, const Indiv& s2);
};

//************************************* WlSpec *************************************//

class WlSpec {
public:
    virtual bool applies_to_queue(unsigned int queue) const = 0;

    virtual bool spec_is_empty() const;
    virtual bool spec_is_all() const;
    virtual unsigned int ast_size() const;

    friend ostream& operator<<(ostream& os, const WlSpec* wl_spec);
    virtual bool operator==(const WlSpec& other) const = 0;
    bool operator<(const WlSpec& other) const;

protected:
    virtual string to_string() const = 0;

private:
    virtual int type_id() const = 0;
    virtual bool less_than(const WlSpec& other) const = 0;
};

//************************************* UNIQ *************************************//
class Unique : public WlSpec {

public:
    Unique(metric_t metric, qset_t qset);

    bool applies_to_queue(unsigned int queue) const override;

    qset_t get_qset() const;
    metric_t get_metric() const;

    // Override operators
    bool operator==(const WlSpec& other) const override;

private:
    metric_t metric;
    qset_t qset;

    string to_string() const override;
    int type_id() const override;
    bool less_than(const WlSpec& other) const override;
};

//************************************* SAME *************************************//
class Same : public WlSpec {

public:
    Same(metric_t metric, unsigned int queue);

    bool applies_to_queue(unsigned int queue) const override;

    unsigned int get_queue() const;
    metric_t get_metric() const;

    bool operator==(const WlSpec& other) const override;

private:
    metric_t metric;
    unsigned int queue;

    string to_string() const override;
    int type_id() const override;
    bool less_than(const WlSpec& other) const override;
};

//************************************* INCR *************************************//
class Incr : public WlSpec {

public:
    Incr(metric_t metric, unsigned int queue);

    bool applies_to_queue(unsigned int queue) const override;

    unsigned int get_queue() const;
    metric_t get_metric() const;

    bool operator==(const WlSpec& other) const override;

private:
    metric_t metric;
    unsigned int queue;

    string to_string() const override;
    int type_id() const override;
    bool less_than(const WlSpec& other) const override;
};

//************************************* DECR *************************************//
class Decr : public WlSpec {

public:
    Decr(metric_t metric, unsigned int queue);

    bool applies_to_queue(unsigned int queue) const override;

    unsigned int get_queue() const;
    metric_t get_metric() const;

    bool operator==(const WlSpec& other) const override;

private:
    metric_t metric;
    unsigned int queue;

    string to_string() const override;
    int type_id() const override;
    bool less_than(const WlSpec& other) const override;
};

//************************************* COMP *************************************//

class Comp : public WlSpec {
public:
    Comp(Lhs* lhs, Op op, Rhs* rhs);
    Comp(unsigned int, Op, unsigned int) = delete; // This is not allowed

    virtual bool spec_is_empty() const override;
    bool spec_is_all() const override;
    unsigned int ast_size() const override;
    bool applies_to_queue(unsigned int queue) const override;
    pair<metric_t, qset_t> get_zero_queues() const;


    Lhs* get_lhs() const;
    Op get_op() const;
    Rhs* get_rhs() const;

    bool operator==(const WlSpec& other) const override;

private:
    Lhs* lhs;
    Op op;
    Rhs* rhs;

    bool is_empty = false;
    bool is_all = false;

    string to_string() const override;
    int type_id() const override;
    bool less_than(const WlSpec& other) const override;

    void normalize();
};

//************************************* Timed WlSpec *************************************//

class TimedSpec {

public:
    TimedSpec(WlSpec* wl_spec, time_range_t time_range, unsigned int total_time);
    TimedSpec(WlSpec* wl_spec, unsigned int until_time, unsigned int total_time);

    bool spec_is_empty() const;
    bool spec_is_all() const;
    bool applies_to_queue(unsigned int queue) const;

    time_range_t get_time_range() const;
    WlSpec* get_wl_spec() const;

    void set_time_range_ub(unsigned int ub);

protected:
    WlSpec* wl_spec;
    time_range_t time_range;
    unsigned int total_time;

    bool is_empty = false;
    bool is_all = false;

    void normalize();

    friend ostream& operator<<(ostream& os, const TimedSpec& spec);
    friend ostream& operator<<(ostream& os, const TimedSpec* spec);
    friend bool operator==(const TimedSpec& spec1, const TimedSpec& spec2);
    friend bool operator!=(const TimedSpec& spec1, const TimedSpec& spec2);
    friend bool operator<(const TimedSpec& spec1, const TimedSpec& spec2);
};

//************************************* Workload *************************************//
typedef map<time_range_t, set<WlSpec*>> timeline_t;

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

    friend ostream& operator<<(ostream& os, const Workload& wl);
    friend ostream& operator<<(ostream& os, const Workload* wl);

private:
    unsigned int max_size;
    unsigned int queue_cnt;
    unsigned int total_time;
    set<WlSpec*> empty_set;

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
