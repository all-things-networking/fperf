#ifndef test_runner_hpp
#define test_runner_hpp

#include "map"
#include "string"
#include "vector"

typedef bool test_case_t();

class TestRunner {
public:
  TestRunner();

  void add_test_case(std::string name,test_case_t *test_case);

  bool run(std::vector<std::string> test_names);

  std::string get_report();

private:
  std::map<std::string, test_case_t *> test_cases;
  std::map<std::string, bool> results;
};

#endif