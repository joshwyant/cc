#ifndef COMMON_PUBLIC_ITERATOR_H__
#define COMMON_PUBLIC_ITERATOR_H__

#include <stdbool.h>
#include <stddef.h>

typedef struct KeyInfo KeyInfo;

typedef struct RelationalKeyInfo RelationalKeyInfo;

typedef struct KeyValuePair KeyValuePair;

typedef enum {
  COLLECTION_NONE,
  COLLECTION_ARRAY = 1 << 0,
  COLLECTION_LIST = 1 << 1,
  COLLECTION_LINKED_LIST = 1 << 2,
  COLLECTION_BACK_LIST = 1 << 3,
  COLLECTION_STACK = 1 << 4,
  COLLECTION_QUEUE = 1 << 5,
  COLLECTION_PRIORITY_QUEUE = 1 << 6,
  COLLECTION_SET = 1 << 7,
  COLLECTION_MAP = 1 << 8,
  COLLECTION_CUSTOM = 1 << 9,
} CollectionType;

#define DECLARE_ITERATOR_TYPE(name, T)                                         \
  typedef struct name##Iterator name##Iterator;                                \
  struct name##Iterator {                                                      \
    CollectionType collection_type;                                            \
    void *collection;                                                          \
    size_t elem_size;                                                          \
    T *(*current)(const name##Iterator *iter);                                 \
    bool (*move_next)(name##Iterator * iter);                                  \
    bool (*eof)(const name##Iterator *iter);                                   \
    long long impl_data1;                                                      \
    long long impl_data2;                                                      \
    int version;                                                               \
  };

// Default (generic) iterator
DECLARE_ITERATOR_TYPE(, void)

#define DECLARE_SINK_TYPE(name, T)                                             \
  typedef struct name##Sink name##Sink;                                        \
  struct name##Sink {                                                          \
    CollectionType collection_type;                                            \
    void *collection;                                                          \
    size_t elem_size;                                                          \
    T *(*add)(name##Sink * sink, const T *elem);                               \
    void *state;                                                               \
  };

// Default (generic) sink
DECLARE_SINK_TYPE(, void)

#define DECLARE_INDEXER_TYPE(name, T)                                          \
  typedef struct name##Indexer name##Indexer;                                  \
  struct name##Indexer {                                                       \
    CollectionType collection_type;                                            \
    void *collection;                                                          \
    size_t elem_size;                                                          \
    size_t (*size)(const name##Indexer *indexer);                              \
    T *(*get)(const name##Indexer *indexer, size_t index);                     \
    void (*set)(name##Indexer * indexer, size_t index, const T *value);        \
  };

// Default (generic) indexer
DECLARE_INDEXER_TYPE(, void)

struct KeyInfo {
  size_t key_size;
  int (*hash_fn)(const void *key);
  bool (*eq_fn)(const void *key_a, const void *key_b);
};

struct RelationalKeyInfo {
  KeyInfo *key_info;
  int (*compare_fn)(const void *a, const void *b);
};

struct KeyValuePair {
  void *key;
  void *value;
};

void for_each(Iterator *iter, void (*action)(void *elem));

void for_each_ext(Iterator *iter,
                  void (*action)(void *elem, const Iterator *iter));

bool Iterator_copy(const Sink *dest, Iterator *iter);

void Indexer_sort(const Indexer *indexer,
                  int (*compare_fn)(const void *a, const void *b));

bool Iterator_sort(const Sink *dest, Iterator *iter,
                   int (*compare_fn)(const void *a, const void *b));

void Iterator_map(const Sink *dest, Iterator *iter,
                  void (*map_fn)(void *dest, const void *elem));

void Iterator_flat_map(const Sink *dest, Iterator *iter,
                       void (*map_fn)(Sink *dest, const void *elem));

// TODO: sum, product, etc.
void Iterator_reduce(const void *dest, Iterator *iter,
                     void (*reduce_fn)(void *dest, const void *elem));

void Iterator_filter(const Sink *dest, Iterator *iter,
                     bool (*filter_fn)(const void *elem));

int String_compare(const void *a, const void *b);

int StringCase_compare(const void *a, const void *b);

int String_hash(const void *key);

int StringCase_hash(const void *key);

#define DECLARE_CONTAINER_FN(name, T, hash_fn, eq_fn)                          \
  DECLARE_ITERATOR_TYPE(name, T)                                               \
  DECLARE_INDEXER_TYPE(name, T)                                                \
  DECLARE_SINK_TYPE(name, T)                                                   \
  extern KeyInfo name##KeyInfo;                                                \
  int hash_fn(const void *k);                                                  \
  bool eq_fn(const void *_a, const void *_b);                                  \
  void name##_for_each(name##Iterator *iter, void (*action)(T * elem));        \
  T name##_reduce(const T initial, name##Iterator *iter,                       \
                  T (*reduce_fn)(const T accum, const T elem));                \
  bool name##_eof(const name##Iterator *iter);                                 \
  T name##_next(const name##Iterator *iter);

#define DECLARE_CONTAINER(name, T)                                             \
  DECLARE_CONTAINER_FN(name, T, name##_hash, name##_eq)

#define DEFINE_CONTAINER_FN(name, T, hash_fn, eq_fn)                           \
  KeyInfo name##KeyInfo = {sizeof(T), hash_fn, eq_fn};                         \
  void name##_for_each(name##Iterator *iter, void (*action)(T * elem)) {       \
    for_each((Iterator *)iter, (void (*)(void *))action);                      \
  }                                                                            \
  bool name##_eof(const name##Iterator *iter) {                                \
    return iter->eof((const Iterator *)iter);                                  \
  }                                                                            \
  T name##_next(const name##Iterator *iter) {                                  \
    ASSERT(!iter->eof(iter));                                                  \
    return *(T *)iter->move_next(iter);                                        \
  }

#define DEFINE_CONTAINER(name, T, hash_expr, eq_expr)                          \
  int name##_hash(const void *k) {                                             \
    T key = *(const T *)k;                                                     \
    return (hash_expr);                                                        \
  }                                                                            \
  bool name##_eq(const void *_a, const void *_b) {                             \
    T a = *(const T *)_a;                                                      \
    T b = *(const T *)_b;                                                      \
    return (eq_expr);                                                          \
  }                                                                            \
  T (*_##name##_current_reduce_fn)(const T, const T) = NULL;                   \
  void _##name##__reduce(const void *dest, const void *elem) {                 \
    *(T *)dest =                                                               \
        _##name##_current_reduce_fn(*(const T *)dest, *(const T *)elem);       \
  }                                                                            \
  T name##_reduce(const T initial, name##Iterator *iter,                       \
                  T (*reduce_fn)(const T accum, const T elem)) {               \
    T (*last_fn)(const T, const T) = _##name##_current_reduce_fn;              \
    _##name##_current_reduce_fn = reduce_fn;                                   \
    Iterator_reduce(&initial, (Iterator *)iter,                                \
                    (void (*)(void *, const void *))_##name##__reduce);        \
    _##name##_current_reduce_fn = last_fn;                                     \
    return initial;                                                            \
  }                                                                            \
  DEFINE_CONTAINER_FN(name, T, name##_hash, name##_eq)

#define DECLARE_RELATIONAL_CONTAINER_FN(name, T, hash_fn, compare_fn)          \
  DECLARE_CONTAINER_FN(name, T, hash_fn, name##_eq)                            \
  extern RelationalKeyInfo name##RelationalKeyInfo;                            \
  int compare_fn(const void *a, const void *b);

#define DECLARE_RELATIONAL_CONTAINER(name, T)                                  \
  DECLARE_RELATIONAL_CONTAINER_FN(name, T, name##_hash, name##_compare)

#define DECLARE_CONTAINER_REDUCER(name, T, func_name)                          \
  T name##_##func_name(name##Iterator *iter);

#define DECLARE_RELATIONAL_CONTAINER_BASIC(name, T)                            \
  DECLARE_RELATIONAL_CONTAINER(name, T)                                        \
  DECLARE_CONTAINER_REDUCER(name, T, sum)                                      \
  DECLARE_CONTAINER_REDUCER(name, T, product)                                  \
  DECLARE_CONTAINER_REDUCER(name, T, min)                                      \
  DECLARE_CONTAINER_REDUCER(name, T, max)

#define DEFINE_RELATIONAL_CONTAINER_FN(name, T, hash_fn, compare_fn)           \
  bool name##_eq(const void *a, const void *b) {                               \
    return name##_compare(a, b) == 0;                                          \
  }                                                                            \
  DEFINE_CONTAINER_FN(name, T, hash_fn, name##_eq)                             \
  RelationalKeyInfo name##RelationalKeyInfo = {&name##KeyInfo, compare_fn};

#define DEFINE_RELATIONAL_CONTAINER(name, T, hash_expr, compare_expr)          \
  int name##_compare(const void *_a, const void *_b) {                         \
    T a = *(const T *)_a;                                                      \
    T b = *(const T *)_b;                                                      \
    return (compare_expr);                                                     \
  }                                                                            \
  DEFINE_CONTAINER(name, T, hash_expr, (name##_compare(_a, _b) == 0))          \
  RelationalKeyInfo name##RelationalKeyInfo = {&name##KeyInfo, name##_compare};

#define DEFINE_CONTAINER_REDUCER(name, T, func_name, initial, expr)            \
  T _##name##__##func_name(const T a, const T b) { return (expr); }            \
  T name##_##func_name(name##Iterator *iter) {                                 \
    return name##_reduce((initial), iter, _##name##__##func_name);             \
  }

#define DEFINE_RELATIONAL_CONTAINER_BASIC(name, T)                             \
  DEFINE_RELATIONAL_CONTAINER(name, T, key * 7, a - b)                         \
  DEFINE_CONTAINER_REDUCER(name, T, sum, 0, a + b)                             \
  DEFINE_CONTAINER_REDUCER(name, T, product, 1, a *b)                          \
  DEFINE_CONTAINER_REDUCER(name, T, min, (T)0x7FFFFFFFFFFFFFFF,                \
                           a <= b ? a : b)                                     \
  DEFINE_CONTAINER_REDUCER(name, T, max, (T)0x8000000000000000, a >= b ? a : b)

DECLARE_RELATIONAL_CONTAINER(Int, int);

DECLARE_RELATIONAL_CONTAINER(Long, long);

DECLARE_RELATIONAL_CONTAINER(Char, char);

DECLARE_RELATIONAL_CONTAINER(Float, float);

DECLARE_RELATIONAL_CONTAINER(Double, double);

DECLARE_RELATIONAL_CONTAINER(UnsignedInt, unsigned int);

DECLARE_RELATIONAL_CONTAINER(UnsignedLong, unsigned long);

DECLARE_RELATIONAL_CONTAINER(UnsignedChar, unsigned char);

DECLARE_RELATIONAL_CONTAINER(String, char *);

DECLARE_RELATIONAL_CONTAINER(StringCase, char *);

#endif // COMMON_PUBLIC_ITERATOR_H__
