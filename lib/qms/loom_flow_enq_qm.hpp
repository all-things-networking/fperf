//
//  loom_flow_enq_qm.hpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 5/3/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef loom_flow_enq_qm_hpp
#define loom_flow_enq_qm_hpp

#include "queuing_module.hpp"

class LoomFlowEnqQM : public QueuingModule {
public:
    LoomFlowEnqQM(cid_t id,
                  unsigned int total_time,
                  std::vector<QueueInfo> in_queue_info,
                  QueueInfo out_queue_info1,
                  QueueInfo out_queue_info2,
                  NetContext& net_ctx);

    void add_constrs(NetContext& net_ctx, std::map<std::string, expr>& constr_map);


private:
    void add_proc_vars(NetContext& net_ctx);
    void constrs_if_not_taken(NetContext& net_ctx, std::map<std::string, expr>& constr_map);
};


#endif /* loom_flow_enq_qm_hpp */
