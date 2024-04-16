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

  wl.add_spec(TimedSpec(new Comp(new Indiv(metric_t::CENQ, 0), Op(Op::Type::GE), new Time(4)), time_range_t(0, last_t), total_time));

  tbf->set_base_workload(wl);

  cid_t queue_id = tbf->get_in_queue()->get_id();

  Query sat_query(query_quant_t::FORALL, time_range_t(last_t, last_t), queue_id,
                  metric_t::CDEQ, Op(Op::Type::EQ), last_t * link_rate);

  tbf->set_query(sat_query);

  if (tbf->satisfy_query() != solver_res_t::SAT)
    return false;

  Query unsat_query(query_quant_t::FORALL, time_range_t(last_t, last_t),
                    queue_id, metric_t::CDEQ, Op(Op::Type::GT), last_t * link_rate);

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

  wl.add_spec(TimedSpec(new Comp(new Indiv(metric_t::CENQ, 0), Op(Op::Type::GT), new Constant((unsigned int)total_time * max_tokens)), time_range_t(last_t, last_t), total_time));

  tbf->set_base_workload(wl);

  cid_t queue_id = tbf->get_in_queue()->get_id();

  Query sat_query(query_quant_t::EXISTS, time_range_t(0, last_t), queue_id,
                  metric_t::DEQ, Op(Op::Type::EQ), max_tokens);
  tbf->set_query(sat_query);

  if (tbf->satisfy_query() != solver_res_t::SAT)
    return false;

  Query unsat_query(query_quant_t::EXISTS, time_range_t(0, last_t), queue_id,
                    metric_t::DEQ, Op(Op::Type::GT), max_tokens);
  tbf->set_query(unsat_query);

  return tbf->satisfy_query() == solver_res_t::UNSAT;
}

void TBFTest::add_to_runner(TestRunner *runner) {
  runner->add_test_case("tbf_deq_avg", test_deq_avg);
  runner->add_test_case("tbf_max_burst", test_max_burst);
}
