#include "queuing_module.hpp"

class SimpleQM : public QueuingModule {
public:
    SimpleQM(cid_t id,
         unsigned int total_time,
         std::vector<QueueInfo> in_queue_info,
         QueueInfo out_queue_info,
         NetContext& net_ctx);

    void add_constrs(NetContext& net_ctx,
                     std::map<std::string, expr>& constr_map);

private:
    void add_proc_vars(NetContext& net_ctx);
};
