#ifndef COMMON_PUBLIC_FOR_EACH_MACROS_H__
#define COMMON_PUBLIC_FOR_EACH_MACROS_H__

#define CONCATENATE(arg1, arg2)   CONCATENATE1(arg1, arg2)
#define CONCATENATE1(arg1, arg2)  CONCATENATE2(arg1, arg2)
#define CONCATENATE2(arg1, arg2)  arg1##arg2

// Places a pair of arguments side-by-side.
#define PAIR(a, b) a b

// Turns the arument list into a comma-separated tuple surrounded by parentheses.
#define TUPLE(...) (__VA_ARGS__)

// Expands the sequence listed in the arguments into a comma-separated list.
#define SEQ(...) __VA_ARGS__

#define EVAL(F, ...) PAIR(F, __VA_ARGS__)

// Expands the macro F with the arguments in `tuple'.
#define EVAL_TUPLE(F, tuple) PAIR(F, tuple)

// Expands the tuple into a sequence.
#define SEQ_FROM_TUPLE(tuple) EVAL_TUPLE(SEQ, tuple)

#define FOR_EACH_1(what, x) what(x)
#define FOR_EACH_2(what, x, ...)\
  what(x)\
  FOR_EACH_1(what,  __VA_ARGS__)
#define FOR_EACH_3(what, x, ...)\
  what(x)\
  FOR_EACH_2(what, __VA_ARGS__)
#define FOR_EACH_4(what, x, ...)\
  what(x)\
  FOR_EACH_3(what,  __VA_ARGS__)
#define FOR_EACH_5(what, x, ...)\
  what(x)\
 FOR_EACH_4(what,  __VA_ARGS__)
#define FOR_EACH_6(what, x, ...)\
  what(x)\
  FOR_EACH_5(what,  __VA_ARGS__)
#define FOR_EACH_7(what, x, ...)\
  what(x)\
  FOR_EACH_6(what,  __VA_ARGS__)
#define FOR_EACH_8(what, x, ...)\
  what(x)\
  FOR_EACH_7(what,  __VA_ARGS__)
#define FOR_EACH_9(what, x, ...)\
  what(x)\
  FOR_EACH_8(what,  __VA_ARGS__)
#define FOR_EACH_10(what, x, ...)\
  what(x)\
  FOR_EACH_9(what,  __VA_ARGS__)

#define FOR_EACH_NARG(...) FOR_EACH_NARG_(__VA_ARGS__, FOR_EACH_RSEQ_N())
#define FOR_EACH_NARG_(...) FOR_EACH_ARG_N(__VA_ARGS__) 
#define FOR_EACH_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N 
#define FOR_EACH_RSEQ_N() 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define FOR_EACH_(N, what, ...) CONCATENATE(FOR_EACH_, N)(what, __VA_ARGS__)
// Do something for each argument.
#define FOR_EACH(what, ...) FOR_EACH_(FOR_EACH_NARG(__VA_ARGS__), what, __VA_ARGS__)

#define FOR_EACH1_1(what, a, x) what(a, x)
#define FOR_EACH1_2(what, a, x, ...)\
  what(a, x)\
  FOR_EACH1_1(what,  a, __VA_ARGS__)
#define FOR_EACH1_3(what, a, x, ...)\
  what(a, x)\
  FOR_EACH1_2(what, a, __VA_ARGS__)
#define FOR_EACH1_4(what, a, x, ...)\
  what(a, x)\
  FOR_EACH1_3(what, a,  __VA_ARGS__)
#define FOR_EACH1_5(what, a, x, ...)\
  what(a, x)\
 FOR_EACH1_4(what, a,  __VA_ARGS__)
#define FOR_EACH1_6(what, a, x, ...)\
  what(a, x)\
  FOR_EACH1_5(what, a,  __VA_ARGS__)
#define FOR_EACH1_7(what, a, x, ...)\
  what(a, x)\
  FOR_EACH1_6(what, a,  __VA_ARGS__)
#define FOR_EACH1_8(what, a, x, ...)\
  what(a, x)\
  FOR_EACH1_7(what, a,  __VA_ARGS__)
#define FOR_EACH1_9(what, a, x, ...)\
  what(a, x)\
  FOR_EACH1_8(what, a,  __VA_ARGS__)
#define FOR_EACH1_10(what, a, x, ...)\
  what(a, x)\
  FOR_EACH1_9(what, a,  __VA_ARGS__)
#define FOR_EACH1_11(what, a, x, ...)\
  what(a, x)\
  FOR_EACH1_10(what, a,  __VA_ARGS__)
#define FOR_EACH1_12(what, a, x, ...)\
  what(a, x)\
  FOR_EACH1_11(what, a,  __VA_ARGS__)
#define FOR_EACH1_13(what, a, x, ...)\
  what(a, x)\
  FOR_EACH1_12(what, a,  __VA_ARGS__)
