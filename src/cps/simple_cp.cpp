#include "simple_cp.hpp"
#include "simple_qm.hpp"

#include <sstream>

SimpleCP::SimpleCP(unsigned int total_time) : ContentionPoint(total_time) {
  init();
}

void SimpleCP::add_nodes() {
  // add a rr qm
  QueueInfo info;
  info.size = MAX_QUEUE_SIZE;
  info.max_enq = MAX_ENQ;
  info.max_deq = 1;

  cid_t m_id = "RR";

  SimpleQM *rr_qm = new SimpleQM(m_id, total_time, vector<QueueInfo>(queue_cnt, info),
                         info, net_ctx);

  nodes.push_back(m_id);
  id_to_qm[m_id] = rr_qm;
}

void SimpleCP::add_edges() {}

void SimpleCP::add_metrics() {}

std::string SimpleCP::cp_model_str(model &m, NetContext &net_ctx,
                                   unsigned int t) {}
