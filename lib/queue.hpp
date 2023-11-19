//
//  Queue.hpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/5/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef Queue_hpp
#define Queue_hpp

#include <map>
#include <string>
#include <vector>

#include "metric.hpp"
#include "net_context.hpp"
#include "util.hpp"

class Metric;
enum class metric_t;

enum class queue_t { QUEUE = 0, IMM_QUEUE, LINK };

struct QueueInfo {
    unsigned int size;
    unsigned int max_enq;
    unsigned int max_deq;
    unsigned int meta;
    queue_t type;
};

/* *********************** Queue ********************** */

class Queue {
public:
    Queue(cid_t module_id,
          cid_t queue_id,
          unsigned int size,
          unsigned int max_enq,
          unsigned int max_deq,
          unsigned int total_time,
          NetContext& net_ctx);

    unsigned int size();
    unsigned int max_enq();
    unsigned int max_deq();

    vector<expr>& operator[](int ind);
    vector<expr>& elem(int ind);

    vector<expr>& enqs(unsigned int ind);
    expr& enq_cnt(unsigned int t);
    expr& deq_cnt(unsigned int t);

    void add_metric(metric_t metric_type, Metric* m);
    Metric* get_metric(metric_t metric_type);

    cid_t get_id() const;

    virtual void add_constrs(NetContext& net_ctx, map<string, expr>& constr_map);

    string get_model_str(model& m, NetContext& net_ctx, unsigned int t);

    friend ostream& operator<<(ostream& os, const Queue& q);

protected:
    cid_t id;
    unsigned int size_;
    unsigned int max_enq_;
    unsigned int max_deq_;

    unsigned int total_time;

    vector<expr>* elems_;
    vector<expr>* enqs_;
    vector<expr> enq_cnt_;
    vector<expr> deq_cnt_;

    vector<expr>* tmp_val;
    vector<expr>* enq_ind;
    vector<expr> tail;
    vector<expr> tmp_tail;

    map<metric_t, Metric*> metrics;

    void add_vars(NetContext& net_ctx);

    void sliding_window_vars(NetContext& net_ctx);
    virtual void sliding_window_constrs(NetContext& net_ctx, map<string, expr>& constr_map);
};

/* *********************** ImmQueue ********************** */
/* Enqueued packets appear in the queue in the same time step */

class ImmQueue : public Queue {
public:
    ImmQueue(cid_t module_id,
             cid_t queue_id,
             unsigned int size,
             unsigned int max_enq,
             unsigned int max_deq,
             unsigned int total_time,
             NetContext& net_ctx);

    friend ostream& operator<<(ostream& os, const ImmQueue& q);

protected:
    void sliding_window_constrs(NetContext& net_ctx, map<string, expr>& constr_map);
};

/* *********************** Link ********************** */
/* Packets that come in at time t, appear on the other side at time t + 1 */

class Link : public Queue {
public:
    Link(cid_t module_id, cid_t queue_id, unsigned int total_time, NetContext& net_ctx);

    void add_constrs(NetContext& net_ctx, map<string, expr>& constr_map);


    friend ostream& operator<<(ostream& os, const Link& q);
};
#endif /* Queue_hpp */
