//
//  leaf_spine.hpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 12/24/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef leaf_spine_hpp
#define leaf_spine_hpp

#include "aipg.hpp"
#include "cenq.hpp"
#include "contention_point.hpp"
#include "dst.hpp"
#include "ecmp.hpp"
#include "qsize.hpp"

using namespace std;

class LeafSpine : public ContentionPoint {
public:
    LeafSpine(unsigned int leaf_cnt,
              unsigned int spine_cnt,
              unsigned int servers_per_leaf,
              unsigned int total_time,
              bool reduce_queues);

private:
    unsigned int leaf_cnt;
    unsigned int spine_cnt;
    unsigned int servers_per_leaf;
    unsigned int leaf_port_cnt;
    unsigned int spine_port_cnt;
    unsigned int server_cnt;
    bool reduce_queues;

    vector<CEnq*> cenq;
    vector<AIPG*> aipg;
    vector<Dst*> dst;
    vector<Ecmp*> ecmp;
    vector<QSize*> qsize;

    vector<unsigned int> leaf_voq_input_map;
    vector<unsigned int> leaf_voq_output_map;
    vector<unsigned int> spine_voq_input_map;
    vector<unsigned int> spine_voq_output_map;

    void add_nodes();
    void add_edges();
    void add_metrics();

    string cp_model_str(model& m, NetContext& net_ctx, unsigned int t);
};


#endif /* leaf_spine_hpp */
