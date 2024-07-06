#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *new_qhead = malloc(sizeof(struct list_head));
    if (!new_qhead)
        return NULL;
    INIT_LIST_HEAD(new_qhead);
    return new_qhead;
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
    return;
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new_ele = malloc(sizeof(element_t));
    if (!new_ele)
        return false;
    INIT_LIST_HEAD(&new_ele->list);
    new_ele->value = strdup(s);
    if (!new_ele->value) {
        free(new_ele);
        return false;
    }
    list_add(&new_ele->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new_ele = malloc(sizeof(element_t));
    if (!new_ele)
        return false;

    INIT_LIST_HEAD(&new_ele->list);
    new_ele->value = strdup(s);
    if (!new_ele->value) {
        free(new_ele);
        return false;
    }
    list_add_tail(&new_ele->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *f = list_first_entry(head, element_t, list);
    list_del(&f->list);

    if (sp) {
        size_t copy_size =
            strlen(f->value) < (bufsize - 1) ? strlen(f->value) : (bufsize - 1);
        strncpy(sp, f->value, copy_size);
        sp[copy_size] = '\0';
    }
    return f;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *f = list_last_entry(head, element_t, list);
    list_del(&f->list);

    if (sp) {
        size_t copy_size =
            strlen(f->value) < (bufsize - 1) ? strlen(f->value) : (bufsize - 1);
        strncpy(sp, f->value, copy_size);
        sp[copy_size] = '\0';
    }
    return f;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int size = 0;
    struct list_head *cur;
    list_for_each (cur, head)
        size++;
    return size;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;

    struct list_head *first = head->next;
    struct list_head *second = head->prev;
    while ((first != second) && (first->next != second)) {
        first = first->next;
        second = second->prev;
    }
    element_t *node = list_entry(first, element_t, list);
    list_del(first);
    free(node->value);
    free(node);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;

    bool dup = false;
    element_t *cur, *tmp;
    list_for_each_entry_safe (cur, tmp, head, list) {
        if (&tmp->list != head && !strcmp(cur->value, tmp->value)) {
            list_del(&cur->list);
            free(cur->value);
            free(cur);
            dup = true;
        } else if (dup) {
            list_del(&cur->list);
            free(cur->value);
            free(cur);
            dup = false;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head))
        return;

    struct list_head *first, *second;
    list_for_each_safe (first, second, head) {
        if (second == head)
            break;
        first->prev->next = second;
        second->prev = first->prev;
        first->next = second->next;
        first->prev = second;
        second->next->prev = first;
        second->next = first;

        second = first->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        node->next = node->prev;
        node->prev = safe;
    }
    node->next = node->prev;
    node->prev = safe;
    return;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head)
        return;

    int times = q_size(head) / k;
    struct list_head *tail;

    LIST_HEAD(tmp);
    LIST_HEAD(new_head);

    for (int i = 0; i < times; i++) {
        int j = 0;
        list_for_each (tail, head) {
            if (j >= k)
                break;
            j++;
        }
        list_cut_position(&tmp, head, tail->prev);
        q_reverse(&tmp);
        list_splice_tail_init(&tmp, &new_head);
    }
    list_splice_init(&new_head, head);
}

struct list_head *mergeTwolists(struct list_head *l1, struct list_head *l2)
{
    struct list_head *head = NULL;
    struct list_head **head_ptr = &head;
    for (; l1 && l2; head_ptr = &(*head_ptr)->next) {
        element_t *ele_1 = list_entry(l1, element_t, list);
        element_t *ele_2 = list_entry(l2, element_t, list);
        if (strcmp(ele_1->value, ele_2->value) >= 0) {
            *head_ptr = l2;
            l2 = l2->next;
        } else {
            *head_ptr = l1;
            l1 = l1->next;
        }
    }
    *head_ptr = (struct list_head *) ((uintptr_t) l1 | (uintptr_t) l2);
    return head;
}

