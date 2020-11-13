/*
 * table.c -- symbol table
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <util/memory.h>
#include <util/errors.h>
#include "identifier.h"
#include "types/types.h"
#include "table.h"

static Entry *newEntry(Identifier *name, entry_kind kind) {
    Entry *entry = (Entry *) allocate(sizeof(Entry));
    entry->kind = kind;
    entry->name = name;

    return entry;
}

Entry *newTypeEntry(Identifier *name, Type *type) {
    Entry *entry = newEntry(name, ENTRY_KIND_TYPE);
    entry->u.typeEntry.type = type;
    return entry;
}


Entry *newVarEntry(Identifier *name, Type *type, bool isRef) {
    Entry *entry = newEntry(name, ENTRY_KIND_VAR);
    entry->u.varEntry.type = type;
    entry->u.varEntry.isRef = isRef;
    return entry;
}


Entry *newProcEntry(Identifier *name, ParamTypes *paramTypes, SymbolTable *localTable) {
    Entry *entry = newEntry(name, ENTRY_KIND_PROC);
    entry->u.procEntry.paramTypes = paramTypes;
    entry->u.procEntry.localTable = localTable;
    return entry;
}

Entry *newPredefinedProcEntry(Identifier *name, ParamTypes *paramTypes, int argumentAreaSize) {
    Entry *entry = newEntry(name, ENTRY_KIND_PROC);
    entry->u.procEntry.paramTypes = paramTypes;
    entry->u.procEntry.argumentArea = argumentAreaSize;
    entry->u.procEntry.localTable = NULL;
    return entry;
}


SymbolTable *newTable(SymbolTable *upperLevel) {
    SymbolTable *table;

    table = (SymbolTable *) allocate(sizeof(SymbolTable));
    table->bintree = NULL;
    table->upperLevel = upperLevel;
    return table;
}


static void enterPredefinedTypes(SymbolTable *table) {
    enter(table, newTypeEntry(newIdentifier("int"), intType));
}


static void enterPredefinedProcs(SymbolTable *table) {
    ParamTypes *eop;
    Entry *procEntry;

    eop = emptyParamTypes();

    /* printi(i: int) */
    procEntry = newPredefinedProcEntry(newIdentifier("printi"),
                                       newPredefinedParamTypes(intType, false, 0, eop),
                                       4);
    enter(table, procEntry);

    /* printc(i: int) */
    procEntry = newPredefinedProcEntry(newIdentifier("printc"),
                                       newPredefinedParamTypes(intType, false, 0, eop),
                                       4);
    enter(table, procEntry);

    /* readi(ref i: int) */
    procEntry = newPredefinedProcEntry(newIdentifier("readi"),
                                       newPredefinedParamTypes(intType, true, 0, eop),
                                       4);
    enter(table, procEntry);

    /* readc(ref i: int) */
    procEntry = newPredefinedProcEntry(newIdentifier("readc"),
                                       newPredefinedParamTypes(intType, true, 0, eop),
                                       4);
    enter(table, procEntry);

    /* exit() */
    procEntry = newPredefinedProcEntry(newIdentifier("exit"), eop,
                                       0);
    enter(table, procEntry);

    /* time(ref i: int) */
    procEntry = newPredefinedProcEntry(newIdentifier("time"),
                                       newPredefinedParamTypes(intType, true, 0, eop),
                                       4);
    enter(table, procEntry);

    /* clearAll(color: int) */
    procEntry = newPredefinedProcEntry(newIdentifier("clearAll"),
                                       newPredefinedParamTypes(intType, false, 0, eop),
                                       4);
    enter(table, procEntry);

    /* setPixel(x: int, y: int, color: int) */
    procEntry = newPredefinedProcEntry(newIdentifier("setPixel"),
                                       newPredefinedParamTypes(intType, false, 0,
                                                               newPredefinedParamTypes(intType, false, 4,
                                                                                       newPredefinedParamTypes(intType,
                                                                                                               false, 8,
                                                                                                               eop))),
                                       12);
    enter(table, procEntry);

    /* drawLine(x1: int, y1: int, x2: int, y2: int, color: int) */
    procEntry = newPredefinedProcEntry(newIdentifier("drawLine"),
                                       newPredefinedParamTypes(intType, false, 0,
                                                               newPredefinedParamTypes(intType, false, 4,
                                                                                       newPredefinedParamTypes(intType,
                                                                                                               false, 8,
                                                                                                               newPredefinedParamTypes(
                                                                                                                       intType,
                                                                                                                       false,
                                                                                                                       12,
                                                                                                                       newPredefinedParamTypes(
                                                                                                                               intType,
                                                                                                                               false,
                                                                                                                               16,
                                                                                                                               eop))))),
                                       20);
    enter(table, procEntry);

    /* drawCircle(x0: int, y0: int, radius: int, color: int) */
    procEntry = newPredefinedProcEntry(newIdentifier("drawCircle"),
                                       newPredefinedParamTypes(intType, false, 0,
                                                               newPredefinedParamTypes(intType, false, 4,
                                                                                       newPredefinedParamTypes(intType,
                                                                                                               false, 8,
                                                                                                               newPredefinedParamTypes(
                                                                                                                       intType,
                                                                                                                       false,
                                                                                                                       12,
                                                                                                                       eop)))),
                                       16);
    enter(table, procEntry);
}

