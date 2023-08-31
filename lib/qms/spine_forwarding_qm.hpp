//
//  spine_forwarding_qm.hpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 12/23/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef spine_forwarding_qm_hpp
#define spine_forwarding_qm_hpp

#include "queuing_module.hpp"
#include <map>

class SpineForwardingQM : public QueuingModule {
public:
    SpineForwardingQM(cid_t id,
                      unsigned int total_time,
                      unsigned int spine_id,
                      unsigned int leaf_cnt,
                      unsigned int servers_per_leaf,
                      map<unsigned int, unsigned int> output_voq_map,
                      QueueInfo in_queue_info,
                      std::vector<QueueInfo> out_queue_info,
                      NetContext& net_ctx);
    
    void add_constrs(NetContext& net_ctx,
                     std::map<std::string, expr>& constr_map);


private:
    unsigned int spine_id;
    unsigned int leaf_cnt;
    unsigned int servers_per_leaf; 
    map<unsigned int, unsigned int> output_voq_map;
    
    void add_proc_vars(NetContext& net_ctx);
};

#endif /* spine_forwarding_qm_hpp */
