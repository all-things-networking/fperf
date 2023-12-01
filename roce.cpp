//
//  rr_scheduler.cpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 2/18/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "roce.hpp"
#include "roce_qm.hpp"
#include "roce_switch_xbar_qm.hpp"

#include <sstream>

RoceScheduler::RoceScheduler(unsigned int total_time) :
    ContentionPoint(total_time)
{
    // for S1 (top left)
    control_flows.push_back({ {1, 0}, {2, 0}, {3, 2} });
    // for S2 (top right)
    control_flows.push_back({ {1, 1}, {2, 2}, {3, 1} });
    // for S3 (bot right)
    control_flows.push_back({ {1, 0}, {2, 0}, {3, 2} });
    // for S4 (bot left)
    control_flows.push_back({ {1, 2}, {2, 1}, {3, 2} });
    
    // For S1 to S4
    ingress.push_back(3);
    ingress.push_back(3);
    ingress.push_back(3);
    ingress.push_back(3);

    // For S1 to S4
    egress.push_back(3);
    egress.push_back(3);
    egress.push_back(3);
    egress.push_back(3);
    
    voq_input_maps.push_back({ 0, 0, 0, 1, 1, 1, 2, 2, 2 });
    voq_input_maps.push_back({ 0, 0, 0, 1, 1, 1, 2, 2, 2});
    voq_input_maps.push_back({ 0, 0, 0, 1, 1, 1, 2, 2, 2 });
    voq_input_maps.push_back({ 0, 0, 0, 1, 1, 1, 2, 2, 2 });

    voq_output_maps.push_back({ 0, 1, 2, 0, 1, 2, 0, 1, 2 });
    voq_output_maps.push_back({ 0, 1, 2, 0, 1, 2, 0, 1, 2 });
    voq_output_maps.push_back({ 0, 1, 2, 0, 1, 2 ,0, 1, 2 });
    voq_output_maps.push_back({ 0, 1, 2, 0, 1, 2, 0, 1, 2 });

    init();
}

void RoceScheduler::add_nodes() {
    // add a rr qm
    QueueInfo info;
    info.size = MAX_QUEUE_SIZE;
    info.max_enq = MAX_ENQ;
    info.max_deq = 1;

    QueueInfo link_info;
    link_info.size = 1;
    link_info.max_enq = 1;
    link_info.max_deq = 1;
    link_info.type = queue_t::LINK;

    QueueInfo imm_info;
    imm_info.size = 10;
    imm_info.max_enq = 1;
    imm_info.max_deq = 1;
    imm_info.type = queue_t::IMM_QUEUE;

    unsigned int buffer_size = 5;

    std::map<unsigned int, unsigned int> control_flow;
    for (unsigned int i = 0; i < switch_cnt; i++) {
        for (unsigned int j = 0; j < ingress[i]; j++) {
            cid_t m_id = "roce" + to_string(i) + "_" + to_string(j);
            cid_t s_id = "s" + to_string(i);
            RoCEQM* roce_qm = new RoCEQM(m_id,
                s_id,
                total_time,
                buffer_size,
                threshold,
                j,
                vector<QueueInfo>(1, info),
                vector<QueueInfo>(egress[i], info),
                control_flows[i],
                net_ctx);

            nodes.push_back(m_id);
            id_to_qm[m_id] = roce_qm;
        }
    }

    for (int i = 0; i < switch_cnt; i++) {
        cid_t m_id = "roce_xBar" + to_string(i);
        cid_t s_id = "s" + to_string(i);
        RoceSwitchXBarQM* xbar_qm = new RoceSwitchXBarQM(m_id, s_id, total_time,
            voq_input_maps[i], voq_output_maps[i],
            vector<QueueInfo>(ingress[i] * egress[i], imm_info),
            vector<QueueInfo>(egress[i], link_info),
            net_ctx);
        nodes.push_back(m_id);
        id_to_qm[m_id] = xbar_qm;
    }
}

