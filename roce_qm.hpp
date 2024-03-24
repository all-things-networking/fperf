//
//  priority_qm.hpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/12/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef roce_qm_hpp
#define roce_qm_hpp

#include "queuing_module.hpp"

class RoCEQM : public QueuingModule {
public:
    RoCEQM(cid_t id,
        cid_t sid,
        unsigned int total_time,
        unsigned int buffer_size,
        unsigned int threshold,
        unsigned int prio_voq_start,
        unsigned int return_to_sender,
        std::vector<QueueInfo> in_queue_info,
        std::vector<QueueInfo> out_queue_info,
        std::map<unsigned int, unsigned int> control_flow,
        NetContext& net_ctx);

    void add_constrs(NetContext& net_ctx,
        std::map<std::string, expr>& constr_map);

protected:
    cid_t sid;
    // map flow ID to which output queue it should go to
    std::map<unsigned int, unsigned int> control_flow;
    unsigned int buffer_size;
    unsigned int threshold;
    unsigned int prio_voq_start;
    unsigned int port_cnt;
    unsigned int return_to_sender;
    expr_vector pause_state;
    expr_vector sent_pause;
    vector<expr>* voqs_full;


private:

    void add_proc_vars(NetContext& net_ctx);
};

#endif /* RoCE_qm_hpp */
