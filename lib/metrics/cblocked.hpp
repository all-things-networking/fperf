//
//  cblocked.hpp
//  FPerf
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

    virtual void eval(Example* eg, unsigned int time, cid_t qind, metric_val& res);
};

#endif /* cblocked_hpp */
