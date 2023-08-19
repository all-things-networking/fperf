#include "test_runner.hpp"
#include <iostream>

using namespace std;

TestRunner::TestRunner() { test_cases = map<string, test_case_t *>(); }

bool test_name_match(vector<string> test_names, string name) {
  if (test_names.empty())
    return true;
  else
    return find(test_names.begin(), test_names.end(), name) != test_names.end();
}

bool TestRunner::run(vector<string> test_names) {
  for (auto entry : test_cases) {
    if (test_name_match(test_names, entry.first)) {
      cout << "Running: " << entry.first << endl;
      if (!entry.second())
        return false;
    }
  }
  return true;
}

void TestRunner::add_test_case(string name, test_case_t *test_case) {
  if (test_cases.find(name) != test_cases.end())
    throw runtime_error("Test case with the same name already exists:" + name);
  test_cases.insert(pair(name, test_case));
}