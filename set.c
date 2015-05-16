#include "node.h"
#include "set.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Wrapper for the strcmp function that takes two generic pointers.
 */
int generic_strcmp(void *str1, void *str2) {
    return strcmp((char *) str1, (char *) str2);
}

/**
 * Adds the given item to the set, maintaining the invariant that there are no
 * duplicates contained within.
 * If a memory error occurs, this method returns 0. Otherwise, it returns 1 -
 * even if the insertion was skipped because the item already exists.
 */
int set_add(Set *set, void *item) {
    Node *new, *ptr, *prev;
    int c, retval;

    if (!set || !(new = create_node(item, NULL))) {
        retval = 0;
    }
    else if (!set->head) {
        // First item added to the set. Create a new node
        set->head = new;
        retval = 1;
    }
    else if ((c = set->compare(item, set->head->data)) <= 0) {
        if (c < 0) {
            // Belongs at the head of the set
            new->next = set->head;
            set->head = new;
        }
        else {
            // Duplicate
            printf("Found duplicate at head\n");
            free(new);
            new = NULL;
        }
        retval = 1;
    }
    else {
        // Find a match or a place to insert
        ptr = set->head;
        prev = NULL;
        for (; ptr; prev = ptr, ptr = ptr->next) {
            c = set->compare(item, ptr->data);
            if (c == 0) {
                // Duplicate.
            printf("Found duplicate\n");
                free(new);
                new = NULL;
                return 1;
            }
            else if (c < 0) {
                // Node should be inserted in here.
                new->next = ptr;
                prev->next = new;
                return 1;
            }
        }

        // The item belongs at the end of the set.
        prev->next = new;
        retval = 1;
    }
    return retval;
}

/**
 * Returns one if the set contains the item; zero otherwise.
 */
int set_contains(Set *set, void *item) {
    Node *ptr;
    if (!set) {
        return 0;
    }
    for (ptr = set->head; ptr != NULL; ptr = ptr->next) {
        if (set->compare(item, ptr->data) == 0) {
            return 1;
        }
    }
    return 0;
}

/**
 * Creates a copy of the given set. Returns a pointer to the copy, or NULL if an
 * error occurrs or if the given set is NULL.
 */
Set *set_copy(Set *set) {
    Set *copy;
    Node *ptr;

    if (!set) {
        return NULL;
    }
    else {
        copy = set_create(set->compare);
        for (ptr = set->head; copy && ptr; ptr = ptr->next) {
            set_add(copy, ptr->data);
        }
        return copy;
    }
}

/**
 * Creates a new set of size zero. The argument is a pointer to a comparison
 * function, which is used to determine what is placed into the set.
 * If the function succeeds, this returns a pointer to the new set; otherwise,
 * it returns NULL.
 */
Set *set_create(CompFunc func) {
    Set *set = (Set *) malloc(sizeof(struct Set));
    if (func && set) {
        set->head = NULL;
        set->compare = func;
        return set;
    }
    else {
        free(set);
        return NULL;
    }
}

/**
 * Destroys the given set, freeing all associated memory. Do not attempt to use
 * a set iterator with a set that has already been destroyed.
 */
void set_destroy(Set *set) {
    Node *prev, *ptr;

    if (set) {
        ptr = set->head;
        prev = NULL;
        while (ptr) {
            prev = ptr;
            ptr = ptr->next;
            destroy_node(prev);
        }
        free(set);
    }
}

/**
 * Returns the intersection of the two sets - that is, the elements common to
 * both sets. Both sets should contain the same type of item and have the same
 * comparison function.
 * If the function succeeds, it returns a pointer to a valid set; if memory
 * allocation fails it returns NULL. If one set is NULL, it returns a copy of
 * the other set.
 */
Set *set_intersection(Set *s1, Set *s2) {
    Node *p1, *p2;
    Set *result;
    int c;

    if (!s1 || set_isempty(s1)) {
        return set_copy(s2);
    }
    else if (!s2 || set_isempty(s2)) {
        return set_copy(s1);
    }
    else if ((result = set_create(s1->compare)) == NULL) {
        return NULL;
    }

    p1 = s1->head;
    p2 = s2->head;

    while(p1 != NULL && p2 != NULL) {
        c = result->compare(p1->data, p2->data);
        if (c < 0) {
            p1 = p1->next;
        }
        else if (c > 0) {
            p2 = p2->next;
        }
        else {
            c = set_add(result, p1->data);
            if (c == 0) {
                fprintf(stderr, "Warning: an error occurred during the set"
                                "intersection.\n");
            }
            p1 = p1->next;
            p2 = p2->next;
        }
    }
    return result;
}

/**
 * Returns a positive number if the set is nonempty and zero otherwise.
 */
int set_isempty(Set *set) {
    return !(set && set->head);
}

/**
 * Returns the union of the two sets - that is a combination of the elements of
 * both sets, maintaining the invariant that there are no duplicates contained
 * within. Both sets should contain the same type of item and have the same
 * comparison function.
 * If both sets are empty, this returns the empty set. If either set is NULL,
 * this returns a copy of the other set.
 */
Set *set_union(Set *s1, Set *s2) {
    Node *ptr;
    Set *result;

    if (!s1) {
        return set_copy(s2);
    }
    else if (!s2) {
        return set_copy(s1);
    }
    else if (!(result = set_create(s1->compare))) {
        return NULL;
    }

    // Naive version
    for (ptr = s1->head; ptr; ptr = ptr->next) {
        set_add(result, ptr->data);
    }
    for (ptr = s2->head; ptr; ptr = ptr->next) {
        set_add(result, ptr->data);
    }
    return result;
}

/**
 * Creates an iterator object that allows the caller to walk the set, item by
 * item. If the function succeeds, it returns a pointer to a set iterator.
 * Otherwise, it returns NULL.
 */
SetIterator *setiterator_create(Set *set) {
    SetIterator *iterator = (SetIterator *) malloc(sizeof(struct SetIterator));
    if (set && iterator) {
        iterator->ptr = set->head;
        return iterator;
    }
    else {
        free(iterator);
        return NULL;
    }
}

/**
 * Destroys the set iterator, freeing all associated memory.
 */
void setiterator_destroy(SetIterator *iterator) {
    if (iterator) {
        free(iterator);
    }
}

/**
 * Returns the next object in the iteration, or NULL if the end of the iteration
 * has been reached.
 */
void *setiterator_next(SetIterator *iterator) {
    void *data;

    if (!iterator || !iterator->ptr) {
        return NULL;
    }
    else {
        data = iterator->ptr->data;
        iterator->ptr = iterator->ptr->next;
        return data;
    }
}
