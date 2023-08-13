#include "test_runner.hpp"

TestRunner::TestRunner() { test_cases = vector<test_case_t *>(); }

bool TestRunner::run() {
  for (test_case_t *test_case : test_cases)
    if (!test_case())
      return false;
  return true;
}

void TestRunner::add_test_case(test_case_t *test_case) {
  test_cases.push_back(test_case);
}