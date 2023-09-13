#ifndef METRICS_ENQ_HPP
#define METRICS_ENQ_HPP

#include "metric.hpp"

class Deq : public Metric {
public:
    Deq(Queue* queue, unsigned int total_time, NetContext& net_ctx);

    void add_constrs(NetContext& net_ctx, std::map<std::string, expr>& constr_map);

    unsigned int eval(const IndexedExample* eg, unsigned int time, unsigned int qind);

private:
    void add_vars(NetContext& net_ctx);
};

#endif