#define FOR_EACH1_14(what, a, x, ...)\
  what(a, x)\
  FOR_EACH1_13(what, a,  __VA_ARGS__)
#define FOR_EACH1_15(what, a, x, ...)\
  what(a, x)\
  FOR_EACH1_14(what, a,  __VA_ARGS__)
#define FOR_EACH1_16(what, a, x, ...)\
  what(a, x)\
  FOR_EACH1_15(what, a,  __VA_ARGS__)

#define FOR_EACH1_NARG(...) FOR_EACH1_NARG_(__VA_ARGS__, FOR_EACH1_RSEQ_N())
#define FOR_EACH1_NARG_(...) FOR_EACH1_ARG_N(__VA_ARGS__) 
#define FOR_EACH1_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, N, ...) N 
#define FOR_EACH1_RSEQ_N() 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define FOR_EACH1_(N, what, a, ...) CONCATENATE(FOR_EACH1_, N)(what, a, __VA_ARGS__)
// Do something for each argument.
#define FOR_EACH1(what, a, ...) FOR_EACH1_(FOR_EACH1_NARG(__VA_ARGS__), what, a, __VA_ARGS__)

#define FOR_EACH2_1(what, a, b, x) what(a, b, x)
#define FOR_EACH2_2(what, a, b, x, ...)\
  what(a, b, x)\
  FOR_EACH2_1(what,  a, b, __VA_ARGS__)
#define FOR_EACH2_3(what, a, b, x, ...)\
  what(a, b, x)\
  FOR_EACH2_2(what, a, b, __VA_ARGS__)
#define FOR_EACH2_4(what, a, b, x, ...)\
  what(a, b, x)\
  FOR_EACH2_3(what, a, b,  __VA_ARGS__)
#define FOR_EACH2_5(what, a, b, x, ...)\
  what(a, b, x)\
 FOR_EACH2_4(what, a, b,  __VA_ARGS__)
#define FOR_EACH2_6(what, a, b, x, ...)\
  what(a, b, x)\
  FOR_EACH2_5(what, a, b,  __VA_ARGS__)
#define FOR_EACH2_7(what, a, b, x, ...)\
  what(a, b, x)\
  FOR_EACH2_6(what, a, b,  __VA_ARGS__)
#define FOR_EACH2_8(what, a, b, x, ...)\
  what(a, b, x)\
  FOR_EACH2_7(what, a, b,  __VA_ARGS__)
#define FOR_EACH2_9(what, a, b, x, ...)\
  what(a, b, x)\
  FOR_EACH2_8(what, a, b,  __VA_ARGS__)
#define FOR_EACH2_10(what, a, b, x, ...)\
  what(a, b, x)\
  FOR_EACH2_9(what, a, b,  __VA_ARGS__)
#define FOR_EACH2_11(what, a, b, x, ...)\
  what(a, b, x)\
  FOR_EACH2_10(what, a, b,  __VA_ARGS__)
#define FOR_EACH2_12(what, a, b, x, ...)\
  what(a, b, x)\
  FOR_EACH2_11(what, a, b,  __VA_ARGS__)
#define FOR_EACH2_13(what, a, b, x, ...)\
  what(a, b, x)\
  FOR_EACH2_12(what, a, b,  __VA_ARGS__)
#define FOR_EACH2_14(what, a, b, x, ...)\
  what(a, b, x)\
  FOR_EACH2_13(what, a, b,  __VA_ARGS__)
#define FOR_EACH2_15(what, a, b, x, ...)\
  what(a, b, x)\
  FOR_EACH2_14(what, a, b,  __VA_ARGS__)
#define FOR_EACH2_16(what, a, b, x, ...)\
  what(a, b, x)\
  FOR_EACH2_15(what, a, b,  __VA_ARGS__)

#define FOR_EACH2_NARG(...) FOR_EACH2_NARG_(__VA_ARGS__, FOR_EACH2_RSEQ_N())
#define FOR_EACH2_NARG_(...) FOR_EACH2_ARG_N(__VA_ARGS__) 
#define FOR_EACH2_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, N, ...) N 
#define FOR_EACH2_RSEQ_N() 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define FOR_EACH2_(N, what, a, b, ...) CONCATENATE(FOR_EACH2_, N)(what, a, b, __VA_ARGS__)
// Do something for each argument.
#define FOR_EACH2(what, a, b, ...) FOR_EACH2_(FOR_EACH2_NARG(__VA_ARGS__), what, a, b, __VA_ARGS__)

#define CARTESIAN_(what, Y, x) FOR_EACH1(what, x, SEQ_FROM_TUPLE(Y))
#define CARTESIAN(what, X, Y) FOR_EACH2(CARTESIAN_, what, Y, SEQ_FROM_TUPLE(X))

#endif // COMMON_PUBLIC_FOR_EACH_MACROS_H__
