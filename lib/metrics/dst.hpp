//
//  dst.hpp
//  AutoPerf
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
    Dst(Queue* queue,
        unsigned int total_time,
        NetContext& net_ctx);
    
    void populate_val_exprs(NetContext& net_ctx);

    unsigned int eval(const IndexedExample* eg,
                      unsigned int time,
                      unsigned int qind);    
};

#endif /* dst_hpp */
