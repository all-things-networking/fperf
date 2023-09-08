#include <iostream>

#include "rr_scheduler.hpp"
#include "rr_scheduler_test.hpp"

bool test_unsat_query() {
  unsigned int in_queue_cnt = 5;
  unsigned int period = 5;
  unsigned int recur = 2;

  unsigned int total_time = recur * period;

  RRScheduler *rr = new RRScheduler(in_queue_cnt, total_time);

  unsigned int queue1 = 1;
  unsigned int queue2 = 2;

  Workload wl(100, in_queue_cnt, total_time);

  for (unsigned int q = 0; q < in_queue_cnt; q++) {
    wl.add_wl_spec(
        TimedSpec(WlSpec(TONE(metric_t::CENQ, q), op_t::GE, TIME(1)),
                  time_range_t(0, total_time - 1), total_time));
  }

  wl.add_wl_spec(TimedSpec(WlSpec(TONE(metric_t::CENQ, queue1), op_t::GT,
                                  TONE(metric_t::CENQ, queue2)),
                           time_range_t(total_time - 1, total_time - 1),
                           total_time));

  std::cout << "base workload: " << std::endl << wl << std::endl;

  rr->set_base_workload(wl);

  // Query
  cid_t queue1_id = rr->get_in_queues()[queue1]->get_id();
  cid_t queue2_id = rr->get_in_queues()[queue2]->get_id();

  Query query(query_quant_t::FORALL,
              time_range_t(total_time - 1 - (period - 1), total_time - 1),
              qdiff_t(queue2_id, queue1_id), metric_t::CDEQ, op_t::GE, 3);

  rr->set_query(query);

  solver_res_t result = rr->satisfy_query();

  cout << result << endl;

  return result == solver_res_t::UNSAT;
}

bool test_sat_query() {
  unsigned int in_queue_cnt = 5;
  unsigned int period = 5;
  unsigned int recur = 2;
  unsigned int rate = 4;

  unsigned int total_time = recur * period;

  RRScheduler *rr = new RRScheduler(in_queue_cnt, total_time);

  unsigned int queue1 = 1;
  unsigned int queue2 = 2;

  Workload wl(100, in_queue_cnt, total_time);

  for (unsigned int i = 1; i <= recur; i++) {
    for (unsigned int q = 0; q < in_queue_cnt; q++) {
      wl.add_wl_spec(
          TimedSpec(WlSpec(TONE(metric_t::CENQ, q), op_t::GE, i * rate),
                    time_range_t(i * period - 1, i * period - 1), total_time));
    }
  }

  wl.add_wl_spec(TimedSpec(WlSpec(TONE(metric_t::CENQ, queue1), op_t::GT,
                                  TONE(metric_t::CENQ, queue2)),
                           time_range_t(total_time - 1, total_time - 1),
                           total_time));

  std::cout << "base workload: " << std::endl << wl << std::endl;

  rr->set_base_workload(wl);

  // Query
  cid_t queue1_id = rr->get_in_queues()[queue1]->get_id();
  cid_t queue2_id = rr->get_in_queues()[queue2]->get_id();

  Query query(query_quant_t::FORALL,
              time_range_t(total_time - 1 - (period - 1), total_time - 1),
              qdiff_t(queue2_id, queue1_id), metric_t::CDEQ, op_t::GE, 3);

  rr->set_query(query);

  solver_res_t result = rr->satisfy_query();

  cout << result << endl;

  return result == solver_res_t::SAT;
}

void RRSchedulerTest::add_to_runner(TestRunner *runner) {
  runner->add_test_case("rr_unsat", test_sat_query);
  runner->add_test_case("rr_sat", test_unsat_query);
}
