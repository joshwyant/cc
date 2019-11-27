#include "protected/vector.h"

#include <math.h>
#include <string.h>

#include "../test/stubs.h"
#include "public/collections.h"
#include "public/assert.h"

bool Vector_init(Vector *vector, size_t elem_size);

void Vector_cleanup(Vector *vector);

DEFINE_VECTOR(Int, int);
DEFINE_VECTOR(Long, long);
DEFINE_VECTOR(Char, char);
DEFINE_VECTOR(Float, float);
DEFINE_VECTOR(Double, double);
DEFINE_VECTOR(UnsignedInt, unsigned int);
DEFINE_VECTOR(UnsignedLong, unsigned long);
DEFINE_VECTOR(UnsignedChar, unsigned char);
DEFINE_VECTOR(CString, char *);

void _Vector_log(Vector *vector)
{
  char print_buffer[1000];
  LOG_FORMAT(TRACE, "_Vector_log(*vector: %p):", vector);
  ASSERT(vector);
  
  LOG_INDENT();
  LOG_FORMAT(TRACE, "%15s: %p", "*data", vector->data);
  for (size_t i = 0; i < vector->elem_count; i++) {
    vector->print(vector, print_buffer, vector->data + vector->elem_size * i);
    LOG_FORMAT(TRACE, "%10s[%3zu]:%s", "data", i, print_buffer);
  }
  LOG_FORMAT(TRACE, "%15s: %zu", "data_size", vector->data_size);
  LOG_FORMAT(TRACE, "%15s: %zu", "elem_size", vector->elem_size);
  LOG_FORMAT(TRACE, "%15s: %zu", "elem_count", vector->elem_count);
  LOG_FORMAT(TRACE, "%15s: %zu", "reserve_count", vector->reserve_count);
  LOG_FORMAT(TRACE, "%15s: %d", "version", vector->version);
  LOG_DEINDENT();
}

void _Vector_log_element(int level, Vector *vector, char *message, const void *element)
{
  char print_buffer[1000];
  vector->print(vector, print_buffer, element);
  LOG_FORMAT(TRACE, "%s: %s", message, print_buffer);
}

Vector *Vector_alloc(size_t elem_size) 
{
  LOG_FORMAT(TRACE, "Vector_alloc(elem_size: %zu) called", elem_size);
  ASSERT(elem_size > 0);

  Vector *vector = malloc(sizeof(Vector));
  LOG_FORMAT_INDENT(TRACE, "Vector allocated -> %p", vector);

  LOG_INDENT();
  if (vector && !Vector_init(vector, elem_size)) {
    free(vector);
    LOG_DEINDENT();
    return NULL;
  }
  LOG_DEINDENT();
  return vector;
}

void _Vector_default_print_fn(const Vector *self, char *str, const unsigned char *elem) {
  char byte[4];
  str[0] = '\0';
  for (size_t i = 0; i < self->elem_size; i++) {
    sprintf(byte, " %02x", elem[i]);
    strcat(str, byte);
  }
}

void Vector_print(Vector *vector, char *buffer, size_t index)
{
  vector->print(vector, buffer, Vector_get(vector, index));
}

bool Vector_init(Vector *vector, size_t elem_size) 
{
  LOG_FORMAT(TRACE, "Vector_init(*vector: %p, elem_size: %zu) called", vector, elem_size);
  ASSERT(vector != NULL);
  ASSERT(elem_size > 0);

  vector->data = NULL;
  vector->data_size = 0;
  vector->elem_size = elem_size;
  vector->elem_count = 0;
  vector->reserve_count = 0;
  vector->version = 0;
  vector->print = (void(*)(const Vector*, char *, const void *))_Vector_default_print_fn;
  return true;
}

size_t Vector_count(const Vector *vector) 
{
  ASSERT(vector != NULL);
  return vector->elem_count;
}

bool Vector_empty(const Vector *vector) 
{
  ASSERT(vector != NULL);
  bool result = vector->elem_count == 0;
  LOG_FORMAT_INDENT(TRACE, "Vector_empty = %d", result);
  return result;
}

size_t Vector_capacity(const Vector *vector) 
{
  ASSERT(vector != NULL);
  return vector->data_size / vector->elem_size;
}

