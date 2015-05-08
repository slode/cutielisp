#include "contest.h"
#include "template.h"

CONTEST_SUITE(template_suite_test)
CONTEST_CASE(test_function_call_test) 
{
  CONTEST_TRUE(true);
}
CONTEST_CASE(test_template_function) 
{

  CONTEST_EQUAL(template_function(), 2);
}

CONTEST_SUITE_END
