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

  int st = fread(buf, 1, len, file);
  (void) st;
  buf[len] = '\0';
  fclose(file);

  return buf;
}

int load_file(Atom env, const char *path)
{
  char *text;
  int status = 0;

//  printf("Reading %s...\n", path);
  text = slurp(path);
  if (text) {
    const char *p = text;
    Atom expr;
    while (read_expr(p, &p, &expr).type == Error_OK) {
      Atom result;
      Error err = eval_expr(expr, env, &result);
      if (ERROR_RAISED(err)) {
        print_error(err);
        putchar('\n');
        printf("Error in expression:\n\t");
        print_expr(expr);
        putchar('\n');
        status = 1;
        break;
      } 
    }
    free(text);
  }
  return status;
}

