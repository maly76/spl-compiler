/*
 * errors.c -- error reporting
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "errors.h"

void error(char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    fprintf(stderr, "An error occurred: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);

    exit(1);
}

void notImplemented() {
    error("You have not implemented this operation yet!");
}


static void splError(int errorCode, int line, const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    fprintf(stderr, "An error occurred:\n");
    if (line >= 0) {
        fprintf(stderr, "Line %d: ", line);
    }
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);

    exit(errorCode);
}

void syntaxError(int line, char *msg) {
    splError(100, line, msg);
}

void undefinedType(int line, Identifier *name) {
    splError(101, line, "undefined type %s", name->string);
}

void notAType(int line, Identifier *name) {
    splError(102, line, "%s is not a type", name->string);
}

void redeclarationAsType(int line, Identifier *name) {
    splError(103, line, "redeclaration of %s as type", name->string);
}

void mustBeAReferenceParameter(int line, Identifier *name) {
    splError(104, line, "parameter %s must be a reference parameter", name->string);
}

void redeclarationAsProcedure(int line, Identifier *name) {
    splError(105, line, "redeclaration of %s as procedure", name->string);
}

void redeclarationAsParameter(int line, Identifier *name) {
    splError(106, line, "redeclaration of %s as parameter", name->string);
}

void redeclarationAsVariable(int line, Identifier *name) {
    splError(107, line, "redeclaration of %s as variable", name->string);
}

void assignmentHasDifferentTypes(int line) {
    splError(108, line, "assignment has different types");
}

void assignmentRequiresIntegers(int line) {
    splError(109, line, "assignment requires integer variable");
}

void ifConditionMustBeBoolean(int line) {
    splError(110, line, "'if' test expression must be of type boolean");
}

void whileConditionMustBeBoolean(int line) {
    splError(111, line, "'while' test expression must be of type boolean");
}

void undefinedProcedure(int line, Identifier *name) {
    splError(112, line, "undefined procedure %s", name->string);
}

void callOfNonProcedure(int line, Identifier *name) {
    splError(113, line, "call of non-procedure %s", name->string);
}

void argumentTypeMismatch(int line, Identifier *name, int argumentIndex) {
    splError(114, line, "procedure %s argument %d type mismatch", name->string, argumentIndex);
}

void argumentMustBeAVariable(int line, Identifier *name, int argumentIndex) {
    splError(115, line, "procedure %s argument %d must be a variable", name->string, argumentIndex);
}

void tooFewArguments(int line, Identifier *name) {
    splError(116, line, "procedure %s called with too few arguments", name->string);
}

void tooManyArguments(int line, Identifier *name) {
    splError(117, line, "procedure %s called with too many arguments", name->string);
}

void operatorDifferentTypes(int line) {
    splError(118, line, "expression combines different types");
}

void comparisonNonInteger(int line) {
    splError(119, line, "comparison requires integer operands");
}

void arithmeticOperatorNonInteger(int line) {
    splError(120, line, "arithmetic operation requires integer operands");
}

void undefinedVariable(int line, Identifier *name) {
    splError(121, line, "undefined variable %s", name->string);
}

void notAVariable(int line, Identifier *name) {
    splError(122, line, "%s is not a variable", name->string);
}

void indexingNonArray(int line) {
    splError(123, line, "illegal indexing a non-array");
}

void indexingWithNonInteger(int line) {
    splError(124, line, "illegal indexing with a non-integer");
}

void mainIsMissing() {
    splError(125, -1, "procedure 'main' is missing");
}

void mainIsNotAProcedure() {
    splError(126, -1, "'main' is not a procedure");
}

void mainMustNotHaveParameters() {
    splError(127, -1, "procedure 'main' must not have any parameters");
}

void illegalApostrophe(int line) {
    splError(99, line, "illegal use of apostrophe");
}

void illegalCharacter(int line, char character) {
    splError(99, line, "Illegal character 0x%02x", (unsigned char) character);
}

void registerOverflow() {
    splError(140, -1, "There are not enough registers to run this program!");
}