SymbolTable *initializeGlobalTable(void) {
    SymbolTable *table;

    table = newTable(NULL);
    enterPredefinedTypes(table);
    enterPredefinedProcs(table);
    return table;
}

Entry *enter(SymbolTable *table, Entry *entry) {
    unsigned key;
    Bintree *newtree;
    Bintree *current;
    Bintree *previous;
    Identifier *sym = entry->name;

    key = sym->stamp;
    newtree = (Bintree *) allocate(sizeof(Bintree));
    newtree->sym = sym;
    newtree->key = key;
    newtree->entry = entry;
    newtree->left = NULL;
    newtree->right = NULL;
    if (table->bintree == NULL) {
        table->bintree = newtree;
    } else {
        current = table->bintree;
        while (1) {
            if (current->key == key) {
                /* symbol already in table */
                return NULL;
            }
            previous = current;
            if (current->key > key) {
                current = current->left;
            } else {
                current = current->right;
            }
            if (current == NULL) {
                if (previous->key > key) {
                    previous->left = newtree;
                } else {
                    previous->right = newtree;
                }
                break;
            }
        }
    }
    return entry;
}

static Entry *lookupBintree(Bintree *bintree, unsigned key) {
    while (bintree != NULL) {
        if (bintree->key == key) {
            return bintree->entry;
        }
        if (bintree->key > key) {
            bintree = bintree->left;
        } else {
            bintree = bintree->right;
        }
    }
    return NULL;
}


Entry *lookup(SymbolTable *table, Identifier *name) {
    unsigned key;
    Entry *entry;

    key = name->stamp;
    while (table != NULL) {
        entry = lookupBintree(table->bintree, key);
        if (entry != NULL) {
            return entry;
        }
        table = table->upperLevel;
    }
    return NULL;
}


void showEntry(Entry *entry) {
    switch (entry->kind) {
        case ENTRY_KIND_TYPE:
            printf("type: ");
            showType(entry->u.typeEntry.type);
            break;
        case ENTRY_KIND_VAR:
            printf("var: ");
            if (entry->u.varEntry.isRef) {
                printf("ref ");
            }
            showType(entry->u.varEntry.type);
            break;
        case ENTRY_KIND_PROC:
            printf("proc: ");
            showParamTypes(entry->u.procEntry.paramTypes);
            break;
        default:
            error("unknown entry kind %d in showEntry", entry->kind);
    }
    printf("\n");
}


static void showBintree(Bintree *bintree) {
    if (bintree == NULL) {
        return;
    }
    showBintree(bintree->left);
    printf("  %-10s --> ", bintree->sym->string);
    showEntry(bintree->entry);
    showBintree(bintree->right);
}


void showTable(SymbolTable *table) {
    int level;

    level = 0;
    while (table != NULL) {
        printf("  level %d\n", level);
        showBintree(table->bintree);
        table = table->upperLevel;
        level++;
    }
}

ParamTypes *newParamTypes(Type *type, bool isRef, ParamTypes *next) {
    return newPredefinedParamTypes(type, isRef, 0, next);
}

ParamTypes *newPredefinedParamTypes(Type *type, bool isRef, int offset, ParamTypes *next) {
    ParamTypes *paramTypes;

    paramTypes = (ParamTypes *) allocate(sizeof(ParamTypes));
    paramTypes->isEmpty = false;
    paramTypes->type = type;
    paramTypes->isRef = isRef;
    paramTypes->offset = offset;
    paramTypes->next = next;
    return paramTypes;
}

void showParamTypes(ParamTypes *paramTypes) {
    printf("(");
    while (!paramTypes->isEmpty) {
        if (paramTypes->isRef) {
            printf("ref ");
        }
        showType(paramTypes->type);
        paramTypes = paramTypes->next;
        if (!paramTypes->isEmpty) {
            printf(", ");
        }
    }
    printf(")");
}

ParamTypes *emptyParamTypes(void) {
    static ParamTypes *nil = NULL;

    if (nil == NULL) {
        nil = (ParamTypes *) allocate(sizeof(ParamTypes));
        nil->isEmpty = true;
    }
    return nil;
}