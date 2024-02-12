//
//  buggy_2l_rr_scheduler.hpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 08/03/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef buggy_2l_rr_scheduler_hpp
#define buggy_2l_rr_scheduler_hpp

#include "aipg.hpp"
#include "cdeq.hpp"
#include "cenq.hpp"
#include "dst.hpp"
#include "ecmp.hpp"
#include "contention_point.hpp"

using namespace std;

class Buggy2LRRScheduler : public ContentionPoint {
public:
    Buggy2LRRScheduler(unsigned int queue_cnt, unsigned int total_time);

private:
    unsigned int queue_cnt;
    vector<CEnq*> cenq;
    vector<CDeq*> cdeq;
    vector<AIPG*> aipg;
    vector<Dst*> dst;
    vector<Ecmp*> ecmp;

    void add_nodes();
    void add_edges();
    void add_metrics();

    string cp_model_str(model& m, NetContext& net_ctx, unsigned int t);
};

#endif /* buggy_2l_rr_scheduler_hpp */
