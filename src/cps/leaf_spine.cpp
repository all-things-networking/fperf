//
//  leaf_spine.cpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 12/24/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "util.hpp"
#include "leaf_spine.hpp"
#include "switch_xbar_qm.hpp"
#include "spine_forwarding_qm.hpp"
#include "leaf_forwarding_qm.hpp"
#include "math.h"

#include <sstream>

unsigned int PKT_METRIC1_MAX;
unsigned int PKT_METRIC2_MAX;

LeafSpine::LeafSpine(unsigned int leaf_cnt,
                     unsigned int spine_cnt,
                     unsigned int servers_per_leaf,
                     unsigned int total_time,
                     bool reduce_queues):
ContentionPoint(total_time),
leaf_cnt(leaf_cnt),
spine_cnt(spine_cnt),
servers_per_leaf(servers_per_leaf),
reduce_queues(reduce_queues)
{
    leaf_port_cnt = servers_per_leaf + spine_cnt;
    spine_port_cnt = leaf_cnt;
    server_cnt = servers_per_leaf * leaf_cnt;
    init();
}

void LeafSpine::add_nodes(){
    QueueInfo info;
    info.size = MAX_QUEUE_SIZE;
    info.max_enq = MAX_ENQ;
    info.max_deq = 1;
    info.type = queue_t::QUEUE;

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

    // Leaves
    unsigned int leaf_voq_ind = 0;
    for (unsigned int i = 0; i < leaf_port_cnt; i++){
        for (unsigned int j = 0; j < leaf_port_cnt; j++){
            // can't send to itself
            if (reduce_queues && i == j) continue;
            
            // spines don't send to each otehr
            if (reduce_queues && i >= servers_per_leaf && j >= servers_per_leaf) continue;

            leaf_voq_input_map.push_back(i);
            leaf_voq_output_map.push_back(j);
            leaf_voq_ind += 1;
        }
    }

    for (unsigned int i = 0; i < leaf_cnt; i++){
        // Cross Bar
        cid_t m_id = "L" + to_string(i) + "_XBar";
        
        
        /*SwitchXBarQM* xbar_qm = new SwitchXBarQM(m_id,
                                                 total_time,
                                                 leaf_voq_input_map, leaf_voq_output_map,
                                                 vector<QueueInfo>(leaf_voq_ind, imm_info),
                                                 vector<QueueInfo>(leaf_port_cnt, info),
                                                 net_ctx);
        */
        LeafXBarQM* xbar_qm = new LeafXBarQM(m_id,
                                             i,
                                             servers_per_leaf,
                                             spine_cnt,
                                             total_time,
                                             leaf_voq_input_map, leaf_voq_output_map,
                                             vector<QueueInfo>(leaf_voq_ind, imm_info),
                                             vector<QueueInfo>(leaf_port_cnt, link_info),
                                             net_ctx);

        nodes.push_back(m_id);
        id_to_qm[m_id] = xbar_qm;
        
        // Forwarding Modules
        unsigned int leaf_base_port = 0;
        for (unsigned int j = 0; j < leaf_port_cnt; j++){
            map<unsigned int, unsigned int> output_voq_map;
            for (unsigned int i = leaf_base_port; i < leaf_voq_ind; i++){
                unsigned int input_port = leaf_voq_input_map[i];
                if (input_port == j){
                    unsigned int output_port = leaf_voq_output_map[i];
                    output_voq_map[output_port] = i - leaf_base_port;
                }
                else if (input_port > j){
                    leaf_base_port = i;
                    break;
                }
            }

            m_id = "L" + to_string(i) + "_FW" + to_string(j);

            unsigned int leaf_in_queue_cnt = 0;
            for (unsigned int k = 0; k < leaf_voq_ind; k++){
                if (leaf_voq_input_map[k] == j) leaf_in_queue_cnt += 1;
            }

            LeafForwardingQM* fw_qm;
            if (j < servers_per_leaf){
                fw_qm = new LeafForwardingQM(m_id,
                                               total_time,
                                               i,
                                               j,
                                               servers_per_leaf,
                                               server_cnt,
                                               spine_cnt,
                                               output_voq_map,
                                               info,
                                               vector<QueueInfo>(leaf_in_queue_cnt, imm_info),
                                               net_ctx);
            }
            else {
                fw_qm = new LeafForwardingQM(m_id,
                                               total_time,
                                               i,
                                               j,
                                               servers_per_leaf,
                                               server_cnt,
                                               spine_cnt,
                                               output_voq_map,
                                               link_info,
                                               vector<QueueInfo>(leaf_in_queue_cnt, imm_info),
                                               net_ctx);
            }
 
            nodes.push_back(m_id);
            id_to_qm[m_id] = fw_qm;
        }
    }
    
    // Spines
    unsigned int spine_voq_ind = 0;
    for (unsigned int i = 0; i < spine_port_cnt; i++){
        for (unsigned int j = 0; j < spine_port_cnt; j++){
            // can't send to itself
            if (reduce_queues && i == j) continue;
            
            spine_voq_input_map.push_back(i);
            spine_voq_output_map.push_back(j);
        
            spine_voq_ind += 1;
        }
    }

    for (unsigned int i = 0; i < spine_cnt; i++){
        // Cross Bar
        cid_t m_id = "S" + to_string(i) + "_XBar";
        SwitchXBarQM* xbar_qm = new SwitchXBarQM(m_id,
                                                 total_time,
                                                 spine_voq_input_map, spine_voq_output_map,
                                                 vector<QueueInfo>(spine_voq_ind, imm_info),
                                                 vector<QueueInfo>(spine_port_cnt, link_info),
                                                 net_ctx);
        nodes.push_back(m_id);
        id_to_qm[m_id] = xbar_qm;
        
        // Forwarding Modules
        unsigned int spine_base_port = 0;
        for (unsigned int j = 0; j < spine_port_cnt; j++){
            map<unsigned int, unsigned int> output_voq_map;
            for (unsigned int i = spine_base_port; i < spine_voq_ind; i++){
                unsigned int input_port = spine_voq_input_map[i];
                if (input_port == j){
                    unsigned int output_port = spine_voq_output_map[i];
                    output_voq_map[output_port] = i - spine_base_port;
                }
                else if (input_port > j){
                    spine_base_port = i;
                    break;
                }
            }
            
            m_id = "S" + to_string(i) + "_FW" + to_string(j);
            unsigned int spine_in_queue_cnt = 0;
            for (unsigned int k = 0; k < spine_voq_ind; k++){
                if (spine_voq_input_map[k] == j) spine_in_queue_cnt += 1;
            }
            SpineForwardingQM* fw_qm = new SpineForwardingQM(m_id,
                                                             total_time,
                                                             i,
                                                             leaf_cnt,
                                                             servers_per_leaf,  
                                                             output_voq_map,
                                                             link_info,
                                                             vector<QueueInfo>(spine_in_queue_cnt, imm_info),
                                                             net_ctx);
            nodes.push_back(m_id);
            id_to_qm[m_id] = fw_qm;
        }
    }
    
}

