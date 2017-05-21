#include <stdlib.h>
#include <string.h>

#include "cutie.h"


Error make_error(
    int type,
    const char *message,
    const char *file_name,
    const char *function_name,
    int line_number)
{
  Error err;
  err.type = type;
  err.message = strdup(message);
  err.file_name = strdup(file_name);
  err.function_name = strdup(function_name);
  err.line_number = line_number;
  return err;
}

Atom cons(Atom car_val, Atom cdr_val) {
  Atom p;
  p.type = ATOM_PAIR;
  p.value.pair = (struct Pair*)cutie_malloc(sizeof(struct Pair));
  car(p) = car_val;
  cdr(p) = cdr_val;
  return p;
}

Atom make_integer(long x) {
  Atom p;
  p.type = ATOM_INTEGER;
  p.value.integer = x;
  return p;
}

Atom make_real(double x) {
  Atom p;
  p.type = ATOM_REAL;
  p.value.real = x;
  return p;
}

Atom make_string(const char *s) {
  Atom a;
  a.type = ATOM_STRING;
  a.value.string = strdup(s);
  return a;
}
 
Atom sym_table = {ATOM_NIL, {0}};

Atom make_symbol(const char *s) {
  Atom a, p;

  p = sym_table;
  while(!nilp(p)) {
    a = car(p);
    if (strcmp(a.value.symbol, s) == 0) {
      return a;
    }
    p = cdr(p);
  }

  a.type = ATOM_SYMBOL;
  a.value.symbol = strdup(s);
  sym_table = cons(a, sym_table);
  return a;
}

Atom make_builtin(Builtin fn)
{
  Atom a;
  a.type = ATOM_BUILTIN;
  a.value.builtin = fn;
  return a;
}

Error make_closure(Atom env, Atom args, Atom body, Atom *result)
{
  Atom p;

  if (!listp(body))
    return ERROR(Error_Syntax, "Function body must be a list");

  /* Check argument names are all symbols */
  p = args;
  while (!nilp(p)) {
    if (p.type == ATOM_SYMBOL) {
      break;
    } else if (p.type != ATOM_PAIR || car(p).type != ATOM_SYMBOL) {
      return ERROR(Error_Type, "Arguments need to be symbols");
    }
    p = cdr(p);
  }

  *result = cons(env, cons(args, body));
  result->type = ATOM_CLOSURE;
  return ERROR_OK();
}
