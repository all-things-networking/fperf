#include "rr_scheduler_test.hpp"
#include "tbf_test.hpp"
#include "test_runner.hpp"
#include <iostream>

using namespace std;

int main(int argc, char **argv) {

  vector<string> arguments(argv + 1, argv + argc);
  for (string test : arguments)
    cout << test << ", ";
  cout << endl;

  TestRunner runner;

  RRSchedulerTest rrSchedulerTest;
  rrSchedulerTest.add_to_runner(&runner);

  TBFTest tbfTest;
  tbfTest.add_to_runner(&runner);

  bool result = runner.run(arguments);

  cout << runner.get_report();

  return !result;
}
