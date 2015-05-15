#ifndef TYPES_H
#define TYPES_H

typedef enum {
  Error_OK = 0,
  Error_Syntax,
  Error_UnBound,
  Error_Args,
  Error_Type,
  Error_DivideByZero,
} Error;


struct Atom;
typedef int (*Builtin)(struct Atom args, struct Atom *result);

struct Atom {
  enum {
    ATOM_NIL,
    ATOM_PAIR,
    ATOM_SYMBOL,
    ATOM_INTEGER,
    ATOM_BUILTIN,
    ATOM_CLOSURE,
    ATOM_MACRO,
    ATOM_STRING,
  } type;

  union {
    struct Pair *pair;
    char* symbol;
    char* string;
    long int integer;
    Builtin builtin;
  } value;
};


struct Pair {
  struct Atom atom[2];
};

typedef struct Atom Atom;

#endif
