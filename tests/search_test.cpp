#include "search_test.hpp"
#include "iostream"
#include "params.hpp"
#include "query.hpp"
#include "rr_scheduler.hpp"
#include "search.hpp"
#include "simple_cp.hpp"
#include "tests.hpp"

bool test_search() {
  uint total_time = 5;
  uint last_t = total_time - 1;

  SimpleCP *ss = new SimpleCP(total_time);

  Workload wl(100, 1, total_time);
  auto comp_spec = std::make_shared<Comp>(Indiv(metric_t::CENQ, 0), op_t::LE, static_cast<uint>(1));
  wl.add_spec(TimedSpec(comp_spec, time_range_t(last_t, last_t), total_time));
  ss->set_base_workload(wl);

  // Query
  cid_t queue_id = ss->get_in_queue()->get_id();

  Query query(query_quant_t::FORALL, time_range_t(last_t, last_t), queue_id,
              metric_t::CDEQ, op_t::GE, 1);

  ss->set_query(query);

  ss->satisfy_query();

  IndexedExample *base_eg = new IndexedExample();
  qset_t target_queues;

  if (!ss->generate_base_example(base_eg, target_queues, 1))
    return false;

  DistsParams dists_params;
  dists_params.in_queue_cnt = ss->in_queue_cnt();
  dists_params.total_time = total_time;
  dists_params.pkt_meta1_val_max = 1;
  dists_params.pkt_meta2_val_max = 1;
  dists_params.random_seed = 7000;
  Dists *dists = new Dists(dists_params);
  SharedConfig *config = new SharedConfig(total_time, 1, target_queues, dists);

  if (!ss->set_shared_config(config))
    return false;

  deque<IndexedExample *> good_examples;
  ss->generate_good_examples2(base_eg, 50, good_examples);

  deque<IndexedExample *> bad_examples;
  ss->generate_bad_examples(50, bad_examples);

  Search search(ss, query, 10, config, good_examples, bad_examples);
  search.run();

  return true;
}

void SearchTest::add_to_runner(TestRunner *runner) {
  runner->add_test_case("search", test_search);
}
