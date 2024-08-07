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

    if (sp && elem->value) {
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

    if (sp && elem->value) {
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
    while (cur != head) {
        element_t *cur_elem = list_entry(cur, element_t, list);
        bool duplicate = false;
        safe = cur->next;
        while (safe != head) {
            element_t *safe_elem = list_entry(safe, element_t, list);
            if (strcmp(cur_elem->value, safe_elem->value) == 0) {
                list_del(&safe_elem->list);
                free(safe_elem->value);
                free(safe_elem);
                duplicate = true;
            }
            safe = safe->next;
        }
        if (duplicate) {
            list_del(&cur_elem->list);
            free(cur_elem->value);
            free(cur_elem);
        }
        cur = cur->next;
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
        cur->prev = next;
        prev = cur;
        cur = next;
    }
    head->next = prev;
    prev->prev = head;
}

/* Sort elements of queue in ascending order or descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return;

    struct list_head sorted;
    INIT_LIST_HEAD(&sorted);

    struct list_head *node, *next;
    list_for_each_safe (node, next, head) {
        list_del(node);
        struct list_head *pos;
        list_for_each (pos, &sorted) {
            element_t *a = list_entry(node, element_t, list);
            element_t *b = list_entry(pos, element_t, list);
            if ((descend && strcmp(a->value, b->value) > 0) ||
                (!descend && strcmp(a->value, b->value) < 0)) {
                break;
            }
        }
        list_add_tail(node, pos);
    }

    list_splice(&sorted, head);
}

/* Reverse the first k elements of queue */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head) || k <= 0)
        return;

    struct list_head *cur = head->next;
    struct list_head temp_head;
    INIT_LIST_HEAD(&temp_head);

    for (int i = 0; i < k && cur != head; i++) {
        struct list_head *next = cur->next;
        list_del(cur);
        list_add(cur, &temp_head);
        cur = next;
    }

    list_splice_tail(&temp_head, head);
}

/* Descend the queue */
int q_descend(struct list_head *head)
{
    if (!head)
        return 0;

    q_sort(head, true);
    return q_size(head);
}

/* Ascend the queue */
int q_ascend(struct list_head *head)
{
    if (!head)
        return 0;

    q_sort(head, false);
    return q_size(head);
}

/* Merge all the queues into one sorted queue */
int q_merge(struct list_head *head, bool descend)
{
    if (!head)
        return 0;

    queue_contex_t *ctx, *next_ctx;
    struct list_head merged_head;
    INIT_LIST_HEAD(&merged_head);

    list_for_each_entry_safe (ctx, next_ctx, head, chain) {
        if (ctx->q) {
            list_splice_tail_init(ctx->q, &merged_head);
            ctx->q = NULL;
        }
    }

    q_sort(&merged_head, descend);

    ctx = list_first_entry(head, queue_contex_t, chain);
    ctx->q = &merged_head;

    return q_size(&merged_head);
}
