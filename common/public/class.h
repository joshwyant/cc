#ifndef COMMON_PUBLIC_CLASS_H__
#define COMMON_PUBLIC_CLASS_H__

#include <stddef.h>

typedef struct Class Class;

#define DECLARE_VALUE_TYPE(name, T)                                            \
  typedef struct name name;                                                    \
  extern Class *name##Class;                                                   \
  name *name##_create(T value);                                                \
  T name##_unbox(name *obj);

DECLARE_VALUE_TYPE(Char, char)
DECLARE_VALUE_TYPE(Short, short)
DECLARE_VALUE_TYPE(Int, int)
DECLARE_VALUE_TYPE(Long, long)
DECLARE_VALUE_TYPE(Pointer, void *)
DECLARE_VALUE_TYPE(Float, float)
DECLARE_VALUE_TYPE(Double, double)
DECLARE_VALUE_TYPE(LongDouble, long double)
DECLARE_VALUE_TYPE(UnsignedChar, unsigned char)
DECLARE_VALUE_TYPE(UnsignedShort, unsigned short)
DECLARE_VALUE_TYPE(UnsignedInt, unsigned int)
DECLARE_VALUE_TYPE(UnsignedLong, unsigned long)
DECLARE_VALUE_TYPE(CString, char *)

typedef struct Object Object;
typedef struct Value Value;

extern Class *ObjectClass;
extern Class *ValueClass;

const char *Class_name(Class *c);
const size_t Class_sizeof(Class *c);
const bool Class_derived_from(Class *c, Class *parent);
Class *Object_class(void *o);
bool Object_is(void *o, Class *c);
size_t Object_sizeof(void *o);
int Object_hash(void *o);
bool Object_equals(void *a, void *b);
bool Object_valid(void *o);
void *Cast(void *o, Class *c);
void *Create(Class *c, ...);
void *CreateProtected(Class *c, ...);
void *Destroy(void *o);
void *Clone(void *o);

#endif
