#include <stdio.h>
#include <stdlib.h>

#include "readline.h"
#include "cutie.h"

int main()
{

  puts("CutieLisp Version 0.0.1");
  puts("Press Ctrl+c to Exit\n");

  Atom env = setup_env();
  load_file(env, "library.lisp");

  char *input = 0;
  while (1) {
    if (input) { free(input); }

    input = readline("cutie> ");
    if (!input) { return 0; }
    add_history(input);

    Error err;
    Atom sexpr, result;

    const char *p = input;
    err = cutie_parse(p, &sexpr);

    if (!ERROR_RAISED(err)) {
      err = eval_expr(sexpr, env, &result);
    }

    if (ERROR_RAISED(err)) {
      print_error(err);
    } else {
      print_expr(result);
    }
    putchar('\n');
  }

  return 0;
}
