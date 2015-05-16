#ifndef SORTED_LIST_H
#define SORTED_LIST_H

#include "node.h"
#include "record.h"
#include <stdlib.h>

/*
 * Standard comparison function for two records.
 */
typedef int (*RecComp)(struct Record *, struct Record *);

/*
 * Sorted list type.
 */
struct SortedList {
    Node *head;
    RecComp compare;
};

typedef struct SortedList SortedList;

/*
 * Creates a new sorted list of size zero. Returns a new sorted list object on
 * success; on failure, this function returns NULL.
 */
SortedList *sl_create(RecComp);

/*
 * Destroys the list pointed to by the given list pointer. Since the list is
 * implemented as a linked list, this method walks the list and frees all of
 * the dynamically allocated list nodes along the way.
 */
void sl_destroy(SortedList *);

/**
 * Inserts a record into the list, maintaining sorted order.
 */
int sl_putrecord(SortedList *, const char *, const char *);

/*
 * Iterator type for user to "walk" through the list item by item, from
 * beginning to end.
 */
struct SortedListIterator {
    Node *ptr;
};

typedef struct SortedListIterator SortedListIterator;

/*
 * Creates an iterator object that will allow the caller to "walk" through the
 * list from beginning to the end.
 *
 * If the function succeeds, it returns a non-NULL SortedListIterT object.
 * Else, it returns NULL.
 */
SortedListIterator *create_iter(SortedList *);

/*
 * Destroys an iterator object.
 */
void destroy_iter(SortedListIterator *);

/*
 * Returns the next object in the iteration, or NULL if the end of the iteration
 * has been reached.
 */
Record *next_item(SortedListIterator *);

#endif
