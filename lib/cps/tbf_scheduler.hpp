#include "contention_point.hpp"
#include "cenq.hpp"
#include "cdeq.hpp"
#include "qsize.hpp"
#include "tbf_qm.hpp"

class TBFScheduler : public ContentionPoint {
public:
    TBFScheduler(unsigned int total_time);
private:
    std::vector<CEnq*> cenq;
    std::vector<CDeq*> cdeq;

    void add_nodes();
    void add_edges();
    void add_metrics();

    TBFQM* qm;

    virtual std::string cp_model_str(model& m, NetContext& net_ctx, unsigned int t);
};

