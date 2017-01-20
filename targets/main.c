#include <stdio.h>
#include <stdlib.h>

#include "readline.h"
#include "cutie.h"

int main()
{

  puts("CuteLisp Version 0.0.1");
  puts("Press Ctrl+c to Exit\n");

  Atom env = setup_env(nil);
  load_file(env, "library.lisp");

  char *input;
  while (1) {
    if (input) { free(input); }

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
    putchar('\n');
  }

  return 0;
}
