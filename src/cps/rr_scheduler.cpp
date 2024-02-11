//
//  rr_scheduler.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 2/18/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "rr_scheduler.hpp"
#include "rr_qm.hpp"
#include "icenq.hpp"

#include <sstream>

RRScheduler::RRScheduler(unsigned int queue_cnt, unsigned int total_time):
ContentionPoint(total_time),
queue_cnt(queue_cnt) {
    init();
}

void RRScheduler::add_nodes() {
    // add a rr qm
    QueueInfo info;
    info.size = MAX_QUEUE_SIZE;
    info.max_enq = MAX_ENQ;
    info.max_deq = 1;

    cid_t m_id = "RR";

    RRQM* rr_qm = new RRQM(m_id, total_time, vector<QueueInfo>(queue_cnt, info), info, net_ctx);

    nodes.push_back(m_id);
    id_to_qm[m_id] = rr_qm;
}

void RRScheduler::add_edges() {
}

void RRScheduler::add_metrics() {
    // CEnq
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        Queue* queue = in_queues[q];
        CEnq* ce = new CEnq(queue, total_time, net_ctx);
        cenq.push_back(ce);
        metrics[metric_t::CENQ][queue->get_id()] = ce;
        queue->add_metric(metric_t::CENQ, ce);
    }


    // ICEnq1
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        Queue* queue = in_queues[q];
        ICEnq* ce1 = new ICEnq(queue, 1, total_time, net_ctx);
        icenq1.push_back(ce1);
        metrics[metric_t::ICENQ1][queue->get_id()] = ce1;
        queue->add_metric(metric_t::ICENQ1, ce1);
    }

    // ICEnq2
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        Queue* queue = in_queues[q];
        ICEnq* ce1 = new ICEnq(queue, 2, total_time, net_ctx);
        icenq1.push_back(ce1);
        metrics[metric_t::ICENQ2][queue->get_id()] = ce1;
        queue->add_metric(metric_t::ICENQ2, ce1);
    }


    // CDeq
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        Queue* queue = in_queues[q];
        CDeq* cd = new CDeq(queue, total_time, net_ctx);
        cdeq.push_back(cd);
        metrics[metric_t::CDEQ][queue->get_id()] = cd;
        queue->add_metric(metric_t::CDEQ, cd);
    }

    /*
    // QSize
    for (unsigned int q = 0; q < in_queues.size(); q++){
        Queue* queue = in_queues[q];
        QSize* qs = new QSize(queue, total_time, net_ctx);
        qsize.push_back(qs);
        metrics[metric_t::QSIZE][queue->get_id()] = qs;
        queue->add_metric(metric_t::QSIZE, qs);
    }
    */

    // AIPG
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        Queue* queue = in_queues[q];
        AIPG* g = new AIPG(queue, total_time, net_ctx);
        aipg.push_back(g);
        metrics[metric_t::AIPG][queue->get_id()] = g;
        queue->add_metric(metric_t::AIPG, g);
    }
}

string RRScheduler::cp_model_str(model& m, NetContext& net_ctx, unsigned int t) {
    (void) net_ctx;

    stringstream ss;
    cid_t rr_id = nodes[0];
    RRQM* rr_qm = (RRQM*) id_to_qm[rr_id];

    int last_served_queue = -1;
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        if (m.eval(rr_qm->last_served_queue(q, t)).is_true()) {
            last_served_queue = q;
            break;
        }
    }
    ss << "last serviced queue: " << last_served_queue << endl;
    ss << "qid: cenq, cdeq, deq_cnt" << endl;
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        Queue* queue = in_queues[q];
        ss << queue->get_id() << ": " << m.eval(cenq[q]->val(t).second).get_numeral_int() << ", "
           << m.eval(cdeq[q]->val(t).second).get_numeral_int() << ", " <<
            // m.eval(qsize[q]->val(t)).get_numeral_int() << ", " <<
            m.eval(queue->deq_cnt(t)).get_numeral_int() << endl;
    }
    return ss.str();
}
