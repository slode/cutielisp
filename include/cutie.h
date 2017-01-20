
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

struct Atom;
typedef Error (*Builtin)(struct Atom args, struct Atom *result);

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
static const Atom nil;

#define car(p) ((p).value.pair->atom[0])
#define cdr(p) ((p).value.pair->atom[1])
#define nilp(atom) ((atom).type == ATOM_NIL)

Error make_error(
    int type,
    const char *message,
    const char *file_name,
    const char *function_name,
    int line_number);

#define ERROR(type, message) make_error(type, message, __FILE__, __FUNCTION__, __LINE__)
#define ERROR_OK() make_error(0, "", __FILE__, __FUNCTION__, __LINE__)
#define ERROR_RAISED(err) (err.type != Error_OK)


Atom cons(Atom car_val, Atom cdr_val);
Atom make_integer(long x);
Atom make_string(const char *s);
Atom make_symbol(const char *s);
Atom make_builtin(Builtin fn);
Error make_closure(Atom env, Atom args, Atom body, Atom *result);

/* Builtins */
Error builtin_add(Atom args, Atom *result);
Error builtin_subtract(Atom args, Atom *result);
Error builtin_multiply(Atom args, Atom *result);
Error builtin_divide(Atom args, Atom *result);

Error builtin_numeq(Atom args, Atom *result);
Error builtin_less(Atom args, Atom *result);

Error builtin_car(Atom args, Atom *result);
Error builtin_cdr(Atom args, Atom *result);
Error builtin_cons(Atom args, Atom *result);

Error builtin_stringeq(Atom args, Atom *result);
Error builtin_stringless(Atom args, Atom *result);
Error builtin_stringconcat(Atom args, Atom *result);
Error builtin_stringsubstr(Atom args, Atom *result);

Error apply(Atom fn, Atom args, Atom *result);
Error builtin_apply(Atom args, Atom *result);
Error builtin_eq(Atom args, Atom *result);

Error builtin_pairp(Atom args, Atom *result);
Error builtin_stringp(Atom args, Atom *result);
Error builtin_symbolp(Atom args, Atom *result);
Error builtin_numberp(Atom args, Atom *result);
Error builtin_error(Atom args, Atom *result);

/* ENV */
Atom create_env(Atom parent);
Atom setup_env();

Error env_get(Atom env, Atom symbol, Atom *result);
Error env_set(Atom env, Atom symbol, Atom value);
Error env_set_existing(Atom env, Atom symbol, Atom value);

/* IO */
void print_expr(Atom atom);
void print_error(Error err);
Error lex(const char *str, const char **start, const char **end);
Error read_expr(const char *input, const char **end, Atom *result);
Error parse_simple(const char *start, const char *end, Atom *result);
Error read_string(const char *input, const char **end, Atom *result);
Error read_list(const char *start, const char **end, Atom *result);
Error read_expr(const char *input, const char **end, Atom *result);

/* Evaluation */
int listp(Atom expr);
Atom copy_list(Atom list);
Error eval_expr(Atom expr, Atom env, Atom *result);
Error apply(Atom fn, Atom args, Atom *result);

/* Load list code */
char *slurp(const char *path);
void load_file(Atom env, const char *path);
