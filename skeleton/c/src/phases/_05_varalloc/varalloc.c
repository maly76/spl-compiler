/*
 * varalloc.c -- variable allocation
 */

#include <absyn/absyn.h>
#include <table/table.h>
#include <stdio.h>
#include <util/errors.h>
#include "types/types.h"
#include "varalloc.h"

/**
 * Formats the variables of a procedure to a human readable format and prints it
 * @param procDec       The procedure Declaration
 * @param globalTable   The global symbol table
 */
static void showProcedureVarAlloc(GlobalDeclaration *procDec, SymbolTable *globalTable) {
    SymbolTable *localTable;
    Entry *procEntry, *localEntry;
    ParamTypes *paramTypes;
    ParameterList *parameterList;
    VariableDeclarationList *variableList;
    int argNum;

    procEntry = lookup(globalTable, procDec->name);
    localTable = procEntry->u.procEntry.localTable;
    printf("\nVariable allocation for procedure '%s'\n", procDec->name->string);

    argNum = 1;
    paramTypes = procEntry->u.procEntry.paramTypes;
    while (!paramTypes->isEmpty) {
        printf("arg %d: sp + %d\n", argNum, paramTypes->offset);

        paramTypes = paramTypes->next;
        argNum++;
    }
    printf("size of argument area = %d\n", procEntry->u.procEntry.argumentArea);


    parameterList = procDec->u.procedureDeclaration.parameters;
    while (!parameterList->isEmpty) {
        printf("param '%s': fp + %d\n",
               parameterList->head->name->string,
               lookup(localTable, parameterList->head->name)->u.varEntry.offset);

        parameterList = parameterList->tail;
    }

    variableList = procDec->u.procedureDeclaration.variables;
    while (!variableList->isEmpty) {
        localEntry = lookup(localTable, variableList->head->name);
        if (localEntry->kind == ENTRY_KIND_VAR) {
            printf("var '%s': fp - %d\n",
                   variableList->head->name->string,
                   -localEntry->u.varEntry.offset);
        }

        variableList = variableList->tail;
    }

    printf("size of localvar area = %d\n", procEntry->u.procEntry.localvarArea);
    printf("size of outgoing area = %d\n", procEntry->u.procEntry.outgoingArea);
}

/**
  * Formats the variable allocation to a human readable format and prints it
  *
  * @param program      The abstract syntax tree of the program
  * @param globalTable  The symbol table containing all symbols of the spl program
  */
static void showVarAllocation(Program *program, SymbolTable *globalTable) {
    GlobalDeclarationList *declarationList;

    declarationList = program;
    while (!declarationList->isEmpty) {
        if (declarationList->head->kind == DECLARATION_PROCEDUREDECLARATION) {
            showProcedureVarAlloc(declarationList->head, globalTable);
        }

        declarationList = declarationList->tail;
    }
}

void allocVars(Program *program, SymbolTable *globalTable, bool showVarAlloc) {
    //TODO (assignment 5): Allocate stack slots for all parameters and local variables

    notImplemented();

    if (showVarAlloc) showVarAllocation(program, globalTable);
}
