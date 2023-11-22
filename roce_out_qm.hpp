//
//  priority_qm.hpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/12/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef roce_out_qm_hpp
#define roce_out_qm_hpp

#include "queuing_module.hpp"

class RoCEQM_OUT : public QueuingModule {
public:
    RoCEQM_OUT(cid_t id,
        unsigned int total_time, 
        std::vector<unsigned int> voq_input_map,
        std::vector<unsigned int> voq_output_map,
        std::vector<QueueInfo> in_queue_info,
        std::vector<QueueInfo> out_queue_info,
        NetContext& net_ctx);

    void add_constrs(NetContext& net_ctx,
        std::map<std::string, expr>& constr_map);

protected:
    unsigned int port_cnt;
    vector<unsigned int> voq_input_map;
    vector<unsigned int> voq_output_map;

    vector<vector<expr>>* in_to_out_;
    vector<vector<expr>>* out_from_in_;

    map<unsigned int, vector<unsigned int>> input_voq_map;
    map<unsigned int, vector<unsigned int>> output_voq_map;

private:
    void add_proc_vars(NetContext& net_ctx);
};

#endif /* RoCE_qm_hpp */
