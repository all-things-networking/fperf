//
//  leaf_forwarding_qm.hpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 12/23/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef leaf_forwarding_qm_hpp
#define leaf_forwarding_qm_hpp

#include "queuing_module.hpp"
#include <map>

class LeafForwardingQM : public QueuingModule {
public:
    LeafForwardingQM(cid_t id,
                     unsigned int total_time,
                     unsigned int leaf_id,
                     unsigned int fw_id,
                     unsigned int servers_per_leaf,
                     unsigned int server_cnt,
                     unsigned int spine_cnt,
                     map<unsigned int, unsigned int> output_voq_map,
                     QueueInfo in_queue_info,
                     std::vector<QueueInfo> out_queue_info,
                     NetContext& net_ctx);

    void add_constrs(NetContext& net_ctx, std::map<std::string, expr>& constr_map);


private:
    unsigned int leaf_id;
    unsigned int fw_id;
    unsigned int servers_per_leaf;
    unsigned int server_cnt;
    unsigned int spine_cnt;
    map<unsigned int, unsigned int> output_voq_map;

    void add_proc_vars(NetContext& net_ctx);
};

#endif /* leaf_forwarding_qm_hpp */
