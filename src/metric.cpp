//
//  metric.cpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/17/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//


#include "metric.hpp"

#include <iostream>
#include <sstream>

const map<metric_t, metric_properties> Metric::properties = {{metric_t::CENQ, {true, true, true}},
                                                             {metric_t::AIPG, {true, false, false}},
                                                             {metric_t::DST, {true, false, false}},
                                                             {metric_t::ECMP, {true, false, false}},
                                                             {metric_t::QSIZE,
                                                              {true, false, false}},
                                                             {metric_t::CDEQ, {true, true, true}},
                                                             {metric_t::CBLOCKED,
                                                              {true, false, false}}};

Metric::Metric(metric_t m, Queue* queue, unsigned int total_time, NetContext& net_ctx):
m_type(m),
queue(queue),
total_time(total_time) {
    std::stringstream ss;
    ss << m;

    id = get_unique_id(queue->get_id(), ss.str());

    for (unsigned int t = 0; t < total_time; t++) {
        value_.push_back(net_ctx.bool_val(false));
        valid_.push_back(net_ctx.bool_val(false));
    }
}

m_val_expr_t Metric::val(unsigned int ind) {
    return m_val_expr_t(valid_[ind], value_[ind]);
}

void Metric::init(NetContext& net_ctx) {
    populate_val_exprs(net_ctx);
}

cid_t Metric::get_id() {
    return id;
}

metric_t Metric::get_type() {
    return m_type;
}

std::ostream& operator<<(std::ostream& os, const metric_t& metric) {
    switch (metric) {
        case (metric_t::CENQ): os << "cenq"; break;
        case (metric_t::QSIZE): os << "qsize"; break;
        case (metric_t::CDEQ): os << "cdeq"; break;
        case (metric_t::CBLOCKED): os << "cblocked"; break;
        case (metric_t::AIPG): os << "aipg"; break;
        case (metric_t::DST): os << "dst"; break;
        case (metric_t::ECMP): os << "ecmp"; break;
    }
    return os;
}
