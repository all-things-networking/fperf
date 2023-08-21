#include "search_test.hpp"
#include "iostream"
#include "params.hpp"
#include "query.hpp"
#include "rr_scheduler.hpp"
#include "search.hpp"
#include "simple_cp.hpp"
#include "tests.hpp"

using namespace std;

bool test_search() {
  uint total_time = 10;

  SimpleCP *ss = new SimpleCP(total_time);

  Workload wl(100, 1, total_time);
  wl.add_wl_spec(TimedSpec(WlSpec(TONE(metric_t::CENQ, 0), comp_t::GE, TIME(1)),
                           time_range_t(0, total_time - 1), total_time));
  wl.add_wl_spec(TimedSpec(WlSpec(TONE(metric_t::CENQ, 0), comp_t::LE, TIME(3)),
                           time_range_t(0, total_time - 1), total_time));
  ss->set_base_workload(wl);

  // Query
  cid_t queue_id = ss->get_in_queue()->get_id();

  Query query(query_quant_t::FORALL, time_range_t(0, total_time - 1), queue_id,
              metric_t::CENQ, comp_t::GT, 1);

  ss->set_query(query);

  solver_res_t res = ss->satisfy_query();

  IndexedExample *base_eg = new IndexedExample();
  qset_t target_queues;

  if (!ss->generate_base_example(base_eg, target_queues, 1))
    return false;

  DistsParams dists_params;
  Dists *dists = new Dists(dists_params);
  SharedConfig *config = new SharedConfig(10, 1, target_queues, dists);

  if (!ss->set_shared_config(config))
    return false;

  deque<IndexedExample *> good_examples;
  ss->generate_good_examples2(base_eg, 5, good_examples);

  deque<IndexedExample *> bad_examples;
  ss->generate_bad_examples(5, bad_examples);

  Search search(ss, query, 10, config, good_examples, bad_examples);
  search.run();

  return true;
}

void SearchTest::add_to_runner(TestRunner *runner) {
  runner->add_test_case("search", test_search);
}
