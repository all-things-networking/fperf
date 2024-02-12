//
//  rr_scheduler.hpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 2/18/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef rr_scheduler_hpp
#define rr_scheduler_hpp

#include "aipg.hpp"
#include "cdeq.hpp"
#include "cenq.hpp"
#include "dst.hpp"
#include "ecmp.hpp"
#include "contention_point.hpp"
#include "qsize.hpp"

using namespace std;

class RRScheduler : public ContentionPoint {
public:
    RRScheduler(unsigned int queue_cnt, unsigned int total_time);

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

#endif /* rr_scheduler_hpp */
