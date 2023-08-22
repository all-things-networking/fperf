#include "deq.hpp"

Deq::Deq(Queue *queue, unsigned int total_time, NetContext &net_ctx)
    : Metric(metric_t::DEQ, queue, total_time, net_ctx) {
  init(net_ctx);
}

unsigned int Deq::eval(const IndexedExample *eg, unsigned int time,
                       unsigned int qind) {
  return eg->deqs[qind][time];
}

void Deq::add_vars(NetContext &net_ctx) { (void)net_ctx; }

void Deq::add_constrs(NetContext &net_ctx,
                      std::map<std::string, expr> &constr_map) {
  (void)net_ctx;

  char constr_name[100];

  for (unsigned int t = 0; t < total_time; t++) {
    expr constr_expr = val_[t] == queue->deq_cnt(t);
    sprintf(constr_name, "%s_val[%d]", id.c_str(), t);
    constr_map.insert(named_constr(constr_name, constr_expr));
  }
}
