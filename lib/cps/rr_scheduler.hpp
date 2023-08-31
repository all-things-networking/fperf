//
//  rr_scheduler.hpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 2/18/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef rr_scheduler_hpp
#define rr_scheduler_hpp

#include "contention_point.hpp"
#include "cenq.hpp"
#include "cdeq.hpp"
#include "qsize.hpp"
#include "aipg.hpp"

class RRScheduler: public ContentionPoint{
public:
    RRScheduler(unsigned int queue_cnt,
                unsigned int total_time);
    
private:
    unsigned int queue_cnt;
    std::vector<CEnq*> cenq;
    std::vector<CDeq*> cdeq;
    std::vector<AIPG*> aipg;
    
    void add_nodes();
    void add_edges();
    void add_metrics();
    
    std::string cp_model_str(model&m, NetContext& net_ctx, unsigned int t);
};

#endif /* rr_scheduler_hpp */
