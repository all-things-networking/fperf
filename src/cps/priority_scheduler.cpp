//
//  priority_scheduler.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/16/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "priority_scheduler.hpp"
#include "priority_qm.hpp"

#include <sstream>

PrioScheduler::PrioScheduler(unsigned int prio_levels, unsigned int total_time):
ContentionPoint(total_time),
prio_levels(prio_levels) {
    init();
}

void PrioScheduler::add_nodes() {
    // add a priority qm
    QueueInfo info;
    info.size = 10;
    info.max_enq = 4;
    info.max_deq = 1;
    info.type = queue_t::QUEUE;

    cid_t m_id = "Prio";

    PriorityQM* prio_qm = new PriorityQM(
        m_id, total_time, vector<QueueInfo>(prio_levels, info), info, net_ctx);

    nodes.push_back(m_id);

    id_to_qm[m_id] = prio_qm;
}

void PrioScheduler::add_edges() {
}

void PrioScheduler::add_metrics() {

    // CBlocked
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        Queue* queue = in_queues[q];
        CBlocked* cb = new CBlocked(queue, total_time, net_ctx);
        cblocked.push_back(cb);
        metrics[metric_t::CBLOCKED][queue->get_id()] = cb;
        queue->add_metric(metric_t::CBLOCKED, cb);
    }


    // CEnq
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        Queue* queue = in_queues[q];
        CEnq* ce = new CEnq(queue, total_time, net_ctx);
        cenq.push_back(ce);
        metrics[metric_t::CENQ][queue->get_id()] = ce;
        queue->add_metric(metric_t::CENQ, ce);
    }


    // AIPG
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        Queue* queue = in_queues[q];
        AIPG* g = new AIPG(queue, total_time, net_ctx);
        aipg.push_back(g);
        metrics[metric_t::AIPG][queue->get_id()] = g;
        queue->add_metric(metric_t::AIPG, g);
    }
}

std::string PrioScheduler::cp_model_str(model& m, NetContext& net_ctx, unsigned int t) {
    (void) net_ctx;
    stringstream ss;
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        Queue* queue = in_queues[q];
        ss << queue->get_id() << ": " << m.eval(cblocked[q]->val(t)).get_numeral_int() << ", "
           << m.eval(cenq[q]->val(t)).get_numeral_int() << std::endl;
    }
    return ss.str();
}
