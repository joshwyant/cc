#ifndef COMMON_PROTECTED_BACK_LIST_H__
#define COMMON_PROTECTED_BACK_LIST_H__

#include "../public/back_list.h"

struct BackList
{
    size_t elem_size;
    size_t count;
    BackListNode *head;
    BackListNode *tail;
    int version;
};

struct BackListNode
{
    BackListNode *prev;
    BackListNode *next;
    unsigned char data[];
};

#endif // COMMON_PROTECTED_BACK_LIST_H__
