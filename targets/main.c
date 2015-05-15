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
int make_closure(Atom env, Atom args, Atom body, Atom *result)
{
  Atom p;

  if (!listp(body))
    return Error_Syntax;

  /* Check argument names are all symbols */
  p = args;
  while (!nilp(p)) {
    if (p.type == ATOM_SYMBOL) {
      break;
    } else if (p.type != ATOM_PAIR || car(p).type != ATOM_SYMBOL) {
      return Error_Type;
    }
    p = cdr(p);
  }

  *result = cons(env, cons(args, body));
  result->type = ATOM_CLOSURE;
  return Error_OK;
}

/* Builtins */
int builtin_add(Atom args, Atom *result)
{
  Atom a, b;

  if (nilp(args) || nilp(cdr(args)) || !nilp(cdr(cdr(args))))
    return Error_Args;

  a = car(args);
  b = car(cdr(args));

  if (a.type != ATOM_INTEGER || b.type != ATOM_INTEGER)
    return Error_Type;

  *result = make_integer(a.value.integer + b.value.integer);

  return Error_OK;
}

int builtin_subtract(Atom args, Atom *result)
{
  Atom a, b;

  if (nilp(args) || nilp(cdr(args)) || !nilp(cdr(cdr(args))))
    return Error_Args;

  a = car(args);
  b = car(cdr(args));

  if (a.type != ATOM_INTEGER || b.type != ATOM_INTEGER)
    return Error_Type;

  *result = make_integer(a.value.integer - b.value.integer);

  return Error_OK;
}

int builtin_multiply(Atom args, Atom *result)
{
  Atom a, b;

  if (nilp(args) || nilp(cdr(args)) || !nilp(cdr(cdr(args))))
    return Error_Args;

  a = car(args);
  b = car(cdr(args));

  if (a.type != ATOM_INTEGER || b.type != ATOM_INTEGER)
    return Error_Type;

  *result = make_integer(a.value.integer * b.value.integer);

  return Error_OK;
}

int builtin_divide(Atom args, Atom *result)
{
  Atom a, b;

  if (nilp(args) || nilp(cdr(args)) || !nilp(cdr(cdr(args))))
    return Error_Args;

  a = car(args);
  b = car(cdr(args));

  if (a.type != ATOM_INTEGER || b.type != ATOM_INTEGER)
    return Error_Type;

  if (b.value.integer == 0)
    return Error_DivideByZero;

  *result = make_integer(a.value.integer - b.value.integer);

  return Error_OK;
}

int builtin_car(Atom args, Atom *result)
{
  if (nilp(args) || !nilp(cdr(args)))
    return Error_Args;

  if (nilp(car(args)))
    *result = nil;
  else if (car(args).type != ATOM_PAIR) {
    return Error_Type;
  } else
    *result = car(car(args));

  return Error_OK;
}

int builtin_cdr(Atom args, Atom *result)
{
  if (nilp(args) || !nilp(cdr(args)))
    return Error_Args;

  if (nilp(car(args)))
    *result = nil;
  else if (car(args).type != ATOM_PAIR)
    return Error_Type;
  else
    *result = cdr(car(args));

  return Error_OK;
}

int builtin_cons(Atom args, Atom *result)
{
  if (nilp(args) || nilp(cdr(args)) || !nilp(cdr(cdr(args))))
    return Error_Args;

  *result = cons(car(args), car(cdr(args)));

  return Error_OK;
}

int builtin_numeq(Atom args, Atom *result)
{
  Atom a, b;

  if (nilp(args) || nilp(cdr(args)) || !nilp(cdr(cdr(args))))
    return Error_Args;

  a = car(args);
  b = car(cdr(args));

  if (a.type != ATOM_INTEGER || b.type != ATOM_INTEGER)
    return Error_Type;

  *result = (a.value.integer == b.value.integer) ? make_symbol("T") : nil;

  return Error_OK;
}

int builtin_less(Atom args, Atom *result)
{
  Atom a, b;

  if (nilp(args) || nilp(cdr(args)) || !nilp(cdr(cdr(args))))
    return Error_Args;

  a = car(args);
  b = car(cdr(args));

  if (a.type != ATOM_INTEGER || b.type != ATOM_INTEGER)
    return Error_Type;

  *result = (a.value.integer < b.value.integer) ? make_symbol("T") : nil;

  return Error_OK;
}
int apply(Atom fn, Atom args, Atom *result);
int builtin_apply(Atom args, Atom *result)
{
  Atom fn;

  if (nilp(args) || nilp(cdr(args)) || !nilp(cdr(cdr(args))))
    return Error_Args;

  fn = car(args);
  args = car(cdr(args));

  if (!listp(args))
    return Error_Syntax;

  return apply(fn, args, result);
}

