//
//  2l_rr_qm.hpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 7/172l/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef buggy_2l_rr_qm_hpp
#define buggy_2l_rr_qm_hpp

#include "queuing_module.hpp"

class Buggy2LRRQM : public QueuingModule {
public:
    Buggy2LRRQM(cid_t id,
                unsigned int total_time,
                std::vector<QueueInfo> in_queue_info,
                QueueInfo out_queue_info,
                NetContext& net_ctx);
    
    void add_constrs(NetContext& net_ctx,
                     std::map<std::string, expr>& constr_map);
    
    Queue* new_fifo();
    Queue* old_fifo();
    expr inactive(unsigned int q, unsigned int t);
    
private:
    Queue* new_queues;
    Queue* old_queues;
    vector<expr>* inactive_;
    
    void add_proc_vars(NetContext& net_ctx);
};

#endif /* buggy_2l_rr_qm_hpp */
