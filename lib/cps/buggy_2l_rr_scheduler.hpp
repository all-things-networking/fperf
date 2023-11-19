//
//  buggy_2l_rr_scheduler.hpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 08/03/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef buggy_2l_rr_scheduler_hpp
#define buggy_2l_rr_scheduler_hpp

#include "aipg.hpp"
#include "cdeq.hpp"
#include "cenq.hpp"
#include "contention_point.hpp"

class Buggy2LRRScheduler : public ContentionPoint {
public:
    Buggy2LRRScheduler(unsigned int queue_cnt, unsigned int total_time);

private:
    unsigned int queue_cnt;
    std::vector<CEnq*> cenq;
    std::vector<CDeq*> cdeq;
    std::vector<AIPG*> aipg;

    void add_nodes();
    void add_edges();
    void add_metrics();

    std::string cp_model_str(model& m, NetContext& net_ctx, unsigned int t);
};

#endif /* buggy_2l_rr_scheduler_hpp */
