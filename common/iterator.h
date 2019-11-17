#ifndef COMMON_ITERATOR_H__
#define COMMON_ITERATOR_H__

#include <stdbool.h>
#include <stddef.h>

typedef struct Iterator Iterator;

typedef struct Sink Sink;

typedef struct Indexer Indexer;

typedef struct KeyInfo KeyInfo;

typedef struct RelationalKeyInfo RelationalKeyInfo;

typedef struct KeyValuePair KeyValuePair;

typedef enum {
    COLLECTION_NONE,
    COLLECTION_ARRAY            = 1 << 0,
    COLLECTION_LIST             = 1 << 1,
    COLLECTION_LINKED_LIST      = 1 << 2,
    COLLECTION_BACK_LIST        = 1 << 3,
    COLLECTION_STACK            = 1 << 4,
    COLLECTION_QUEUE            = 1 << 5,
    COLLECTION_PRIORITY_QUEUE   = 1 << 6,
    COLLECTION_SET              = 1 << 7,
    COLLECTION_MAP              = 1 << 8,
    COLLECTION_CUSTOM           = 1 << 9,
} CollectionType;

struct Iterator {
    CollectionType collection_type;
    void *collection;
    size_t elem_size;
    void *(*current)(const Iterator *iter);
    bool (*move_next)(Iterator *iter);
    bool (*eof)(const Iterator *iter);
    int impl_data1;
    int impl_data2;
    int version;
};

struct Sink {
    CollectionType collection_type;
    void *collection;
    size_t elem_size;
    void *(*add)(const Sink *sink, const void *elem);
};

struct Indexer {
    CollectionType collection_type;
    void *collection;
    size_t elem_size;
    size_t (*size)(const Indexer *indexer);
    void *(*get)(const Indexer *indexer, size_t index);
};

struct KeyInfo {
    size_t key_size;
    int (*hash_fn)(const void *key);
    bool (*eq_fn)(const void *key_a, const void *key_b);
};

struct RelationalKeyInfo {
    KeyInfo key_info;
    int (*comare_fn)(const void *a, const void *b);
};

struct KeyValuePair {
    void *key;
    void *value;
};

void for_each(Iterator *iter, void (*action)(void *elem));

void for_each_ext(Iterator *iter, void (*action)(void *elem, const Iterator *iter));

bool iter_copy(const Sink *dest, Iterator *iter);

void indexer_sort(const Indexer *indexer, int (*compare_fn)(const void *a, const void *b));

bool iter_sort(const Sink *dest, Iterator *iter, int (*compare_fn)(const void *a, const void *b));

void iter_map(const Sink *dest, Iterator *iter, void (*map_fn)(void *dest, const void *elem));

void iter_flat_map(const Sink *dest, Iterator *iter, void (*map_fn)(Sink *dest, const void *elem));

void iter_filter(const Sink *dest, Iterator *iter, bool (*filter_fn)(const void *elem));

int int_compare(const void *a, const void *b);

int long_compare(const void *a, const void *b);

int char_compare(const void *a, const void *b);

int float_compare(const void *a, const void *b);

int double_compare(const void *a, const void *b);

int unsigned_int_compare(const void *a, const void *b);

int unsigned_long_compare(const void *a, const void *b);

int unsigned_char_compare(const void *a, const void *b);

int string_compare(const void *a, const void *b);

int string_case_compare(const void *a, const void *b);

int int_hash(const void *key);

int long_hash(const void *key);

int char_hash(const void *key);

int float_hash(const void *key);

int double_hash(const void *key);

int unsigned_int_hash(const void *key);

int unsigned_long_hash(const void *key);

int unsigned_char_hash(const void *key);

int string_hash(const void *key);

int string_case_hash(const void *key);

#endif // COMMON_ITERATOR_H__