struct list_head *mergesort(struct list_head *head)
{
    if (!head || !head->next)
        return head;

    struct list_head *first = head;
    struct list_head *last = head->prev;

    while (first != last && first->next != last) {
        first = first->next;
        last = last->prev;
    }
    struct list_head *l1 = head;
    struct list_head *l2 = first->next;
    l2->prev = head->prev;
    l1->prev = first;
    first->next = NULL;

    return mergeTwolists(mergesort(l1), mergesort(l2));
}

void __qsort(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    head->prev->next = NULL; /* Make the linked list from circular to linear */
    head->next->prev = head->prev;
    head->next = mergesort(head->next);

    /* Reconstruct the linked list back to a circular doubly linked list */
    struct list_head *cur = head;
    struct list_head *next = head->next;
    while (next) {
        next->prev = cur;
        cur = next;
        next = next->next;
    }
    cur->next = head;
    head->prev = cur;
}

/* Linux kernel list sort */

typedef int (*list_cmp_func_t)(const struct list_head *,
                               const struct list_head *);

static int cmpfunc(const struct list_head *a, const struct list_head *b)
{
    element_t *ela = list_entry(a, element_t, list);
    element_t *elb = list_entry(b, element_t, list);
    return strcmp(ela->value, elb->value);
}

static struct list_head *merge(list_cmp_func_t cmp,
                               struct list_head *a,
                               struct list_head *b)
{
    struct list_head *head = NULL, **tail = &head;

    for (;;) {
        if (cmp(a, b) <= 0) {
            *tail = a;
            tail = &a->next;
            a = a->next;
            if (!a) {
                *tail = b;
                break;
            }
        } else {
            *tail = b;
            tail = &b->next;
            b = b->next;
            if (!b) {
                *tail = a;
                break;
            }
        }
    }
    return head;
}

static void merge_final(list_cmp_func_t cmp,
                        struct list_head *head,
                        struct list_head *a,
                        struct list_head *b)
{
    struct list_head *tail = head;
    unsigned char count = 0;
    for (;;) {
        if (cmp(a, b) <= 0) {
            tail->next = a;
            a->prev = tail;
            tail = a;
            a = a->next;
            if (!a)
                break;
        } else {
            tail->next = b;
            b->prev = tail;
            tail = b;
            b = b->next;
            if (!b) {
                b = a;
                break;
            }
        }
    }

    tail->next = b;
    do {
        if (__glibc_unlikely(!++count))
            cmp(b, b);
        b->prev = tail;
        tail = b;
        b = b->next;
    } while (b);

    tail->next = head;
    head->prev = tail;
}


void list_sort(struct list_head *head, list_cmp_func_t cmp)
{
    struct list_head *list = head->next, *pending = NULL;
    size_t count = 0; /* Count of pending */

    if (list == head->prev)
        return;

    head->prev->next = NULL;

    do {
        size_t bits;
        struct list_head **tail = &pending;
        for (bits = count; bits & 1; bits >>= 1)
            tail = &(*tail)->prev;

        if (__glibc_likely(bits)) {
            struct list_head *a = *tail, *b = a->prev;

            a = merge(cmp, b, a);
            a->prev = b->prev;
            *tail = a;
        }

        list->prev = pending;
        pending = list;
        list = list->next;
        pending->next = NULL;
        count++;
    } while (list);

    list = pending;
    pending = pending->prev;
    for (;;) {
        struct list_head *next = pending->prev;
        if (!next)
            break;
        list = merge(cmp, pending, list);
        pending = next;
    }
    merge_final(cmp, head, pending, list);
}

/* End of linux kernel list sort */

/* Implementation of timsort */

static inline size_t run_size(struct list_head *head)
{
    if (!head)
        return 0;
    if (!head->next)
        return 1;
    return (size_t) (head->next->prev);
}

struct pair {
    struct list_head *head, *next;
};

static size_t stk_size;

