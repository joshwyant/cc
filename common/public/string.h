#ifndef COMMON_PUBLIC_STRING_H__
#define COMMON_PUBLIC_STRING_H__

#include "class.h"

typedef struct String String;

extern Class *StringClass;

String *String_create(char *cstr);
char *String_c_str(String *self);
size_t String_length(String *self);
void String_cat(String *self, String *b);
void String_cat_c_str(String *self, char *b);
void String_cat_CString(String *self, CString *b);
CString *String_box_CString(String *self);
// TODO: add more

#endif // COMMON_PUBLIC_STRING_H__