bool _Vector_reserve_ext(Vector *vector, size_t num_elems, bool slim) {
  LOG_FORMAT(
      TRACE,
      "_Vector_reserve_ext(*vector: %p, num_elems: %zu, slim: %s) called",
      vector, num_elems, slim ? "true" : "false");
  ASSERT(vector != NULL);
  ASSERT(num_elems >= 0);
  LOG_INDENT();
  _Vector_log(vector);

  if (vector->reserve_count >= num_elems && Vector_capacity(vector) >= num_elems) {
    LOG_FORMAT(TRACE, "Already enough capacity (%zu >= %zu requested elements).", vector->reserve_count, num_elems);
    LOG_DEINDENT();
    return true; // No-op case
  }
  // Increase capacity by factor of 2
  size_t reserve_count = vector->reserve_count == 0 ? 1 : vector->reserve_count;
  while (reserve_count < num_elems) {
    reserve_count <<= 1;
  }
  size_t new_data_size = vector->elem_size * (slim ? num_elems : reserve_count);
  void *data = NULL;
  LOG_FORMAT(TRACE, "New capacity %zu bytes.", new_data_size);
  if ((data = realloc(vector->data, new_data_size)) == NULL) {
    LOG(TRACE, "Unable to realloc.");
    LOG_DEINDENT();
    return false;
  }
  vector->data = data;
  vector->reserve_count = reserve_count;
  vector->data_size = new_data_size;
  LOG(TRACE, "Vector resized. Updated vector:");
  _Vector_log(vector);
  LOG_DEINDENT();
  return true;
} // _Vector_reserve_ext

bool Vector_reserve(Vector *vector, size_t num_elems) 
{
  return _Vector_reserve_ext(vector, num_elems, true);
}

bool Vector_expand(Vector *vector, size_t num_elems)
{
  ASSERT(vector);
  ASSERT(num_elems > vector->elem_count);
  if (!Vector_reserve(vector, num_elems)) return false;
  memset(vector->data + vector->elem_count * vector->elem_size, 0,
         (num_elems - vector->elem_count) * vector->elem_size);
  vector->elem_count = num_elems;
  return true;
}

bool Vector_trim(Vector *vector) 
{
  LOG_FORMAT(TRACE, "Vector_trim(*vector: %p) called", vector);
  ASSERT(vector != NULL);

  if (vector->elem_count == 0) {
    LOG_FORMAT_INDENT(TRACE, "%s", "Nothing in vector, freeing data and re-initializing.");
    if (vector->data) {
      free(vector->data);
      vector->data = NULL;
    }
    vector->data_size = 0;
    vector->reserve_count = 0;
    return true;
  }
  if (vector->elem_count < vector->reserve_count) {
    // Allocate and copy the data
    void *data = realloc(vector->data, vector->elem_size * vector->elem_count);
    if (!data) {
      LOG_FORMAT_INDENT(TRACE, "%s", "Failed to reallocate smaller vector.");
      return false;
    }
    vector->data_size = vector->elem_size * vector->elem_count;
    vector->data = data;
    // Lower the potential capacity.
    while ((vector->reserve_count >> 1) >= vector->elem_count) {
      vector->reserve_count >>= 1;
    }
    LOG_FORMAT_INDENT(
        TRACE, "New *data = %ptr; new potential capacity: %zu, new count/capacity: %zu",
        vector->data, vector->reserve_count, vector->elem_count);
  } else {
    LOG_FORMAT_INDENT(
        TRACE, "Vector already small enough (%zu == %zu), nothing to do.",
        vector->elem_count, vector->reserve_count);
  }
  return true;
} // Vector_trim

void Vector_free(Vector *vector) 
{
  LOG_FORMAT(TRACE, "Vector_free(*vector: %p) called", vector);
  LOG_INDENT();
  // _Vector_log(vector); // Don't do this, the items are freed and can't be printed.
  ASSERT(vector != NULL);
  Vector_cleanup(vector);
  free(vector);
  LOG_DEINDENT();
}

void Vector_cleanup(Vector *vector) 
{
  LOG_FORMAT(TRACE, "Vector_cleanup(*vector: %p) called", vector);
  LOG_INDENT();
  ASSERT(vector != NULL);

  if (vector->data) {
    free(vector->data);
  }
  vector->data = NULL;
  vector->data_size = 0;
  vector->elem_count = 0;
  vector->reserve_count = 0;
  vector->version++;
  LOG_DEINDENT();
} // Vector_cleanup

size_t Vector_element_size(const Vector *vector) 
{
  ASSERT(vector != NULL);
  return vector->elem_size;
}

// Gets the data for the vector.
void *Vector_get_data(const Vector *vector) {
  ASSERT(vector != NULL);
  return vector->data;
}

