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

#define DECLARE_ITERATOR_TYPE(name, type)                                      \
  typedef struct name##Iterator name##Iterator;                                \
  struct name##Iterator {                                                      \
    CollectionType collection_type;                                            \
    void *collection;                                                          \
    size_t elem_size;                                                          \
    type *(*current)(const name##Iterator *iter);                              \
    bool (*move_next)(name##Iterator * iter);                                  \
    bool (*eof)(const name##Iterator *iter);                                   \
    int impl_data1;                                                            \
    int impl_data2;                                                            \
    int version;                                                               \
  };

// Default (generic) iterator
DECLARE_ITERATOR_TYPE(, void)

#define DECLARE_SINK_TYPE(name, type)                                          \
  typedef struct name##Sink name##Sink;                                        \
  struct name##Sink {                                                          \
    CollectionType collection_type;                                            \
    void *collection;                                                          \
    size_t elem_size;                                                          \
    type *(*add)(const name##Sink *sink, const type *elem);                    \
    void *state;                                                               \
  };

// Default (generic) sink
DECLARE_SINK_TYPE(, void)

#define DECLARE_INDEXER_TYPE(name, type)                                       \
  typedef struct name##Indexer name##Indexer;                                  \
  struct name##Indexer {                                                       \
    CollectionType collection_type;                                            \
    void *collection;                                                          \
    size_t elem_size;                                                          \
    size_t (*size)(const name##Indexer *indexer);                              \
    type *(*get)(const name##Indexer *indexer, size_t index);                  \
    void (*set)(name##Indexer * indexer, size_t index, const type *value);     \
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

#define DECLARE_CONTAINER_FN(name, type, hash_fn, eq_fn)                       \
  DECLARE_ITERATOR_TYPE(name, type)                                            \
  DECLARE_INDEXER_TYPE(name, type)                                             \
  DECLARE_SINK_TYPE(name, type)                                                \
  extern KeyInfo name##KeyInfo;                                                \
  int hash_fn(const void *k);                                                  \
  bool eq_fn(const void *_a, const void *_b);                                  \
  void name##_for_each(name##Iterator *iter, void (*action)(type * elem));\
  type name##_reduce(const type initial, name##Iterator *iter,\
                     type (*reduce_fn)(const type accum, const type elem));

#define DECLARE_CONTAINER(name, type)                                          \
  DECLARE_CONTAINER_FN(name, type, name##_hash, name##_eq)

#define DEFINE_CONTAINER_FN(name, type, hash_fn, eq_fn)                        \
  KeyInfo name##KeyInfo = {sizeof(type), hash_fn, eq_fn};                      \
  void name##_for_each(name##Iterator *iter, void (*action)(type * elem)) {    \
    for_each((Iterator *)iter, (void (*)(void *))action);                      \
  }

#define DEFINE_CONTAINER(name, type, hash_expr, eq_expr)                       \
  int name##_hash(const void *k) {                                             \
    type key = *(const type *)k;                                               \
    return (hash_expr);                                                        \
  }                                                                            \
  bool name##_eq(const void *_a, const void *_b) {                             \
    type a = *(const type *)_a;                                                \
    type b = *(const type *)_b;                                                \
    return (eq_expr);                                                          \
  }                                                                            \
  type (*_##name##_current_reduce_fn)(const type, const type) = NULL;          \
  void _##name##__reduce(const void *dest, const void *elem) {                 \
    *(type *)dest =                                                            \
        _##name##_current_reduce_fn(*(const type *)dest, *(const type *)elem); \
  }                                                                            \
  type name##_reduce(const type initial, name##Iterator *iter,                 \
                     type (*reduce_fn)(const type accum, const type elem)) {   \
    type (*last_fn)(const type, const type) = _##name##_current_reduce_fn;     \
    _##name##_current_reduce_fn = reduce_fn;                                   \
    Iterator_reduce(&initial, (Iterator *)iter,                                \
                    (void (*)(void *, const void *))_##name##__reduce);        \
    _##name##_current_reduce_fn = last_fn;                                     \
    return initial;                                                            \
  }                                                                            \
  DEFINE_CONTAINER_FN(name, type, name##_hash, name##_eq)

#define DECLARE_RELATIONAL_CONTAINER_FN(name, type, hash_fn, compare_fn)       \
  DECLARE_CONTAINER_FN(name, type, hash_fn, name##_eq)                         \
  extern RelationalKeyInfo name##RelationalKeyInfo;                            \
  int compare_fn(const void *a, const void *b);

#define DECLARE_RELATIONAL_CONTAINER(name, type)                               \
  DECLARE_RELATIONAL_CONTAINER_FN(name, type, name##_hash, name##_compare)

#define DECLARE_CONTAINER_REDUCER(name, type, func_name) \
  type name##_##func_name(name##Iterator *iter);

#define DECLARE_RELATIONAL_CONTAINER_BASIC(name, type) \
  DECLARE_RELATIONAL_CONTAINER(name, type) \
  DECLARE_CONTAINER_REDUCER(name, type, sum) \
  DECLARE_CONTAINER_REDUCER(name, type, product) \
  DECLARE_CONTAINER_REDUCER(name, type, min) \
  DECLARE_CONTAINER_REDUCER(name, type, max) 

#define DEFINE_RELATIONAL_CONTAINER_FN(name, type, hash_fn, compare_fn)        \
  bool name##_eq(const void *a, const void *b) {                               \
    return name##_compare(a, b) == 0;                                          \
  }                                                                            \
  DEFINE_CONTAINER_FN(name, type, hash_fn, name##_eq)                          \
  RelationalKeyInfo name##RelationalKeyInfo = {&name##KeyInfo, compare_fn};

#define DEFINE_RELATIONAL_CONTAINER(name, type, hash_expr, compare_expr)       \
  int name##_compare(const void *_a, const void *_b) {                         \
    type a = *(const type *)_a;                                                \
    type b = *(const type *)_b;                                                \
    return (compare_expr);                                                     \
  }                                                                            \
  DEFINE_CONTAINER(name, type, hash_expr, (name##_compare(_a, _b) == 0))       \
  RelationalKeyInfo name##RelationalKeyInfo = {&name##KeyInfo, name##_compare};

#define DEFINE_CONTAINER_REDUCER(name, type, func_name, initial, expr) \
  type _##name##__##func_name(const type a, const type b) { return (expr); }\
  type name##_##func_name(name##Iterator *iter) {\
    return name##_reduce((initial), iter, _##name##__##func_name);\
  }

#define DEFINE_RELATIONAL_CONTAINER_BASIC(name, type)                          \
  DEFINE_RELATIONAL_CONTAINER(name, type, key * 7, a - b)\
  DEFINE_CONTAINER_REDUCER(name, type, sum, 0, a + b) \
  DEFINE_CONTAINER_REDUCER(name, type, product, 1, a * b) \
  DEFINE_CONTAINER_REDUCER(name, type, min, (type)0x7FFFFFFFFFFFFFFF, a <= b ? a : b) \
  DEFINE_CONTAINER_REDUCER(name, type, max, (type)0x8000000000000000, a >= b ? a : b)

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
