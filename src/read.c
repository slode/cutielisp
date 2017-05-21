#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "cutie.h"

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

Error parse_simple(const char *start, const char *end, Atom *result)
{
  char *buf, *p;

  /* Is it an integer? */
  long ival = strtol(start, &p, 10);
  if (p == end) {
    result->type = ATOM_INTEGER;
    result->value.integer = ival;
    return ERROR_OK();
  }

  /* Is it a real number? */
  double dval = strtod(start, &p);
  if (p == end) {
    result->type = ATOM_REAL;
    result->value.real = dval;
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
