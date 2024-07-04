#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = (struct list_head *) malloc(sizeof(struct list_head));
    if (head) {
        INIT_LIST_HEAD(head);
    }
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    struct list_head *curr, *tmp;
    list_for_each_safe(curr, tmp, head)
    {
        element_t *elem = list_entry(curr, element_t, list);
        q_release_element(elem);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;

    element_t *new_elem = (element_t *) malloc(sizeof(element_t));
    if (!new_elem)
        return false;

    size_t len = strlen(s);
    new_elem->value = (char *) malloc(len + 1);
    if (!new_elem->value) {
        free(new_elem);
        return false;
    }
    strncpy(new_elem->value, s, len + 1);

    list_add(&new_elem->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;

    element_t *new_elem = (element_t *) malloc(sizeof(element_t));
    if (!new_elem)
        return false;

    size_t len = strlen(s);
    new_elem->value = (char *) malloc(len + 1);
    if (!new_elem->value) {
        free(new_elem);
        return false;
    }
    strncpy(new_elem->value, s, len + 1);

    list_add_tail(&new_elem->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    struct list_head *node = head->next;
    element_t *elem = list_entry(node, element_t, list);

    if (sp) {
        strncpy(sp, elem->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    list_del(node);
    return elem;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    struct list_head *node = head->prev;
    element_t *elem = list_entry(node, element_t, list);

    if (sp) {
        strncpy(sp, elem->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    list_del(node);
    return elem;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int count = 0;
    struct list_head *node;
    list_for_each(node, head) {
        count++;
    }
    return count;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    int size = q_size(head);
    if (size == 0)
        return false;

    struct list_head *node = head->next;
    for (int i = 0; i < size / 2; i++) {
        node = node->next;
    }

    list_del(node);
    element_t *elem = list_entry(node, element_t, list);
    q_release_element(elem);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    struct list_head *curr, *tmp;
    bool has_dup = false;

    list_for_each_safe(curr, tmp, head)
    {
        element_t *elem1 = list_entry(curr, element_t, list);
        struct list_head *next = curr->next;

        while (next != head) {
            element_t *elem2 = list_entry(next, element_t, list);
            struct list_head *tmp_next = next->next;
            if (strcmp(elem1->value, elem2->value) == 0) {
                list_del(next);
                q_release_element(elem2);
                next = tmp_next;
                has_dup = true;
            } else {
                next = next->next;
            }
        }

        if (has_dup) {
            list_del(curr);
            q_release_element(elem1);
            has_dup = false;
        }
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *node = head->next;
    while (node != head && node->next != head) {
        struct list_head *next = node->next;
        list_del(next);
        list_add(next, node);
        node = node->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *node, *tmp;
    list_for_each_safe(node, tmp, head)
    {
        list_move(node, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head) || k <= 1)
        return;

    struct list_head *curr = head->next;
    while (curr != head) {
        struct list_head *start = curr, *end = curr;
        for (int i = 1; i < k && end->next != head; i++) {
            end = end->next;
        }
        if (start == end)
            break;
        
        struct list_head *next = end->next;
        struct list_head *prev = start->prev;
        struct list_head *node = start;

        for (int i = 0; i < k; i++) {
            struct list_head *temp = node->next;
            list_move(node, prev);
            prev = node;
            node = temp;
        }

        curr = next;
    }
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return;

    struct list_head sorted;
    INIT_LIST_HEAD(&sorted);

    while (!list_empty(head)) {
        struct list_head *node = head->next;
        element_t *elem = list_entry(node, element_t, list);
        list_del(node);

        struct list_head *curr;
        list_for_each(curr, &sorted)
        {
            element_t *curr_elem = list_entry(curr, element_t, list);
            if ((descend && strcmp(elem->value, curr_elem->value) > 0) ||
                (!descend && strcmp(elem->value, curr_elem->value) < 0)) {
                break;
            }
        }
        list_add_tail(&elem->list, curr);
    }

    list_splice(&sorted, head);
}

/* Remove every node which has a node with a strictly less value anywhere to the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    struct list_head *node = head->prev;
    char *max_value = list_entry(node, element_t, list)->value;
    int count = 1;

    while (node != head) {
        node = node->prev;
        element_t *elem = list_entry(node, element_t, list);
        if (strcmp(elem->value, max_value) < 0) {
            struct list_head *temp = node->next;
            list_del(node);
            q_release_element(elem);
            node = temp->prev;
        } else {
            max_value = elem->value;
            count++;
        }
    }
    return count;
}

/* Remove every node which has a node with a strictly greater value anywhere to the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    struct list_head *node = head->prev;
    char *min_value = list_entry(node, element_t, list)->value;
    int count = 1;

    while (node != head) {
        node = node->prev;
        element_t *elem = list_entry(node, element_t, list);
        if (strcmp(elem->value, min_value) > 0) {
            struct list_head *temp = node->next;
            list_del(node);
            q_release_element(elem);
            node = temp->prev;
        } else {
            min_value = elem->value;
            count++;
        }
    }
    return count;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return 0;

    queue_contex_t *first_ctx = list_entry(head->next, queue_contex_t, chain);
    struct list_head *merged_queue = first_ctx->q;

    struct list_head *curr;
    list_for_each(curr, head)
    {
        if (curr == head->next)
            continue;

        queue_contex_t *ctx = list_entry(curr, queue_contex_t, chain);
        struct list_head *q = ctx->q;

        struct list_head *node;
        list_for_each(node, q)
        {
            element_t *elem = list_entry(node, element_t, list);
            struct list_head *pos;
            list_for_each(pos, merged_queue)
            {
                element_t *m_elem = list_entry(pos, element_t, list);
                if ((descend && strcmp(elem->value, m_elem->value) > 0) ||
                    (!descend && strcmp(elem->value, m_elem->value) < 0)) {
                    break;
                }
            }
            list_move_tail(node, pos);
        }
        INIT_LIST_HEAD(q);
    }

    return q_size(merged_queue);
}
