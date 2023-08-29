//
//  rr_qm.hpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 2/17/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef rr_qm_hpp
#define rr_qm_hpp

#include "queuing_module.hpp"

class RRQM : public QueuingModule {
public:
    RRQM(cid_t id,
         unsigned int total_time,
         std::vector<QueueInfo> in_queue_info,
         QueueInfo out_queue_info,
         NetContext& net_ctx);
    
    void add_constrs(NetContext& net_ctx,
                     std::map<std::string, expr>& constr_map);
    
    expr last_served_queue(unsigned int q, unsigned int t);
    
private:
    vector<expr>* last_served_queue_;
    void add_proc_vars(NetContext& net_ctx);
};

#endif /* rr_qm_hpp */
