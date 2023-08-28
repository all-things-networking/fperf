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
 TimedSpec   := forall t in [CONST, CONST] (WlSpec | SAME | UNIQUE)
 WlSpec      := TRF COMP RHS
 SAME        := same Q MTRC
 UNIQUE      := unique QSET MTRC
 RHS         := TRF | TIME | CONST
 TRF         := TONE | TSUM
 TSUM        := sigma QSET MTRC
 TIME        := CONST t
----------------------------------------------------------------- */

//************************************* TSUM *************************************//

class TSUM {

public:
    TSUM(qset_t qset, metric_t metric);

    unsigned int ast_size() const;
    bool applies_to_queue(unsigned int queue) const;

    qset_t get_qset() const;
    metric_t get_metric() const;

private:
    qset_t qset;
    metric_t metric;

    friend std::ostream& operator<<(std::ostream& os, const TSUM& tsum);
    friend bool operator==(const TSUM& s1, const TSUM& s2);
    friend bool operator<(const TSUM& s1, const TSUM& s2);
    friend class SpecFactory;
};

//************************************* TONE *************************************//

class TONE {

public:
    TONE(metric_t metric, unsigned int queue);

    bool applies_to_queue(unsigned int queue) const;

    unsigned int get_queue() const;
    metric_t get_metric() const;

private:
    metric_t metric;
    unsigned int queue;

    friend std::ostream& operator<<(std::ostream& os, const TONE& tone);
    friend bool operator==(const TONE& s1, const TONE& s2);
    friend bool operator<(const TONE& s1, const TONE& s2);
};

//************************************* TIME *************************************//

class TIME {

public:
    TIME(unsigned int coeff);
    unsigned int get_coeff() const;

private:
    unsigned int coeff;

    friend std::ostream& operator<<(std::ostream& os, const TIME& time);
    friend bool operator==(const TIME& t1, const TIME& t2);
    friend bool operator<(const TIME& t1, const TIME& t2);
};

//************************************* TRF/LHS *************************************//

typedef std::variant<TSUM, TONE> trf_t;
typedef trf_t lhs_t;

unsigned int lhs_ast_size(const lhs_t lhs);
unsigned int trf_ast_size(const trf_t trf);

bool lhs_applies_to_queue(const lhs_t lhs, unsigned int queue);
bool trf_applies_to_queue(const trf_t trf, unsigned int queue);

std::ostream& operator<<(std::ostream& os, const trf_t& trf);

bool operator==(const trf_t& trf1, const trf_t& trf2);
bool operator<(const trf_t& trf1, const trf_t& trf2);

//************************************* RHS *************************************//

typedef std::variant<trf_t, TIME, unsigned int> rhs_t;

unsigned int rhs_ast_size(const rhs_t rhs);
bool rhs_applies_to_queue(const rhs_t rhs, unsigned int queue);

std::ostream& operator<<(std::ostream& os, const rhs_t& rhs);

bool operator==(const rhs_t& rhs1, const rhs_t& rhs2);
bool operator<(const rhs_t& rhs1, const rhs_t& rhs2);

//************************************* Same *************************************//
class Same {

public:
    Same(metric_t metric, unsigned int queue);

    bool applies_to_queue(unsigned int queue) const;

    unsigned int get_queue() const;
    metric_t get_metric() const;

private:
    metric_t metric;
    unsigned int queue;

    friend std::ostream& operator<<(std::ostream& os, const Same& tone);
    friend bool operator==(const Same& s1, const Same& s2);
    friend bool operator<(const Same& s1, const Same& s2);
};

//************************************* Unique *************************************//
class Unique {

public:
    Unique(qset_t qset, metric_t metric);

    unsigned int ast_size() const;
    bool applies_to_queue(unsigned int queue) const;

    qset_t get_qset() const;
    metric_t get_metric() const;

private:
    qset_t qset;
    metric_t metric;

    friend std::ostream& operator<<(std::ostream& os, const Unique& tsum);
    friend bool operator==(const Unique& s1, const Unique& s2);
    friend bool operator<(const Unique& s1, const Unique& s2);
    friend class SpecFactory;
};


//************************************* WlSpec *************************************//

class WlSpec {
public:
    WlSpec(lhs_t lhs, comp_t comp, rhs_t rhs);

    bool spec_is_empty() const;
    bool spec_is_all() const;
    unsigned int ast_size() const;
    bool applies_to_queue(unsigned int queue) const;
    pair<metric_t, qset_t> get_zero_queues() const;

    lhs_t get_lhs() const;
    comp_t get_comp() const;
    rhs_t get_rhs() const;

private:
    lhs_t lhs;
    comp_t comp;
    rhs_t rhs;

    bool is_empty = false;
    bool is_all = false;

    void normalize();

    friend std::ostream& operator<<(std::ostream& os, const WlSpec& spec);
    friend std::ostream& operator<<(std::ostream& os, const WlSpec* spec);
    friend bool operator==(const WlSpec& spec1, const WlSpec& spec2);
    friend bool operator!=(const WlSpec& spec1, const WlSpec& spec2);
    friend bool operator<(const WlSpec& spec1, const WlSpec& spec2);
};

//************************************* Timed WlSpec *************************************//

class TimedSpec {

public:
    TimedSpec(WlSpec wl_spec, time_range_t time_range, unsigned int total_time);
    TimedSpec(WlSpec wl_spec, unsigned int until_time, unsigned int total_time);

    bool spec_is_empty() const;
    bool spec_is_all() const;
    bool applies_to_queue(unsigned int queue) const;

    time_range_t get_time_range() const;
    WlSpec get_wl_spec() const;

    void set_time_range_ub(unsigned int ub);

private:
    WlSpec wl_spec;
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
typedef map<time_range_t, std::set<WlSpec>> timeline_t;

class Workload {
public:
    Workload(unsigned int max_size, unsigned int queue_cnt, unsigned int total_time);

    void clear();
    void add_wl_spec(TimedSpec spec);
    void rm_wl_spec(TimedSpec spec);
    void mod_wl_spec(TimedSpec old_spec, TimedSpec new_spec);

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
    std::set<WlSpec> empty_set;

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
