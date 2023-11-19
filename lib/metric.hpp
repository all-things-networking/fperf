//
//  metric.hpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/17/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef metric_hpp
#define metric_hpp

#include <map>

#include "example.hpp"
#include "net_context.hpp"
#include "queue.hpp"
#include "util.hpp"

class Queue;

enum class metric_t { CENQ = 0, AIPG, DST, ECMP, QSIZE, CDEQ, CBLOCKED };

enum class metric_granularity_t { PACKET = 0, TIMESTEP };

struct metric_properties {
    bool non_negative;   // Used in normalization to distinguish zero and non-zero comparisons
    bool non_decreasing; // Used in normalization to extend time range to 0 or T
    bool aggregatable;
};

struct metric_val {
    bool valid = false;
    unsigned int value;
};

typedef std::pair<expr, expr> m_val_expr_t;

class Metric {
public:
    static const map<metric_t, metric_properties> properties;

    Metric(metric_t m, Queue* queue, unsigned int total_time, NetContext& net_ctx);

    m_val_expr_t val(unsigned int ind);
    virtual void
    eval(const IndexedExample* eg, unsigned int time, unsigned int qind, metric_val& res) = 0;

    virtual void populate_val_exprs(NetContext& net_ctx) = 0;

    cid_t get_id();
    metric_t get_type();

protected:
    cid_t id;
    metric_t m_type;

    Queue* queue;
    unsigned int total_time;
    std::vector<expr> value_;
    std::vector<expr> valid_;

    void init(NetContext& net_ctx);
};

std::ostream& operator<<(std::ostream& os, const metric_t& metric);

#endif /* metric_hpp */
