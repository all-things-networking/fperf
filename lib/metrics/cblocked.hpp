//
//  cblocked.hpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/17/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef cblocked_hpp
#define cblocked_hpp

#include "metric.hpp"
#include "queue.hpp"

class CBlocked : public Metric {
public:
    CBlocked(Queue* queue, unsigned int total_time, NetContext& net_ctx);

    void add_constrs(NetContext& net_ctx, std::map<std::string, expr>& constr_map);

    unsigned int eval(const IndexedExample* eg, unsigned int time, unsigned int qind);

private:
    void add_vars(NetContext& net_ctx);
};

#endif /* cblocked_hpp */
