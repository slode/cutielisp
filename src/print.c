#include <stdio.h>
#include "cutie.h"

void print_expr(Atom atom) {
  switch (atom.type) {
    case ATOM_NIL:
      printf("NIL");
      break;
    case ATOM_INTEGER:
      printf("%ld", atom.value.integer);
      break;
    case ATOM_REAL:
      printf("%lf", atom.value.real);
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

void print_error(Error err)
{
  switch (err.type) {
    case Error_OK:
      return;
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
