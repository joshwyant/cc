#ifndef COMMON_PROTECTED_CLASS_H__
#define COMMON_PROTECTED_CLASS_H__

#include "../public/class.h"

#include <stdarg.h>
#include <stddef.h>

typedef void (*ctor_t)(void *self, va_list argp);
typedef void (*dtor_t)(void *self);
typedef void *(*cast_t)(void *self, Class *to);
typedef bool (*implements_t)(Class *c);
typedef void *(*clone_t)(void *o);
typedef bool (*equals_t)(void *a, void *b);
typedef int (*hash_t)(void *self);

typedef struct ObjectMethods ObjectMethods;

struct Class
{
    const size_t size;
    const char *name;
    const bool is_publicly_constructible;
    const ctor_t ctor;
    const dtor_t dtor;
    const cast_t cast;
    const implements_t implements;
    const clone_t clone;
};

struct Object
{
    unsigned int magic;
    Class *type;
    Object *derived_ptr;
    ObjectMethods *vtable;
};
struct ObjectMethods
{
    equals_t equals;
    hash_t hash;
};
extern ObjectMethods *DefaultObjectMethodTable;

// Must be called in constructor for composed objects of multiple
// inheritance or interface implementation.
void Object_derive(Object *composed_obj, Object *super_obj);

void Object_ctor(Object *self, va_list argp);
void Object_dtor(Object *self);
void *Object_cast(Object *self, Class *to);
bool Object_implements(Class *c);
void *Object_clone(Object *self);

struct Value
{
    struct Object base;
    char boxed_data[0];
};

void Value_ctor(void *self, va_list argp);
void *Value_cast(Value *self, Class *to);
bool Value_implements(Class *c);
void *Value_clone(Value *self);

#define DECLARE_VALUE_TYPE(name, T)                                            \
  struct name {                                                                \
    struct Value base;                                                         \
    T value;                                                                   \
  };                                                                           \
  void name##_ctor(Value *self, va_list argp);                                 \
  void *name##_cast(Value *self, Class *to);                                   \
  bool name##_implements(Class *c);

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

#endif // COMMON_PROTECTED_CLASS_H__
