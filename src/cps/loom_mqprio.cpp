//
//  loom_mqprio.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 5/3/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "loom_mqprio.hpp"
#include "loom_demux_qm.hpp"
#include "loom_flow_enq_qm.hpp"
#include "loom_nic_enq_qm.hpp"
#include "priority_qm.hpp"
#include "rr_qm.hpp"

#include <sstream>

LoomMQPrio::LoomMQPrio(unsigned int nic_tx_queue_cnt,
                       unsigned int per_core_flow_cnt,
                       unsigned int total_time):
ContentionPoint(total_time),
nic_tx_queue_cnt(nic_tx_queue_cnt),
per_core_flow_cnt(per_core_flow_cnt) {
    init();
}

void LoomMQPrio::add_nodes() {
    QueueInfo info;
    info.size = MAX_QUEUE_SIZE;
    info.max_enq = MAX_ENQ;
    info.max_deq = info.size;
    info.type = queue_t::IMM_QUEUE;

    for (unsigned int i = 0; i < nic_tx_queue_cnt; i++) {

        // Create flow_enq_qm
        cid_t m_id = "FlowEnq" + to_string(i);
        LoomFlowEnqQM* flow_enq_qm = new LoomFlowEnqQM(
            m_id, total_time, vector<QueueInfo>(per_core_flow_cnt, info), info, info, net_ctx);

        nodes.push_back(m_id);
        id_to_qm[m_id] = flow_enq_qm;

        // Create the corresponding rr scheduler
        m_id = "RR" + to_string(i);
        RRQM* rr_qm = new RRQM(
            m_id, total_time, vector<QueueInfo>(tenant_cnt, info), info, net_ctx);

        nodes.push_back(m_id);
        id_to_qm[m_id] = rr_qm;
    }

    cid_t m_id = "NICEnq";
    LoomNICEnqQM* nic_enq_qm = new LoomNICEnqQM(
        m_id, total_time, vector<QueueInfo>(nic_tx_queue_cnt, info), info, info, net_ctx);
    nodes.push_back(m_id);
    id_to_qm[m_id] = nic_enq_qm;

    m_id = "PRIO";
    PriorityQM* prio_qm = new PriorityQM(
        m_id, total_time, vector<QueueInfo>(2, info), info, net_ctx);
    nodes.push_back(m_id);
    id_to_qm[m_id] = prio_qm;

    m_id = "EndDemux";
    LoomDemuxQM* demux_qm = new LoomDemuxQM(m_id, total_time, info, info, info, net_ctx);
    nodes.push_back(m_id);
    id_to_qm[m_id] = demux_qm;
}

void LoomMQPrio::add_edges() {
    for (unsigned int i = 0; i < nic_tx_queue_cnt; i++) {
        cid_t flow_enq_id = "FlowEnq" + to_string(i);
        cid_t rr_id = "RR" + to_string(i);

        // modules edges
        vector<cid_t> edges;
        edges.push_back(rr_id);
        module_edges[flow_enq_id] = edges;

        // queue edges
        vector<qpair> q_edges;
        for (unsigned int j = 0; j < tenant_cnt; j++) {
            q_edges.push_back(qpair(j, j));
        }
        queue_edges[cid_pair(flow_enq_id, rr_id)] = q_edges;
    }

    cid_t nic_enq_id = "NICEnq";
    vector<cid_t> rr_nic_edges;
    rr_nic_edges.push_back(nic_enq_id);

    for (unsigned int i = 0; i < nic_tx_queue_cnt; i++) {
        cid_t rr_id = "RR" + to_string(i);

        // modules edges
        module_edges[rr_id] = rr_nic_edges;

        // queue edges
        vector<qpair> q_edges;
        q_edges.push_back(qpair(0, i));
        queue_edges[cid_pair(rr_id, nic_enq_id)] = q_edges;
    }

    cid_t prio_id = "PRIO";

    // module edges
    vector<cid_t> nic_prio_edges;
    nic_prio_edges.push_back(prio_id);
    module_edges[nic_enq_id] = nic_prio_edges;

    // queue edges
    vector<qpair> nic_prio_qedges;
    nic_prio_qedges.push_back(qpair(0, 0));
    nic_prio_qedges.push_back(qpair(1, 1));
    queue_edges[cid_pair(nic_enq_id, prio_id)] = nic_prio_qedges;

    cid_t demux_id = "EndDemux";

    // module edges
    vector<cid_t> prio_demux_edges;
    prio_demux_edges.push_back(demux_id);
    module_edges[prio_id] = prio_demux_edges;

    // queue edges
    vector<qpair> prio_demux_qedges;
    prio_demux_qedges.push_back(qpair(0, 0));
    queue_edges[cid_pair(prio_id, demux_id)] = prio_demux_qedges;
}

void LoomMQPrio::add_metrics() {
    // CEnq
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        Queue* queue = in_queues[q];
        CEnq* ce = new CEnq(queue, total_time, net_ctx);
        cenq.push_back(ce);
        metrics[metric_t::CENQ][queue->get_id()] = ce;
        queue->add_metric(metric_t::CENQ, ce);
    }

    for (unsigned int q = 0; q < out_queues.size(); q++) {
        Queue* queue = out_queues[q];
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

std::string LoomMQPrio::cp_model_str(model& m, NetContext& net_ctx, unsigned int t) {
    // TODO: implement
    (void) net_ctx;

    stringstream ss;
    for (unsigned int q = 0; q < out_queues.size(); q++) {
        Queue* queue = out_queues[q];
        ss << queue->get_id() << ": "
           << m.eval(queue->get_metric(metric_t::CENQ)->val(t)).get_numeral_int() << std::endl;
    }
    return ss.str();
}
