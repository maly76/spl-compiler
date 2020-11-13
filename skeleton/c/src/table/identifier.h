/*
 * identifier.h -- symbol management
 */


#ifndef _IDENTIFIER_H_
#define _IDENTIFIER_H_


#define INITIAL_HASH_SIZE	100	/* will be increased to next prime */

/**
 * Represents an identifier in SPL.
 * String interning is used to speed up comparisons in the symbol table.
 */
typedef struct identifier {
  char *string;			/* external representation of symbol */
  unsigned stamp;		/* unique random stamp for external use */
  unsigned hashValue;		/* hash value of string, internal use */
  struct identifier *next;		/* symbol chaining, internal use */
} Identifier;

/**
 * Constructs a new Identifier by interning the given string and allocating space for the struct.
 * @param string The string representing the Identifier.
 * @return A reference to a newly created Identifier.
 */
Identifier *newIdentifier(char *string);

#endif /* _IDENTIFIER_H_ */
