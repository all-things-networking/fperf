#ifndef TEST_CLASS_HPP
#define TEST_CLASS_HPP

#include "test_runner.hpp"

class TestClass {
public:
  virtual void add_to_runner(TestRunner * runner) = 0;
};

#endif