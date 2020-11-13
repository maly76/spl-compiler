/*
 * types.h -- type representation
 */


#ifndef _TYPES_H_
#define _TYPES_H_

#include <stdbool.h>

/**
 * Used to differentiate between type kinds.
 */
typedef enum {
    TYPE_KIND_PRIMITIVE,
    TYPE_KIND_ARRAY
} type_kind;

/**
 * Represents the semantic type of expressions and variables.
 *
 * There are two kinds of Types in SPL.
 *
 * 1. Primitive Types representing the builtin types in SPL.
 * There are exactly two primitive types in SPL ("int" and "boolean") which are defined in this file.
 *
 * 2. Array Types representing the types of arrays in SPL.
 * They are constructed every time an ArrayTypeExpression is encountered in the source code.
 */
typedef struct type {
    type_kind kind;
    union {
        struct {
            char *printName;
        } primitiveType;
        struct {
            int size;
            struct type *baseType;
        } arrayType;
    } u;
    int byteSize;
} Type;

/**
 * Contains the information about parameters of a procedure.
 * This information is required for calling the associated procedure.
 *
 * Since a procedure may have an arbitrary amount of parameters, this information is stored in a linked list.
 */
typedef struct paramtypes {
    bool isEmpty;
    Type *type;
    bool isRef;
    int offset;        /* filled in by variable allocator */
    struct paramtypes *next;
} ParamTypes;

/**
 * Creates a new primitive type representing a builtin type in SPL.
 * @param printName The name of the primitive type.
 * @param byteSize The size in bytes required to hold a value of this type.
 * @return A reference to the newly created primitive type.
 */
Type *newPrimitiveType(char *printName, int byteSize);

/**
 * Creates a new array type representing the type of an array in SPL.
 * This automatically calculates the size in byte required to hold a value of this type.
 * @param size The amount of elements an array of this type can hold.
 * @param baseType The type of the arrays elements.
 * @return A reference to the newly created array type.
 */
Type *newArrayType(int size, Type *baseType);

/**
 * Prints a given type in a human readable format.
 * @param type The type to print.
 */
void showType(Type *type);

/**
 * The primitive type "int".
 */
extern Type *intType;
/**
 * The primitive type "boolean".
 */
extern Type *boolType;

#endif /* _TYPES_H_ */