void LeafSpine::add_edges()
{
    // Connect fw and xbar for leaf switches
    for (unsigned int i = 0; i < leaf_cnt; i++){
        cid_t xbar_id = "L" + to_string(i) + "_XBar";
        unsigned int xbar_qind = 0;
        for (unsigned int j = 0; j < leaf_port_cnt; j++){
            cid_t fw_id = "L" + to_string(i) + "_FW" + to_string(j);

            // modules edges
            vector<cid_t> edges;
            edges.push_back(xbar_id);
            module_edges[fw_id] = edges;

            // queue edges
            QueuingModule* fw = id_to_qm[fw_id];
            vector<qpair> q_edges;
            for (unsigned int k = 0; k < fw->out_queue_cnt(); k++){
                q_edges.push_back(qpair(k, xbar_qind));
                xbar_qind += 1;
            }
            queue_edges[cid_pair(fw_id, xbar_id)] = q_edges;
        }
    }
    
    // Connect fw and xbar for spine switches
    for (unsigned int i = 0; i < spine_cnt; i++){
        cid_t xbar_id = "S" + to_string(i) + "_XBar";
        unsigned int xbar_qind = 0;
        for (unsigned int j = 0; j < spine_port_cnt; j++){
            cid_t fw_id = "S" + to_string(i) + "_FW" + to_string(j);

            // modules edges
            vector<cid_t> edges;
            edges.push_back(xbar_id);
            module_edges[fw_id] = edges;

            // queue edges
            QueuingModule* fw = id_to_qm[fw_id];
            vector<qpair> q_edges;
            for (unsigned int k = 0; k < fw->out_queue_cnt(); k++){
                q_edges.push_back(qpair(k, xbar_qind));
                xbar_qind += 1;
            }
            queue_edges[cid_pair(fw_id, xbar_id)] = q_edges;
        }
    }
    // For leaf switch i, output ports server_cnt, ..., end from the XBar
    // are connected to the input port of the ith forwarding module of the spines
    for (unsigned int i = 0; i < leaf_cnt; i++){
        cid_t leaf_xbar_id = "L" + to_string(i) + "_XBar";
        // modules edges
        vector<cid_t> edges;
        for (unsigned int j = 0; j < spine_cnt; j++){
            cid_t spine_fw_id = "S" + to_string(j) + "_FW" + to_string(i);
            edges.push_back(spine_fw_id);
        }
        module_edges[leaf_xbar_id] = edges;

        // queue edges
        for (unsigned int j = 0; j < spine_cnt; j++){
            vector<qpair> q_edges;
            q_edges.push_back(qpair(servers_per_leaf + j, 0));
            cid_t spine_fw_id = "S" + to_string(j) + "_FW" + to_string(i);
            queue_edges[cid_pair(leaf_xbar_id, spine_fw_id)] = q_edges;
        }
    }
    
    // For spine switch i, output queue j of the xbar is connected
    // to the input queue of the (server_per_leaf + i)th forwarding module of the jth leaf
    for (unsigned int i = 0; i < spine_cnt; i++){
        cid_t spine_xbar_id = "S" + to_string(i) + "_XBar";
        unsigned int fw_module_ind = servers_per_leaf + i;
        // modules edges
        vector<cid_t> edges;
        for (unsigned int j = 0; j < leaf_cnt; j++){
            cid_t leaf_fw_id = "L" + to_string(j) + "_FW" + to_string(fw_module_ind);
            edges.push_back(leaf_fw_id);
        }
        module_edges[spine_xbar_id] = edges;
        
        // queue edges
        for (unsigned int j = 0; j < leaf_cnt; j++){
            vector<qpair> q_edges;
            q_edges.push_back(qpair(j, 0));
            cid_t leaf_fw_id = "L" + to_string(j) + "_FW" + to_string(fw_module_ind);
            queue_edges[cid_pair(spine_xbar_id, leaf_fw_id)] = q_edges;
        }
    }
}

