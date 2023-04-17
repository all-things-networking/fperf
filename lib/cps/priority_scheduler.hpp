//
//  priority_scheduler.hpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/16/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef priority_scheduler_hpp
#define priority_scheduler_hpp

#include "contention_point.hpp"
#include "cblocked.hpp"
#include "cenq.hpp"
#include "aipg.hpp"

class PrioScheduler: public ContentionPoint{
public:
    PrioScheduler(unsigned int prio_levels,
                  unsigned int total_time);
    
private:
    unsigned int prio_levels;
    std::vector<CBlocked*> cblocked;
    std::vector<CEnq*> cenq;
    std::vector<AIPG*> aipg;
    
    void add_nodes();
    void add_edges();
    void add_metrics();
    
    std::string cp_model_str(model&m, NetContext& net_ctx, unsigned int t);
};

#endif /* priority_scheduler_hpp */
