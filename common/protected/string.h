#ifndef COMMON_PROTECTED_STRING_H__
#define COMMON_PROTECTED_STRING_H__

#include "../public/class.h"

#include "class.h"

#include <stdarg.h>
#include <stddef.h>

#define SHORT_STRING_SIZE 20

struct String
{
    struct Object base;
    char *ref;
    char short_string[SHORT_STRING_SIZE];
};

void String_ctor(String *self, va_list argp);
void String_dtor(String *self);
void *String_cast(String *self, Class *to);
bool String_implements(Class *c);
void *String_clone(String *self);
bool String_equals(void *a, void *b);

#endif // COMMON_PROTECTED_STRING_H__