int builtin_eq(Atom args, Atom *result)
{
  Atom a, b;
  int eq;

  if (nilp(args) || nilp(cdr(args)) || !nilp(cdr(cdr(args))))
    return Error_Args;

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
    }
  } else {
    eq = 0;
  }

  *result = eq ? make_symbol("T") : nil;
  return Error_OK;
}

int builtin_pairp(Atom args, Atom *result)
{
  if (nilp(args) || !nilp(cdr(args)))
    return Error_Args;

  *result = (car(args).type == ATOM_PAIR) ? make_symbol("T") : nil;
  return Error_OK;
}

/* ENV */
Atom create_env(Atom parent) {
  return cons(parent, nil);
}

int env_get(Atom env, Atom symbol, Atom *result)
{
  Atom parent = car(env);
  Atom bs = cdr(env);

  while (!nilp(bs)) {
    Atom b = car(bs);
    if (car(b).value.symbol == symbol.value.symbol) {
      *result = cdr(b);
      return Error_OK;
    }
    bs = cdr(bs);
  }

  if (nilp(parent))
    return Error_UnBound;

  return env_get(parent, symbol, result);
}

int env_set(Atom env, Atom symbol, Atom value)
{
  Atom bs = cdr(env);
  Atom b = nil;

  while (!nilp(bs)) {
    b = car(bs);
    if (car(b).value.symbol == symbol.value.symbol) {
      cdr(b) = value;
      return Error_OK;
    }
    bs = cdr(bs);
  }

  b = cons(symbol, value);
  cdr(env) = cons(b, cdr(env));

  return Error_OK;
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
      printf("<STRING:\"%s\">", atom.value.string);
      break;
    case ATOM_SYMBOL:
    case ATOM_CLOSURE:
    case ATOM_MACRO:
      printf("%s", atom.value.symbol);
      break;
  }
}

