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

    void add_constrs(NetContext& net_ctx, std::map<std::string, expr>& constr_map);

    unsigned int eval(const IndexedExample* eg, unsigned int time, unsigned int qind);

private:
    void add_vars(NetContext& net_ctx);
};

#endif /* cenq_hpp */
