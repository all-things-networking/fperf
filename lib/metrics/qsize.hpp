//
//  qsize.hpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 2/19/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef qsize_hpp
#define qsize_hpp

#include "metric.hpp"
#include "queue.hpp"

class QSize : public Metric {
public:
    QSize(Queue* queue, unsigned int total_time, NetContext& net_ctx);

    void populate_val_exprs(NetContext& net_ctx);

    virtual void eval(Example* eg, unsigned int time, cid_t qind, metric_val& res);
};

#endif /* qsize_hpp */