int lex(const char *str, const char **start, const char **end)
{
  const char *ws = " \t\n";
  const char *delim = "() \t\n";
  const char *prefix = "()\'`\"";
  const char *eol = "\n";

  str += strspn(str, ws);

  if (str[0] == '\0') {
    *start = *end = NULL;
    return Error_Syntax;
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

  return Error_OK;
}

int read_expr(const char *input, const char **end, Atom *result);

int parse_simple(const char *start, const char *end, Atom *result)
{
  char *buf, *p;

  /* Is it an integer? */
  long val = strtol(start, &p, 10);
  if (p == end) {
    result->type = ATOM_INTEGER;
    result->value.integer = val;
    return Error_OK;
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

  return Error_OK;
}

int read_string(const char *input, const char **end, Atom *result)
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
  while (start != *end)
    *p++ = *start, ++start;
  *p = '\0';
  *end+=1; // Swallow the following \"

  *result = make_string(buf);

  return Error_OK;
}

int read_list(const char *start, const char **end, Atom *result)
{
  Atom p;

  *end = start;
  p = *result = nil;

  for (;;) {
    const char *token;
    Atom item;
    Error err;

    err = lex(*end, &token, end);
    if (err)
      return err;

    if (token[0] == ')')
      return Error_OK;

    if (token[0] == '.' && *end - token == 1) {
      /* Improper list */
      if (nilp(p))
        return Error_Syntax;

      err = read_expr(*end, end, &item);
      if (err)
        return err;

      cdr(p) = item;

      /* Read the closing ')' */
      err = lex(*end, &token, end);
      if (!err && token[0] != ')')
        err = Error_Syntax;

      return err;
    }

    err = read_expr(token, end, &item);
    if (err)
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

int read_expr(const char *input, const char **end, Atom *result)
{
  const char *token;
  Error err;

  err = lex(input, &token, end);
  if (err)
    return err;

  if (token[0] == '(') {
    return read_list(*end, end, result);
  }
  else if (token[0] == ')') {
    return Error_Syntax;
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

int eval_expr(Atom expr, Atom env, Atom *result);
int apply(Atom fn, Atom args, Atom *result)
{
  Atom env, arg_names, body;

  if (fn.type == ATOM_BUILTIN)
    return (*fn.value.builtin)(args, result);
  else if (fn.type != ATOM_CLOSURE)
    return Error_Type;

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
      return Error_Args;
    env_set(env, car(arg_names), car(args));
    arg_names = cdr(arg_names);
    args = cdr(args);
  }
  if (!nilp(args))
    return Error_Args;

  /* Evaluate the body */
  while (!nilp(body)) {
    Error err = eval_expr(car(body), env, result);
    if (err)
      return err;
    body = cdr(body);
  }

  return Error_OK;
}


int eval_expr(Atom expr, Atom env, Atom *result)
{
  Atom op, args, p;
  Error err;

  if (expr.type == ATOM_SYMBOL) {
    if (expr.value.symbol[0] == ':') {
      *result = expr;
      return Error_OK;
    }
    return env_get(env, expr, result);
  } else if (expr.type != ATOM_PAIR) {
    *result = expr;
    return Error_OK;
  }

  if (!listp(expr)) {
    return Error_Syntax;
  }

  op = car(expr);
  args = cdr(expr);

  if (op.type == ATOM_SYMBOL) {
    if (strcmp(op.value.symbol, "QUOTE") == 0) {
      if (nilp(args) || !nilp(cdr(args)))
        return Error_Args;

      *result = car(args);
      return Error_OK;

    } else if (strcmp(op.value.symbol, "DEFINE") == 0) {
      Atom sym, val;

      if (nilp(args) || nilp(cdr(args)))
        return Error_Args;

      sym = car(args);
      if (sym.type == ATOM_PAIR) {
        err = make_closure(env, cdr(sym), cdr(args), &val);
        sym = car(sym);
        if (sym.type != ATOM_SYMBOL)
          return Error_Type;
      } else if (sym.type == ATOM_SYMBOL) {
        if (!nilp(cdr(cdr(args))))
          return Error_Args;
        err = eval_expr(car(cdr(args)), env, &val);
      } else {
        return Error_Type;
      }

      if (err)
        return err;

      *result = sym;
      return env_set(env, sym, val);

    } else if (strcmp(op.value.symbol, "LAMBDA") == 0) {
      if (nilp(args) || nilp(cdr(args)))
        return Error_Args;

      return make_closure(env, car(args), cdr(args), result);

    } else if (strcmp(op.value.symbol, "IF") == 0) {
      Atom cond, val;

      if (nilp(args) || nilp(cdr(args)) || nilp(cdr(cdr(args)))
          || !nilp(cdr(cdr(cdr(args)))))
        return Error_Args;

      err = eval_expr(car(args), env, &cond);
      if (err)
        return err;

      val = nilp(cond) ? car(cdr(cdr(args))) : car(cdr(args));
      return eval_expr(val, env, result);

    } else if (strcmp(op.value.symbol, "DEFMACRO") == 0) {
      Atom name, macro;
      Error err;

      if (nilp(args) || nilp(cdr(args)))
        return Error_Args;

      if (car(args).type != ATOM_PAIR)
        return Error_Syntax;

      name = car(car(args));
      if (name.type != ATOM_SYMBOL)
        return Error_Type;

      err = make_closure(env, cdr(car(args)),
        cdr(args), &macro);
      if (err)
        return err;

      macro.type = ATOM_MACRO;
      *result = name;
      return env_set(env, name, macro);
    }
  }

  /* Evaluate operator */
  err = eval_expr(op, env, &op);
  if (err)
    return err;

  /* Is it a macro? */
  if (op.type == ATOM_MACRO) {
    Atom expansion;
    op.type = ATOM_CLOSURE;
    err = apply(op, args, &expansion);
    if (err)
      return err;
    return eval_expr(expansion, env, result);
  }

  /* Evaluate arguments */
  args = copy_list(args);
  p = args;
  while (!nilp(p)) {
    err = eval_expr(car(p), env, &car(p));
    if (err)
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

void load_file(Atom env, const char *path)
{
  char *text;

  printf("Reading %s...\n", path);
  text = slurp(path);
  if (text) {
    const char *p = text;
    Atom expr;
    while (read_expr(p, &p, &expr) == Error_OK) {
      Atom result;
      Error err = eval_expr(expr, env, &result);
      if (err) {
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

int main() {

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
  env_set(env, make_symbol("T"), make_symbol("T"));
  load_file(env, "library.lisp");

  char *input;
  while (1) {
    input = readline("cutie> ");
    add_history(input);

    Error err;
    Atom sexpr, result;

    const char *p = input;
    err = read_expr(p, &p, &sexpr);

    if (!err) {
      err = eval_expr(sexpr, env, &result);
    }

    switch (err) {
      case Error_OK:
        print_expr(result);
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
    }
    free(input);
    putchar('\n');
  }

  return 0;
}
