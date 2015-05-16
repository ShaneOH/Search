#include "inverted-index.h"
#include "record.h"
#include "set.h"
#include "sorted-list.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/**
 * A custom hash function that returns an index based on a string's first
 * letter. Returns a valid index for the inverted index, or -1 if the string
 * starts with an inappropriate character (something non-alphanumeric).
 */
int hash(const char *str) {
    if isalpha(str[0]) {
        return (int) (tolower(str[0]) - 'a');
    }
    else if (isdigit(str[0])) {
        return (int) (str[0] - '0') + 26;
    }
    else
        return -1;
}

/**
 * Returns a pointer to a new inverted index, or NULL if the call fails.
 */
Index *create_index() {
    Index *index;
    int i;

    index = (Index *) malloc(sizeof(struct Index));
    if (index != NULL) {
        for(i = 0; i < 36; i++) {
            index->lists[i] = NULL;
        }
        return index;
    }
    else
        return NULL;
}

/**
 * Adds or updates another record for the given token in the inverted index.
 */
int put_record(Index *index, const char *tok, const char *fname) {
    SortedList *list;
    int i;

    i = hash(tok);
    if (i == -1) {
        // This isn't a valid token and has no place in the index.
        return 0;
    }
    else {
        // Simply add or update the appropriate sorted list.
        list = index->lists[i];
        if (list == NULL) {
            index->lists[i] = sl_create(reccmp);
        }
        return sl_putrecord(index->lists[i], tok, fname);
    }
}

/**
 * Frees all dynamic memory associated with the given hash map. Note that the
 * use of all iterators associated with the index after its destruction is
 * extremely unsafe.
 */
void destroy_index(Index *index) {
    int i;

    for (i = 0; i < 36; i++) {
        sl_destroy(index->lists[i]);
    }
    free(index);
}

/**
 * Queries the inverted index for files containing the given token.
 * If the index is NULL, or if a memory error occurs, then this returns NULL.
 * Otherwise, this returns a set containing all of the filenames that contain
 * the given token. If there are no such files or if the token is not valid,
 * this returns the empty set.
 */
Set *query(Index *index, char *token) {
    Record *record;
    Set *result;
    SortedList *list;
    SortedListIterator *iterator;
    int h;

    if (!index || !token || !(result = set_create(generic_strcmp))) {
        return NULL;
    }
    else if ((h = hash(token)) == -1) {
        return result;
    }

    list = index->lists[h];
    if (list != NULL) {
        iterator = create_iter(list);
        if (!iterator) {
            free(result);
            return NULL;
        }
        else {
            while((record = next_item(iterator)) != NULL) {
                if (strcmp(token, record->token) != 0) {
                    continue;
                }
                else if (!set_add(result, record->filename)) {
                    // An error occurred while adding an item to the set.
                    free(result);
                    return NULL;
                }
            }
        }
    }

    return result;
}
