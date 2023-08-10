#include "contention_point.hpp"

class SimpleCP : public ContentionPoint {
public:
  SimpleCP(unsigned int total_time);

private:
  unsigned int queue_cnt;

  void add_nodes();
  void add_edges();
  void add_metrics();

  std::string cp_model_str(model &m, NetContext &net_ctx, unsigned int t);
};

