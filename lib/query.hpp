//
//  query.hpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/20/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef query_hpp
#define query_hpp

#include "metric.hpp"
#include "util.hpp"
#include "workload.hpp"

typedef vector<cid_t> qsum_t;
typedef pair<cid_t, cid_t> qdiff_t;
typedef variant<cid_t, qdiff_t, qsum_t> query_lhs_t;

enum class query_quant_t { FORALL = 0, EXISTS };

class Query {
public:
    Query();
    Query(query_quant_t quant,
          time_range_t time_range,
          query_lhs_t query_lhs,
          metric_t metric,
          Op op,
          unsigned int thresh);

    query_quant_t get_quant();
    time_range_t get_time_range();
    metric_t get_metric();
    query_lhs_t get_lhs();
    Op get_op();
    cid_t get_qid();
    unsigned int get_thresh();

private:
    query_quant_t quant;
    time_range_t time_range;
    metric_t metric;
    query_lhs_t lhs;
    Op op;
    unsigned int thresh;
};

#endif /* query_hpp */
