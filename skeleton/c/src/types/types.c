/*
 * types.c -- type representation
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <util/memory.h>
#include <util/errors.h>
#include "types.h"

Type *newPrimitiveType(char *printName, int byteSize) {
    Type *type;

    type = (Type *) allocate(sizeof(Type));
    type->kind = TYPE_KIND_PRIMITIVE;
    type->u.primitiveType.printName = printName;
    type->byteSize = byteSize;
    return type;
}


Type *newArrayType(int size, Type *baseType) {
    Type *type;

    type = (Type *) allocate(sizeof(Type));
    type->kind = TYPE_KIND_ARRAY;
    type->u.arrayType.size = size;
    type->u.arrayType.baseType = baseType;
    type->byteSize = size * baseType->byteSize;
    return type;
}

void showType(Type *type) {
    switch (type->kind) {
        case TYPE_KIND_PRIMITIVE:
            printf("%s", type->u.primitiveType.printName);
            break;
        case TYPE_KIND_ARRAY:
            printf("array [%d] of ", type->u.arrayType.size);
            showType(type->u.arrayType.baseType);
            break;
        default:
            error("unknown type kind %d in showType", type->kind);
    }
}

Type *intType = NULL;
Type *boolType = NULL;
