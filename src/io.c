#include <stdlib.h>
#include <stdio.h>

#include "cutie.h"

char *slurp(const char *path)
{
  FILE *file;
  char *buf;
  long len;

  file = fopen(path, "r");
  if (!file)
    return NULL;
  fseek(file, 0, SEEK_END);
  len = ftell(file);
  fseek(file, 0, SEEK_SET);

  buf = malloc(len+1);
  if (!buf)
    return NULL;

  fread(buf, 1, len, file);
  buf[len] = '\0';
  fclose(file);

  return buf;
}

int load_file(Atom env, const char *path)
{
  char *text;
  int status = 0;

  text = slurp(path);
  if (text) {
    const char *p = text;
    Atom expr;
    Error err;
    while (0) {
      err = read_expr(p, &p, &expr);
      if (ERROR_RAISED(err)) {
        print_error(err);
        putchar('\n');
        printf("Error parsing expression:\n\t");
        print_expr(expr);
        putchar('\n');
        status = 1;
        break;
      }

      Atom result;
      err = eval_expr(expr, env, &result);
      if (ERROR_RAISED(err)) {
        print_error(err);
        putchar('\n');
        printf("Error evaluating expression:\n\t");
        print_expr(expr);
        putchar('\n');
        status = 2;
        break;
      }
    }
    free(text);
  }
  return status;
}

