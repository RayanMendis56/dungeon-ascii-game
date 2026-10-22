#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stddef.h>

typedef void (*ListDataDestructor)(void *data);

typedef struct ListNode {
    void *data;
    struct ListNode *next;
} ListNode;

typedef struct {
    ListNode *head;
    size_t size;
} LinkedList;

void list_init(LinkedList *list);
int list_push_front(LinkedList *list, void *data);
void *list_pop_front(LinkedList *list);
void list_destroy(LinkedList *list, ListDataDestructor destroy_data);

#endif /* LINKED_LIST_H */
