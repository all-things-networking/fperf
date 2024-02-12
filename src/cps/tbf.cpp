#include "tbf.hpp"

TBF::TBF(unsigned int total_time, TBFInfo info): ContentionPoint(total_time), info(info) {
    init();
}

Queue* TBF::get_in_queue() {
    return get_in_queues()[0];
}

Queue* TBF::get_out_queue() {
    return out_queues[0];
}

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

    TBFQM* tbf_qm = new TBFQM(m_id, total_time, in_queue_info, out_queue_info, net_ctx, info);

    nodes.push_back(m_id);
    id_to_qm[m_id] = tbf_qm;
    qm = tbf_qm;
}

void TBF::add_edges() {
}

void TBF::add_metrics() {
    // CEnq
    CEnq* ce = new CEnq(get_in_queue(), total_time, net_ctx);
    cenq.push_back(ce);
    metrics[metric_t::CENQ][get_in_queue()->get_id()] = ce;
    get_in_queue()->add_metric(metric_t::CENQ, ce);

    // CDeq
    CDeq* cd = new CDeq(get_in_queue(), total_time, net_ctx);
    cdeq.push_back(cd);
    metrics[metric_t::CDEQ][get_in_queue()->get_id()] = cd;
    get_in_queue()->add_metric(metric_t::CDEQ, cd);

    // Deq
    Deq* d = new Deq(get_in_queue(), total_time, net_ctx);
    deq.push_back(d);
    metrics[metric_t::DEQ][get_in_queue()->get_id()] = d;
    get_in_queue()->add_metric(metric_t::DEQ, d);

    // DST
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        Queue* queue = in_queues[q];
        Dst* d = new Dst(queue, total_time, net_ctx);
        dst.push_back(d);
        metrics[metric_t::DST][queue->get_id()] = d;
        queue->add_metric(metric_t::DST, d);
    }

    // ECMP
    for (unsigned int q = 0; q < in_queues.size(); q++) {
        Queue* queue = in_queues[q];
        Ecmp* e = new Ecmp(queue, total_time, net_ctx);
        ecmp.push_back(e);
        metrics[metric_t::ECMP][queue->get_id()] = e;
        queue->add_metric(metric_t::ECMP, e);
    }
}

std::string TBF::cp_model_str(model& m, NetContext& net_ctx, unsigned int t) {
    (void) net_ctx;
    stringstream ss;
    ss << "Token Queue: " << m.eval(qm->token_queue[t]) << endl;
    return ss.str();
}