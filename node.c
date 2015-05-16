#include "node.h"
#include <stdlib.h>

/**
 * Creates a new node with the given data. Returns a pointer to a new node, or
 * NULL if memory allocation fails.
 */
Node *create_node(void *data, struct Node *next) {
    Node *node = (Node *) malloc(sizeof(struct Node));
    if (node) {
        node->data = data;
        node->next = next;
        node->references = 0;
        return node;
    }
    else
        return NULL;
}

/**
 * Destroys the node, freeing all associated memory.
 */
void destroy_node(Node *node) {
    free(node);
}
