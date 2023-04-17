//
//  priority_qm.hpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/12/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef priority_qm_hpp
#define priority_qm_hpp

#include "queuing_module.hpp"

class PriorityQM : public QueuingModule {
public:
    PriorityQM(cid_t id,
               unsigned int total_time,
               std::vector<QueueInfo> in_queue_info,
               QueueInfo out_queue_info,
               NetContext& net_ctx);
    
    void add_constrs(NetContext& net_ctx,
                     std::map<std::string, expr>& constr_map);
    
    
private:
    void add_proc_vars(NetContext& net_ctx);
};

#endif /* priority_qm_hpp */
