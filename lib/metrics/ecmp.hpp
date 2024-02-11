//
//  ecmp.hpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 06/06/23.
//  Copyright © 2023 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef ecmp_hpp
#define ecmp_hpp

#include "metric.hpp"
#include "queue.hpp"

class Ecmp : public Metric {
public:
    Ecmp(Queue* queue, unsigned int total_time, NetContext& net_ctx);

    void populate_val_exprs(NetContext& net_ctx);

    virtual void eval(Example* eg, unsigned int time, cid_t qind, metric_val& res);
};

#endif /* ecmp_hpp */
