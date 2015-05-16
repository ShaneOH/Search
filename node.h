#ifndef NODE_H
#define NODE_H

/*
 * Type for a basic linked list node.
 */
struct Node {
    void *data;
    struct Node *next;
    int references;
};

typedef struct Node Node;

/**
 * Creates a new node with the given data.
 */
Node *create_node(void *, struct Node *);

/**
 * Destroys the node, freeing all associated memory.
 */
void destroy_node(Node *);

#endif
