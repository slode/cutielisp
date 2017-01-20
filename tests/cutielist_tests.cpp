#include "contest.h"

extern "C"
{
#include "cutie.h"
}

CONTEST_SUITE(cutie_suite_test)
CONTEST_CASE(test_parser) 
{
  std::vector<std::string> programs;
  programs.push_back("(define a 10)");
  programs.push_back("(set! a 10)");
  programs.push_back("(1 ' set! 10) 10)");
  programs.push_back(")£$(1 ' set! 10) 10)");

  for (std::string ps : programs) {
    const char *p = ps.c_str();
    Error err;
    Atom sexpr;
    err = read_expr((const char*)p, (const char**)&p, &sexpr);
    CONTEST_EQUAL(err.type, Error::Error_OK);
  }
}

CONTEST_CASE(test_eval) 
{
}
CONTEST_SUITE_END
