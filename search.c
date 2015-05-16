#include "parser.h"
#include "set.h"
#include "node.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXBUFSIZE 1024

/**
 * Prints the members of the set to standard out.
 */
void set_print(Set *set) {
    Node *ptr;
    if (set && set->head) {
        for (ptr = set->head; ptr != NULL; ptr = ptr->next) {
            printf("'%s' ", (char *) ptr->data);
        }
        printf("\n");
    }
}

/**
 * Prints the expected program usage to standard out.
 */
void show_usage(void) {
    printf("Usage: search <inverted-index-file>\n");
}

/**
 * Runs the searcher.
 */
int main(int argc, char **argv) {
    Index *index;
    Set *result, *newresult, *temp;
    SetIterator *iterator;
    char *first, *token, *delims, *tempstr;
    char buffer[MAXBUFSIZE];
    int i;

    if (argc != 2) {
        // Unexpected number of arguments.
        fprintf(stderr, "search: Unexpected number of arguments.\n");
        show_usage();
        return 1;
    }
    else if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        // Invoking for help.
        show_usage();
        return 0;
    }

    index = parse(argv[1]);
    if (!index) {
        // Parsing failed.
        return 1;
    }

    delims = " \n";
    while(1) {
        // Main program loop.
        result = set_create(generic_strcmp);

        printf("\nEnter a search query:\n");
        fgets(buffer, MAXBUFSIZE, stdin);
        for (i = 0; i < MAXBUFSIZE; i++) {
            if (buffer[i] == '\0') {
                break;
            }
            else {
                buffer[i] = tolower(buffer[i]);
            }
        }

        // Tokenize the input line by spaces.
        first = strtok(buffer, delims);
        if (!first) {
            // Empty line or error
            printf("That's not a valid input. Try again.\n");
            set_destroy(result);
            continue;
        }
        else if (strcmp(first, "sa") == 0) {
            // Logical AND
            while ((token = strtok(NULL, delims)) != NULL) {
                temp = query(index, token);
                newresult = set_intersection(result, temp);
                free(result);
                free(temp);
                result = newresult;
            }
        }
        else if (strcmp(first, "so") == 0) {
            // Logical OR
            while ((token = strtok(NULL, delims)) != NULL) {
                temp = query(index, token);
                newresult = set_union(result, temp);
                free(result);
                free(temp);
                result = newresult;
            }
        }
        else if (strcmp(first, "q") == 0) {
            // Quit
            printf("Exiting. Goodbye!\n");
            set_destroy(result);
            break;
        }
        else {
            // Invalid input
            printf("That's not a valid input. Try again.\n");
            set_destroy(result);
            continue;
        }

        // Finally, print the result to standard out
        if (result == NULL || set_isempty(result) == 1) {
            // Either an error occurred or there's no result.
            printf("No hits found.\n");
        }
        else {
            // Go through the set and print out all the hits.
            printf("Your search returned: \n");
            iterator = setiterator_create(result);
            while ((tempstr = setiterator_next(iterator)) != NULL) {
                printf("'%s' ", tempstr);
            }
            printf("\n");
            setiterator_destroy(iterator);
        }
        set_destroy(result);
    }

    // Clean up.
    destroy_index(index);
    return 0;
}
