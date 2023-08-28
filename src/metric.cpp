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

const map<metric_t, metric_properties> Metric::properties = {
    {metric_t::CENQ, {metric_granularity_t::TIMESTEP, true, true, true}},
    {metric_t::AIPG, {metric_granularity_t::TIMESTEP, true, false, false}},
    {metric_t::META1, {metric_granularity_t::PACKET, true, false, false}},
    {metric_t::META2, {metric_granularity_t::PACKET, true, false, false}},
    {metric_t::QSIZE, {metric_granularity_t::TIMESTEP, true, false, false}},
    {metric_t::CDEQ, {metric_granularity_t::TIMESTEP, true, true, true}},
    {metric_t::CBLOCKED, {metric_granularity_t::TIMESTEP, true, false, false}}};

Metric::Metric(metric_t m, Queue* queue, unsigned int total_time, NetContext& net_ctx):
m_type(m),
queue(queue),
total_time(total_time) {
    std::stringstream ss;
    ss << m;

    id = get_unique_id(queue->get_id(), ss.str());

    for (unsigned int t = 0; t < total_time; t++) {
        char vname[100];
        std::sprintf(vname, "%s[%d]", id.c_str(), t);
        val_.push_back(net_ctx.int_const(vname));
    }
}

expr& Metric::val(unsigned int ind) {
    return val_[ind];
}

void Metric::init(NetContext& net_ctx) {
    add_vars(net_ctx);
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
    case (metric_t::META1): os << "meta1"; break;
    case (metric_t::META2): os << "meta2"; break;
    }
    return os;
}