static struct pair find_run(struct list_head *list, list_cmp_func_t cmp)
{
    size_t len = 1;
    struct list_head *next = list->next, *head = list;
    struct pair result;

    if (!next) {
        result.head = head, result.next = next;
        return result;
    }

    if (cmp(list, next) > 0) {
        /* decending run, also reverse the list */
        struct list_head *prev = NULL;
        do {
            len++;
            list->next = prev;
            prev = list;
            list = next;
            next = list->next;
            head = list;
        } while (next && cmp(list, next) > 0);
        list->next = prev;
    } else {
        do {
            len++;
            list = next;
            next = list->next;
        } while (next && cmp(list, next) <= 0);
        list->next = NULL;
    }
    head->prev = NULL;
    head->next->prev = (struct list_head *) len;
    result.head = head, result.next = next;
    return result;
}

static struct list_head *merge_at(list_cmp_func_t cmp, struct list_head *at)
{
    size_t len = run_size(at) + run_size(at->prev);
    struct list_head *prev = at->prev->prev;
    struct list_head *list = merge(cmp, at->prev, at);
    list->prev = prev;
    list->next->prev = (struct list_head *) len;
    --stk_size;
    return list;
}

static struct list_head *merge_force_collapse(list_cmp_func_t cmp,
                                              struct list_head *tp)
{
    while (stk_size >= 3) {
        if (run_size(tp->prev->prev) < run_size(tp)) {
            tp->prev = merge_at(cmp, tp->prev);
        } else {
            tp = merge_at(cmp, tp);
        }
    }
    return tp;
}

static struct list_head *merge_collapse(list_cmp_func_t cmp,
                                        struct list_head *tp)
{
    int n;
    while ((n = stk_size) >= 2) {
        if ((n >= 3 &&
             run_size(tp->prev->prev) <= run_size(tp->prev) + run_size(tp)) ||
            (n >= 4 && run_size(tp->prev->prev->prev) <=
                           run_size(tp->prev->prev) + run_size(tp->prev))) {
            if (run_size(tp->prev->prev) < run_size(tp)) {
                tp->prev = merge_at(cmp, tp->prev);
            } else {
                tp = merge_at(cmp, tp);
            }
        } else if (run_size(tp->prev) <= run_size(tp)) {
            tp = merge_at(cmp, tp);
        } else {
            break;
        }
    }

    return tp;
}

static void build_prev_link(struct list_head *head,
                            struct list_head *tail,
                            struct list_head *list)
{
    tail->next = list;
    do {
        list->prev = tail;
        tail = list;
        list = list->next;
    } while (list);

    /* The final links to make a circular doubly-linked list */
    tail->next = head;
    head->prev = tail;
}

void timsort(struct list_head *head, list_cmp_func_t cmp)
{
    stk_size = 0;

    struct list_head *list = head->next, *tp = NULL;
    if (head == head->prev)
        return;

    /* Convert to a null-terminated singly-linked list. */
    head->prev->next = NULL;

    do {
        /* Find next run */
        struct pair result = find_run(list, cmp);
        result.head->prev = tp;
        tp = result.head;
        list = result.next;
        stk_size++;
        tp = merge_collapse(cmp, tp);
    } while (list);

    /* End of input; merge together all the runs. */
    tp = merge_force_collapse(cmp, tp);

    /* The final merge; rebuild prev links */
    struct list_head *stk0 = tp, *stk1 = stk0->prev;
    while (stk1 && stk1->prev)
        stk0 = stk0->prev, stk1 = stk1->prev;
    if (stk_size <= 1) {
        build_prev_link(head, head, stk0);
        return;
    }
    merge_final(cmp, head, stk1, stk0);
}

