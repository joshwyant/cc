#include "protected/class.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "public/assert.h"
#include "../test/stubs.h"

#define OBJECT_MAGIC 0xDEFA0B

Class objectClass = {
    sizeof(Object),
    "Object",
    true,
    (ctor_t)Object_ctor,
    (dtor_t)Object_dtor,
    (cast_t)Object_cast,
    (implements_t)Object_implements,
    (clone_t)Object_clone,
}, *ObjectClass = &objectClass;

Class valueClass = {
    sizeof(Value),
    "Value",
    false,
    (ctor_t)Value_ctor,
    objectClass.dtor,
    (cast_t)Value_cast,
    (implements_t)Value_implements,
    (clone_t)Value_clone,
}, *ValueClass = &valueClass;

ObjectMethods defaultObjectMethodTable = {
    (equals_t)Object_equals,
    (hash_t)Object_hash
}, *ObjectMethodTable = &defaultObjectMethodTable;

void Object_ctor(Object *object, va_list argp)
{
    ASSERT(object);
    object->magic = OBJECT_MAGIC;
    object->type = ObjectClass;
    object->derived_ptr = object;
    object->vtable = DefaultObjectMethodTable;
}
void Object_dtor(Object *self)
{
    // Default: Nothing to do
}

void *Object_cast(Object *self, Class *to)
{
    ASSERT(self);
    if (to == ObjectClass) {
        return self->derived_ptr;
    }
    return NULL;
}

bool Object_equals(void *a, void *b)
{
    ASSERT(Object_valid(a) && Object_valid(b));
    Object *obja = Cast(a, ObjectClass), *objb = Cast(b, ObjectClass);
    if (obja->vtable->hash == Object_hash || !obja->vtable->hash) {
        return obja == objb;
    }
    return obja->vtable->equals(a, b);
}

int Object_hash(void *o)
{
    ASSERT(Object_valid(o));
    Object *obj = Cast(o, ObjectClass);
    if (obj->vtable->hash == Object_hash || !obj->vtable->hash) {
        return 0; // Default hash. Terrible, don't use!
    }
    return obj->vtable->hash(o);
}

bool Object_implements(Class *c)
{
    return c == ObjectClass;
}

void *Object_clone(Object *o)
{
    ASSERT(Object_valid(o));
    return Create(ObjectClass);
}

void Value_ctor(void *self, va_list argp)
{
    Object_ctor(self, argp);
    ((Object *)self)->type = ValueClass;
}

void Value_dtor(void *self)
{
    Object_dtor((void *)self);
}

void *Value_cast(Value *self, Class *to)
{
    if (to == ValueClass) {
        return self;
    }
    if (Value_implements(to)) {
        return Object_cast(&self->base, to);
    }
    return NULL;
}

void Object_derive(Object *composed_obj, Object *super_obj)
{
    ASSERT(Object_valid(composed_obj));
    ASSERT(Object_valid(super_obj));
    composed_obj->derived_ptr = super_obj;
}

bool Value_implements(Class *c)
{
    ASSERT(c);
    return c == ValueClass || Object_implements(c);
}

// Default implementation for derived types
void *Value_clone(Value *v)
{
    ASSERT(Object_valid((Object *)v));
    return Create(v->base.type, v->boxed_data);
}

const char *Class_name(Class *c)
{
    ASSERT(c);
    return c->name;
}

const size_t Class_sizeof(Class *c)
{
    ASSERT(c);
    return c->size;
}

bool Class_derived_from(Class *c, Class *parent)
{
    ASSERT(c && parent);
    return c->implements(parent);
}

Class *Object_class(void *o)
{
    ASSERT(Object_valid(o));
    Object *obj = Cast(o, ObjectClass);
    return obj->type;
}

bool Object_is(void *o, Class *c)
{
    return Class_derived_from();
}

size_t Object_sizeof(void *o)
{
    ASSERT(Object_valid(o));
    Object *obj = Cast(o, ObjectClass);
    return obj->type->size;
}

bool Object_valid(void *o)
{
    return o != NULL && *(unsigned int*)(o) == OBJECT_MAGIC;
}

void *Cast(void *o, Class *c)
{
    ASSERT(Object_valid(o));
    Object *base = ((Object *)o)->derived_ptr;
    return base->type->cast(o, c);
}

void *vCreate(Class *c, va_list argp)
{
    Object *self = calloc(1, c->size);
    c->ctor(self, argp);
    return self;
}

void *Create(Class *c, ...)
{
    ASSERT(c);
    ASSERT(c->is_publicly_constructible);
    va_list argp;
    va_start(argp, c);
    void *self = vCreate(c, argp);
    va_end(argp);
    return self;
}

void *CreateProtected(Class *c, ...)
{
    ASSERT(c);
    va_list argp;
    va_start(argp, c);
    void *self = vCreate(c, argp);
    va_end(argp);
    return self;
}

