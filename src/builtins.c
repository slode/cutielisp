#include <string.h>
#include <stdlib.h>
#include "cutie.h"

/* Builtins */
Error builtin_add(Atom args, Atom *result)
{
  Atom a, b;

  if (nilp(args) || nilp(cdr(args)) || !nilp(cdr(cdr(args))))
    return ERROR(Error_Args, "Requires two arguments.");

  a = car(args);
  b = car(cdr(args));

  if (a.type != ATOM_INTEGER || b.type != ATOM_INTEGER)
    return ERROR(Error_Type, "Arguments must be integers.");

  *result = make_integer(a.value.integer + b.value.integer);

  return ERROR_OK();
}

Error builtin_subtract(Atom args, Atom *result)
{
  Atom a, b;

  if (nilp(args) || nilp(cdr(args)) || !nilp(cdr(cdr(args))))
    return ERROR(Error_Args, "Requires two arguments.");

  a = car(args);
  b = car(cdr(args));

  if (a.type != ATOM_INTEGER || b.type != ATOM_INTEGER)
    return ERROR(Error_Type, "Arguments must be integers.");

  *result = make_integer(a.value.integer - b.value.integer);

  return ERROR_OK();
}

Error builtin_multiply(Atom args, Atom *result)
{
  Atom a, b;

  if (nilp(args) || nilp(cdr(args)) || !nilp(cdr(cdr(args))))
    return ERROR(Error_Args, "Requires two arguments.");

  a = car(args);
  b = car(cdr(args));

  if (a.type != ATOM_INTEGER || b.type != ATOM_INTEGER)
    return ERROR(Error_Type, "Arguments must be integers.");

  *result = make_integer(a.value.integer * b.value.integer);

  return ERROR_OK();
}

Error builtin_divide(Atom args, Atom *result)
{
  Atom a, b;

  if (nilp(args) || nilp(cdr(args)) || !nilp(cdr(cdr(args))))
    return ERROR(Error_Args, "Requires two arguments.");

  a = car(args);
  b = car(cdr(args));

  if (a.type != ATOM_INTEGER || b.type != ATOM_INTEGER)
    return ERROR(Error_Type, "Arguments must be integers.");

  if (b.value.integer == 0)
    return ERROR(Error_DivideByZero, "Divisor is zero.");

  *result = make_integer(a.value.integer / b.value.integer);

  return ERROR_OK();
}

Error builtin_car(Atom args, Atom *result)
{
  if (nilp(args) || !nilp(cdr(args)))
    return ERROR(Error_Args, "CAR argument required.");

  if (nilp(car(args))) {
    *result = nil;
  } else if (car(args).type != ATOM_PAIR) {
    print_expr(args);
    return ERROR(Error_Type, "CAR argument must be pair.");
  } else
    *result = car(car(args));

  return ERROR_OK();
}

Error builtin_cdr(Atom args, Atom *result)
{
  if (nilp(args) || !nilp(cdr(args)))
    return ERROR(Error_Args, "Argument required.");

  if (nilp(car(args))) {
    *result = nil;
  } else if (car(args).type != ATOM_PAIR) {
    print_expr(args);
    return ERROR(Error_Type, "Argument must be pair.");
  } else
    *result = cdr(car(args));

  return ERROR_OK();
}

Error builtin_cons(Atom args, Atom *result)
{
  if (nilp(args) || nilp(cdr(args)) || !nilp(cdr(cdr(args))))
    return ERROR(Error_Args, "Requires two arguments.");

  *result = cons(car(args), car(cdr(args)));

  return ERROR_OK();
}

Error builtin_numeq(Atom args, Atom *result)
{
  Atom a, b;

  if (nilp(args) || nilp(cdr(args)) || !nilp(cdr(cdr(args))))
    return ERROR(Error_Args, "Requires two arguments.");

  a = car(args);
  b = car(cdr(args));

  if (a.type != ATOM_INTEGER || b.type != ATOM_INTEGER)
    return ERROR(Error_Type, "Arguments must be integers.");

  *result = (a.value.integer == b.value.integer) ? make_symbol("T") : nil;

  return ERROR_OK();
}

Error builtin_less(Atom args, Atom *result)
{
  Atom a, b;

  if (nilp(args) || nilp(cdr(args)) || !nilp(cdr(cdr(args))))
    return ERROR(Error_Args, "Requires two arguments.");

  a = car(args);
  b = car(cdr(args));

  if (a.type != ATOM_INTEGER || b.type != ATOM_INTEGER)
    return ERROR(Error_Type, "Arguments must be integers.");

  *result = (a.value.integer < b.value.integer) ? make_symbol("T") : nil;

  return ERROR_OK();
}

Error builtin_stringeq(Atom args, Atom *result)
{
  Atom a, b;

  if (nilp(args) || nilp(cdr(args)) || !nilp(cdr(cdr(args))))
    return ERROR(Error_Args, "Requires two arguments.");

  a = car(args);
  b = car(cdr(args));

  if (a.type != ATOM_STRING || b.type != ATOM_STRING)
    return ERROR(Error_Type, "Arguments must be strings.");

  *result = (strcmp(a.value.string, b.value.string) == 0) ? make_symbol("T") : nil;

  return ERROR_OK();
}

