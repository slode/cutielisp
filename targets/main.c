#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "readline.h"

#define car(p) ((p).value.pair->atom[0])
#define cdr(p) ((p).value.pair->atom[1])
#define nilp(atom) ((atom).type == ATOM_NIL)

static const Atom nil = {ATOM_NIL, {0}};
static Atom sym_table = {ATOM_NIL, {0}};

Atom cons(Atom car_val, Atom cdr_val);
Atom make_integer(long x);
Atom make_string(const char *s);
Atom make_symbol(const char *s);

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

#define ERROR(type, message) make_error(type, message, __FILE__, __FUNCTION__, __LINE__)
#define ERROR_OK() make_error(0, "", __FILE__, __FUNCTION__, __LINE__)
#define ERROR_RAISED(err) (err.type != Error_OK)

void print_expr(Atom atom);

Atom cons(Atom car_val, Atom cdr_val) {
  Atom p;
  p.type = ATOM_PAIR;
  p.value.pair = (struct Pair*)malloc(sizeof(struct Pair));
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

Atom make_string(const char *s) {
  Atom a;
  a.type = ATOM_STRING;
  a.value.string = strdup(s);
  return a;
}
 
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

int listp(Atom expr);
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
    return ERROR(Error_Args, "Argument required.");

  if (nilp(car(args)))
    *result = nil;
  else if (car(args).type != ATOM_PAIR) {
    return ERROR(Error_Type, "Argument must be pair.");
  } else
    *result = car(car(args));

  return ERROR_OK();
}

