//
//  metric.hpp
//  FPerf
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

enum class metric_t { CENQ = 0, AIPG, META1, META2, QSIZE, CDEQ, CBLOCKED };

enum class metric_granularity_t { PACKET = 0, TIMESTEP };

struct metric_properties {
    metric_granularity_t granularity;
    bool non_negative;
    bool non_decreasing;
    bool aggregatable;
};

struct metric_val {
    bool valid = false;
    unsigned int value;
};

class Metric {
public:
    static const map<metric_t, metric_properties> properties;

    Metric(metric_t m, Queue* queue, unsigned int total_time, NetContext& net_ctx);

    expr& val(unsigned int ind);
    virtual unsigned int eval(const IndexedExample* eg, unsigned int time, unsigned int qind) = 0;

    virtual void add_constrs(NetContext& net_ctx, std::map<std::string, expr>& constr_map) = 0;

    cid_t get_id();
    metric_t get_type();

protected:
    cid_t id;
    metric_t m_type;

    Queue* queue;
    unsigned int total_time;
    std::vector<expr> val_;

    void init(NetContext& net_ctx);

private:
    virtual void add_vars(NetContext& net_ctx) = 0;
};

std::ostream& operator<<(std::ostream& os, const metric_t& metric);

#endif /* metric_hpp */
