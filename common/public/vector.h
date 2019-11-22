#ifndef COMMON_PUBLIC_VECTOR_H__
#define COMMON_PUBLIC_VECTOR_H__

#include <stdbool.h>
#include <stddef.h>

#include "iterator.h"

// A dynamically growing vector.
typedef struct Vector Vector;

// Creates a new Vector object.
Vector *Vector_alloc(size_t elem_size);

// Gets the number of elements in the Vector
size_t Vector_count(const Vector *vector);

// Returns whether the vector is empty.
bool Vector_empty(const Vector *vector);

// Gets the capacity of the Vector
size_t Vector_capacity(const Vector *vector);

// Reserves memory for a total of `num_elems' objects.
// Capacity always increases by a factor of 2.
// Returns whether the allocation was successful.
bool Vector_reserve(Vector *vector, size_t num_elems);

// Reserves memory for a total of `num_elems' objects.
// Increases the count of elements in the vector,
// but does not initialize the new objects.
// Capacity always increases by a factor of 2.
// Returns whether the allocation was successful.
bool Vector_expand(Vector *vector, size_t num_elems);

// Removes excess memory for the Vector object.
// Returns whether the allocation was successful.
bool Vector_trim(Vector *vector);

// Frees up the Vector object.
void Vector_free(Vector *vector);

// Gets the size of an element in the Vector
size_t Vector_element_size(const Vector *vector);

// Gets the data for the vector.
void *Vector_get_data(const Vector *vector);

// Gets the item at `index'
void *Vector_get(const Vector *vector, size_t index);

// Sets the item at `index' to the given data.
// `index' must be within the bounds of the vector.
void Vector_set(Vector *vector, size_t index, const void *elem);

// Sets the data at `index' to the given data.
// `index' and data must be within the bounds of the vector.
void Vector_set_range(Vector *vector, size_t index, const void *elem, int count);

// Inserts the given element somewhere in the middle of the vector.
// Returns a pointer to the inserted data. Returns NULL if unsuccessful.
void *Vector_insert(Vector *vector, size_t index, const void *elem);

// Inserts the given elements somewhere in the middle of the vector.
// Returns a pointer to the inserted data. Returns NULL if unsuccessful.
void *Vector_insert_range(Vector *vector, size_t index, const void *elems,
                        size_t count);

// Appends the given element to the vector.
// Returns a pointer to the appended data. Returns NULL if unsuccessful.
void *Vector_add(Vector *vector, const void *elem);

// Appends the given elements to the vector.
// Returns a pointer to the appended data. Returns NULL if unsuccessful.
void *Vector_add_range(Vector *vector, const void *elems, size_t count);

// Removes an element from the vector.
void Vector_remove(Vector *vector, size_t start_index);

// Removes all the items from the vector.
// Remember to clean up memory first.
void Vector_clear(Vector *vector);

// Removes a range of elements from the vector.
void Vector_remove_range(Vector *vector, size_t start_index, size_t count);

// Truncates a vector by removing elements after `start_index'.
void Vector_truncate(Vector *vector, size_t start_index);

// Copies a vector. Returns whether successful.
bool Vector_copy(Vector *dest_vector, const Vector *vector);

// Gets an Iterator for this Vector
void Vector_get_iterator(const Vector *vector, Iterator *iter);

// Gets a reverse Iterator for this Vector
void Vector_get_reverse_iterator(const Vector *vector, Iterator *iter);

// Gets a Sink for this vector
void Vector_get_sink(const Vector *vector, Sink *sink);

// Gets an Indexer for this vector
void Vector_get_indexer(const Vector *vector, Indexer *indexer);