Error builtin_cdr(Atom args, Atom *result)
{
  if (nilp(args) || !nilp(cdr(args)))
    return ERROR(Error_Args, "Argument required.");

  if (nilp(car(args)))
    *result = nil;
  else if (car(args).type != ATOM_PAIR)
    return ERROR(Error_Type, "Argument must be pair.");
  else
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


Error apply(Atom fn, Atom args, Atom *result);
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


/* ENV */
Atom create_env(Atom parent) {
  return cons(parent, nil);
}

Error env_get(Atom env, Atom symbol, Atom *result)
{
  Atom parent = car(env);
  Atom bs = cdr(env);

  while (!nilp(bs)) {
    Atom b = car(bs);
    if (car(b).value.symbol == symbol.value.symbol) {
      *result = cdr(b);
      return ERROR_OK();
    }
    bs = cdr(bs);
  }

  if (nilp(parent))
    return ERROR(Error_UnBound, symbol.value.symbol);

  return env_get(parent, symbol, result);
}

Error env_set(Atom env, Atom symbol, Atom value)
{
  Atom bs = cdr(env);
  Atom b = nil;

  while (!nilp(bs)) {
    b = car(bs);
    if (car(b).value.symbol == symbol.value.symbol) {
      cdr(b) = value;
      return ERROR_OK();
    }
    bs = cdr(bs);
  }

  b = cons(symbol, value);
  cdr(env) = cons(b, cdr(env));

  return ERROR_OK();
}

Error env_set_existing(Atom env, Atom symbol, Atom value)
{
  Atom parent = car(env);
  Atom bs = cdr(env);

  while (!nilp(bs)) {
    Atom b = car(bs);
    if (car(b).value.symbol == symbol.value.symbol) {
      cdr(b) = value;
      return ERROR_OK();
    }
    bs = cdr(bs);
  }

  if (nilp(parent))
    return ERROR(Error_UnBound, symbol.value.symbol);

  return env_set_existing(parent, symbol, value);
}

/* IO */
void print_expr(Atom atom) {
  switch (atom.type) {
    case ATOM_NIL:
      printf("NIL");
      break;
    case ATOM_INTEGER:
      printf("%ld", atom.value.integer);
      break;
    case ATOM_PAIR:
      putchar('(');
      print_expr(car(atom));
      atom = cdr(atom);
      while (!nilp(atom)) {
        if (atom.type == ATOM_PAIR) {
          putchar(' ');
          print_expr(car(atom));
          atom = cdr(atom);
        } else {
          printf(" . ");
          print_expr(atom);
          break;
        }
      }
      putchar(')');
      break;
    case ATOM_BUILTIN:
      printf("#<BUILTIN:%p>", atom.value.builtin);
      break;
    case ATOM_STRING:
      printf("\"%s\"", atom.value.string);
      break;
    case ATOM_CLOSURE:
      printf("#<CLOSURE>");
      break;
    case ATOM_MACRO:
      printf("#<MACRO>");
      break;
    case ATOM_ERROR:
    case ATOM_SYMBOL:
      printf("%s", atom.value.symbol);
      break;
  }
}

Error lex(const char *str, const char **start, const char **end)
{
  const char *ws = " \t\n";
  const char *delim = "() \t\n";
  const char *prefix = "()\'`\"";
  const char *eol = "\n";

  str += strspn(str, ws);

  if (str[0] == '\0') {
    *start = *end = NULL;
    return ERROR(Error_Syntax, "End-of-input reached.");
  }

  *start = str;

  if (strchr(prefix, str[0]) != NULL)
    *end = str + 1;
  else if (str[0] == ',')
    *end = str + (str[1] == '@' ? 2 : 1);
  else if (str[0] == ';')
    *end = str + strcspn(str, eol);
  else
    *end = str + strcspn(str, delim);

  return ERROR_OK();
}

Error read_expr(const char *input, const char **end, Atom *result);

Error parse_simple(const char *start, const char *end, Atom *result)
{
  char *buf, *p;

  /* Is it an integer? */
  long val = strtol(start, &p, 10);
  if (p == end) {
    result->type = ATOM_INTEGER;
    result->value.integer = val;
    return ERROR_OK();
  }

  /* NIL or symbol */
  buf = malloc(end - start + 1);
  p = buf;
  while (start != end)
    *p++ = toupper(*start), ++start;
  *p = '\0';

  if (strcmp(buf, "NIL") == 0)
    *result = nil;
  else {
    *result = make_symbol(buf);
  }

  free(buf);

  return ERROR_OK();
}

Error read_string(const char *input, const char **end, Atom *result)
{
  const char *eos = "\"";
  char *buf, *p;
  const char *start;
  start = input;
  *end = start;

  do {
    *end = *end + strcspn(*end+1, eos)+1;
  } while (*(*end-1) == '\\');

  buf = malloc(*end - start + 1);
  p = buf;
  while (start != *end) {
    *p++ = *start, ++start;
  }
  *p = '\0';
  *end+=1; // Swallow the following \"

  *result = make_string(buf);

  return ERROR_OK();
}

Error read_list(const char *start, const char **end, Atom *result)
{
  Atom p;

  *end = start;
  p = *result = nil;

  for (;;) {
    const char *token;
    Atom item;
    Error err;

    err = lex(*end, &token, end);
    if (ERROR_RAISED(err))
      return err;

    if (token[0] == ')')
      return ERROR_OK();

    if (token[0] == '.' && *end - token == 1) {
      /* Improper list */
      if (nilp(p))
        return ERROR(Error_Syntax, "Improper list error");

      err = read_expr(*end, end, &item);
      if (ERROR_RAISED(err))
        return err;

      cdr(p) = item;

      /* Read the closing ')' */
      err = lex(*end, &token, end);
      if (!ERROR_RAISED(err) && token[0] != ')')
        err = ERROR(Error_Syntax, "Error syntax!");

      return err;
    }

    err = read_expr(token, end, &item);
    if (ERROR_RAISED(err))
      return err;

    if (nilp(p)) {
      /* First item */
      *result = cons(item, nil);
      p = *result;
    } else {
      cdr(p) = cons(item, nil);
      p = cdr(p);
    }
  }
}

Error read_expr(const char *input, const char **end, Atom *result)
{
  const char *token;
  Error err;

  err = lex(input, &token, end);
  if (ERROR_RAISED(err))
    return err;

  if (token[0] == '(') {
    return read_list(*end, end, result);
  }
  else if (token[0] == ')') {
    return ERROR(Error_Syntax, "')' reached unexpectedly.");
  }
  else if (token[0] == '\'') {
    *result = cons(make_symbol("QUOTE"), cons(nil, nil));
    return read_expr(*end, end, &car(cdr(*result)));
  }
  else if (token[0] == '`') {
    *result = cons(make_symbol("QUASIQUOTE"), cons(nil, nil));
    return read_expr(*end, end, &car(cdr(*result)));
  }
  else if (token[0] == ',') {
    *result = cons(make_symbol(
      token[1] == '@' ? "UNQUOTE-SPLICING" : "UNQUOTE"),
      cons(nil, nil));
    return read_expr(*end, end, &car(cdr(*result)));
  } else if (token[0] == ';') {
    // Found a comment. Skip until newline and continue reading.
    return read_expr(*end, end, result);
  } else if (token[0] == '\"') {
    return read_string(*end, end, result);
  }
  else {
    return parse_simple(token, *end, result);
  }
}

/* Evaluation */
int listp(Atom expr)
{
  while (!nilp(expr)) {
    if (expr.type != ATOM_PAIR)
      return 0;
    expr = cdr(expr);
  }
  return 1;
}

Atom copy_list(Atom list)
{
  Atom a, p;

  if (nilp(list))
    return nil;

  a = cons(car(list), nil);
  p = a;
  list = cdr(list);

  while (!nilp(list)) {
    cdr(p) = cons(car(list), nil);
    p = cdr(p);
    list = cdr(list);
  }

  return a;
}

Error eval_expr(Atom expr, Atom env, Atom *result);
Error apply(Atom fn, Atom args, Atom *result)
{
  Atom env, arg_names, body;

  if (fn.type == ATOM_BUILTIN)
    return (*fn.value.builtin)(args, result);
  else if (fn.type != ATOM_CLOSURE)
    return ERROR(Error_Type, "Type must be closure.");

  env = create_env(car(fn));
  arg_names = car(cdr(fn));
  body = cdr(cdr(fn));

  /* Bind the arguments */
  while (!nilp(arg_names)) {
    if (arg_names.type == ATOM_SYMBOL) {
      env_set(env, arg_names, args);
      args = nil;
      break;
    } 
    if (nilp(args))
      return ERROR(Error_Args, "Argument required.");
    env_set(env, car(arg_names), car(args));
    arg_names = cdr(arg_names);
    args = cdr(args);
  }
  if (!nilp(args))
    return ERROR(Error_Args, "Argument required.");

  /* Evaluate the body */
  while (!nilp(body)) {
    Error err = eval_expr(car(body), env, result);
    if (ERROR_RAISED(err))
      return err;
    body = cdr(body);
  }

  return ERROR_OK();
}

void load_file(Atom env, const char *path);

Error eval_expr(Atom expr, Atom env, Atom *result)
{
  Atom op, args, p;
  Error err;

  if (expr.type == ATOM_SYMBOL) {
    if (expr.value.symbol[0] == ':') {
      *result = expr;
      return ERROR_OK();
    }
    return env_get(env, expr, result);
  } else if (expr.type != ATOM_PAIR) {
    *result = expr;
    return ERROR_OK();
  }

  if (!listp(expr)) {
    return ERROR(Error_Syntax, "Expression must be list.");
  }

  op = car(expr);
  args = cdr(expr);

  if (op.type == ATOM_SYMBOL) {
    if (strcmp(op.value.symbol, "QUOTE") == 0) {
      if (nilp(args) || !nilp(cdr(args)))
        return ERROR(Error_Args, "QUOTE requires an argument.");

      *result = car(args);
      return ERROR_OK();

    } else if (strcmp(op.value.symbol, "DEFINE") == 0) {
      Atom sym, val;

      if (nilp(args) || nilp(cdr(args)))
        return ERROR(Error_Args, "DEFINE requires two arguments.");

      sym = car(args);
      if (sym.type == ATOM_PAIR) {
        err = make_closure(env, cdr(sym), cdr(args), &val);
        sym = car(sym);
        if (sym.type != ATOM_SYMBOL)
          return ERROR(Error_Type, "DEFINE first argument must be symbol.");
      } else if (sym.type == ATOM_SYMBOL) {
        if (!nilp(cdr(cdr(args))))
          return ERROR(Error_Args, "DEFINE argument error.");
        err = eval_expr(car(cdr(args)), env, &val);
      } else {
        return ERROR(Error_Type, "DEFINE argument error.");
      }

      if (ERROR_RAISED(err))
        return err;

      *result = sym;
      return env_set(env, sym, val);

    } else if (strcmp(op.value.symbol, "SET!") == 0) {
      Atom sym, val;

      if (nilp(args) || nilp(cdr(args)))
        return ERROR(Error_Args, "SET! requires two arguments.");

      sym = car(args);
      if (sym.type == ATOM_PAIR) {
        err = make_closure(env, cdr(sym), cdr(args), &val);
        sym = car(sym);
        if (sym.type != ATOM_SYMBOL)
          return ERROR(Error_Type, "SET! first argument not symbol");
      } else if (sym.type == ATOM_SYMBOL) {
        if (!nilp(cdr(cdr(args))))
          return ERROR(Error_Args, "SET! argument error.");
        err = eval_expr(car(cdr(args)), env, &val);
      } else {
        return ERROR(Error_Type, "SET! argument error.");
      }

      if (ERROR_RAISED(err))
        return err;

      *result = sym;
      return env_set_existing(env, sym, val);

    } else if (strcmp(op.value.symbol, "PROGN") == 0) {
      Atom body = args;

      /* Evaluate the body */
      while (!nilp(body)) {
        Error err = eval_expr(car(body), env, result);
        if (ERROR_RAISED(err))
          return err;
        body = cdr(body);
      }
      return ERROR_OK();

    } else if (strcmp(op.value.symbol, "LAMBDA") == 0) {
      if (nilp(args) || nilp(cdr(args)))
        return ERROR(Error_Args, "LAMBDA requires two arguments.");

      return make_closure(env, car(args), cdr(args), result);

    } else if (strcmp(op.value.symbol, "IF") == 0) {
      Atom cond, val;

      if (nilp(args) || nilp(cdr(args)) || nilp(cdr(cdr(args)))
          || !nilp(cdr(cdr(cdr(args)))))
        return ERROR(Error_Args, "IF requires three arguments.");

      err = eval_expr(car(args), env, &cond);
      if (ERROR_RAISED(err))
        return err;

      val = nilp(cond) ? car(cdr(cdr(args))) : car(cdr(args));
      return eval_expr(val, env, result);

    } else if (strcmp(op.value.symbol, "DEFMACRO") == 0) {
      Atom name, macro;
      Error err;

      if (nilp(args) || nilp(cdr(args)))
        return ERROR(Error_Args, "DEFMACRO requires two arguments.");

      if (car(args).type != ATOM_PAIR)
        return ERROR(Error_Syntax, "DEFMACRO syntax error.");

      name = car(car(args));
      if (name.type != ATOM_SYMBOL)
        return ERROR(Error_Type, "DEFMACRO type error.");

      err = make_closure(env, cdr(car(args)),
        cdr(args), &macro);
      if (ERROR_RAISED(err))
        return err;

      macro.type = ATOM_MACRO;
      *result = name;
      return env_set(env, name, macro);

    } else if (strcmp(op.value.symbol, "LOAD") == 0) {
      Atom a;

      if (nilp(args))
        return ERROR(Error_Args, "LOAD takes one argument.");

      a = car(args);

      err = eval_expr(a, env, &a);
      if (ERROR_RAISED(err))
        return err;

      if (a.type != ATOM_STRING)
        return ERROR(Error_Type, "LOAD argument must be a string.");

      load_file(env, a.value.string);
      *result = make_symbol("T");
      return ERROR_OK();
    }
  }

  /* Evaluate operator */
  err = eval_expr(op, env, &op);
  if (ERROR_RAISED(err))
    return err;

  /* Is it a macro? */
  if (op.type == ATOM_MACRO) {
    Atom expansion;
    op.type = ATOM_CLOSURE;
    err = apply(op, args, &expansion);
    if (ERROR_RAISED(err))
      return err;
    return eval_expr(expansion, env, result);
  }

  /* Evaluate arguments */
  args = copy_list(args);
  p = args;
  while (!nilp(p)) {
    err = eval_expr(car(p), env, &car(p));
    if (ERROR_RAISED(err))
      return err;

    p = cdr(p);
  }

  return apply(op, args, result);
}

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

void print_error(Error err);
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
        print_expr(result);
        putchar('\n');
      }
    }
    free(text);
  }
}

void print_error(Error err)
{
  switch (err.type) {
    case Error_OK:
      puts("No error!");
      break;
    case Error_Syntax:
      puts("Syntax error.");
      break;
    case Error_UnBound:
      puts("Symbol not bound.");
      break;
    case Error_Args:
      puts("Wrong number of arguments.");
      break;
    case Error_Type:
      puts("Wrong type.");
      break;
    case Error_DivideByZero:
      puts("Division-by-zero error.");
      break;
    case Error_OutOfBounds:
      puts("Index out of bounds.");
      break;
  }
  printf("Error: '%s' in function %s %s:%d\n",
      err.message, err.function_name, err.file_name, err.line_number);
}

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
