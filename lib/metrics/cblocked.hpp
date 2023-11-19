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

    void populate_val_exprs(NetContext& net_ctx);

    void eval(const IndexedExample* eg, unsigned int time, unsigned int qind, metric_val& res);
};

#endif /* cblocked_hpp */
