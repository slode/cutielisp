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

  for (std::string ps : programs) {
    const char *p = ps.c_str();
    Error err;
    Atom sexpr;
    err = read_expr((const char*)p, (const char**)&p, &sexpr);
    CONTEST_TRUE(!ERROR_RAISED(err));
  }
}

CONTEST_CASE(test_parser_input)
{
  std::vector<std::string> programs;
  programs.push_back(")£$(1 ' set! 10) 10)");

  for (std::string ps : programs) {
    const char *p = ps.c_str();
    Atom sexpr;
    Error err = read_expr((const char*)p, (const char**)&p, &sexpr);
    CONTEST_TRUE(ERROR_RAISED(err));
  }
}

namespace {
Error builtin_square(Atom args, Atom *result)
{
  if (nilp(args))
    return ERROR(Error::Error_Args, "Requires one arguments.");

  Atom a = car(args);
  if (a.type != AtomType::ATOM_INTEGER)
    return ERROR(Error::Error_Args, "Argument must be integer");

  *result = make_integer(a.value.integer * a.value.integer);
  return ERROR_OK();
}
}

CONTEST_CASE(make_new_builtin) 
{
  Atom env = setup_env();
  env_set(env, make_symbol("TEST-FUNC"), make_builtin(builtin_square));

  Atom sexpr, result;

  const char *p = "(test-func 12)";
  Error err = read_expr(p, &p, &sexpr);
  CONTEST_TRUE(!ERROR_RAISED(err));

  err = eval_expr(sexpr, env, &result);
  CONTEST_TRUE(!ERROR_RAISED(err));
  CONTEST_EQUAL(result.value.integer, (long)144);
}

CONTEST_SUITE_END
