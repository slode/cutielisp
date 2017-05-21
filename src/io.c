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

  buf = malloc(len);
  if (!buf)
    return NULL;

  fread(buf, 1, len, file);
  fclose(file);

  return buf;
}

void load_file(Atom env, const char *path)
{
  char *text;

  printf("Reading %s...\n", path);
  text = slurp(path);
  if (text) {
    const char *p = text;
    Atom expr;
    while (read_expr(p, &p, &expr).type == Error_OK) {
      Atom result;
      Error err = eval_expr(expr, env, &result);
      if (ERROR_RAISED(err)) {
        print_error(err);
        printf("Error in expression:\n\t");
        print_expr(expr);
        putchar('\n');
      } else {
       // print_expr(result);
       // putchar('\n');
      }
    }
    free(text);
  }
}
