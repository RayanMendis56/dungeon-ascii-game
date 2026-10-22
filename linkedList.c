#include <stdlib.h>

#include "linkedList.h"

/*
 * Nodes store void pointers so the list can hold any caller-owned payload.
 * The optional destructor lets list_destroy release payloads when required.
 */
void list_init(LinkedList *list)
{
    list->head = NULL;
    list->size = 0;
}

int list_push_front(LinkedList *list, void *data)
{
    ListNode *node = malloc(sizeof(*node));
    int result = 0;

    if (node != NULL) {
        node->data = data;
        node->next = list->head;
        list->head = node;
        list->size++;
        result = 1;
    }

    return result;
}

void *list_pop_front(LinkedList *list)
{
    void *data = NULL;

    if (list->head != NULL) {
        ListNode *node = list->head;

        data = node->data;
        list->head = node->next;
        list->size--;
        free(node);
    }

    return data;
}

void list_destroy(LinkedList *list, ListDataDestructor destroy_data)
{
    void *data;

    while (list->head != NULL) {
        data = list_pop_front(list);
        if (destroy_data != NULL) {
            destroy_data(data);
        }
    }

}
