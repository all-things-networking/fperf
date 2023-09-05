//
//  loom_mqprio.hpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 5/3/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef loom_mqprio_hpp
#define loom_mqprio_hpp

#include "contention_point.hpp"
#include "cenq.hpp"
#include "aipg.hpp"

class LoomMQPrio: public ContentionPoint{
public:
    LoomMQPrio(unsigned int nic_tx_queue_cnt,
               unsigned int per_core_flow_cnt,
               unsigned int total_time);
    
private:
    unsigned int nic_tx_queue_cnt;
    unsigned int per_core_flow_cnt;
    unsigned int tenant_cnt = 2;
    
    std::vector<CEnq*> cenq;
    std::vector<AIPG*> aipg;
    
    void add_nodes();
    void add_edges();
    void add_metrics();
    
    std::string cp_model_str(model&m, NetContext& net_ctx, unsigned int t);
};


#endif /* loom_mqprio_hpp */
