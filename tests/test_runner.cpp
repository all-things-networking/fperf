#include "test_runner.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

#define DASHES "---------------"

using namespace std;

TestRunner::TestRunner() {
  test_cases = map<string, test_case_t *>();
  results = map<string, bool>();
}

bool test_name_match(vector<string> test_names, string name) {
  if (test_names.empty())
    return true;
  else
    return find(test_names.begin(), test_names.end(), name) != test_names.end();
}

bool TestRunner::run(vector<string> test_names) {
  bool overall_result = true;
  for (auto entry : test_cases) {
    if (test_name_match(test_names, entry.first)) {
      cout << "Running: " << entry.first << endl;
      bool result = entry.second();
      results.insert(pair(entry.first, result));
      overall_result &= result;
    }
  }
  return overall_result;
}

void TestRunner::add_test_case(string name, test_case_t *test_case) {
  if (test_cases.find(name) != test_cases.end())
    throw runtime_error("Test case with the same name already exists:" + name);
  test_cases.insert(pair(name, test_case));
}

string TestRunner::get_report() {
  stringstream ss;
  ss << endl << DASHES << " Test Results " << DASHES << endl << endl;
  for (auto entry : results)
    ss << entry.first << ": " << (entry.second ? "Pass" : "Fail") << endl;
  ss << endl << DASHES << DASHES << DASHES << endl;
  return ss.str();
}