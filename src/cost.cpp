//
//  cost.cpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 2/28/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "cost.hpp"
#include "params.hpp"

#include <vector>

std::vector<unsigned int> wl_cost_weights = {SPEC_CNT_WEIGHT,
                                             NUM_QUEUES_WITH_SPECS_WEIGHT,
                                             MAX_SPEC_CNT_PER_QUEUE_WEIGHT,
                                             AVG_SPEC_CNT_OVER_QUEUES,
                                             AVG_TIME_RANGE_OVER_SPECS_WEIGHT};

unsigned int uint_val(const wl_cost_t cost) {
    return cost;
}

unsigned int uint_val(const cost_t cost) {
    return EXAMPLE_WEIGHT_IN_COST * cost.first + uint_val(cost.second);
}

int operator-(const cost_t cost1, const cost_t cost2) {
    return ((int) uint_val(cost1)) - uint_val(cost2);
}

bool operator<(const cost_t cost1, const cost_t cost2) {
    return uint_val(cost1) < uint_val(cost2);
}

bool operator<=(const cost_t cost1, const cost_t cost2) {
    return uint_val(cost1) <= uint_val(cost2);
}

bool operator>(const cost_t cost1, const cost_t cost2) {
    return uint_val(cost1) > uint_val(cost2);
}

bool operator>=(const cost_t cost1, const cost_t cost2) {
    return uint_val(cost1) >= uint_val(cost2);
}

bool operator==(const cost_t cost1, const cost_t cost2) {
    return uint_val(cost1) == uint_val(cost2);
}

bool operator!=(const cost_t cost1, const cost_t cost2) {
    return uint_val(cost1) != uint_val(cost2);
}


// std::ostream& operator<<(std::ostream& os, const cost_t cost)
//{
//     os << "[";
//     os << cost.first << ", ";
//     os << cost.second << "]";
//     return os;
// }
