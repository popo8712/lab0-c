#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include "list.h"

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;

    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, l, list) {
        free(entry->value);
        free(entry);
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;

    element_t *new_element = malloc(sizeof(element_t));
    if (!new_element)
        return false;

    INIT_LIST_HEAD(&new_element->list);

    new_element->value = strdup(s);
    if (!new_element->value) {
        free(new_element);
        return false;
    }

    list_add(&new_element->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;

    element_t *new_element = malloc(sizeof(element_t));
    if (!new_element)
        return false;

    INIT_LIST_HEAD(&new_element->list);

    new_element->value = strdup(s);
    if (!new_element->value) {
        free(new_element);
        return false;
    }

    list_add_tail(&new_element->list, head);
    return true;
}

/* Remove the element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *elem = list_first_entry(head, element_t, list);
    list_del(&elem->list);

    if (sp) {
        strncpy(sp, elem->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return elem;
}

/* Remove the element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *elem = list_last_entry(head, element_t, list);
    list_del(&elem->list);

    if (sp) {
        strncpy(sp, elem->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return elem;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int count = 0;
    struct list_head *node;
    list_for_each (node, head)
        count++;
    return count;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    struct list_head *slow = head->next, *fast = head->next;
    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }

    element_t *elem = list_entry(slow, element_t, list);
    list_del(&elem->list);
    free(elem->value);
    free(elem);
    return true;
}

/* Delete all nodes that have duplicate string value */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    struct list_head *cur = head->next, *safe;
    element_t *elem, *next_elem;

    while (cur != head && cur->next != head) {
        elem = list_entry(cur, element_t, list);
        next_elem = list_entry(cur->next, element_t, list);

        if (strcmp(elem->value, next_elem->value) == 0) {
            while (cur->next != head &&
                   strcmp(elem->value,
                          list_entry(cur->next, element_t, list)->value) == 0) {
                safe = cur->next;
                list_del(cur);
                free(elem->value);
                free(elem);
                cur = safe;
                elem = list_entry(cur, element_t, list);
            }
            safe = cur->next;
            list_del(cur);
            free(elem->value);
            free(elem);
            cur = safe;
        } else {
            cur = cur->next;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *node;
    for (node = head->next; node != head && node->next != head;
         node = node->next->next) {
        struct list_head *next = node->next;
        list_del(node);
        list_add(node, next);
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *prev = head, *cur = head->next, *next;
    while (cur != head) {
        next = cur->next;
        cur->next = prev;
        prev = cur;
        cur = next;
    }
    head->next = prev;
}

/* Sort elements of queue in ascending order or descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return;

    struct list_head *node, *next;
    list_for_each_safe (node, next, head) {
        list_del(node);
    }

    struct list_head sorted;
    INIT_LIST_HEAD(&sorted);

    list_for_each_safe (node, next, head) {
        struct list_head *pos;
        list_for_each (pos, &sorted) {
            if (descend) {
                if (strcmp(list_entry(node, element_t, list)->value,
                           list_entry(pos, element_t, list)->value) > 0) {
                    break;
                }
            } else {
                if (strcmp(list_entry(node, element_t, list)->value,
                           list_entry(pos, element_t, list)->value) < 0) {
                    break;
                }
            }
        }
        list_add_tail(node, pos);
    }
    list_splice(&sorted, head);
}