// Gets the item at `index'
void *Vector_get(const Vector *vector, size_t index) 
{
  ASSERT(vector != NULL);
  ASSERT(index >= 0);
  ASSERT(index < vector->elem_count && "index is greater than vector size.");

  return vector->data + vector->elem_size * index;
}

// Sets the item at `index' to the given data.
// `index' must be within the bounds of the vector.
void Vector_set(Vector *vector, size_t index, const void *elem) 
{
  Vector_set_range(vector, index, elem, 1);
}

// Sets the data at `index' to the given data.
// `index' and data must be within the bounds of the vector.
void Vector_set_range(Vector *vector, size_t index, const void *elems, int count) 
{
  ASSERT(vector != NULL);
  ASSERT(index >= 0);
  ASSERT(index < vector->elem_count && "index is >= vector size.");
  ASSERT(index + count <= vector->elem_count &&
         "index + count is greater than vector size.");
  ASSERT(elems != NULL);
  ASSERT(count >= 0);

  memcpy(vector->data + vector->elem_size * index, elems, vector->elem_size * count);

  vector->version++;
}

void *Vector_insert(Vector *vector, size_t index, const void *elem) 
{
  return Vector_insert_range(vector, index, elem, 1);
}

void *Vector_insert_range(Vector *vector, size_t index, const void *elems,
                        size_t count) 
{
  LOG_FORMAT(TRACE,
             "Vector_insert_range(*vector: %p, index: %zu, *elems: %p, count: "
             "%zu) called",
             vector, index, elems, count);
  ASSERT(vector != NULL);
  ASSERT(index >= 0);
  ASSERT(elems != NULL);
  ASSERT(count >= 0);
  ASSERT(index <= vector->elem_count);
  LOG_INDENT();

  vector->version++;
  if (count == 0) {
    goto out_null; // No-op case
  }
  if (!_Vector_reserve_ext(vector, vector->elem_count + count, false)) {
    goto out_null;
  }
  size_t shifted_count = vector->elem_count - index;
  size_t shifted_size = vector->elem_size * shifted_count;
  size_t data_size = vector->elem_size * count;
  void *insertion_dest = vector->data + vector->elem_size * index;
  // Shift the data to make room if not appending
  if (index != vector->elem_count) {
    LOG_FORMAT(TRACE, "Shifting %zu elems (%zu bytes)", shifted_count, shifted_size);
    memmove(insertion_dest + data_size, insertion_dest, shifted_size);
  }
  if (elems !=
      insertion_dest) { // a trick to shift the data without initializing
    LOG_FORMAT(TRACE, "Copying %zu bytes from %p to %p", data_size, elems, insertion_dest);
    // Copy the elements to the vector
    memcpy(insertion_dest, elems, data_size);
  }
  vector->elem_count += count;
  LOG(TRACE, "New vector contents:");
  _Vector_log(vector);
  LOG_DEINDENT();
  return insertion_dest;
out_null:
  LOG_DEINDENT();
  return NULL;
} // Vector_insert_range

// Appends the given element to the end of the vector.
// Returns a pointer to the appended element. Returns NULL if unsuccessful.
void *Vector_add(Vector *vector, const void *elem) 
{
  _Vector_log_element(TRACE, vector, "Adding element", elem);
  return Vector_insert_range(vector, vector->elem_count, elem, 1);
}

// Appends the given element to the end of the vector.
// Returns a pointer to the appended element. Returns NULL if unsuccessful.
void *Vector_add_range(Vector *vector, const void *elems, size_t count) 
{
  return Vector_insert_range(vector, vector->elem_count, elems, count);
}

// Removes an element from the vector.
void Vector_remove(Vector *vector, size_t start_index)
{
  Vector_remove_range(vector, start_index, 1); // Additional assertions done here
}

void Vector_clear(Vector *vector) 
{
  Vector_truncate(vector, 0);
}

void Vector_truncate(Vector *vector, size_t start_index) 
{
  Vector_remove_range(vector, start_index, vector->elem_count - start_index);
}

