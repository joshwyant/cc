#ifndef COMMON_PUBLIC_COLLECTION_H__
#define COMMON_PUBLIC_COLLECTION_H__

#include "array.h"
#include "assert.h"
#include "for_each_macros.h"
#include "forward_list.h"
#include "iterator.h"
#include "list.h"
#include "map.h"
#include "priority_queue.h"
#include "queue.h"
#include "set.h"
#include "stack.h"
#include "vector.h"

// Cartesians:
#define DOCOLLECTION(fn, t) EVAL_TUPLE(fn, (SEQ_FROM_TUPLE(t)))
#define DOMAP(k, v) EVAL_TUPLE(DECLARE_MAP, (SEQ_FROM_TUPLE(k), SEQ_FROM_TUPLE(v)))

#define DO_COLLECTIONS(...) \
    CARTESIAN(DOMAP, (__VA_ARGS__, (StringCase, char *)), (__VA_ARGS__)) \
    CARTESIAN(DOCOLLECTION, (DECLARE_VECTOR, DECLARE_LIST, DECLARE_ARRAY, DECLARE_FORWARD_LIST), (__VA_ARGS__))

DO_COLLECTIONS(
    (Char, char), 
    (Short, short), 
    (Int, int), 
    (Long, long), 
    (Ptr, void *), 
    (Float, float), 
    (Double, double), 
    (LongDouble, long double), 
    (UnsignedChar, unsigned char), 
    (UnsignedShort, unsigned short), 
    (UnsignedInt, unsigned int),
    (UnsignedLong, unsigned long), 
    (String, char*))

#endif // COMMON_PUBLIC_COLLECTION_H__
