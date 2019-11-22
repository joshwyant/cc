#ifndef COMMON_PROTECTED_FORWARD_LIST_H__
#define COMMON_PROTECTED_FORWARD_LIST_H__

#include "../public/forward_list.h"

struct ForwardList
{
    size_t elem_size;
    size_t count;
    ForwardListNode *head;
    ForwardListNode *tail;
    int version;
};

struct ForwardListNode
{
    ForwardListNode *next;
    unsigned char data[];
};

#endif // COMMON_PROTECTED_FORWARD_LIST_H__
