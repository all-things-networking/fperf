//
//  cost.hpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 2/28/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef cost_hpp
#define cost_hpp

#include <iostream>
#include <tuple>

typedef unsigned int wl_cost_t;
typedef std::pair<unsigned int, wl_cost_t> cost_t;

unsigned int uint_val(const wl_cost_t cost);
unsigned int uint_val(const cost_t cost);

int operator-(const cost_t cost1, const cost_t cost2);

bool operator<(const cost_t cost1, const cost_t cost2);
bool operator<=(const cost_t cost1, const cost_t cost2);
bool operator>(const cost_t cost1, const cost_t cost2);
bool operator>=(const cost_t cost1, const cost_t cost2);
bool operator==(const cost_t cost1, const cost_t cost2);
bool operator!=(const cost_t cost1, const cost_t cost2);

// std::ostream& operator<<(std::ostream& os, const cost_t cost);


#endif /* cost_hpp */
