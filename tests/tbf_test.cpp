#include "tbf_test.hpp"
#include "iostream"
#include "tbf.hpp"

/* r : link rate, T: total time
 * WL: cenq(t) > 4r
 * QR: cdeq(10) <= r*T
 * We want to ensure that the average dequeue count is at most r
 */
bool test_deq_avg() {

  unsigned int total_time = 10;
  unsigned int last_t = total_time - 1;

  TBFInfo info;
  info.link_rate = 4;
  info.max_tokens = 10;
  info.max_enq = 10;

  TBF *tbf = new TBF(total_time, info);

  // Base workload
  Workload wl(100, 1, total_time);

  wl.add_wl_spec(TimedSpec(WlSpec(TONE(metric_t::CENQ, 0), comp_t::GE, TIME(1)),
                           time_range_t(0, last_t), total_time));
  tbf->set_base_workload(wl);

  // Query
  cid_t queue_id = tbf->get_out_queue()->get_id();

  Query query(query_quant_t::FORALL, time_range_t(last_t, last_t), queue_id,
              metric_t::CDEQ, comp_t::GT, 40);

  tbf->set_query(query);

  return tbf->satisfy_query() == solver_res_t::UNSAT;
}

/*
 * Here we are enqueueing with higher than link_rate and check whether
 * burst is limited to the token queue size
 */
bool test_max_burst() {
  cout << "TBF" << endl;

  unsigned int total_time = 10;
  unsigned int last_t = total_time - 1;

  TBFInfo info;
  info.link_rate = 3;
  info.max_tokens = 5;
  info.max_enq = 10;

  TBF *tbf = new TBF(total_time, info);

  // Base workload
  Workload wl(100, 1, total_time);

  wl.add_wl_spec(
      TimedSpec(WlSpec(TONE(metric_t::CENQ, 0), comp_t::GE, (unsigned int)30),
                time_range_t(last_t, last_t), total_time));

  tbf->set_base_workload(wl);

  // Query
  cid_t queue_id = tbf->get_in_queue()->get_id();

  Query sat_query(query_quant_t::EXISTS, time_range_t(0, last_t), queue_id,
                  metric_t::DEQ, comp_t::GT, 4);
  tbf->set_query(sat_query);

  if (tbf->satisfy_query() != solver_res_t::SAT)
    return false;

  Query unsat_query(query_quant_t::EXISTS, time_range_t(0, last_t), queue_id,
                    metric_t::DEQ, comp_t::GT, 5);
  tbf->set_query(unsat_query);

  return tbf->satisfy_query() == solver_res_t::UNSAT;
}

bool test_burst() {
  cout << "TBF" << endl;

  unsigned int total_time = 10;
  unsigned int last_t = total_time - 1;

  TBFInfo info;
  info.link_rate = 3;
  info.max_tokens = 6;
  info.max_enq = 10;

  TBF *tbf = new TBF(total_time, info);

  // Base workload
  Workload wl(100, 1, total_time);

  wl.add_wl_spec(
      TimedSpec(WlSpec(TONE(metric_t::CENQ, 0), comp_t::GE, (unsigned int)6),
                time_range_t(last_t, last_t), total_time));

  tbf->set_base_workload(wl);

  // Query
  cid_t queue_id = tbf->get_in_queue()->get_id();

  Query sat_query(query_quant_t::EXISTS, time_range_t(0, last_t), queue_id,
                  metric_t::DEQ, comp_t::GE, 6);
  tbf->set_query(sat_query);

  auto res = tbf->satisfy_query();

  return tbf->satisfy_query() == solver_res_t::SAT;
}

void TBFTest::add_to_runner(TestRunner *runner) {
  runner->add_test_case("tbf_deq_avg", test_deq_avg);
  runner->add_test_case("tbf_max_burst", test_max_burst);
  runner->add_test_case("tbf_burst", test_burst);
}
