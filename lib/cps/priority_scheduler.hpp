//
//  priority_scheduler.hpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/16/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef priority_scheduler_hpp
#define priority_scheduler_hpp

#include "aipg.hpp"
#include "cblocked.hpp"
#include "cenq.hpp"
#include "dst.hpp"
#include "ecmp.hpp"
#include "contention_point.hpp"

using namespace std;

class PrioScheduler : public ContentionPoint {
public:
    PrioScheduler(unsigned int prio_levels, unsigned int total_time);

private:
    unsigned int prio_levels;
    vector<CBlocked*> cblocked;
    vector<CEnq*> cenq;
    vector<AIPG*> aipg;
    vector<Dst*> dst;
    vector<Ecmp*> ecmp;

    void add_nodes();
    void add_edges();
    void add_metrics();

    string cp_model_str(model& m, NetContext& net_ctx, unsigned int t);
};

#endif /* priority_scheduler_hpp */
