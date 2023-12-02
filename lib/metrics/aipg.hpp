//
//  aipg.hpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 8/5/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef aipg_hpp
#define aipg_hpp

#include "metric.hpp"
#include "queue.hpp"

class AIPG : public Metric {
public:
    AIPG(Queue* queue, unsigned int total_time, NetContext& net_ctx);

    void populate_val_exprs(NetContext& net_ctx);

    void eval(const IndexedExample* eg, unsigned int time, unsigned int qind, metric_val& res);
};


#endif /* aipg_hpp */
