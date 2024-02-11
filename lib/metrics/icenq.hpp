//
//  cenq.hpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 2/11/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//



#ifndef icenq_hpp
#define icenq_hpp

#include "metric.hpp"
#include "queue.hpp"

class ICEnq : public Metric  {
private:
    int id;
public:
    ICEnq(Queue* queue, int id, unsigned int total_time, NetContext& net_ctx);

    void populate_val_exprs(NetContext& net_ctx);

    void eval(Example* eg, unsigned int time, cid_t qind, metric_val& res);
};

#endif