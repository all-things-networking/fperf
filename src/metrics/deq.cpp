#include "deq.hpp"

Deq::Deq(Queue *queue, unsigned int total_time, NetContext &net_ctx)
    : Metric(metric_t::DEQ, queue, total_time, net_ctx) {
  init(net_ctx);
}

unsigned int Deq::eval(const IndexedExample *eg, unsigned int time,
                       unsigned int qind) {
  unsigned int res = 0;
  for (unsigned int t = 0; t <= time; t++) {
    res = eg->deqs[qind][t];
  }
  return res;
}

void Deq::add_vars(NetContext &net_ctx) { (void)net_ctx; }

void Deq::add_constrs(NetContext &net_ctx,
                      std::map<std::string, expr> &constr_map) {

  (void)net_ctx;

  char constr_name[100];

  //  // Constraints for the value of cdeq
  //  expr constr_expr = val_[0] == queue->deq_cnt(0);
  //  sprintf(constr_name, "%s_val[0]", id.c_str());
  //  constr_map.insert(named_constr(constr_name, constr_expr));

  for (unsigned int t = 0; t < total_time; t++) {
    expr constr_expr = val_[t] == queue->deq_cnt(t);
    sprintf(constr_name, "%s_val[%d]", id.c_str(), t);
    constr_map.insert(named_constr(constr_name, constr_expr));
  }
}
