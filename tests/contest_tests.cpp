#include "contest.h"
#include <string>

CONTEST_SUITE(contest_suite_test)
CONTEST_CASE(test_equality) 
{
  CONTEST_EQUAL(1, 1);
  CONTEST_EQUAL(1.0, 1.0);
  CONTEST_EQUAL("1", "1");
}

CONTEST_CASE(test_truth) 
{
  CONTEST_TRUE(1 == 1);
  CONTEST_TRUE(1.0 == 1.0);
  CONTEST_TRUE(std::string("1") == "1");
  CONTEST_TRUE(true);
  CONTEST_TRUE(!false);
}

int returns_int(int i) {
  return i;
}

int raises_exception() {
  throw "Some text.";
  return 99;
}

CONTEST_CASE(test_function_call_test) 
{
  CONTEST_EQUAL(returns_int(1), 1);
  try {
    raises_exception();
    CONTEST_TRUE(false);
  } catch (const char*) {}
}

CONTEST_SUITE_END