Error builtin_stringless(Atom args, Atom *result)
{
  Atom a, b;

  if (nilp(args) || nilp(cdr(args)) || !nilp(cdr(cdr(args))))
    return ERROR(Error_Args, "Requires two arguments.");

  a = car(args);
  b = car(cdr(args));

  if (a.type != ATOM_STRING || b.type != ATOM_STRING)
    return ERROR(Error_Type, "Arguments must be strings.");


  char *s1 = a.value.string, *s2 = b.value.string;
  int index = 0;
  while( (*s1 != '\0') && (*s1 == *s2) ){
      s1++; 
      s2++;
      index++;
  }
  *result = (*s1 < *s2) ? make_integer(index) : nil;
  return ERROR_OK();
}

Error builtin_stringconcat(Atom args, Atom *result)
{
  Atom a, b;

  if (nilp(args) || nilp(cdr(args)) || !nilp(cdr(cdr(args))))
    return ERROR(Error_Args, "Requires two arguments.");

  a = car(args);
  b = car(cdr(args));

  if (a.type != ATOM_STRING || b.type != ATOM_STRING)
    return ERROR(Error_Type, "Arguments must be strings.");

  char *buf = malloc(strlen(a.value.string) + strlen(b.value.string) + 1);
  strcat(buf, a.value.string);
  strcat(buf, b.value.string);
  *result = make_string(buf);
  free(buf);
  return ERROR_OK();
}

Error builtin_stringsubstr(Atom args, Atom *result)
{
  Atom a, b, c;

  if (nilp(args) || nilp(cdr(args)) || nilp(cdr(cdr(args))) || !nilp(cdr(cdr(cdr(args)))))
    return ERROR(Error_Args, "Requires two or three arguments.");

  a = car(args);
  b = car(cdr(args));
  c = car(cdr(cdr(args)));

  if (a.type != ATOM_STRING || b.type != ATOM_INTEGER || !(c.type == ATOM_INTEGER || c.type == ATOM_NIL))
    return ERROR(Error_Type, "Arguments must be <string> <integer> <optional integer>.");

  int maxlen = strlen(a.value.string);
  int start = b.value.integer, len = 0;
  if (c.type == ATOM_NIL) {
    len = maxlen - start;
  } else {
    len = c.value.integer;
  }

  if (start + len > maxlen) {
    return ERROR(Error_OutOfBounds, "Index out of bounds.");
  }

  char *buf = malloc(maxlen + 1);
  strncpy(buf, a.value.string + start, len);
  *result = make_string(buf);
  free(buf);
  return ERROR_OK();
}


Error builtin_apply(Atom args, Atom *result)
{
  Atom fn;

  if (nilp(args) || nilp(cdr(args)) || !nilp(cdr(cdr(args))))
    return ERROR(Error_Args, "Requires two arguments.");

  fn = car(args);
  args = car(cdr(args));

  if (!listp(args))
    return ERROR(Error_Syntax, "Arguments must be a list.");

  return apply(fn, args, result);
}

Error builtin_eq(Atom args, Atom *result)
{
  Atom a, b;
  int eq;

  if (nilp(args) || nilp(cdr(args)) || !nilp(cdr(cdr(args))))
    return ERROR(Error_Args, "Requires two arguments.");

  a = car(args);
  b = car(cdr(args));

  if (a.type == b.type) {
    switch (a.type) {
    case ATOM_NIL:
      eq = 1;
      break;
    case ATOM_PAIR:
    case ATOM_CLOSURE:
    case ATOM_MACRO:
      eq = (a.value.pair == b.value.pair);
      break;
    case ATOM_STRING:
      eq = (strcmp(a.value.string, b.value.string) == 0);
      break;
    case ATOM_SYMBOL:
      eq = (a.value.symbol == b.value.symbol);
      break;
    case ATOM_INTEGER:
      eq = (a.value.integer == b.value.integer);
      break;
    case ATOM_BUILTIN:
      eq = (a.value.builtin == b.value.builtin);
      break;
    case ATOM_ERROR:
      eq = 0;
    }
  } else {
    eq = 0;
  }

  *result = eq ? make_symbol("T") : nil;
  return ERROR_OK();
}

Error builtin_pairp(Atom args, Atom *result)
{
  if (nilp(args) || !nilp(cdr(args)))
    return ERROR(Error_Args, "Requires a single argument.");

  *result = (car(args).type == ATOM_PAIR) ? make_symbol("T") : nil;
  return ERROR_OK();
}

Error builtin_stringp(Atom args, Atom *result)
{
  if (nilp(args) || !nilp(cdr(args)))
    return ERROR(Error_Args, "Requires a single argument.");

  *result = (car(args).type == ATOM_STRING) ? make_symbol("T") : nil;
  return ERROR_OK();
}

Error builtin_symbolp(Atom args, Atom *result)
{
  if (nilp(args) || !nilp(cdr(args)))
    return ERROR(Error_Args, "Requires a single argument.");

  *result = (car(args).type == ATOM_SYMBOL) ? make_symbol("T") : nil;
  return ERROR_OK();
}

Error builtin_numberp(Atom args, Atom *result)
{
  if (nilp(args) || !nilp(cdr(args)))
    return ERROR(Error_Args, "Requires a single argument.");

  *result = (car(args).type == ATOM_INTEGER) ? make_symbol("T") : nil;
  return ERROR_OK();
}

Error builtin_error(Atom args, Atom *result)
{
  if (nilp(args) || !nilp(cdr(args)))
    return ERROR(Error_Args, "Requires a single argument.");

  *result = (car(args).type == ATOM_STRING) ? car(args) : nil;
  return ERROR(Error_Syntax, car(args).value.string);
}

