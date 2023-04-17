//
//  qsize.hpp
//  AutoPerf
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
    QSize(Queue* queue,
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

#endif /* qsize_hpp */