void RoceScheduler::add_edges()
{
    // connect each qm to its respective xbar
    for (int i = 0; i < switch_cnt; i++) {
        cid_t xbar_id = "roce_xBar" + to_string(i);
        unsigned int xbar_qind = 0;
        for (int j = 0; j < ingress[i]; j++) {
            cid_t fw_id = "roce" + to_string(i) + "_" + to_string(j);
            //module edges
            vector<cid_t> edges;
            edges.push_back(xbar_id);
            module_edges[fw_id] = edges;

            //queue edges
            QueuingModule* fw = id_to_qm[fw_id];
            vector<qpair> q_edges;
            for (unsigned int k = 0; k < fw->out_queue_cnt(); k++) {
                q_edges.push_back(qpair(k, xbar_qind)); // is this needed???
                xbar_qind += 1;
            }
            queue_edges[cid_pair(fw_id, xbar_id)] = q_edges;
        }
    }

    // initialize module edges
    for (int i = 0; i < switch_cnt; i++) {
        cid_t s = "roce" + to_string(i);
        module_edges[s] = {};
    }

    // Need to connect each switch to other switch in a ring fashion
    for (int i = 0; i < switch_cnt; i++) {
        cid_t switch_out = "roce_xBar" + to_string(i);
        cid_t switch_in1 = "roce" + to_string((i + 1) % switch_cnt);
        cid_t switch_in2 = i == 0 ? "roce" + to_string(switch_cnt - 1) : "roce" + to_string(i - 1);

        // queue edges
        vector<qpair> q_edges;
        unsigned int k = i % 2 == 0 ? 0 : 1;
        switch_in1 += "_" + to_string(k);
        q_edges.push_back(qpair(k, 0));
        queue_edges[cid_pair(switch_out, switch_in1)] = q_edges;

        q_edges = {};
        k = k == 0 ? 1 : 0; 
        q_edges.push_back(qpair(k, 0));
        switch_in2 += "_" + to_string(k);
        queue_edges[cid_pair(switch_out, switch_in2)] = q_edges;

        //module edges
        module_edges[switch_out].push_back(switch_in1);
        module_edges[switch_out].push_back(switch_in2);
    }
}

void RoceScheduler::add_metrics() {
    //IN
    // CEnq
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        Queue* queue = in_queues[q];
        CEnq* ce = new CEnq(queue, total_time, net_ctx);
        cenq.push_back(ce);
        metrics[metric_t::CENQ][queue->get_id()] = ce;
        queue->add_metric(metric_t::CENQ, ce);
    }

    // CDeq
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        Queue* queue = in_queues[q];
        CDeq* cd = new CDeq(queue, total_time, net_ctx);
        cdeq.push_back(cd);
        metrics[metric_t::CDEQ][queue->get_id()] = cd;
        queue->add_metric(metric_t::CDEQ, cd);
    }

    // OUTPUTS
    // AIPG
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        Queue* queue = in_queues[q];
        AIPG* g = new AIPG(queue, total_time, net_ctx);
        aipg.push_back(g);
        metrics[metric_t::AIPG][queue->get_id()] = g;
        queue->add_metric(metric_t::AIPG, g);
    }

    // MIDDLE

    // S1 xbar
    Queue* s1_0_queue = id_to_qm["roce_xBar1"]->get_in_queue(0);
    QSize* s1_0_qsize = new QSize(s1_0_queue, total_time, net_ctx);
    qsize.push_back(s1_0_qsize);
    metrics[metric_t::QSIZE][s1_0_queue->get_id()] = s1_0_qsize;
    s1_0_queue->add_metric(metric_t::QSIZE, s1_0_qsize);

    Queue* s1_1_queue = id_to_qm["roce_xBar1"]->get_in_queue(1);
    QSize* s1_1_qsize = new QSize(s1_1_queue, total_time, net_ctx);
    qsize.push_back(s1_1_qsize);
    metrics[metric_t::QSIZE][s1_1_queue->get_id()] = s1_1_qsize;
    s1_1_queue->add_metric(metric_t::QSIZE, s1_1_qsize);

    // S3 xbar
    Queue* s3_0_queue = id_to_qm["roce_xBar3"]->get_in_queue(0);
    QSize* s3_0_qsize = new QSize(s3_0_queue, total_time, net_ctx);
    qsize.push_back(s3_0_qsize);
    metrics[metric_t::QSIZE][s3_0_queue->get_id()] = s3_0_qsize;
    s3_0_queue->add_metric(metric_t::QSIZE, s3_0_qsize);

    Queue* s3_1_queue = id_to_qm["roce_xBar3"]->get_in_queue(1);
    QSize* s3_1_qsize = new QSize(s3_1_queue, total_time, net_ctx);
    qsize.push_back(s3_1_qsize);
    metrics[metric_t::QSIZE][s3_1_queue->get_id()] = s3_1_qsize;
    s3_1_queue->add_metric(metric_t::QSIZE, s3_1_qsize);

    Queue* s_0_queue = id_to_qm["roce0_1"]->get_out_queue(1);
    QSize* s0_1_qsize = new QSize(s_0_queue, total_time, net_ctx);
    qsize.push_back(s0_1_qsize);
    metrics[metric_t::QSIZE][s0_1_qsize->get_id()] = s0_1_qsize;
    s_0_queue->add_metric(metric_t::QSIZE, s0_1_qsize);

}

std::string RoceScheduler::cp_model_metric(model& m, unsigned int t, string qid, bool in_q, int q_index, metric_t metric) {
    Queue* q;
    if (in_q) {
        q = id_to_qm[qid]->get_in_queue(q_index);
    }
    else {
        q = id_to_qm[qid]->get_out_queue(q_index);
    } 
    Metric* m1 = q->get_metric(metric);

    string s = "";
    s += m1->get_id() + ": ";
    expr val1 = m.eval(m1->val(t));
    if (val1.is_numeral())
        s += to_string(val1.get_numeral_int());
    s += "\n";
    return s;
}

