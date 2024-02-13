#include "cdeq.hpp"
#include "cenq.hpp"
#include "contention_point.hpp"
#include "deq.hpp"
#include "dst.hpp"
#include "ecmp.hpp"
#include "aipg.hpp"
#include "qsize.hpp"
#include "tbf_qm.hpp"

class TBF : public ContentionPoint {
public:
    TBF(unsigned int total_time, TBFInfo info);
    Queue* get_in_queue();
    Queue* get_out_queue();

private:
    std::vector<CEnq*> cenq;
    std::vector<CDeq*> cdeq;
    std::vector<Deq*> deq;
    std::vector<Dst*> dst;
    std::vector<Ecmp*> ecmp;
    std::vector<AIPG*> aipg;

    void add_nodes();
    void add_edges();
    void add_metrics();

    TBFQM* qm;
    TBFInfo info;

    virtual std::string cp_model_str(model& m, NetContext& net_ctx, unsigned int t);
};