void *Destroy(void *o)
{
    ASSERT(Object_valid(o));
    Object *obj = Cast(o, ObjectClass);
    obj->type->dtor(o);
    free(obj);
}

void *Clone(void *o)
{
    ASSERT(Object_valid(o));
    Object *obj = Cast(o, ObjectClass);
    return obj->type->clone(o);
}

int Char_hash(char *value) { return 0; /* TODO */ }
int Short_hash(short *value) { return 0; /* TODO */ }
int Int_hash(int *value) { return 0; /* TODO */ }
int Long_hash(long *value) { return 0; /* TODO */ }
int Pointer_hash(void **value) { return 0; /* TODO */ }
int Float_hash(float *value) { return 0; /* TODO */ }
int Double_hash(double *value) { return 0; /* TODO */ }
int LongDouble_hash(long double *value) { return 0; /* TODO */ }
int UnsignedChar_hash(unsigned char *value) { return 0; /* TODO */ }
int UnsignedShort_hash(unsigned short *value) { return 0; /* TODO */ }
int UnsignedInt_hash(unsigned int *value) { return 0; /* TODO */ }
int UnsignedLong_hash(unsigned long *value) { return 0; /* TODO */ }
int CString_hash(char **value) { return 0; /* TODO */ }

bool CString_equals(void *a, void *b) {
  ASSERT(Object_valid(a) && Object_valid(b));
  if (a == b)
    return true;
  Object *obja = Cast(a, ObjectClass), *objb = Cast(b, ObjectClass);
  if (obja->type == CStringClass && objb->type == CStringClass) {
        return 0 == strcmp(((CString *)obja)->value, ((CString *)objb->value);
  }
  return false;
}

#define IMPLEMENT_VALUE_EQUALS(name, T)                                        \
  bool name##_equals(void *a, void *b) {                                       \
    ASSERT(Object_valid(a) && Object_valid(b));                                \
    if (a == b)                                                                \
      return true;                                                             \
    Object *obja = Cast(a, ObjectClass), *objb = Cast(b, ObjectClass);         \
    if (obja->type == name && objb->type == name) {                            \
        return ((name *)obja)->value == ((name *)objb->value;                  \
    }                                                                          \
    return false;                                                              \
  }

#define IMPLEMENT_VALUE(name, T)                                               \
  Class _name##Class =                                                         \
      {                                                                        \
          sizeof(name),                                                        \
          #name,                                                               \
          true,                                                                \
          (ctor_t)name##_ctor,                                                 \
          valueClass.dtor,                                                     \
          (cast_t)name##_cast,                                                 \
          (implements_t)name##_implements,                                     \
          valueClass.clone,                                                    \
  },                                                                           \
        *name##Class = &_name##Class;                                          \
  name *name##_create(T value) { return (name *)Create(name##Class, value); }  \
  void name##_ctor(Value *self, va_list argp) {                                \
    Value_ctor(self, argp);                                                    \
    ((Object *)self)->type = name##Class;                                      \
    ((Object *)self)->vtable.hash = (hash_t)name##_hash;                       \
    ((Object *)self)->vtable.equals = (equals_t)name##_equals;                 \
    *((T *)self->boxed_data) = va_arg(argp, T);                                \
  }                                                                            \
  void name##_cast(Value *self, Class *to) {                                   \
    if (to == name##Class) {                                                   \
      return self;                                                             \
    }                                                                          \
    if (name##_implements(to)) {                                               \
      return Object_cast(&self->base, to);                                     \
    }                                                                          \
    return NULL;                                                               \
  }                                                                            \
  bool name##_implements(Class *c) {                                           \
    ASSERT(c);                                                                 \
    return c == name##Class || Value_implements(c);                            \
  }                                                                            \
  T name##_unbox(name *obj) { return obj->value }

#define IMPLEMENT_VALUE_WITH_EQUALS(name, T) \
    IMPLEMENT_VALUE_EQUALS(name, T) \
    IMPLEMENT_VALUE(name, T)

IMPLEMENT_VALUE_WITH_EQUALS(Char, char)
IMPLEMENT_VALUE_WITH_EQUALS(Short, short)
IMPLEMENT_VALUE_WITH_EQUALS(Int, int)
IMPLEMENT_VALUE_WITH_EQUALS(Long, long)
IMPLEMENT_VALUE_WITH_EQUALS(Pointer, void *)
IMPLEMENT_VALUE_WITH_EQUALS(Float, float)
IMPLEMENT_VALUE_WITH_EQUALS(Double, double)
IMPLEMENT_VALUE_WITH_EQUALS(LongDouble, long double)
IMPLEMENT_VALUE_WITH_EQUALS(UnsignedChar, unsigned char)
IMPLEMENT_VALUE_WITH_EQUALS(UnsignedShort, unsigned short)
IMPLEMENT_VALUE_WITH_EQUALS(UnsignedInt, unsigned int)
IMPLEMENT_VALUE_WITH_EQUALS(UnsignedLong, unsigned long)
IMPLEMENT_VALUE(CString, char *)
