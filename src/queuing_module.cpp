//
//  queuing_module.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/9/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "queuing_module.hpp"

QueuingModule::QueuingModule(cid_t id,
                               unsigned int total_time,
                               std::vector<QueueInfo> in_queue_info,
                               std::vector<QueueInfo> out_queue_info,
                               NetContext& net_ctx):
id(id),
total_time(total_time),
out_queue_info(out_queue_info)
{
    unsigned int qid = 0;
    // Instantiate input queues
    cid_t base_in_id = id;
    
    for (unsigned int i = 0; i < in_queue_info.size(); i++){
        QueueInfo qinfo = in_queue_info[i];
        Queue* q;
        switch (qinfo.type){
            case queue_t::LINK: {
                q = new Link(base_in_id, std::to_string(qid),
                             total_time, net_ctx);
                break;
            }
            case queue_t::IMM_QUEUE: {
                q = new ImmQueue(base_in_id, std::to_string(qid),
                                 qinfo.size,
                                 qinfo.max_enq,
                                 qinfo.max_deq,
                                 total_time, net_ctx);
                break;
            }
            default: {
                q = new Queue(base_in_id, std::to_string(qid),
                              qinfo.size,
                              qinfo.max_enq,
                              qinfo.max_deq,
                              total_time, net_ctx);
                break;
            }
        }
        in_queues.push_back(q);
        qid++;
    }
    
    // Placeholder pointers for output queues
    for (unsigned int i = 0; i < out_queue_info.size(); i++){
        Queue* q = nullptr;
        out_queues.push_back(q);
    }
}

void QueuingModule::init(NetContext& net_ctx){
    add_proc_vars(net_ctx);
}

unsigned long QueuingModule::in_queue_cnt(){
    return in_queues.size();
}

unsigned long QueuingModule::out_queue_cnt(){
    return out_queues.size();
}

Queue* QueuingModule::get_in_queue(unsigned int ind){
    return in_queues[ind];
}

Queue* QueuingModule::get_out_queue(unsigned int ind){
    return out_queues[ind];
}

void QueuingModule::set_out_queue(unsigned int ind, Queue* queue){
    out_queues[ind] = queue;
}

QueueInfo QueuingModule::get_out_queue_info(unsigned int ind){
    return out_queue_info[ind];
}

cid_t QueuingModule::get_id(){
    return id;
}

