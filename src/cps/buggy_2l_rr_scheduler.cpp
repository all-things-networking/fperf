//
//  buggy_2l_rr_scheduler.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 08/03/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "buggy_2l_rr_scheduler.hpp"
#include "buggy_2l_rr_qm.hpp"

#include <sstream>

Buggy2LRRScheduler::Buggy2LRRScheduler(unsigned int queue_cnt,
                                       unsigned int total_time):
ContentionPoint(total_time),
queue_cnt(queue_cnt)
{
    init();
}

void Buggy2LRRScheduler::add_nodes(){
    // add a 2l_rr qm
    QueueInfo info;
    info.size = MAX_QUEUE_SIZE;
    info.max_enq = MAX_ENQ;
    info.max_deq = 1;
    
    cid_t m_id = "2LRR";
    
    Buggy2LRRQM* rr_qm = new Buggy2LRRQM(m_id,
                                         total_time,
                                         vector<QueueInfo>(queue_cnt, info),
                                         info,
                                         net_ctx);
    
    nodes.push_back(m_id);
    id_to_qm[m_id] = rr_qm;
}

void Buggy2LRRScheduler::add_edges()
{}

void Buggy2LRRScheduler::add_metrics(){
    // CEnq
    for (unsigned int q = 0; q < in_queues.size(); q++){
        Queue* queue = in_queues[q];
        CEnq* ce = new CEnq(queue, total_time, net_ctx);
        cenq.push_back(ce);
        metrics[metric_t::CENQ][queue->get_id()] = ce;
        queue->add_metric(metric_t::CENQ, ce);
    }
    
    // CDeq
    for (unsigned int q = 0; q < in_queues.size(); q++){
        Queue* queue = in_queues[q];
        CDeq* cd = new CDeq(queue, total_time, net_ctx);
        cdeq.push_back(cd);
        metrics[metric_t::CDEQ][queue->get_id()] = cd;
        queue->add_metric(metric_t::CDEQ, cd);
    }
    
    // AIPG
    for (unsigned int q = 0; q < in_queues.size(); q++){
        Queue* queue = in_queues[q];
        AIPG* g = new AIPG(queue, total_time, net_ctx);
        aipg.push_back(g);
        metrics[metric_t::AIPG][queue->get_id()] = g;
        queue->add_metric(metric_t::AIPG, g);
    }
}

std::string Buggy2LRRScheduler::cp_model_str(model& m,
                                             NetContext& net_ctx,
                                             unsigned int t){
    stringstream ss;

    cid_t rr_id = nodes[0];
    Buggy2LRRQM* rr_qm = (Buggy2LRRQM*) id_to_qm[rr_id];

    Queue* new_queues = rr_qm->new_fifo();
    ss << new_queues->get_model_str(m, net_ctx, t) << std::endl;
    Queue* old_queues = rr_qm->old_fifo();
    ss << old_queues->get_model_str(m, net_ctx, t) << std::endl;
    for (unsigned int q = 0; q < in_queues.size(); q++){
        ss << (m.eval(rr_qm->inactive(q, t)).is_true() ? 1 : 0);
    }
    ss << std::endl;
    
    ss << "qid: cdeq" << std::endl;
    for (unsigned int q = 0; q < in_queues.size(); q++){
        Queue* queue = in_queues[q];
        ss << queue->get_id() << ": " << m.eval(cdeq[q]->val(t)).get_numeral_int() << std::endl;
    }
    return ss.str();
}
