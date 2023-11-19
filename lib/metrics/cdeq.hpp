//
//  cdeq.hpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 2/19/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef cdeq_hpp
#define cdeq_hpp

#include "metric.hpp"
#include "queue.hpp"

class CDeq : public Metric {
public:
    CDeq(Queue* queue, unsigned int total_time, NetContext& net_ctx);

    void populate_val_exprs(NetContext& net_ctx);

    void eval(const IndexedExample* eg, unsigned int time, unsigned int qind, metric_val& res);
};

#endif /* cdeq_hpp */
