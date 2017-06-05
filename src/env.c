#include "cutie.h"

/* ENV */
Atom create_env(Atom parent) {
  return cons(parent, nil);
}

Atom setup_env() {
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
  env_set(env, make_symbol("PRINT"), make_builtin(builtin_print));

  /* these are implemented in eval */
  env_set(env, make_symbol("DEFINE"), make_symbol("DEFINE"));
  env_set(env, make_symbol("DEFMACRO"), make_symbol("DEFMACRO"));
  env_set(env, make_symbol("IF"), make_symbol("IF"));
  env_set(env, make_symbol("LAMBDA"), make_symbol("LAMBDA"));
  env_set(env, make_symbol("LOAD"), make_symbol("LOAD"));
  env_set(env, make_symbol("PROGN"), make_symbol("PROGN"));
  env_set(env, make_symbol("QUOTE"), make_symbol("QUOTE"));
  env_set(env, make_symbol("SET!"), make_symbol("SET!"));
  env_set(env, make_symbol("WHILE"), make_symbol("WHILE"));
  return env;
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