#define DECLARE_VECTOR(name, T)                                                  \
  /* A dynamically growing vector. */                                            \
  typedef struct name##Vector name##Vector;                                        \
  /* Creates a new name##Vector object. */                                       \
  name##Vector *name##Vector_alloc();                                              \
  /* Gets the data for the vector. */                                            \
  T *name##Vector_get_data(const name##Vector *vector);                              \
  /* Gets the item at `index' */                                               \
  T name##Vector_get(const name##Vector *self, size_t index);                      \
  /* Gets the item at `index' */                                               \
  T *name##Vector_get_ref(const name##Vector *self, size_t index);                 \
  /* Sets the item at `index' to the given data. `index' must be within the    \
   * bounds of the vector. */                                                    \
  void name##Vector_set(name##Vector *self, size_t index, const T elem);           \
  /* Sets the item at `index' to the given data. `index' must be within the    \
   * bounds of the vector. */                                                    \
  void name##Vector_set_ref(name##Vector *self, size_t index, const T *elem);      \
  /* Sets the data at `index' to the given data. `index' and data must be      \
   * within the bounds of the vector. */                                         \
  void name##Vector_set_range(name##Vector *self, size_t index, const T *elem,     \
                            int count);                                        \
  /* Inserts the given element somewhere in the middle of the vector. Returns a  \
   * pointer to the inserted data. Returns NULL if unsuccessful. */            \
  T *name##Vector_insert(name##Vector *self, size_t index, const T elem);          \
  /* Inserts the given element somewhere in the middle of the vector. Returns a  \
   * pointer to the inserted data. Returns NULL if unsuccessful. */            \
  T *name##Vector_insert_ref(name##Vector *self, size_t index, const T *elem);     \
  /* Inserts the given elements somewhere in the middle of the vector. Returns a \
   * pointer to the inserted data. Returns NULL if unsuccessful. */            \
  T *name##Vector_insert_range(name##Vector *self, size_t index, const T *elems,   \
                             size_t count);                                    \
  /* Appends the given element to the vector. Returns a pointer to the appended  \
   * data. Returns NULL if unsuccessful. */                                    \
  T *name##Vector_add(name##Vector *self, const T elem);                           \
  /* Appends the given element to the vector. Returns a pointer to the appended  \
   * data. Returns NULL if unsuccessful. */                                    \
  T *name##Vector_add_ref(name##Vector *self, const T *elem);                      \
  /* Appends the given elements to the vector. Returns a pointer to the appended \
   * data. Returns NULL if unsuccessful. */                                    \
  T *name##Vector_add_range(name##Vector *self, const T *elems, size_t count);     \
  /* Gets an Iterator for this name##Vector */                                   \
  void name##Vector_get_iterator(const name##Vector *self, name##Iterator *iter);  \
  /* Gets a reverse Iterator for this name##Vector */                            \
  void name##Vector_get_reverse_iterator(const name##Vector *self,                 \
                                       name##Iterator *iter);                  \
  /* Gets a Sink for this vector */                                              \
  void name##Vector_get_sink(const name##Vector *self, name##Sink *sink);          \
  /* Gets an Indexer for this vector */                                          \
  void name##Vector_get_indexer(const name##Vector *self, name##Indexer *indexer);

#define DEFINE_VECTOR(name, T)                                                   \
  name##Vector *name##Vector_alloc() {                                             \
    return (name##Vector *)Vector_alloc(sizeof(T));                                \
  }                                                                            \
  T *name##Vector_get_data(const name##Vector *self) {                             \
    return (T *)Vector_get_data((const Vector *)self);                             \
  }                                                                            \
  T name##Vector_get(const name##Vector *self, size_t index) {                     \
    return *(T *)Vector_get((const Vector *)self, index);                          \
  }                                                                            \
  T *name##Vector_get_ref(const name##Vector *self, size_t index) {                \
    return (T *)Vector_get((const Vector *)self, index);                           \
  }                                                                            \
  void name##Vector_set(name##Vector *self, size_t index, const T elem) {          \
    Vector_set((Vector *)self, index, &elem);                                      \
  }                                                                            \
  void name##Vector_set_ref(name##Vector *self, size_t index, const T *elem) {     \
    Vector_set((Vector *)self, index, elem);                                       \
  }                                                                            \
  void name##Vector_set_range(name##Vector *self, size_t index, const T *elem,     \
                            int count) {                                       \
    Vector_set_range((Vector *)self, index, elem, count);                          \
  }                                                                            \
  T *name##Vector_insert(name##Vector *self, size_t index, const T elem) {         \
    return (T *)Vector_insert((Vector *)self, index, &elem);                       \
  }                                                                            \
  T *name##Vector_insert_ref(name##Vector *self, size_t index, const T *elem) {    \
    return (T *)Vector_insert((Vector *)self, index, elem);                        \
  }                                                                            \
  T *name##Vector_insert_range(name##Vector *self, size_t index, const T *elems,   \
                             size_t count) {                                   \
    return (T *)Vector_insert_range((Vector *)self, index, elems, count);          \
  }                                                                            \
  T *name##Vector_add(name##Vector *self, const T elem) {                          \
    return (T *)Vector_add((Vector *)self, &elem);                                 \
  }                                                                            \
  T *name##Vector_add_ref(name##Vector *self, const T *elem) {                     \
    return (T *)Vector_add((Vector *)self, elem);                                  \
  }                                                                            \
  T *name##Vector_add_range(name##Vector *self, const T *elems, size_t count) {    \
    return (T *)Vector_add_range((Vector *)self, elems, count);                    \
  }                                                                            \
  void name##Vector_get_iterator(const name##Vector *self, name##Iterator *iter) { \
    Vector_get_iterator((const Vector *)self, (Iterator *)iter);                   \
  }                                                                            \
  void name##Vector_get_reverse_iterator(const name##Vector *self,                 \
                                       name##Iterator *iter) {                 \
    Vector_get_reverse_iterator((const Vector *)self, (Iterator *)iter);           \
  }                                                                            \
  void name##Vector_get_sink(const name##Vector *self, name##Sink *sink) {         \
    Vector_get_sink((const Vector *)self, (Sink *)sink);                           \
  }                                                                            \
  void name##Vector_get_indexer(const name##Vector *self,                          \
                              name##Indexer *indexer) {                        \
    Vector_get_indexer((const Vector *)self, (Indexer *)indexer);                  \
  }

#endif // COMMON_PUBLIC_VECTOR_H__
