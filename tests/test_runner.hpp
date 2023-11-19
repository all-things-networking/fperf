#ifndef test_runner_hpp
#define test_runner_hpp

#include "map"
#include "string"
#include "vector"

using namespace std;

typedef bool test_case_t();

class TestRunner {
public:
  TestRunner();

  void add_test_case(string name,test_case_t *test_case);

  bool run(vector<string> test_names);

  string get_report();

private:
  map<string, test_case_t *> test_cases;
  map<string, bool> results;
};

#endif