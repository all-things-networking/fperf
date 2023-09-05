//
//  loom_demux_qm.hpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 5/4/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef loom_demux_qm_hpp
#define loom_demux_qm_hpp

#include "queuing_module.hpp"

class LoomDemuxQM : public QueuingModule {
public:
    LoomDemuxQM(cid_t id,
                unsigned int total_time,
                QueueInfo in_queue_info,
                QueueInfo out_queue_info1,
                QueueInfo out_queue_info2,
                NetContext& net_ctx);
    
    void add_constrs(NetContext& net_ctx,
                     std::map<std::string, expr>& constr_map);
    
    
private:
    void add_proc_vars(NetContext& net_ctx);
};

#endif /* loom_demux_qm_hpp */
