#include "cdeq.hpp"
#include "cenq.hpp"
#include "aipg.hpp"
#include "contention_point.hpp"

class SimpleCP : public ContentionPoint {
public:
  SimpleCP(unsigned int total_time);
  Queue *get_in_queue();
  Queue *get_out_queue();

private:
  std::vector<CEnq *> cenq;
  std::vector<CDeq *> cdeq;
  std::vector<AIPG*> aipg;

  void add_nodes();
  void add_edges();
  void add_metrics();

  std::string cp_model_str(model &m, NetContext &net_ctx, unsigned int t);
};

class SimpleQM : public QueuingModule {
public:
  SimpleQM(cid_t id, unsigned int total_time, QueueInfo queue_info, NetContext &net_ctx);

  void add_constrs(NetContext &net_ctx, std::map<std::string, expr> &constr_map);
private:
  void add_proc_vars(NetContext &net_ctx);
};
