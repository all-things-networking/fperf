#include "search_test.hpp"
#include "iostream"
#include "params.hpp"
#include "query.hpp"
#include "rr_scheduler.hpp"
#include "search.hpp"
#include "tests.hpp"

using namespace std;

void test_search() {
  RRScheduler *rr = new RRScheduler(2, 10);

  Workload wl(100, 1, 10);
  rr->set_base_workload(wl);

  Query query(query_quant_t::FORALL, time_range_t(9, 9), "bar", metric_t::CENQ,
              comp_t::GE, 3);
  rr->set_query(query);

  IndexedExample *base_eg = new IndexedExample();
  qset_t target_queues;

  if (!rr->generate_base_example(base_eg, target_queues, 1))
    return;

  // Set shared config
  DistsParams dists_params;
  Dists *dists = new Dists(dists_params);
  SharedConfig *config = new SharedConfig(10, 2, target_queues, dists);
  rr->set_shared_config(config);

  deque<IndexedExample *> good_examples;
  rr->generate_good_examples2(base_eg, 5, good_examples);

  deque<IndexedExample *> bad_examples;
  rr->generate_bad_examples(5, bad_examples);

  Search search(rr, query, 10, config, good_examples, bad_examples);
  search.run();
}

SearchTest::SearchTest() {}
