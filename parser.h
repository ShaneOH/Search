#ifndef PARSER_H
#define PARSER_H

#include "inverted-index.h"
#include <stdlib.h>

/**
 * Parses the given file into an index in memory.
 */
Index *parse(char *);

#endif
