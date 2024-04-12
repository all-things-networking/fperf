//
//  queuing_module.hpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/9/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef queuing_module_hpp
#define queuing_module_hpp

#include <string>
#include <vector>

#include "net_context.hpp"
#include "queue.hpp"
#include "statement.hpp"

class QueuingModule {
public:
    QueuingModule(cid_t id,
                  unsigned int total_time,
                  vector<QueueInfo> in_queue_info,
                  vector<QueueInfo> out_queue_info,
                  NetContext& net_ctx);

    virtual void add_constrs(NetContext& net_ctx, map<string, expr>& constr_map) = 0;
    // TODO: change schedule method to abstract method after each qm has user interface
    virtual Statement* schedule(); // user-defined interface

    unsigned long in_queue_cnt();
    unsigned long out_queue_cnt();

    Queue* get_in_queue(unsigned int ind);
    Queue* get_out_queue(unsigned int ind);
    void set_out_queue(unsigned int ind, Queue* queue);
    QueueInfo get_out_queue_info(unsigned int ind);

    cid_t get_id();

protected:
    cid_t id;
    unsigned int total_time;
    vector<Queue*> in_queues;
    vector<Queue*> out_queues;
    vector<QueueInfo> out_queue_info;

    void init(NetContext& net_ctx);
    // iteratively add constraints for all queues at time global.curr_t
    // TODO: change t_generate_constrs method to abstract method after each qm has user interface
    virtual void t_generate_constrs(NetContext& net_ctx, map<string, expr>& constr_map, Global_Var& global);

private:
    virtual void add_proc_vars(NetContext& net_ctx) = 0;
};

#endif /* queuing_module_hpp */
