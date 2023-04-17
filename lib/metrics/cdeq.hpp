//
//  cdeq.hpp
//  AutoPerf
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
    CDeq(Queue* queue,
         unsigned int total_time,
         NetContext& net_ctx);
    
    void add_constrs(NetContext& net_ctx,
                     std::map<std::string, expr>& constr_map);

    unsigned int eval(const IndexedExample* eg,
                      unsigned int time,
                      unsigned int qind);    
   
private:
    void add_vars(NetContext& net_ctx);
};

#endif /* cdeq_hpp */
