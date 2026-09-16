#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_

#define IsNil(sentinel, ptr) ((ptr) == 0 || (ptr) == sentinel)
#define SetNil(sentinel, ptr) ((ptr) = sentinel)

#define sll_queue_push_back_ex(sentinel, first, last, node, next) \
    do { \
        if (IsNil(sentinel, first)) { \
            (first) = (last) = (node); \
            SetNil(sentinel, (node)->next); \
        } else { \
            (last)->next = (node); \
            (last) = (node); \
            SetNil(sentinel, (node)->next); \
        } \
    } while(0)
#define sll_queue_push_back(first, last, node) \
    sll_queue_push_back_ex(0, first, last, node, next)

#endif  // LINKED_LIST_H_
