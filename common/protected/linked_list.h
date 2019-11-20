#ifndef COMMON_PROTECTED_LINKED_LIST_H__
#define COMMON_PROTECTED_LINKED_LIST_H__

#include "../public/linked_list.h"

struct LinkedList
{
    size_t elem_size;
    size_t count;
    LinkedListNode *head;
    LinkedListNode *tail;
    int version;
};

struct LinkedListNode
{
    LinkedListNode *next;
    unsigned char data[];
};

#endif // COMMON_PROTECTED_LINKED_LIST_H__
