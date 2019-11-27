#include "protected/string.h"

#include "public/assert.h"
#include "../test/stubs.h"

Class stringClass = {
    sizeof(String),
    "String",
    true,
    (ctor_t)String_ctor,
    (dtor_t)String_dtor,
    (cast_t)String_cast,
    (implements_t)String_implements,
    (clone_t)String_clone,
}, *StringClass = &stringClass;


void String_init(String *self, char *c_str)
{
    ASSERT(self);
    size_t len = strlen(c_str);
    if (c_str == NULL) {
        self->ref = NULL;
        self->short_string[0] = '\0';
    } else if (strlen(c_str) < SHORT_STRING_SIZE) {
        self->ref = NULL;
        strcpy(self->short_string, c_str);
    } else {
        self->ref = calloc(strlen(c_str) + 1);
        strcpy(self->ref, c_str);
    }
}

void String_ctor(String *self, va_list argp)
{
    Object_ctor(self, argp);
    ((Object *)self)->type = StringClass;
    String_init(self, va_arg(argp, char *));
}
void String_dtor(String *self)
{
    ASSERT(self);
    free(self->ref);
}
void *String_cast(String *self, Class *to)
{
    if (to == StringClass) {
        return self;
    }
    if (String_implements(to)) {
        return Object_cast(&self->base, to);
    }
    return NULL;
}
bool String_implements(Class *c)
{
    ASSERT(c);
    return c == StringClass || Object_implements(c);
}
void *String_clone(String *self)
{
    ASSERT(Object_valid((Object *)self));
    return String_create(self->ref ? self->ref : self->short_string);
}
bool String_equals(void *a, void *b)
{
  ASSERT(Object_valid(a) && Object_valid(b));
  if (a == b)
    return true;
  Object *obja = Cast(a, ObjectClass), *objb = Cast(b, ObjectClass);
  char *stra, *strb;
  if (obja->type == StringClass) {
      String *str = (String *)obja;
      stra = str->ref ? str->ref : str->short_string;
  } else if (obja->type == CStringClass) {
      CString *str = (CString *)obja;
      stra = str->value;
  } else return false;
  if (objb->type == StringClass) {
      String *str = (String *)objb;
      strb = str->ref ? str->ref : str->short_string;
  } else if (objb->type == CStringClass) {
      CString *str = (CString *)objb;
      strb = str->value;
  } else return false;
  if (stra == NULL || strb == NULL) return false;
  return 0 == strcmp(stra, strb);
}

String *String_create(char *cstr)
{
    return (String *)Create(StringClass, cstr);
}
char *String_c_str(String *self)
{
    return self->ref ? self->ref : self->short_string;
}
size_t String_length(String *self)
{
    return strlen(String_c_str(self));
}
void String_cat(String *self, String *b)
{
  String_cat_c_str(self, String_c_str(b));
}
void String_cat_c_str(String *self, char *b)
{
  if (!self->ref &&
      (String_length(self) + strlen(b) < SHORT_STRING_SIZE)) {
    strcat(String_c_str(self), b);
  } else {
    self->ref = realloc(self->ref, String_length(self) + strlen(b) + 1);
    strcat(self->ref, b);
  }
}
void String_cat_CString(String *self, CString *b)
{
  String_cat_c_str(self, CString_unbox(b));
}
CString *String_box_CString(String *self)
{
    return CString_create(String_c_str(self));
}
