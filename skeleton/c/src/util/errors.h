/*
 * errors.h -- error reporting
 */

#ifndef SPL_ERRORS_H
#define SPL_ERRORS_H

#include <table/identifier.h>

/**
 * Displays an error to the user and aborts execution.
 *
 * Calling this function will exit the program with an exit code of 1.
 *
 * @param fmt A format string used to display the error message.
 * @param ... Additional parameters used by the format string.
 */
void error(char *fmt, ...);

/**
 * A function used to mark specific code blocks as "not implemented".
 * This function has to be removed from said code blocks and replaced by a correctly working code
 * as part of the different assignments.
 */
void notImplemented();

void syntaxError(int line, char *msg);
void undefinedType(int line, Identifier *name);
void notAType(int line, Identifier *name);
void redeclarationAsType(int line, Identifier *name);
void mustBeAReferenceParameter(int line, Identifier *name);
void redeclarationAsProcedure(int line, Identifier *name);
void redeclarationAsParameter(int line, Identifier *name);
void redeclarationAsVariable(int line, Identifier *name);
void assignmentHasDifferentTypes(int line);
void assignmentRequiresIntegers(int line);
void ifConditionMustBeBoolean(int line);
void whileConditionMustBeBoolean(int line);
void undefinedProcedure(int line, Identifier *name);
void callOfNonProcedure(int line, Identifier *name);
void argumentTypeMismatch(int line, Identifier *name, int argumentIndex);
void argumentMustBeAVariable(int line, Identifier *name, int argumentIndex);
void tooFewArguments(int line, Identifier *name);
void tooManyArguments(int line, Identifier *name);
void operatorDifferentTypes(int line);
void comparisonNonInteger(int line);
void arithmeticOperatorNonInteger(int line);
void undefinedVariable(int line, Identifier *name);
void notAVariable(int line, Identifier *name);
void indexingNonArray(int line);
void indexingWithNonInteger(int line);
void mainIsMissing();
void mainIsNotAProcedure();
void mainMustNotHaveParameters();
void illegalApostrophe(int line);
void illegalCharacter(int line, char character);

void registerOverflow();

#endif //SPL_ERRORS_H
