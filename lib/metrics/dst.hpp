//
//  dst.hpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 06/06/23.
//  Copyright © 2023 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef dst_hpp
#define dst_hpp

#include "metric.hpp"
#include "queue.hpp"

class Dst : public Metric {
public:
    Dst(Queue* queue, unsigned int total_time, NetContext& net_ctx);

    void populate_val_exprs(NetContext& net_ctx);

    virtual void eval(Example* eg, unsigned int time, cid_t qind, metric_val& res);
};

#endif /* dst_hpp */
