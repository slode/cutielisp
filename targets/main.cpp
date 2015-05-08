#include <stdio.h>
#include <stdlib.h>

/* If we are compiling on Windows compile these functions */
#ifdef _WIN32
#include <string.h>

static char buffer[2048];

/* Fake readline function */
char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy)-1] = '\0';
  return cpy;
}

/* Fake add_history function */
void add_history(char* unused) {}

/* Otherwise include the editline headers */
#else
#include <readline/readline.h>
#include <readline/history.h>
#endif


struct Atom {
  enum {
    NIL,
    PAIR,
    SYMBOL,
    INTEGER
  } type;

  union {
    struct Pair *pair;
    const char* symbol;
    long int value;
  } value;
};

struct Pair {
  struct Atom atom[2];
};

typedef struct Atom Atom;

#define car(p) ((p).value.pair->atom[0])
#define cdr(p) ((p).value.pair->atom[1])
#define nilp(atom) ((atom).type == Atom.NIL)

static const Atom nil = {Atom::NIL};

Atom cons(Atom car_val, Atom cdr_val) {
  Atom p;
  p.type = Atom::PAIR;
  p.value.pair = (struct Pair*)malloc(sizeof(struct Pair));
  car(p) = car_val;
  cdr(p) = cdr_val;
  return p;
}

Atom make_int(long x) {
  Atom p;
  p.type = Atom::INTEGER;
  p.value.integer = x;
  return p;
}

Atom make_symbol(const char *s) {
  Atom p;
  p.type = Atom::SYMBOL;
  p.value.symbol = strdup(s);
  return p;
}

void print_expr(Atom atom) {
  switch (atom.type) {
    case Atom::NIL:
      printf("NIL");
      break;
    case Atom::INTEGER:
      printf("%ld", atom.value.integer);
      break;
    case Atom::PAIR:
      putchar('(');
      print_expr(car(atom));
      atom = cdr(atom);
      while (!nilp(atom)) {
        if (atom.type == Atom::PAIR) {
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
    case Atom::SYMBOL:
      printf("%s", atom.value.symbol);
      break;
  }
}

Atom* parse(char* s) {
  


}

Atom* eval(Atom *ast) {
  return ast;
}

void print_Atom(Atom *value) {

}

int main(int argc, char** argv) {
   
  puts("Lisp Version 0.1");
  puts("Press Ctrl+c to Exit\n");
   
  while (1) {
    
    /* Now in either case readline will be correctly defined */
    char* input = readline("lispy> ");
    add_history(input);

    Atom *ast = parse(input);
    Atom *value = eval(ast);
    print_atom(value);

    free(input);
  }
  return 0;
}
