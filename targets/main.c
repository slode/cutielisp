#include "readline.h"
#include "cutie.h"

int main()
{

  puts("CuteLisp Version 0.0.1");
  puts("Press Ctrl+c to Exit\n");

  Atom env = create_env(nil);
  env_set(env, make_symbol("+"), make_builtin(builtin_add));
  env_set(env, make_symbol("-"), make_builtin(builtin_subtract));
  env_set(env, make_symbol("*"), make_builtin(builtin_multiply));
  env_set(env, make_symbol("/"), make_builtin(builtin_divide));
  env_set(env, make_symbol("CAR"), make_builtin(builtin_car));
  env_set(env, make_symbol("CDR"), make_builtin(builtin_cdr));
  env_set(env, make_symbol("CONS"), make_builtin(builtin_cons));
  env_set(env, make_symbol("="), make_builtin(builtin_numeq));
  env_set(env, make_symbol("<"), make_builtin(builtin_less));
  env_set(env, make_symbol("APPLY"), make_builtin(builtin_apply));
  env_set(env, make_symbol("EQ?"), make_builtin(builtin_eq));
  env_set(env, make_symbol("PAIR?"), make_builtin(builtin_pairp));
  env_set(env, make_symbol("SYMBOL?"), make_builtin(builtin_symbolp));
  env_set(env, make_symbol("STRING?"), make_builtin(builtin_stringp));
  env_set(env, make_symbol("NUMBER?"), make_builtin(builtin_numberp));
  env_set(env, make_symbol("ERROR"), make_builtin(builtin_error));
  env_set(env, make_symbol("T"), make_symbol("T"));
  env_set(env, make_symbol("STRING-EQUAL"), make_builtin(builtin_stringeq));
  env_set(env, make_symbol("STRING-LESSP"), make_builtin(builtin_stringless));
  env_set(env, make_symbol("STRING-CONCAT"), make_builtin(builtin_stringconcat));
  env_set(env, make_symbol("STRING-SUBSTR"), make_builtin(builtin_stringsubstr));

  load_file(env, "library.lisp");

  char *input;
  while (1) {
    input = readline("cutie> ");
    add_history(input);

    Error err;
    Atom sexpr, result;

    const char *p = input;
    err = read_expr(p, &p, &sexpr);

    if (!ERROR_RAISED(err)) {
      err = eval_expr(sexpr, env, &result);
    }

    if (ERROR_RAISED(err)) {
      print_error(err);
    } else {
      print_expr(result);
    }
    free(input);
    putchar('\n');
  }

  return 0;
}
