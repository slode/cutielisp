#ifndef TYPES_H
#define TYPES_H

typedef struct Error {
  enum {
    Error_OK = 0,
    Error_Syntax,
    Error_UnBound,
    Error_Args,
    Error_Type,
    Error_DivideByZero,
    Error_OutOfBounds,
  } type;

  const char *message;
  const char *file_name;
  const char *function_name;
  int line_number;
} Error;

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
    ATOM_ERROR,
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

typedef Error (*Builtin)(struct Atom args, struct Atom *result);

#endif
