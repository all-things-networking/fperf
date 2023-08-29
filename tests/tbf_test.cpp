#include "tbf_test.hpp"
#include "iostream"
#include "search.hpp"
#include "tbf.hpp"

/* r : link rate, T: total time
 * WL: cenq(t) >= r * t
 * QR: cdeq(T) > r * T
 *
 * We want to ensure that the average dequeue count is at most r
 */
bool test_deq_avg() {

  unsigned int link_rate = 4;
  unsigned int total_time = 10;
  unsigned int last_t = total_time - 1;

  TBFInfo info;
  info.link_rate = link_rate;
  info.max_tokens = 10;
  info.max_enq = 10;

  TBF *tbf = new TBF(total_time, info);

  Workload wl(100, 1, total_time);

  wl.add_wl_spec(TimedSpec(WlSpec(TONE(metric_t::CENQ, 0), comp_t::GE, TIME(4)),
                           time_range_t(0, last_t), total_time));
  tbf->set_base_workload(wl);

  cid_t queue_id = tbf->get_in_queue()->get_id();

  Query sat_query(query_quant_t::FORALL, time_range_t(last_t, last_t), queue_id,
                  metric_t::CDEQ, comp_t::EQ, last_t * link_rate);

  tbf->set_query(sat_query);

  if (tbf->satisfy_query() != solver_res_t::SAT)
    return false;

  Query unsat_query(query_quant_t::FORALL, time_range_t(last_t, last_t),
                    queue_id, metric_t::CDEQ, comp_t::GT, last_t * link_rate);

  tbf->set_query(unsat_query);

  return tbf->satisfy_query() == solver_res_t::UNSAT;
}

/* m: token bucket size, r: link rate, T: total time
 * WL: cenq(T) > r * T
 * QR: exists t: deq(t) > m
 *
 * In average we are enqueueing more than the token bucket size, and we want to
 * ensure that bursts do not exceed the token bucket size
 */
bool test_max_burst() {
  unsigned int link_rate = 3;
  unsigned int max_tokens = 5;
  unsigned int total_time = 10;
  unsigned int last_t = total_time - 1;

  TBFInfo info;
  info.link_rate = link_rate;
  info.max_tokens = max_tokens;
  info.max_enq = 10;

  TBF *tbf = new TBF(total_time, info);

  Workload wl(100, 1, total_time);

  wl.add_wl_spec(TimedSpec(WlSpec(TONE(metric_t::CENQ, 0), comp_t::GT,
                                  (unsigned int)total_time * max_tokens),
                           time_range_t(last_t, last_t), total_time));

  tbf->set_base_workload(wl);

  cid_t queue_id = tbf->get_in_queue()->get_id();

  Query sat_query(query_quant_t::EXISTS, time_range_t(0, last_t), queue_id,
                  metric_t::DEQ, comp_t::EQ, max_tokens);
  tbf->set_query(sat_query);

  if (tbf->satisfy_query() != solver_res_t::SAT)
    return false;

  Query unsat_query(query_quant_t::EXISTS, time_range_t(0, last_t), queue_id,
                    metric_t::DEQ, comp_t::GT, max_tokens);
  tbf->set_query(unsat_query);

  return tbf->satisfy_query() == solver_res_t::UNSAT;
}

bool test_search() {
  unsigned int link_rate = 4;
  unsigned int total_time = 8;
  unsigned int last_t = total_time - 1;

  TBFInfo info;
  info.link_rate = link_rate;
  info.max_tokens = 6;
  info.max_enq = 10;

  TBF *tbf = new TBF(total_time, info);

  Workload wl(100, 1, total_time);

  wl.add_wl_spec(TimedSpec(WlSpec(TONE(metric_t::CENQ, 0), comp_t::EQ, (unsigned int) 0),
                           time_range_t(0, 1), total_time));
  wl.add_wl_spec(TimedSpec(WlSpec(TONE(metric_t::CENQ, 0), comp_t::EQ, (unsigned int) link_rate * 3),
                           time_range_t(4, 4), total_time));
  tbf->set_base_workload(wl);

  cid_t queue_id = tbf->get_in_queue()->get_id();

  Query sat_query(query_quant_t::EXISTS, time_range_t(0, last_t), queue_id,
                  metric_t::DEQ, comp_t::GT, link_rate);

  tbf->set_query(sat_query);

  tbf->satisfy_query();

//  return true;

  IndexedExample *base_eg = new IndexedExample();
  qset_t target_queues;

  bool res = tbf->generate_base_example(base_eg, target_queues, 1);

  if (!res) {
    cout << "ERROR: couldn't generate base example" << endl;
    return false;
  }

  // Set shared config
  DistsParams dists_params;
  dists_params.in_queue_cnt = tbf->in_queue_cnt();
  dists_params.total_time = total_time;
  dists_params.pkt_meta1_val_max = 2;
  dists_params.pkt_meta2_val_max = 2;

  Dists *dists = new Dists(dists_params);
  SharedConfig *config =
      new SharedConfig(total_time, tbf->in_queue_cnt(), target_queues, dists);
  bool config_set = tbf->set_shared_config(config);
  if (!config_set)
    return false;

  deque<IndexedExample *> good_examples;
  tbf->generate_good_examples2(base_eg, 50, good_examples);

  deque<IndexedExample *> bad_examples;
  tbf->generate_bad_examples(50, bad_examples);

  Search search(tbf, sat_query, 10, config, good_examples, bad_examples);

  search.run();

  return true;
}

void TBFTest::add_to_runner(TestRunner *runner) {
  runner->add_test_case("tbf_deq_avg", test_deq_avg);
  runner->add_test_case("tbf_max_burst", test_max_burst);
//  runner->add_test_case("tbf_search", test_search);
}
