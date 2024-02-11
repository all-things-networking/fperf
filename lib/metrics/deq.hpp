#ifndef METRICS_ENQ_HPP
#define METRICS_ENQ_HPP

#include "metric.hpp"

class Deq : public Metric {
public:
    Deq(Queue* queue, unsigned int total_time, NetContext& net_ctx);

    void populate_val_exprs(NetContext& net_ctx);

    virtual void eval(Example* eg, unsigned int time, cid_t qind, metric_val& res);
};

#endif
