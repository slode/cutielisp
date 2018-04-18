#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "readline.h"
#include "cutie.h"

/* static to enable readline completion */
static Atom env;

char* env_generator(const char* text, int state)
{
    static int len;
    static Atom bs;
 
    if (!state) {
      len = strlen(text);
      bs = cdr(env);
    }

    while (!nilp(bs)) {
      Atom b = car(bs);
      if (strncasecmp(car(b).value.symbol, text, len) == 0) {
        bs = cdr(bs);
        return strdup(car(b).value.symbol);
      }
      bs = cdr(bs);
    }

    return ((char *)NULL);
}

static char** env_completion(const char * text, int start, int end)
{

    (void) start; (void) end;
    rl_attempted_completion_over = 1;
    return rl_completion_matches(text, env_generator);
}


int main(int argc, char **argv)
{
  env = setup_env();

  // Execute file mode
  if (argc > 1) {
    const char *scriptname = argv[1];
    int result = load_file(env, scriptname);
    return result;
  }

  rl_attempted_completion_function = env_completion;

  // Interactive mode
  puts("CutieLisp Version 0.0.1");
  puts("Press Ctrl+c to Exit\n");

  load_file(env, "library.lsp");

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
