#ifndef COMMON_ITERATOR_H__
#define COMMON_ITERATOR_H__

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
  int (*comare_fn)(const void *a, const void *b);
};

struct KeyValuePair {
  void *key;
  void *value;
};

void for_each(Iterator *iter, void (*action)(void *elem));

void for_each_ext(Iterator *iter,
                  void (*action)(void *elem, const Iterator *iter));

bool iter_copy(const Sink *dest, Iterator *iter);

void indexer_sort(const Indexer *indexer,
                  int (*compare_fn)(const void *a, const void *b));

bool iter_sort(const Sink *dest, Iterator *iter,
               int (*compare_fn)(const void *a, const void *b));

void iter_map(const Sink *dest, Iterator *iter,
              void (*map_fn)(void *dest, const void *elem));

void iter_flat_map(const Sink *dest, Iterator *iter,
                   void (*map_fn)(Sink *dest, const void *elem));

void iter_filter(const Sink *dest, Iterator *iter,
                 bool (*filter_fn)(const void *elem));

#define DECLARE_CONTAINER_FN(name, type, hash_fn, eq_fn)                       \
  DECLARE_ITERATOR_TYPE(name, type)                                            \
  DECLARE_INDEXER_TYPE(name, type)                                             \
  DECLARE_SINK_TYPE(name, type)                                                \
  extern KeyInfo name##KeyInfo;                                                \
  int hash_fn(const void *k);                                                  \
  bool eq_fn(const void *_a, const void *_b);

#define DECLARE_CONTAINER(name, type)                                          \
  DECLARE_CONTAINER_FN(name, type, name##Hash, name##Eq)

#define DEFINE_CONTAINER_FN(name, type, hash_fn, eq_fn)                        \
  KeyInfo name##KeyInfo = {sizeof(type), hash_fn, eq_fn};

#define DEFINE_CONTAINER(name, type, hash_expr, eq_expr)                       \
  int name##Hash(const void *k) {                                              \
    type key = *(const type *)k;                                               \
    return (hash_expr);                                                        \
  }                                                                            \
  bool name##Eq(const void *_a, const void *_b) {                              \
    type a = *(const type *)_a;                                                \
    type b = *(const type *)_b;                                                \
    return (eq_expr);                                                          \
  }                                                                            \
  DEFINE_CONTAINER_FN(name, type, name##Hash, name##Eq)

#define DECLARE_RELATIONAL_CONTAINER_FN(name, type, hash_fn, compare_fn)       \
  DECLARE_CONTAINER_FN(name, type, hash_fn, name##Eq)                          \
  extern RelationalKeyInfo name##RelationalKeyInfo;                            \
  int compare_fn(const void *a, const void *b);

#define DECLARE_RELATIONAL_CONTAINER(name, type)                               \
  DECLARE_RELATIONAL_CONTAINER_FN(name, type, name##Hash, name##Compare)

#define DEFINE_RELATIONAL_CONTAINER_FN(name, type, hash_fn, compare_fn)        \
  bool name##Eq(const void *a, const void *b) {                                \
    return name##Compare(a, b) == 0;                                           \
  }                                                                            \
  DEFINE_CONTAINER_FN(name, type, hash_fn, name##Eq)                           \
  RelationalKeyInfo name##RelationalKeyInfo = {&name##KeyInfo, compare_fn};

#define DEFINE_RELATIONAL_CONTAINER(name, type, hash_expr, compare_expr)       \
  int name##Compare(const void *_a, const void *_b) {                          \
    type a = *(const type *)_a;                                                \
    type b = *(const type *)_b;                                                \
    return (compare_expr);                                                     \
  }                                                                            \
  DEFINE_CONTAINER(name, type, hash_expr, (name##Compare(_a, _b) == 0))        \
  RelationalKeyInfo name##RelationalKeyInfo = {&name##KeyInfo, name##Compare};

#define DEFINE_RELATIONAL_CONTAINER_BASIC(name, type)                          \
  DEFINE_RELATIONAL_CONTAINER(name, type, key * 7, a - b)

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

#endif // COMMON_ITERATOR_H__
