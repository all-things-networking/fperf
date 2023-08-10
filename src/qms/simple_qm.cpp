#include "simple_qm.hpp"

SimpleQM::SimpleQM(cid_t id,
           unsigned int total_time,
           std::vector<QueueInfo> in_queue_info,
           QueueInfo out_queue_info,
           NetContext& net_ctx):
        QueuingModule(id, total_time, in_queue_info,
                      std::vector<QueueInfo>{out_queue_info},
                      net_ctx)
{
    init(net_ctx);
}

void SimpleQM::add_proc_vars(NetContext& net_ctx){
}

void SimpleQM::add_constrs(NetContext& net_ctx,
                       std::map<std::string, expr>& constr_map){
}