std::string RoceScheduler::cp_model_str(model& m,
    NetContext& net_ctx,
    unsigned int t) {

    (void)m;
    (void)net_ctx;
    (void)t;

    stringstream ss;
    ss << "cenqs: " << endl;
    for (unsigned int q = 0; q < out_queues.size(); q++) {
        ss << q << ": ";
        CEnq* ce = cenq[q];
        expr val = m.eval(ce->val(t));
        if (val.is_numeral()) ss << val.get_numeral_int();
        ss << endl;
    }


    Queue* s1_0_queue = id_to_qm["roce_xBar1"]->get_in_queue(0);
    Queue* s1_1_queue = id_to_qm["roce_xBar1"]->get_in_queue(1);
    Queue* s3_0_queue = id_to_qm["roce_xBar3"]->get_in_queue(0);
    Queue* s3_1_queue = id_to_qm["roce_xBar3"]->get_in_queue(1);

    Metric* m1 = s1_0_queue->get_metric(metric_t::QSIZE);
    Metric* m2 = s1_1_queue->get_metric(metric_t::QSIZE);
    Metric* m3 = s3_0_queue->get_metric(metric_t::QSIZE);
    Metric* m4 = s3_1_queue->get_metric(metric_t::QSIZE);

    ss << m1->get_id() << ": ";
    expr val1 = m.eval(m1->val(t));
    if (val1.is_numeral()) ss << val1.get_numeral_int();
    ss << endl;

    ss << m2->get_id() << ": ";
    expr val2 = m.eval(m2->val(t));
    if (val2.is_numeral()) ss << val2.get_numeral_int();
    ss << endl;

    ss << m3->get_id() << ": ";
    expr val3 = m.eval(m3->val(t));
    if (val3.is_numeral()) ss << val3.get_numeral_int();
    ss << endl;

    for (int i = 0; i <= t; i++) {
        //auto name = "s0_0_pause_state_[" + to_string(i) + "]";
        auto name = "s0_0_sent_pause_[" + to_string(i) + "]";
        expr ttt = m.eval(net_ctx.get_bool_const(name.data()));
        ss << ttt.bool_value();
        ss << " ";
    }
    ss << endl;

    for (int i = 0; i <= t; i++) {
        //auto name = "s0_0_pause_state_[" + to_string(i) + "]";
        auto name = "s1_0_pause_state_[" + to_string(i) + "]";
        expr ttt = m.eval(net_ctx.get_bool_const(name.data()));
        ss << ttt.bool_value();
        ss << " ";
    }
    ss << endl;

    for (int i = 0; i <= t; i++) {
        //auto name = "s0_0_pause_state_[" + to_string(i) + "]";
        auto name = "s1_0_sent_pause_[" + to_string(i) + "]";
        expr ttt = m.eval(net_ctx.get_bool_const(name.data()));
        ss << ttt.bool_value();
        ss << " ";
    }
    ss << endl;

    for (int i = 0; i <= t; i++) {
        //auto name = "s0_0_pause_state_[" + to_string(i) + "]";
        auto name = "s0_0_pause_state_[" + to_string(i) + "]";
        expr ttt = m.eval(net_ctx.get_bool_const(name.data()));
        ss << ttt.bool_value();
        ss << " ";
    }
    ss << endl;

    for (int i = 1; i <= t; i++) {
        //auto name = "s0_0_pause_state_[" + to_string(i) + "]";
        auto name = "roce_xBar0.0_curr_size_[" + to_string(i) + "]";
        expr ttt = m.eval(net_ctx.get_int_const(name.data()));
        if (ttt.is_numeral()) ss << ttt.get_numeral_int();
        ss << " | ";
    }
    ss << endl;

    for (int i = 1; i <= t; i++) {
        //auto name = "s0_0_pause_state_[" + to_string(i) + "]";
        auto name = "roce_xBar0.1_curr_size_[" + to_string(i) + "]";
        expr ttt = m.eval(net_ctx.get_int_const(name.data()));
        if (ttt.is_numeral()) ss << ttt.get_numeral_int();
        ss << " | ";
    }
    ss << endl;

    for (int i = 1; i <= t; i++) {
        //auto name = "s0_0_pause_state_[" + to_string(i) + "]";
        auto name = "roce_xBar0.2_curr_size_[" + to_string(i) + "]";
        expr ttt = m.eval(net_ctx.get_int_const(name.data()));
        if (ttt.is_numeral()) ss << ttt.get_numeral_int();
        ss << " | ";
    }
    ss << endl;
    /*ss << m4->get_id() << ": ";
    expr val4 = m.eval(m3->val(t));
    if (val3.is_numeral()) ss << val4.get_numeral_int();

    ss << endl;*/
    
    string test = cp_model_metric(m, t, "roce0_1", false, 1, metric_t::QSIZE);
    
    ss << test;
    return ss.str();
}
