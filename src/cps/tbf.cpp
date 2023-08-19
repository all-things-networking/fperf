#include "tbf.hpp"

TBF::TBF(unsigned int total_time, TBFInfo info)
    : ContentionPoint(total_time), info(info) {
  init();
}

Queue *TBF::get_in_queue() { return get_in_queues()[0]; }

Queue *TBF::get_out_queue() { return out_queues[0]; }

void TBF::add_nodes() {
  // add a rr qm
  QueueInfo in_queue_info;
  in_queue_info.size = MAX_QUEUE_SIZE;
  in_queue_info.max_enq = info.max_enq;
  in_queue_info.max_deq = MAX_QUEUE_SIZE;
  in_queue_info.type = queue_t::QUEUE;

  QueueInfo out_queue_info;
  out_queue_info.size = MAX_QUEUE_SIZE;
  out_queue_info.max_enq = MAX_QUEUE_SIZE;
  out_queue_info.max_deq = MAX_QUEUE_SIZE;
  out_queue_info.type = queue_t::QUEUE;

  cid_t m_id = "TBF";

  TBFQM *tbf_qm =
      new TBFQM(m_id, total_time, in_queue_info, out_queue_info, net_ctx, info);

  nodes.push_back(m_id);
  id_to_qm[m_id] = tbf_qm;
  qm = tbf_qm;
}

void TBF::add_edges() {}

void TBF::add_metrics() {
  // CEnq
  CEnq *ce = new CEnq(get_in_queue(), total_time, net_ctx);
  cenq.push_back(ce);
  metrics[metric_t::CENQ][get_in_queue()->get_id()] = ce;
  get_in_queue()->add_metric(metric_t::CENQ, ce);

  // CDeq
  CDeq *cd = new CDeq(get_out_queue(), total_time, net_ctx);
  cdeq.push_back(cd);
  metrics[metric_t::CDEQ][get_out_queue()->get_id()] = cd;
  get_out_queue()->add_metric(metric_t::CDEQ, cd);

  // Deq
  Deq *d = new Deq(get_in_queue(), total_time, net_ctx);
  deq.push_back(d);
  metrics[metric_t::DEQ][get_in_queue()->get_id()] = d;
  get_in_queue()->add_metric(metric_t::DEQ, d);
}

std::string TBF::cp_model_str(model &m, NetContext &net_ctx, unsigned int t) {
  stringstream ss;
  ss << "Token Queue: " << m.eval(qm->token_queue[t]) << endl;
  return ss.str();
}