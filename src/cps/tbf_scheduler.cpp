#include "tbf_scheduler.hpp"

TBFScheduler::TBFScheduler(unsigned int total_time)
    : ContentionPoint(total_time) {
  init();
}

void TBFScheduler::add_nodes() {
  // add a rr qm
  QueueInfo info;
  info.size = MAX_QUEUE_SIZE;
  info.max_enq = MAX_ENQ;
  info.max_deq = MAX_QUEUE_SIZE;
  info.type = queue_t::QUEUE;

  cid_t m_id = "TBF";

  TBFQM *tbf_qm =
      new TBFQM(m_id, total_time, vector<QueueInfo>(1, info), info, net_ctx);

  nodes.push_back(m_id);
  id_to_qm[m_id] = tbf_qm;
  qm = tbf_qm;
}

void TBFScheduler::add_edges() {}

void TBFScheduler::add_metrics() {
  // CEnq
  for (unsigned int q = 0; q < in_queues.size(); q++) {
    Queue *queue = in_queues[q];
    CEnq *ce = new CEnq(queue, total_time, net_ctx);
    cenq.push_back(ce);
    metrics[metric_t::CENQ][queue->get_id()] = ce;
    queue->add_metric(metric_t::CENQ, ce);
  }

  // CDeq
  for (unsigned int q = 0; q < out_queues.size(); q++) {
    Queue *queue = out_queues[q];
    CDeq *cd = new CDeq(queue, total_time, net_ctx);
    cdeq.push_back(cd);
    metrics[metric_t::CDEQ][queue->get_id()] = cd;
    queue->add_metric(metric_t::CDEQ, cd);
  }
}

std::string TBFScheduler::cp_model_str(model &m, NetContext &net_ctx,
                                       unsigned int t) {
  stringstream ss;
  ss << "Token Queue: " << m.eval(qm->token_queue[t]) << endl;
  return ss.str();
}