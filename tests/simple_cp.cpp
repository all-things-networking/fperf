#include "simple_cp.hpp"

#include "rr_qm.hpp"

#include <sstream>

SimpleCP::SimpleCP(unsigned int total_time) : ContentionPoint(total_time) {
  init();
}

void SimpleCP::add_nodes() {
  QueueInfo info;
  info.size = MAX_QUEUE_SIZE;
  info.max_enq = 4;
  info.max_deq = 2;
  info.type = queue_t::QUEUE;

  cid_t m_id = "SimpleQM";

  SimpleQM *qm = new SimpleQM(m_id, total_time, info, net_ctx);

  nodes.push_back(m_id);
  id_to_qm[m_id] = qm;
}

Queue *SimpleCP::get_in_queue() { return in_queues[0]; }

Queue *SimpleCP::get_out_queue() { return out_queues[0]; }

void SimpleCP::add_edges() {}

void SimpleCP::add_metrics() {
  for (unsigned int q = 0; q < in_queues.size(); q++) {
    Queue *queue = in_queues[q];
    CEnq *ce = new CEnq(queue, total_time, net_ctx);
    cenq.push_back(ce);
    metrics[metric_t::CENQ][queue->get_id()] = ce;
    queue->add_metric(metric_t::CENQ, ce);
  }

  for (unsigned int q = 0; q < in_queues.size(); q++) {
    Queue *queue = in_queues[q];
    CDeq *cd = new CDeq(queue, total_time, net_ctx);
    cdeq.push_back(cd);
    metrics[metric_t::CDEQ][queue->get_id()] = cd;
    queue->add_metric(metric_t::CDEQ, cd);
  }

  for (unsigned int q = 0; q < in_queues.size(); q++){
      Queue* queue = in_queues[q];
      AIPG* g = new AIPG(queue, total_time, net_ctx);
      aipg.push_back(g);
      metrics[metric_t::AIPG][queue->get_id()] = g;
      queue->add_metric(metric_t::AIPG, g);
  }
}

string SimpleCP::cp_model_str(model &m, NetContext &net_ctx,
                                   unsigned int t) {
  (void) m;
    (void) net_ctx;
    (void) t;
  return "";
}

SimpleQM::SimpleQM(cid_t id, unsigned int total_time, QueueInfo queue_info,
                   NetContext &net_ctx)
    : QueuingModule(id, total_time, vector<QueueInfo>{queue_info},
                    vector<QueueInfo>{queue_info}, net_ctx) {
  init(net_ctx);
}

void SimpleQM::add_proc_vars(NetContext &net_ctx) { (void) net_ctx; }

void SimpleQM::add_constrs(NetContext &net_ctx,
                           map<string, expr> &constr_map) {

  Queue *in_queue = in_queues[0];
  Queue *out_queue = out_queues[0];

  for (unsigned int t = 0; t < total_time; t++) {
    string constr_name = format_string("%s_deq_count_%d", id.c_str(), t);
    expr constr_expr = net_ctx.bool_val(true);
    if (t == 0)
      constr_expr = in_queue->deq_cnt(t) == 0;
    else
      constr_expr =
          in_queue->deq_cnt(t) == ite(net_ctx.pkt2val(in_queue->elem(0)[t]),
                                      net_ctx.int_val(1), net_ctx.int_val(0));
    constr_map.insert(named_constr(constr_name, constr_expr));
  }

  for (unsigned int t = 0; t < total_time; t++) {
    for (unsigned int i = 0; i < out_queue->max_enq(); i++) {
      string constr_name =
          format_string("%s_output_from_%d_%d", id.c_str(), t, i);
      expr constr_expr =
          out_queue->enqs(i)[t] ==
          ((t > 0 && i == 0) ? in_queue->elem(i)[t] : net_ctx.null_pkt());
      constr_map.insert(named_constr(constr_name, constr_expr));
    }
  }
}
