#include "rr_scheduler_test.hpp"
#include "search_test.hpp"
#include "test_runner.hpp"
#include <iostream>

using namespace std;

int main(int argc, char **argv) {

  vector<string> arguments(argv + 1, argv + argc);

  TestRunner runner;

  RRSchedulerTest rrSchedulerTest;
  rrSchedulerTest.add_to_runner(&runner);

  SearchTest searchTest;
  searchTest.add_to_runner(&runner);

  bool result = runner.run(arguments);

  cout << runner.get_report();

  return !result;
}
