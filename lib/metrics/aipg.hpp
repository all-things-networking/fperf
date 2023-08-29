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
    AIPG(Queue* queue,
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


#endif /* aipg_hpp */
