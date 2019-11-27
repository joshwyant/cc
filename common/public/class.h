#ifndef COMMON_PUBLIC_CLASS_H__
#define COMMON_PUBLIC_CLASS_H__

#include <stddef.h>

typedef struct Class Class;

typedef struct Object Object;
typedef struct Value Value;
typedef struct Comparable Comparable;
typedef struct Char Char;
typedef struct Short Short;
typedef struct Int Int;
typedef struct Long Long;
typedef struct Pointer Pointer;
typedef struct Float Float;
typedef struct Double Double;
typedef struct LongDouble LongDouble;
typedef struct UnsignedChar UnsignedChar;
typedef struct UnsignedShort UnsignedShort;
typedef struct UnsignedInt UnsignedInt;
typedef struct UnsignedLong UnsignedLong;
typedef struct CString CString;

extern Class *ObjectClass;
extern Class *ValueClass;
extern Class *CharClass;
extern Class *ShortClass;
extern Class *IntClass;
extern Class *LongClass;
extern Class *PointerClass;
extern Class *FloatClass;
extern Class *DoubleClass;
extern Class *LongDoubleClass;
extern Class *UnsignedCharClass;
extern Class *UnsignedShortClass;
extern Class *UnsignedIntClass;
extern Class *UnsignedLongClass;
extern Class *CStringClass;

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

Char *Char_create(char value);
Short *Short_create(short value);
Int *Int_create(int value);
Long *Long_create(long value);
Pointer *Pointer_create(void *value);
Float *Float_create(float value);
Double *Double_create(double value);
LongDouble *LongDouble_create(long double value);
UnsignedChar *UnsignedChar_create(unsigned char value);
UnsignedShort *UnsignedShort_create(unsigned short value);
UnsignedInt *UnsignedInt_create(unsigned int value);
UnsignedLong *UnsignedLong_create(unsigned long value);
CString *CString_create(char *value);
char Char_unbox(Char *obj);
short Short_unbox(Short *obj);
int Int_unbox(Int *obj);
long Long_unbox(Long *obj);
void *Pointer_unbox(Pointer *obj);
float Float_unbox(Float *obj);
double Double_unbox(Double *obj);
long double LongDouble_unbox(Long *obj);
unsigned char UnsignedChar_unbox(UnsignedChar *obj);
unsigned short UnsignedShort_unbox(UnsignedShort *obj);
unsigned int UnsignedInt_unbox(UnsignedInt *obj);
unsigned long UnsignedLong_unbox(UnsignedLong *obj);
char *CString_unbox(CString *obj);

#endif
