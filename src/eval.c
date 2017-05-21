#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cutie.h"

const Atom nil = {ATOM_NIL, {0}};

/* IO */

int nilp(Atom atom)
{
  return atom.type == ATOM_NIL;
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

int error_raised(Error err) {
  return err.type != Error_OK;
}

