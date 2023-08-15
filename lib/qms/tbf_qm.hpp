#include "queuing_module.hpp"

struct TBFInfo {
  unsigned int max_tokens;
  unsigned int link_rate;
};

class TBFQM : public QueuingModule {
public:
  TBFQM(cid_t id, unsigned int total_time, QueueInfo in_queue_info,
        QueueInfo out_queue_info, NetContext &net_ctx, TBFInfo info);

  void add_constrs(NetContext &net_ctx,
                   std::map<std::string, expr> &constr_map);
  vector<expr> token_queue;

private:
  TBFInfo info;
  void add_proc_vars(NetContext &net_ctx);
};
