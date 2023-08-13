#ifndef test_runner_hpp
#define test_runner_hpp

#include "vector"

typedef bool test_case_t();

using namespace std;

class TestRunner {
public:
  TestRunner();

  void add_test_case(test_case_t *test_case);

  bool run();

private:
  vector<test_case_t *> test_cases;
};

#endif