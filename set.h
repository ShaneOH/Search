#ifndef SET_H
#define SET_H

#include "node.h"

/*
 * Standard comparison function for two generic objects.
 */
typedef int (*CompFunc)(void *, void *);

/**
 * A wrapper to strcmp that takes two generic void pointers.
 */
int generic_strcmp(void *, void *);

/**
 * A set structure implemented as a linked list.
 */
struct Set {
    Node *head;
    CompFunc compare;
};

typedef struct Set Set;

/**
 * Adds the given item to a set, maintaining the invariant that there are no
 * duplicates contained within.
 */
int set_add(Set *, void *);

/**
 * Returns one if the item is contained in the set; zero otherwise.
 */
int set_contains(Set *, void *);

/**
 * Creates a copy of the given set.
 */
Set *set_copy(Set *);

/**
 * Creates a new set of size zero.
 */
Set *set_create(CompFunc);

/**
 * Destroys the given set, freeing all associated memory.
 */
void set_destroy(Set *);

/**
 * Returns the intersection of the two sets - that is, the elements common to
 * both sets. Both sets should contain the same type of item and have the same
 * comparison function.
 */
Set *set_intersection(Set *, Set *);

/**
 * Returns a positive number if the set is nonempty and zero otherwise.
 */
int set_isempty(Set *);

/**
 * Returns the union of the two sets - that is a combination of the elements of
 * both sets, maintaining the invariant that there are no duplicates contained
 * within. Both sets should contain the same type of item and have the same
 * comparison function.
 */
Set *set_union(Set *, Set *);

/**
 * Iterator type for the user to walk the set item by item from beginning to
 * end.
 */
struct SetIterator {
    Node *ptr;
};

typedef struct SetIterator SetIterator;

/**
 * Creates an iterator object that allows the caller to walk the set, item by
 * item.
 */
SetIterator *setiterator_create(Set *);

/**
 * Destroys the set iterator, freeing all associated memory.
 */
void setiterator_destroy(SetIterator *);

/**
 * Returns the next object in the iteration, or NULL if the end of the iteration
 * has been reached.
 */
void *setiterator_next(SetIterator *);

#endif
