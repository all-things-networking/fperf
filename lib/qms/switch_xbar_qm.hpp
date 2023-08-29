//
//  switch_xbar_qm.hpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 12/13/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef switch_xbar_qm_hpp
#define switch_xbar_qm_hpp

#include "queuing_module.hpp"

class SwitchXBarQM : public QueuingModule {
public:
    SwitchXBarQM(cid_t id,
                unsigned int total_time,
                std::vector<unsigned int> voq_input_map,
                std::vector<unsigned int> voq_output_map,
                std::vector<QueueInfo> in_queue_info,
                std::vector<QueueInfo> out_queue_info,
                NetContext& net_ctx);
    
    virtual void add_constrs(NetContext& net_ctx,
                             std::map<std::string, expr>& constr_map);
    
    bool connected(unsigned int in_port, unsigned int out_port);    
    expr in_to_out(unsigned int in_port, unsigned int out_port, unsigned int t);
    expr out_from_in(unsigned int out_port, unsigned int in_port, unsigned int t);
    expr in_prio_head(unsigned int in_port, unsigned int out_port, unsigned int t);
    expr out_prio_head(unsigned int out_port, unsigned int in_port, unsigned int t);

protected:
    vector<unsigned int> voq_input_map;
    vector<unsigned int> voq_output_map;
    unsigned int port_cnt;
    
    vector<vector<expr>>* in_to_out_;
    vector<vector<expr>>* out_from_in_;
    
    vector<vector<expr>>* in_prio_head_;
    vector<vector<expr>>* out_prio_head_;
   
    map<unsigned int, vector<unsigned int>> input_voq_map;
    map<unsigned int, vector<unsigned int>> output_voq_map;
 
    void add_proc_vars(NetContext& net_ctx);
};

/* ********** LeafXBar ************ */

class LeafXBarQM: public SwitchXBarQM {
public:
    LeafXBarQM(cid_t id,
               unsigned int leaf_id,
               unsigned int servers_per_leaf,
               unsigned int spine_cnt,
               unsigned int total_time,
               std::vector<unsigned int> voq_input_map,
               std::vector<unsigned int> voq_output_map,
               std::vector<QueueInfo> in_queue_info,
               std::vector<QueueInfo> out_queue_info,
               NetContext& net_ctx);

    void add_constrs(NetContext& net_ctx,
                     std::map<std::string, expr>& constr_map);

private:
    unsigned int leaf_id;
    unsigned int servers_per_leaf;
    unsigned int spine_cnt;
};

class ReducedLeafXBarQM: public SwitchXBarQM {
public:
    ReducedLeafXBarQM(cid_t id,
                       unsigned int leaf_id,
                       unsigned int starting_qid,
                       unsigned int servers_per_leaf,
                       unsigned int spine_cnt,
                       unsigned int total_time,
                       std::vector<unsigned int> voq_input_map,
                       std::vector<unsigned int> voq_output_map,
                       std::vector<QueueInfo> in_queue_info,
                       std::vector<QueueInfo> out_queue_info,
                       NetContext& net_ctx);

    void add_constrs(NetContext& net_ctx,
                     std::map<std::string, expr>& constr_map);

private:
    unsigned int leaf_id;
    unsigned int starting_qid;
    unsigned int servers_per_leaf;
    unsigned int spine_cnt;
};



/* ********** SpineXBar ************ */
#endif /* switch_xbar_qm_hpp */