void LeafSpine::add_metrics(){
   //// Inputs
   // CEnq
   for (unsigned int q = 0; q < in_queues.size(); q++){
        Queue* queue = in_queues[q];
        CEnq* ce = new CEnq(queue, total_time, net_ctx);
        cenq.push_back(ce);
        metrics[metric_t::CENQ][queue->get_id()] = ce;
        queue->add_metric(metric_t::CENQ, ce);
    }
    
    // AIPG
    for (unsigned int q = 0; q < in_queues.size(); q++){
        Queue* queue = in_queues[q];
        AIPG* g = new AIPG(queue, total_time, net_ctx);
        aipg.push_back(g);
        metrics[metric_t::AIPG][queue->get_id()] = g;
        queue->add_metric(metric_t::AIPG, g);
    }
    
    //// Outputs
    // CEnq
    for (unsigned int q = 0; q < out_queues.size(); q++){
        Queue* queue = out_queues[q];
        CEnq* ce = new CEnq(queue, total_time, net_ctx);
        cenq.push_back(ce);
        metrics[metric_t::CENQ][queue->get_id()] = ce;
        queue->add_metric(metric_t::CENQ, ce);
    }

    //// Middle
    // QSize

    // L0_XBar.in[1]
    Queue* l0_queue = id_to_qm["L0_XBar"]->get_in_queue(servers_per_leaf - 1);
    QSize* s = new QSize(l0_queue, total_time, net_ctx);
    qsize.push_back(s);
    metrics[metric_t::QSIZE][l0_queue->get_id()] = s;
    l0_queue->add_metric(metric_t::QSIZE, s);

    // S0_XBar.in[1]
    Queue* s0_queue = id_to_qm["S0_XBar"]->get_in_queue(1);
    s = new QSize(s0_queue, total_time, net_ctx);
    qsize.push_back(s);
    metrics[metric_t::QSIZE][s0_queue->get_id()] = s;
    s0_queue->add_metric(metric_t::QSIZE, s);

    // L2_XBar.in[7]
    unsigned int prev_queues = servers_per_leaf * (servers_per_leaf - 1 + spine_cnt) + 1; 
    Queue* l2_queue = id_to_qm["L2_XBar"]->get_in_queue(prev_queues);
    s = new QSize(l2_queue, total_time, net_ctx);
    qsize.push_back(s);
    metrics[metric_t::QSIZE][l2_queue->get_id()] = s;
    l2_queue->add_metric(metric_t::QSIZE, s);

    /*
    for (unsigned int i = 0; i < spine_cnt; i++){
        cid_t spine_xbar_id = "S" + to_string(i) + "_XBar";
        QueuingModule* m = id_to_qm[spine_xbar_id];
        for (unsigned int q = 0; q < m->in_queue_cnt(); q++){
            Queue* queue = m->get_in_queue(q);
            QSize* s = new QSize(queue, total_time, net_ctx);
            qsize.push_back(s);
            metrics[metric_t::QSIZE][queue->get_id()] = s;
            queue->add_metric(metric_t::QSIZE, s);
        }
    } 

    for (unsigned int i = 0; i < leaf_cnt; i++){
        cid_t spine_xbar_id = "L" + to_string(i) + "_XBar";
        QueuingModule* m = id_to_qm[spine_xbar_id];
        for (unsigned int q = 0; q < m->in_queue_cnt(); q++){
            Queue* queue = m->get_in_queue(q);
            QSize* s = new QSize(queue, total_time, net_ctx);
            qsize.push_back(s);
            metrics[metric_t::QSIZE][queue->get_id()] = s;
            queue->add_metric(metric_t::QSIZE, s);
        }
    }
    */
}

