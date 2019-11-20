#ifndef COMMON_PUBLIC_MAP_H__
#define COMMON_PUBLIC_MAP_H__

#include <stdbool.h>
#include <stddef.h>

#include "iterator.h"

// A map data structure.
typedef struct Map Map;

// Creates a new Map object.
Map *Map_alloc(KeyInfo *key_info, size_t elem_size);

// Gets the number of elements in the Map
size_t Map_count(const Map *map);

// Returns whether the map is empty
bool Map_empty(const Map *map);

// Frees up the Map object.
void Map_free(Map *map);

// Gets the size of an element in the Map
size_t Map_element_size(const Map *map);

// Gets the key info for the Map
const KeyInfo *Map_key_info(const Map *map);

// Copies the key and value to the map and returns pointer to new key/value
// pair. Returns NULL in the key if unsuccessful.
const KeyValuePair Map_add(Map *map, const void *key, const void *data);

// Looks up the key in map and returns the key/value pair if found.
const KeyValuePair Map_find(const Map *map, const void *key);

// Looks up the key in map and stores the data in the given location.
// Returns whether successful.
bool Map_get(const Map *map, const void *key, void *data_out);

// Checks whether the given key exists in the map.
bool Map_exists(const Map *map, const void *key);

// Removes an item from the map.
void Map_delete(Map *map, const void *key);

// Removes all the items from the map.
// Remember to clean up memory first.
void Map_clear(Map *map);

// Copies a map. Returns whether successful.
bool Map_copy(Map *dest_map, const Map *map);

// Creates the union of two maps.
bool Map_union(Map *dest_map, const Map *a, const Map *b);

// Creates the intersection of two maps.
bool Map_intersection(Map *dest_map, const Map *a, const Map *b);

// Creates the difference of two maps.
bool Map_difference(Map *dest_map, const Map *a, const Map *b);

// Creates the symmetric difference of two maps.
bool Map_symmetric_difference(Map *dest_map, const Map *a, const Map *b);

// Stores the union of two maps into the first map.
bool Map_union_with(Map *dest_map, const Map *map);

// Stores the intersection of two maps into the first map.
bool Map_intersect_with(Map *dest_map, const Map *map);

// Stores the difference of two maps into the first map.
bool Map_difference_with(Map *dest_map, const Map *map);

// Stores the symmetric difference of two maps into the first map.
bool Map_symmetric_difference_with(Map *dest_map, const Map *map);

// Gets a key/value Iterator for this Map in an undefined order.
void Map_get_iterator(const Map *map, Iterator *iter);

// Gets a key Iterator for this Map in an undefined order.
void Map_get_key_iterator(const Map *map, Iterator *iter);

// Gets a value Iterator for this Map in an undefined order.
void Map_get_value_iterator(const Map *map, Iterator *iter);

#endif // COMMON_PUBLIC_MAP_H__