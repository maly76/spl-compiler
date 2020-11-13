/*
 * procedurebodycheck.h -- semantic analysis
 */


#ifndef _PROCEDUREBODYCHECK_H_
#define _PROCEDUREBODYCHECK_H_

#include <absyn/absyn.h>
#include <table/table.h>

/**
 * This function is used to check if the currently compiled SPL program is semantically valid.
 * Every statement and expression has to be checked, to ensure that every type is correct.
 *
 * Types calculated in this function can be stored in the type field of Expressions, Variables and TypeExpressions.
 *
 * @param program The program to be checked for semantic correctness.
 * @param globalTable The symbol table for the current program.
 */
void check(Program *program, SymbolTable *globalTable);

#endif /* _PROCEDUREBODYCHECK_H_ */
