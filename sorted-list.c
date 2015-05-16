#include "node.h"
#include "record.h"
#include "sorted-list.h"
#include <stdlib.h>

/*
 * Creates a new sorted list of size zero. Returns a new sorted list object on
 * success; on failure, this function returns NULL.
 */
SortedList *sl_create(RecComp cf) {
    SortedList *list = (SortedList *) malloc(sizeof(struct SortedList));
    if (list) {
        list->head = NULL;
        list->compare = cf;
        return list;
    }
    else
        return NULL;
}

/*
 * Destroys the list pointed to by the given list pointer. Since the list is
 * implemented as a linked list, this method walks the list and frees all of
 * the dynamically allocated list nodes along the way.
 */
void sl_destroy(SortedList *list) {
    Node *ptr, *prev;
    Record *rec;

    if (list != NULL) {
        ptr = list->head;
        prev = NULL;
        while (ptr) {
            prev = ptr;
            ptr = ptr->next;
            rec = (Record *) prev->data;
            destroy_record(rec);
            free(prev);
        }
        free(list);
    }
}

/*
 * Inserts an object into the list, maintaining the invariant that each item is
 * less than or equal to the one after it. If the insertion is successful, the
 * function returns 1. If the insertion fails, or if the list pointer is NULL,
 * then it returns 0.
 */
int sl_putrecord(SortedList *list, const char *token, const char *filename) {
    Node *new, *ptr, *prev;
    Record *record, *temp;
    int c, retval;

    temp = create_record(token, filename, 1);
    if (!list || !temp) {
        retval = 0;
    }
    else if (!list->head) {
        // First item added to the list. Create a new node
        record = create_record(token, filename, 1);
        new = create_node(record, NULL);
        if (record && new) {
            list->head = new;
            retval = 1;
        }
        else {
            free(temp);
            free(record);
            free(new);
            retval = 0;
        }
    }
    else if ((c = list->compare(temp, (Record *) list->head->data)) <= 0) {
        if (c < 0) {
            // Belongs at the head of the list
            record = create_record(token, filename, 1);
            new = create_node(record, list->head);
            if (record && new) {
                list->head = new;
                retval = 1;
            }
            else {
                free(temp);
                free(record);
                free(new);
                retval = 0;
            }
        }
        else {
            // Match - update record!
            free(temp);
            temp = (Record *) list->head->data;
            temp->hits++;
            retval = 1;
        }
    }
    else {
        // Find a match or a place to insert
        ptr = list->head;
        prev = NULL;
        for (; ptr; prev = ptr, ptr = ptr->next) {
            c = list->compare(temp, ptr->data);
            if (c == 0) {
                // Update this node and we're done.
                free(temp);
                temp = (Record *) ptr->data;
                temp->hits++;
                return 1;
            }
            else if (c < 0) {
                // Node should be inserted in here.
                record = create_record(token, filename, 1);
                new = create_node(record, ptr);
                if (record && new) {
                    prev->next = new;
                    return 1;
                }
                else {
                    free(temp);
                    free(record);
                    free(new);
                    return 0;
                }
            }
        }

        // The item belongs at the end of the list.
        record = create_record(token, filename, 1);
        new = create_node(record, NULL);
        if (record && new) {
            prev->next = new;
            retval = 1;
        }
        else {
            free(temp);
            free(record);
            free(new);
            retval = 0;
        }
    }

    return retval;
}

/*
 * Creates a new iterator that allows you to traverse a sorted list. If the
 * allocation succeeds, this function returns a pointer to a new iterator
 * object; otherwise, it returns NULL.
 */
SortedListIterator *create_iter(SortedList *list) {
    if (!list) {
        return NULL;
    }

    SortedListIterator *iterator =
        (SortedListIterator *) malloc(sizeof(struct SortedListIterator));
    if (iterator) {
        iterator->ptr = list->head;
        return iterator;
    }
    else
        return NULL;
}

/*
 * Destroys an iterator, freeing its memory. If the iterator is pointing to a
 * list node that was deleted by the list during iteration, the iterator will
 * free the node if it is the last one pointing to it.
 */
void destroy_iter(SortedListIterator *iter) {
    if (!iter) {
        return;
    }

    free(iter);
}

/*
 * Returns the next item in the iteration. If the end of the list is reached,
 * this function returns NULL.
 *
 * In the face of concurrent modification, this iterator attempts to be as
 * robust as possible: if the current node in the iteration is deleted, a call
 * to this function will return what would have been the next element in the
 * sequence. Additions or deletions to other parts of the list will not affect
 * the iterator.
 *
 * If the list for the given iterator is destroyed, then the result of a call
 * to this function is unsafe and undefined.
 */
Record *next_item(SortedListIterator *iter) {
    if (!iter || !iter->ptr) {
        return NULL;
    }

    Record *data = (Record *) iter->ptr->data;
    iter->ptr = iter->ptr->next;

    return data;
}
