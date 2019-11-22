#ifndef COMMON_PROTECTED_LIST_H__
#define COMMON_PROTECTED_LIST_H__

#include "../public/list.h"

struct List
{
    size_t elem_size;
    size_t count;
    ListNode *head;
    ListNode *tail;
    int version;
};

struct ListNode
{
    ListNode *prev;
    ListNode *next;
    unsigned char data[];
};

#endif // COMMON_PROTECTED_LIST_H__