// Removes a range of elements from the vector.
void Vector_remove_range(Vector *vector, size_t start_index, size_t count) 
{
  LOG_FORMAT(
      TRACE,
      "Vector_remove_range(*vector: %p, start_index: %zu, count: %zu) called",
      vector, start_index, count);
  ASSERT(vector != NULL);
  ASSERT(start_index >= 0);
  ASSERT(start_index <= vector->elem_count);
  ASSERT(count >= 0);
  ASSERT(start_index + count <= vector->elem_count);
  LOG_INDENT();

  if (count == vector->elem_count) {
    LOG(TRACE, "Clearing the entire vector.");
    Vector_cleanup(vector); // clean slate; everything short of freeing the vector.
    LOG_DEINDENT();
    return;
  }
  vector->version++;
  void *new_data = vector->data;
  // Can we reduce the size of the vector by at least half?
  if (vector->reserve_count >> 1 >= vector->elem_count - count) {
    size_t old_capacity = vector->reserve_count;
    do {
      vector->reserve_count >>= 1;
    } while (vector->reserve_count >> 1 > vector->elem_count - count);
    LOG_FORMAT(TRACE, "Reducing the capacity of the vector from %zu to %zu...",
               old_capacity, vector->reserve_count);
    // Use malloc for less copying.
    new_data = malloc(vector->reserve_count * vector->elem_size);
    if (new_data == NULL) {
      new_data = vector->data;
      LOG(TRACE, "Size reduction was unsuccessful.");
    } else {
      if (vector->data && start_index > 0) {
        // Copy the first part of the old data.
        memcpy(new_data, vector->data, start_index * vector->elem_size);
      }
    }
  }
  if (count > 0 && start_index + count < vector->elem_count) {
    // Shift the end of the vector down
    memmove(vector->elem_size * start_index + new_data,
           vector->elem_size * (start_index + count) + vector->data,
           vector->elem_size * (vector->elem_count - (start_index + count)));
  }
  if (new_data && new_data != vector->data) {
    free(vector->data);
    LOG_FORMAT(TRACE, "Old, freed data: %p; new data: %p", vector->data, new_data);
  }
  vector->elem_count -= count;
  vector->data = new_data;
  LOG(TRACE, "Updated vector:");
  _Vector_log(vector);
  LOG_DEINDENT();
} // Vector_remove_range

// Copies a vector. Returns whether successful.
bool Vector_copy(Vector *dest_vector, const Vector *vector) 
{
  LOG_FORMAT(TRACE, "Vector_copy(*dest_vector: %p, *vector: %p) called", dest_vector, vector);
  ASSERT(dest_vector != NULL);
  ASSERT(vector != NULL);
  ASSERT(vector->elem_size == dest_vector->elem_size);
  // Is there enough space?
  if (dest_vector->reserve_count != vector->reserve_count ||
      dest_vector->data_size / dest_vector->elem_size < vector->elem_count) {
    Vector_cleanup(dest_vector);
    if (!Vector_reserve(dest_vector, vector->elem_count)) {
      return false;
    }
  }
  memcpy(dest_vector->data, vector->data, vector->elem_size * vector->elem_count);
  dest_vector->elem_count = vector->elem_count;
  LOG(TRACE, "New, copied vector:");
  _Vector_log(dest_vector);
  return true;
} // Vector_copy

void *Vector_iter_current_(const Iterator *iter);
void *Vector_iter_current_reverse_(const Iterator *iter);
bool Vector_iter_eof_(const Iterator *iter);
bool Vector_iter_eof_reverse_(const Iterator *iter);
bool Vector_iter_move_next_(Iterator *iter);
bool Vector_iter_move_next_reverse_(Iterator *iter);

void *Vector_iter_current_(const Iterator *iter) 
{
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_VECTOR);
  Vector *vector = iter->collection;
  ASSERT(vector != NULL);
  ASSERT(iter->version == vector->version &&
         "Collection changed while iterating.");
  if (Vector_iter_eof_(iter)) {
    return NULL;
  }
  return Vector_get(vector, iter->impl_data1);
}

void *Vector_iter_current_reverse_(const Iterator *iter) 
{
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_VECTOR);
  Vector *vector = iter->collection;
  ASSERT(vector != NULL);
  ASSERT(iter->version == vector->version &&
         "Collection changed while iterating.");
  if (Vector_iter_eof_reverse_(iter)) {
    return NULL;
  }
  return Vector_get(vector, iter->impl_data1);
}

bool Vector_iter_eof_(const Iterator *iter) 
{
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_VECTOR);
  Vector *vector = iter->collection;
  ASSERT(vector != NULL);
  ASSERT(iter->version == vector->version &&
         "Collection changed while iterating.");
  return Vector_empty(vector) || iter->impl_data1 >= (int)vector->elem_count;
}

bool Vector_iter_eof_reverse_(const Iterator *iter) 
{
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_VECTOR);
  Vector *vector = iter->collection;
  ASSERT(vector != NULL);
  ASSERT(iter->version == vector->version &&
         "Collection changed while iterating.");
  return Vector_empty(vector) || iter->impl_data1 < 0;
}