std::string LeafSpine::cp_model_str(model& m,
                                 NetContext& net_ctx,
                                 unsigned int t){
    (void) m;
    (void) net_ctx;
    (void) t;
    
    stringstream ss;
    ss << "cenqs: " << endl;
    for (unsigned int q = 0; q < out_queues.size(); q++){
        ss << q << ": ";
        CEnq* ce = cenq[in_queues.size() + q];
        expr val = m.eval(ce->val(t).second);
        if (val.is_numeral()) ss << val.get_numeral_int();
        ss << endl;
    }    

    
    Queue* l0_queue = id_to_qm["L0_XBar"]->get_in_queue(servers_per_leaf - 1);
    Queue* s0_queue = id_to_qm["S0_XBar"]->get_in_queue(1);
    unsigned int prev_queues = servers_per_leaf * (servers_per_leaf - 1 + spine_cnt) + 1; 
    Queue* l2_queue = id_to_qm["L2_XBar"]->get_in_queue(prev_queues);

    Metric* m1 = l0_queue->get_metric(metric_t::QSIZE);
    Metric* m2 = s0_queue->get_metric(metric_t::QSIZE);
    Metric* m3 = l2_queue->get_metric(metric_t::QSIZE);

    ss << m1->get_id() << ": ";
    expr val1 = m.eval(m1->val(t).second);
    if (val1.is_numeral()) ss << val1.get_numeral_int();
    ss << endl;

    ss << m2->get_id() << ": ";
    expr val2 = m.eval(m2->val(t).second);
    if (val2.is_numeral()) ss << val2.get_numeral_int();
    ss << endl;
 
    ss << m3->get_id() << ": ";
    expr val3 = m.eval(m3->val(t).second);
    if (val3.is_numeral()) ss << val3.get_numeral_int();
    ss << endl;

    /*
    for (unsigned int i = 0; i < qsize.size(); i++){
        QSize* s = qsize[i];
        ss << s->get_id() << ": ";
        expr val = m.eval(s->val(t));
        if (val.is_numeral()) ss << val.get_numeral_int();
        ss << endl;
    }
    */
    /*    
    cid_t xbar_id = "L0_XBar";
    LeafXBarQM* xbar_qm = (LeafXBarQM*) id_to_qm[xbar_id];    
    unsigned int port_cnt = servers_per_leaf + spine_cnt;

    ss << "in_to_out: " << endl;
    for (unsigned int i = 0; i < port_cnt; i++){
        bool found = false;
        for (unsigned int j = 0; j < port_cnt; j++){
            if (!xbar_qm->connected(i, j)) continue;
            expr is_one = m.eval(xbar_qm->in_to_out(i, j, t));
            if (is_one.is_true()){
                found = true;
                ss << i << ": " << j << endl;
                break;
            }
        }
        if (!found){
            ss << i << ": x" << endl;
        }
    }
    ss << endl;

    ss << "out_from_in: " << endl;
    for (unsigned int j = 0; j < port_cnt; j++){
        bool found = false;
        for (unsigned int i = 0; i < port_cnt; i++){
            if (!xbar_qm->connected(i, j)) continue;
            expr is_one = m.eval(xbar_qm->out_from_in(j, i, t));
            if (is_one.is_true()){
                found = true;
                ss << j << ": " << i << endl;
                break;
            }
        }
        if (!found){
            ss << j << ": x" << endl;
        }
    }
    ss << endl;

    ss << "in_prio_heads: " << endl;
    for (unsigned int i = 0; i < port_cnt; i++){
        for (unsigned int j = 0; j < port_cnt; j++){
            if (!xbar_qm->connected(i, j)) continue;
            expr is_one = m.eval(xbar_qm->in_prio_head(i, j, t));
            if (is_one.is_true()){
                ss << i << ": " << j << endl;
                break;
            }
        }
    }
    ss << endl;
    
    ss << "out_prio_heads: " << endl;
    for (unsigned int j = 0; j < port_cnt; j++){
        for (unsigned int i = 0; i < port_cnt; i++){
            if (!xbar_qm->connected(i, j)) continue;
            expr is_one = m.eval(xbar_qm->out_prio_head(j, i, t));
            if (is_one.is_true()){
                ss << j << ": " << i << endl;
            }
        }
    }
    */

    return ss.str();
}
