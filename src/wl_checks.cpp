//
// Created by Amir Hossein Seyhani on 12/15/25.
//

#include "buggy_2l_rr_scheduler.hpp"
#include "gen/utils.hpp"
#include "gen/wl_parser.hpp"
#include "loom_mqprio.hpp"
#include "priority_scheduler.hpp"
#include "rr_scheduler.hpp"

void print_vector(vector<string> wl_lines) {
    for (string line : wl_lines)
        cout << line << endl;
}


void read_wl(ContentionPoint* cp, string input_file) {
    vector<vector<string>> wls = read_wl_file(input_file);
    for (int i = 0; i < wls.size(); ++i) {
        WorkloadParser parser(cp->in_queue_cnt(), cp->total_time);
        auto wl_lines = wls[i];
        string res_stat = wl_lines[0];
        cout << "WL: " << i + 1 << "/" << wls.size() << " " << res_stat << endl;
        wl_lines.erase(wl_lines.begin());
        parser.parse(wl_lines);
        Workload* wl = parser.wl;
        cout << "Input workload:" << endl;
        print_vector(wl_lines);
        cout << "Parsed workload:" << endl;
        cout << wl << endl;

        auto res = cp->check_wl_and_not_query(*wl);
        if (res_stat == "SAT") {
            assert(res == solver_res_t::SAT);
        } else {
            assert(res == solver_res_t::UNSAT);
        }
    }
}

void check_prio(int buf_size, string input_file) {
    unsigned int prio_levels = 4;
    unsigned int query_thresh = 5;

    unsigned int good_example_cnt = 50;
    unsigned int bad_example_cnt = 50;
    unsigned int total_time = 7;

    PrioScheduler* prio = new PrioScheduler(prio_levels, total_time, buf_size);

    cid_t query_qid = prio->get_in_queues()[2]->get_id();
    Query query(query_quant_t::EXISTS,
                time_range_t(0, prio->get_total_time() - 1),
                query_qid,
                metric_t::CBLOCKED,
                Op(Op::Type::GT),
                query_thresh);
    prio->set_query(query);

    read_wl(prio, input_file);
}

void check_rr(int buf_size, string input_file) {
    cout << "rr" << endl;
    time_typ start_time = noww();

    unsigned int in_queue_cnt = 5;
    unsigned int period = 5;
    unsigned int recur = 2;
    unsigned int rate = 4;

    unsigned int good_example_cnt = 25;
    unsigned int bad_example_cnt = 25;
    unsigned int total_time = recur * period;

    // Create contention point
    RRScheduler* rr = new RRScheduler(in_queue_cnt, total_time, buf_size);

    unsigned int queue1 = 1;
    unsigned int queue2 = 2;

    // Base workload
    Workload wl(100, in_queue_cnt, total_time);

    for (unsigned int i = 1; i <= recur; i++) {
        for (unsigned int q = 0; q < in_queue_cnt; q++) {
            wl.add_spec(TimedSpec(
                new Comp(new Indiv(metric_t::CENQ, q), Op(Op::Type::GE), new Constant(i * rate)),
                time_range_t(i * period - 1, i * period - 1),
                total_time));
        }
    }

    wl.add_spec(TimedSpec(new Comp(new Indiv(metric_t::CENQ, queue1),
                                   Op(Op::Type::GT),
                                   new Indiv(metric_t::CENQ, queue2)),
                          time_range_t(total_time - 1, total_time - 1),
                          total_time));

    cout << "base workload: " << endl << wl << endl;

    rr->set_base_workload(wl);

    // Query
    cid_t queue1_id = rr->get_in_queues()[queue1]->get_id();
    cid_t queue2_id = rr->get_in_queues()[queue2]->get_id();

    Query query(query_quant_t::FORALL,
                time_range_t(total_time - 1 - (period - 1), total_time - 1),
                qdiff_t(queue2_id, queue1_id),
                metric_t::CDEQ,
                Op(Op::Type::GE),
                3);

    rr->set_query(query);

    read_wl(rr, input_file);
}

void check_fq(int buf_size, string input_file) {

    cout << "fq_codel" << endl;
    time_typ start_time = noww();

    unsigned int in_queue_cnt = 5;
    unsigned int total_time = 14;
    unsigned int query_thresh = (total_time / in_queue_cnt) + 3;
    unsigned int last_queue = in_queue_cnt - 1;
    cout << "QUERY TRESH:" << query_thresh << endl;

    unsigned int good_example_cnt = 50;
    unsigned int bad_example_cnt = 50;

    // Create contention point
    Buggy2LRRScheduler* cp = new Buggy2LRRScheduler(in_queue_cnt, total_time, buf_size);

    // Base Workload
    Workload wl(in_queue_cnt * 5, in_queue_cnt, total_time);
    for (unsigned int q = 0; q < last_queue; q++) {
        wl.add_spec(TimedSpec(new Comp(new Indiv(metric_t::CENQ, q), Op(Op::Type::GE), new Time(1)),
                              total_time,
                              total_time));
    }

    cp->set_base_workload(wl);

    // Query
    cid_t query_qid = cp->get_in_queues()[last_queue]->get_id();
    Query query(query_quant_t::FORALL,
                time_range_t(total_time - 1, total_time - 1),
                query_qid,
                metric_t::CDEQ,
                Op(Op::Type::GE),
                query_thresh);

    cp->set_query(query);

    read_wl(cp, input_file);
}

void check_loom(int buf_size, string input_file) {

    cout << "loom" << endl;
    time_typ start_time = noww();

    unsigned int nic_tx_queue_cnt = 4;
    unsigned int per_core_flow_cnt = 3;
    unsigned int query_time = 3;

    unsigned int good_example_cnt = 50;
    unsigned int bad_example_cnt = 50;
    unsigned int total_time = 10;

    // Create contention point
    LoomMQPrio* cp = new LoomMQPrio(nic_tx_queue_cnt, per_core_flow_cnt, total_time, buf_size, 500);


    qset_t tenant1_qset;
    qset_t tenant2_qset;

    for (unsigned int i = 0; i < cp->in_queue_cnt(); i++) {
        if (i % 3 == 0)
            tenant1_qset.insert(i);
        else
            tenant2_qset.insert(i);
    }

    // Base Workload

    Workload wl(20, cp->in_queue_cnt(), total_time);
    wl.add_spec(
        TimedSpec(new Comp(new QSum(tenant1_qset, metric_t::CENQ), Op(Op::Type::GE), new Time(1)),
                  total_time,
                  total_time));
    wl.add_spec(
        TimedSpec(new Comp(new QSum(tenant2_qset, metric_t::CENQ), Op(Op::Type::GE), new Time(1)),
                  total_time,
                  total_time));


    cp->set_base_workload(wl);

    // Query
    Query query(query_quant_t::FORALL,
                time_range_t(total_time - 1 - query_time, total_time - 1),
                qdiff_t(cp->get_out_queue(1)->get_id(), cp->get_out_queue(0)->get_id()),
                metric_t::CENQ,
                Op(Op::Type::GT),
                3u);

    cp->set_query(query);

    cout << "cp setup: " << (get_diff_millisec(start_time, noww()) / 1000.0) << " s" << endl;

    read_wl(cp, input_file);
}
