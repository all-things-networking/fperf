#include "rr_scheduler_test.hpp"
#include "test_runner.hpp"
#include <iostream>

int main() {
  TestRunner runner;
  RRSchedulerTest rrSchedulerTest;
  rrSchedulerTest.add_to_runner(&runner);

  if (!runner.run()) {
    cout << "Failed" << endl;
    return 1;
  }
  return 0;
}
