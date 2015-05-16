#include "inverted-index.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Returns a positive number if the filename is a readable file; zero otherwise.
 */
int is_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    else {
        return 0;
    }
}

/**
 * Parses the given file into an inverted-index in memory. Returns a pointer to
 * the new inverted index, or NULL if an error occurs.
 */
Index *parse(char *filename) {
    FILE *file;
    Index *index;
    char *tname, *token, *lineptr;
    size_t len;
    ssize_t read;

    if (!filename || !is_file(filename)) {
        fprintf(stderr, "Not a valid filename.\n");
        return NULL;
    }
    else if (!(index = create_index())) {
        fprintf(stderr, "An error occurred during memory allocation.\n");
        return NULL;
    }
    else if (!(file = fopen(filename, "r"))) {
        fprintf(stderr, "Could not open file '%s' for writing.\n", filename);
        destroy_index(index);
        return NULL;
    }

    lineptr = NULL;
    len = 0;

    while ((read = getline(&lineptr, &len, file)) != -1) {
        tname = strtok(lineptr, " \n");
        strtok(NULL, " \n");
        while ((token = strtok(NULL, " \n")) != NULL) {
            put_record(index, tname, token);
        }
    }
    free(lineptr);

    fclose(file);
    return index;
}
