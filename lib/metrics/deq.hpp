#ifndef METRICS_ENQ_HPP
#define METRICS_ENQ_HPP

#include "metric.hpp"

class Deq : public Metric {
public:
    Deq(Queue* queue, unsigned int total_time, NetContext& net_ctx);

    void populate_val_exprs(NetContext& net_ctx);

    void eval(const IndexedExample* eg, unsigned int time, unsigned int qind, metric_val& res);
};

#endif
