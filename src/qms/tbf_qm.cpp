#include "tbf_qm.hpp"
#include <format>

int TOKEN_QUEUE_SIZE = 50;
int LINK_RATE = 5;

using namespace std;

TBFQM::TBFQM(cid_t id, unsigned int total_time,
             std::vector<QueueInfo> in_queue_info, QueueInfo out_queue_info,
             NetContext &net_ctx)
    : QueuingModule(id, total_time, in_queue_info,
                    std::vector<QueueInfo>{out_queue_info}, net_ctx) {
  init(net_ctx);
}

template <typename... Args>
string format_string(const string &format, Args... args) {
  char vname[100];
  std::snprintf(vname, 100, format.c_str(), args...);
  string s(vname);
  return s;
}

void TBFQM::add_proc_vars(NetContext &net_ctx) {
  for (unsigned int t = 0; t < total_time; t++) {
    string vname = format_string("%s_token_queue[%d]", id.c_str(), t);
    token_queue.push_back(net_ctx.int_const(vname.data()));
  }
}

void TBFQM::add_constrs(NetContext &net_ctx,
                        std::map<std::string, expr> &constr_map) {
  Queue *in_queue = in_queues[0];
  Queue *out_queue = out_queues[0];

  for (unsigned int t = 1; t < total_time; t++) {
    for (unsigned int i = 0; i <= in_queue->size(); i++) {
      expr is_last_null_packet = net_ctx.bool_val(true);
      if (i > 0)
        is_last_null_packet =
            is_last_null_packet && net_ctx.pkt2val(in_queue->elem(i - 1)[t]);
      if (i < in_queue->size())
        is_last_null_packet =
            is_last_null_packet && !net_ctx.pkt2val(in_queue->elem(i)[t]);
      string constr_name =
          format_string("%s_deq_count_%d_%d", id.c_str(), t, i);
      expr constr_expr = implies(is_last_null_packet,
                                 in_queue->deq_cnt(t) ==
                                     min(net_ctx.int_val(i), token_queue[t]));
      constr_map.insert(named_constr(constr_name, constr_expr));
    }
  }

  for (unsigned int t = 0; t < total_time; t++) {
    string constr_name = format_string("%s_tokens_count_%d", id.c_str(), t);
    expr constr_expr = net_ctx.bool_val(true);
    if (t == 0)
      constr_expr = token_queue[t] == 0;
    else
      constr_expr =
          token_queue[t] ==
          max(net_ctx.int_val(0), min(net_ctx.int_val(TOKEN_QUEUE_SIZE),
                                      token_queue[t - 1] + (int)LINK_RATE -
                                          in_queue->deq_cnt(t - 1)));
    constr_map.insert(named_constr(constr_name, constr_expr));
  }

  for (unsigned int t = 0; t < total_time; t++) {
    for (int i = 0; i < out_queue->max_enq(); ++i) {
      string constr_name =
          format_string("%s_output_from_%d_%d", id.c_str(), i, t);
      expr constr_expr = implies(in_queue->deq_cnt(t) >= i + 1,
                                 out_queue->enqs(i)[t] == in_queue->elem(i)[t]);
      constr_map.insert(named_constr(constr_name, constr_expr));
    }
  }
}
