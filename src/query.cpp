//
//  query.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/20/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "query.hpp"

Query::Query() {
}

Query::Query(query_quant_t quant,
             time_range_t time_range,
             query_lhs_t query_lhs,
             metric_t metric,
             op_t op,
             unsigned int thresh):
quant(quant),
time_range(time_range),
metric(metric),
lhs(query_lhs),
op(op),
thresh(thresh) {
}

query_quant_t Query::get_quant() {
    return quant;
}

time_range_t Query::get_time_range() {
    return time_range;
}

metric_t Query::get_metric() {
    return metric;
}

query_lhs_t Query::get_lhs() {
    return lhs;
}

op_t Query::get_op() {
    return op;
}

unsigned int Query::get_thresh() {
    return thresh;
}

cid_t Query::get_qid() {
    if (holds_alternative<cid_t>(lhs)) {
        return get<cid_t>(lhs);
    } else if (holds_alternative<qdiff_t>(lhs)) {
        return get<qdiff_t>(lhs).first;
    } else if (holds_alternative<qsum_t>(lhs)) {
        return get<qsum_t>(lhs)[0];
    }

    cout << "Query::get_qid: Invalid query" << endl;
    return "";
}