bool Vector_iter_move_next_(Iterator *iter) 
{
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_VECTOR);
  Vector *vector = iter->collection;
  ASSERT(vector != NULL);
  ASSERT(iter->version == vector->version &&
         "Collection changed while iterating.");
  if (Vector_iter_eof_(iter)) {
    return false;
  }
  iter->impl_data1++;
  return !Vector_iter_eof_(iter);
}

bool Vector_iter_move_next_reverse_(Iterator *iter) 
{
  ASSERT(iter != NULL);
  ASSERT(iter->collection_type == COLLECTION_VECTOR);
  Vector *vector = iter->collection;
  ASSERT(vector != NULL);
  ASSERT(iter->version == vector->version &&
         "Collection changed while iterating.");
  if (Vector_iter_eof_reverse_(iter)) {
    return false;
  }
  iter->impl_data1--;
  return !Vector_iter_eof_reverse_(iter);
}

// Gets an Iterator for this Vector
void Vector_get_iterator(const Vector *vector, Iterator *iter) 
{
  ASSERT(vector != NULL);
  ASSERT(iter != NULL);
  iter->collection_type = COLLECTION_VECTOR;
  iter->collection = (void *)vector;
  iter->elem_size = vector->elem_size;
  iter->current = Vector_iter_current_;
  iter->eof = Vector_iter_eof_;
  iter->move_next = Vector_iter_move_next_;
  iter->impl_data1 = -1; // Current index
  iter->impl_data2 = 0;
  iter->version = vector->version;
}

void Vector_get_reverse_iterator(const Vector *vector, Iterator *iter) 
{
  ASSERT(vector != NULL);
  ASSERT(iter != NULL);
  iter->collection_type = COLLECTION_VECTOR;
  iter->collection = (void *)vector;
  iter->elem_size = vector->elem_size;
  iter->current = Vector_iter_current_reverse_;
  iter->eof = Vector_iter_eof_reverse_;
  iter->move_next = Vector_iter_move_next_reverse_;
  iter->impl_data1 = vector->elem_count; // Current index
  iter->impl_data2 = 0;
  iter->version = vector->version;
}

void *Vector_sink_add_(Sink *sink, const void *elem);

void Vector_get_sink(const Vector *vector, Sink *sink) 
{
  ASSERT(vector != NULL);
  ASSERT(sink != NULL);
  sink->collection_type = COLLECTION_VECTOR;
  sink->collection = (void *)vector;
  sink->elem_size = vector->elem_size;
  sink->add = Vector_sink_add_;
}

void *Vector_sink_add_(Sink *sink, const void *elem) 
{
  ASSERT(sink != NULL);
  Vector *vector = sink->collection;
  ASSERT(vector != NULL);
  return Vector_add(vector, elem);
}

size_t Vector_indexer_size_(const Indexer *indexer);
void *Vector_indexer_get_(const Indexer *indexer, size_t index);
void Vector_indexer_set_(Indexer *indexer, size_t index, const void *data);

// Gets an Indexer for this vector
void Vector_get_indexer(const Vector *vector, Indexer *indexer) 
{
  ASSERT(vector != NULL);
  ASSERT(indexer != NULL);
  indexer->collection_type = COLLECTION_VECTOR;
  indexer->collection = (void *)vector;
  indexer->elem_size = vector->elem_size;
  indexer->size = Vector_indexer_size_;
  indexer->get = Vector_indexer_get_;
  indexer->set = Vector_indexer_set_;
}

size_t Vector_indexer_size_(const Indexer *indexer) 
{
  ASSERT(indexer != NULL);
  Vector *vector = indexer->collection;
  ASSERT(vector != NULL);
  return vector->elem_count;
}

void *Vector_indexer_get_(const Indexer *indexer, size_t index) 
{
  ASSERT(indexer != NULL);
  Vector *vector = indexer->collection;
  ASSERT(vector != NULL);
  ASSERT(index >= 0 && index < Vector_count(vector));
  return vector->data + vector->elem_size * index;
}

void Vector_indexer_set_(Indexer *indexer, size_t index, const void *data) 
{
  ASSERT(indexer != NULL);
  Vector *vector = indexer->collection;
  ASSERT(vector != NULL);
  ASSERT(index >= 0 && index < Vector_count(vector));
  ASSERT(data != NULL);
  memcpy(vector->data + vector->elem_size * index, data, vector->elem_size);
}

void Vector_print_fn(Vector *vector, void (*print)(const Vector *, char*, const void*)) {
  ASSERT(vector);
  vector->print = print;
}