/* End of timsort implementation */

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    list_sort(head, cmpfunc);

    if (descend)
        q_reverse(head);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    element_t *right = list_entry(head->prev, element_t, list);
    element_t *left = list_entry(head->prev->prev, element_t, list);
    while (&left->list != head) {
        if (strcmp(right->value, left->value) > 0) {
            left = list_entry(left->list.prev, element_t, list);
            right = list_entry(right->list.prev, element_t, list);
        } else {
            list_del(&left->list);
            free(left->value);
            free(left);
            left = list_entry(right->list.prev, element_t, list);
        }
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    element_t *right = list_entry(head->prev, element_t, list);
    element_t *left = list_entry(head->prev->prev, element_t, list);
    while (&left->list != head) {
        if (strcmp(right->value, left->value) < 0) {
            left = list_entry(left->list.prev, element_t, list);
            right = list_entry(right->list.prev, element_t, list);
        } else {
            list_del(&left->list);
            free(left->value);
            free(left);
            left = list_entry(right->list.prev, element_t, list);
        }
    }
    return q_size(head);
}

/* Helper function for q_merge(), merge two lists together */
int __merge(struct list_head *l1, struct list_head *l2)
{
    if (!l1 || !l2)
        return 0;
    LIST_HEAD(tmp_head);
    while (!list_empty(l1) && !list_empty(l2)) {
        element_t *ele_1 = list_first_entry(l1, element_t, list);
        element_t *ele_2 = list_first_entry(l2, element_t, list);
        element_t *ele_min =
            strcmp(ele_1->value, ele_2->value) < 0 ? ele_1 : ele_2;
        list_move_tail(&ele_min->list, &tmp_head);
    }
    list_splice_tail_init(l1, &tmp_head);
    list_splice_tail_init(l2, &tmp_head);
    list_splice(&tmp_head, l1);
    return q_size(l1);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;
    else if (list_is_singular(head))
        return q_size(list_first_entry(head, queue_contex_t, chain)->q);
    int size = q_size(head);
    int count = (size % 2) ? size / 2 + 1 : size / 2;
    int queue_size = 0;
    for (int i = 0; i < count; i++) {
        queue_contex_t *first = list_first_entry(head, queue_contex_t, chain);
        queue_contex_t *second =
            list_entry(first->chain.next, queue_contex_t, chain);
        while (!list_empty(first->q) && !list_empty(second->q)) {
            queue_size = __merge(first->q, second->q);
            list_move_tail(&second->chain, head);
            first = list_entry(first->chain.next, queue_contex_t, chain);
            second = list_entry(first->chain.next, queue_contex_t, chain);
        }
    }
    return queue_size;
}

/* Replace old entry by new one
 * Reference:
 * https://github.com/torvalds/linux/blob/cf1182944c7cc9f1c21a8a44e0d29abe12527412/include/linux/list.h#L241
 */
static inline void replace(struct list_head *old, struct list_head *new)
{
    new->next = old->next;
    new->next->prev = new;
    new->prev = old->prev;
    new->prev->next = new;
}


/* Replace a with b and re-add a at b's position
 * Reference:
 * https://github.com/torvalds/linux/blob/cf1182944c7cc9f1c21a8a44e0d29abe12527412/include/linux/list.h#L269
 */
static inline void swap(struct list_head *a, struct list_head *b)
{
    struct list_head *pos = b->prev;

    list_del(b);
    replace(a, b);
    if (pos == a)
        pos = b;
    list_add(a, pos);
}

/* Fisher-Yates shuffle Algorithm */
void q_shuffle(struct list_head *head)
{
    if (!head || list_is_singular(head))
        return;

    int len = q_size(head);
    struct list_head *pos, *tmp;

    /* Iterate over the queue backwards safe against removal of list entry */
    for (pos = head->prev, tmp = pos->prev; pos != head && len;
         pos = tmp, tmp = pos->prev, len--) {
        struct list_head *pick = head->prev;
        for (int r = rand() % len; r > 0; r--)
            pick = pick->prev;
        if (pick == pos)
            continue;
        swap(pos, pick);
    }
}
