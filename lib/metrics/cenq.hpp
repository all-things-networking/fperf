//
//  cenq.hpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 2/11/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef cenq_hpp
#define cenq_hpp

#include "metric.hpp"
#include "queue.hpp"

class CEnq : public Metric {
public:
    CEnq(Queue* queue, unsigned int total_time, NetContext& net_ctx);

    void populate_val_exprs(NetContext& net_ctx);

    void eval(Example* eg, unsigned int time, cid_t qind, metric_val& res);
};

#endif /* cenq_hpp */
