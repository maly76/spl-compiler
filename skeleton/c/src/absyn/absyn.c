/*
 * absyn.c -- abstract syntax
 */

#include <util/errors.h>
#include <util/memory.h>
#include <stdio.h>
#include <stdarg.h>
#include "table/identifier.h"
#include "types/types.h"
#include "absyn.h"


/**************************************************************/

const char *BINARY_OPERATOR_NAMES[] = {
        "EQU",
        "NEQ",
        "LST",
        "LSE",
        "GRT",
        "GRE",
        "ADD",
        "SUB",
        "MUL",
        "DIV"
};

TypeExpression *newTypeExpression(int line, type_expression_kind kind) {
    TypeExpression *node = allocate(sizeof(TypeExpression));

    node->line = line;
    node->kind = kind;
    node->dataType = NULL;
    return node;
}


TypeExpression *newNamedTypeExpression(int line, Identifier *name) {
    TypeExpression *node = newTypeExpression(line, TYPEEXPRESSION_NAMEDTYPEEXPRESSION);
    node->u.namedTypeExpression.name = name;
    return node;
}

TypeExpression *newArrayTypeExpression(int line, TypeExpression *ty, int size) {
    TypeExpression *node = newTypeExpression(line, TYPEEXPRESSION_ARRAYTYPEEXPRESSION);
    node->u.arrayTypeExpression.arraySize = size;
    node->u.arrayTypeExpression.baseType = ty;
    return node;
}


GlobalDeclaration *newGlobalDeclaration(int line, global_declaration_kind kind, Identifier *name) {
    GlobalDeclaration *node = allocate(sizeof(GlobalDeclaration));

    node->line = line;
    node->kind = kind;
    node->name = name;
    return node;
}


GlobalDeclaration *newTypeDeclaration(int line, Identifier *name, TypeExpression *ty) {
    GlobalDeclaration *node = newGlobalDeclaration(line, DECLARATION_TYPEDECLARATION, name);
    node->u.typeDeclaration.typeExpression = ty;
    return node;
}

GlobalDeclaration *newProcedureDeclaration(int line, Identifier *name, ParameterList *params,
                                           VariableDeclarationList *decls, StatementList *body) {
    GlobalDeclaration *node = newGlobalDeclaration(line, DECLARATION_PROCEDUREDECLARATION, name);
    node->u.procedureDeclaration.parameters = params;
    node->u.procedureDeclaration.variables = decls;
    node->u.procedureDeclaration.body = body;
    return node;
}


ParameterDeclaration *newParameterDeclaration(int line, Identifier *name, TypeExpression *ty, bool isRef) {
    ParameterDeclaration *node = (ParameterDeclaration *) allocate(sizeof(ParameterDeclaration));
    node->line = line;
    node->name = name;
    node->typeExpression = ty;
    node->isReference = isRef;
    return node;
}


VariableDeclaration *newVariableDeclaration(int line, Identifier *name, TypeExpression *ty) {
    VariableDeclaration *node = (VariableDeclaration *) allocate(sizeof(VariableDeclaration));
    node->line = line;
    node->name = name;
    node->typeExpression = ty;
    return node;
}


Statement *newStatement(int line, statement_kind kind) {
    Statement *node = allocate(sizeof(Statement));

    node->line = line;
    node->kind = kind;
    return node;
}


Statement *newEmptyStatement(int line) {
    Statement *node = newStatement(line, STATEMENT_EMPTYSTATEMENT);
    return node;
}


Statement *newCompoundStatement(int line, StatementList *stms) {
    Statement *node = newStatement(line, STATEMENT_COMPOUNDSTATEMENT);
    node->u.compoundStatement.statements = stms;
    return node;
}


Statement *newAssignStatement(int line, Variable *var, Expression *exp) {
    Statement *node = newStatement(line, STATEMENT_ASSIGNSTATEMENT);
    node->u.assignStatement.target = var;
    node->u.assignStatement.value = exp;
    return node;
}


Statement *newIfStatement(int line, Expression *condition, Statement *thenPart, Statement *elsePart) {
    Statement *node = newStatement(line, STATEMENT_IFSTATEMENT);
    node->u.ifStatement.condition = condition;
    node->u.ifStatement.thenPart = thenPart;
    node->u.ifStatement.elsePart = elsePart;
    return node;
}


Statement *newWhileStatement(int line, Expression *condition, Statement *body) {
    Statement *node = newStatement(line, STATEMENT_WHILESTATEMENT);
    node->u.whileStatement.condition = condition;
    node->u.whileStatement.body = body;
    return node;
}


Statement *newCallStatement(int line, Identifier *name, ExpressionList *args) {
    Statement *node = newStatement(line, STATEMENT_CALLSTATEMENT);
    node->u.callStatement.procedureName = name;
    node->u.callStatement.argumentList = args;
    return node;
}


Expression *newExpression(int line, expression_kind kind) {
    Expression *node = allocate(sizeof(Expression));

    node->line = line;
    node->kind = kind;
    node->dataType = NULL;
    return node;
}


Expression *newBinaryExpression(int line, binary_operator op, Expression *left, Expression *right) {
    Expression *node = newExpression(line, EXPRESSION_BINARYEXPRESSION);
    node->u.binaryExpression.operator = op;
    node->u.binaryExpression.leftOperand = left;
    node->u.binaryExpression.rightOperand = right;
    return node;
}

Expression *newVariableExpression(int line, Variable *var) {
    Expression *node = newExpression(line, EXPRESSION_VARIABLEEXPRESSION);
    node->u.variableExpression.variable = var;
    return node;
}


Expression *newIntLiteral(int line, int val) {
    Expression *node = newExpression(line, EXPRESSION_INTLITERAL);
    node->u.intLiteral.value = val;
    return node;
}


Variable *newVariable(int line, variable_kind kind) {
    Variable *node = allocate(sizeof(Variable));

    node->line = line;
    node->kind = kind;
    node->dataType = NULL;
    return node;
}


Variable *newNamedVariable(int line, Identifier *name) {
    Variable *node = newVariable(line, VARIABLE_NAMEDVARIABLE);
    node->u.namedVariable.name = name;
    return node;
}


Variable *newArrayAccess(int line, Variable *var, Expression *index) {
    Variable *node = newVariable(line, VARIABLE_ARRAYACCESS);
    node->u.arrayAccess.array = var;
    node->u.arrayAccess.index = index;
    return node;
}


StatementList *emptyStatementList(void) {
    static StatementList *nil = NULL;

    if (nil == NULL) {
        nil = (StatementList *) allocate(sizeof(StatementList));
        nil->isEmpty = true;
    }
    return nil;
}


StatementList *newStatementList(Statement *head, StatementList *tail) {
    StatementList *node = (StatementList *) allocate(sizeof(StatementList));
    node->isEmpty = false;
    node->head = head;
    node->tail = tail;
    return node;
}

ExpressionList *emptyExpressionList(void) {
    static ExpressionList *nil = NULL;

    if (nil == NULL) {
        nil = (ExpressionList *) allocate(sizeof(ExpressionList));
        nil->isEmpty = true;
    }
    return nil;
}

ExpressionList *newExpressionList(Expression *head, ExpressionList *tail) {
    ExpressionList *node = (ExpressionList *) allocate(sizeof(ExpressionList));
    node->isEmpty = false;
    node->head = head;
    node->tail = tail;
    return node;
}

GlobalDeclarationList *emptyGlobalDeclarationList(void) {
    static GlobalDeclarationList *nil = NULL;

    if (nil == NULL) {
        nil = (GlobalDeclarationList *) allocate(sizeof(GlobalDeclarationList));
        nil->isEmpty = true;
    }
    return nil;
}

GlobalDeclarationList *newGlobalDeclarationList(GlobalDeclaration *head, GlobalDeclarationList *tail) {
    GlobalDeclarationList *node = (GlobalDeclarationList *) allocate(sizeof(GlobalDeclarationList));
    node->isEmpty = false;
    node->head = head;
    node->tail = tail;
    return node;
}

VariableDeclarationList *emptyVariableList(void) {
    static VariableDeclarationList *nil = NULL;

    if (nil == NULL) {
        nil = (VariableDeclarationList *) allocate(sizeof(VariableDeclarationList));
        nil->isEmpty = true;
    }
    return nil;
}

VariableDeclarationList *newVariableList(VariableDeclaration *head, VariableDeclarationList *tail) {
    VariableDeclarationList *node = (VariableDeclarationList *) allocate(sizeof(VariableDeclarationList));
    node->isEmpty = false;
    node->head = head;
    node->tail = tail;
    return node;
}

ParameterList *emptyParameterList(void) {
    static ParameterList *nil = NULL;

    if (nil == NULL) {
        nil = (ParameterList *) allocate(sizeof(ParameterList));
        nil->isEmpty = true;
    }
    return nil;
}

ParameterList *newParameterList(ParameterDeclaration *head, ParameterList *tail) {
    ParameterList *node = (ParameterList *) allocate(sizeof(ParameterList));
    node->isEmpty = false;
    node->head = head;
    node->tail = tail;
    return node;
}

/**************************************************************/

static void indent(int indentation, char *fmt, ...);
static void showExpression(int indentation, Expression *expression);
static void showVariable(int indentation, Variable *variable);
static void showStatement(int indentation, Statement *statement);
static void showTypeExpression(int indentation, TypeExpression *typeExpression);
static void showParameterDeclaration(int indentation, ParameterDeclaration *parameterDeclaration);
static void showVariableDeclaration(int indentation, VariableDeclaration *variableDeclaration);
static void showGlobalDeclaration(int indentation, GlobalDeclaration *globalDeclaration);

#define INDENTATION_INCREMENT 2

void showAbsyn(Program *program) {
    if (program == NULL) {
        error("Program is NULL!");
        return;
    }

    indent(0, "Program(");
    if (!program->isEmpty) {
        // Print only the head
        printf("\n");
        showGlobalDeclaration(INDENTATION_INCREMENT, program->head);
        program = program->tail;

        // Print the tail of list
        while (!program->isEmpty) {
            printf(",\n");
            showGlobalDeclaration(INDENTATION_INCREMENT, program->head);
            program = program->tail;
        }
    }
    printf(")\n");
}


static void indent(int indentation, char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    printf("%*s", indentation, "");
    vprintf(fmt, ap);
    va_end(ap);
}

static void showInteger(int indentation, int integer) {
    indent(indentation, "%d", integer);
}

static void showBoolean(int indentation, bool boolean) {
    indent(indentation, "%s", boolean ? "true" : "false");
}

static void showIdentifier(int indentation, Identifier *identifier) {
    indent(indentation, "%s", identifier->string);
}

static void errorUndefinedKind(int kind, char *type) {
    error("Unknown kind! %d is not a %s!", kind, type);
}

/*
 * Expressions
 */

static void showBinaryOperator(int indentation, binary_operator binaryOperator) {
    indent(indentation, "%s", BINARY_OPERATOR_NAMES[binaryOperator]);
}

static void showBinaryExpression(int indentation, Expression *expression) {
    indent(indentation, "BinaryExpression(\n");
    showBinaryOperator(indentation + INDENTATION_INCREMENT, expression->u.binaryExpression.operator);
    printf(",\n");

    showExpression(indentation + INDENTATION_INCREMENT, expression->u.binaryExpression.leftOperand);
    printf(",\n");

    showExpression(indentation + INDENTATION_INCREMENT, expression->u.binaryExpression.rightOperand);
    printf(")");
}

static void showIntLiteral(int indentation, Expression *expression) {
    indent(indentation, "IntLiteral(\n");
    showInteger(indentation + INDENTATION_INCREMENT, expression->u.intLiteral.value);
    printf(")");
}

static void showVariableExpression(int indentation, Expression *expression) {
    indent(indentation, "VariableExpression(\n");
    showVariable(indentation + INDENTATION_INCREMENT, expression->u.variableExpression.variable);
    printf(")");
}

static void showExpression(int indentation, Expression *expression) {
    if (expression == NULL) {
        error("Expression is NULL!");
        return;
    }

    switch (expression->kind) {
        case EXPRESSION_BINARYEXPRESSION:
            showBinaryExpression(indentation, expression);
            break;
        case EXPRESSION_INTLITERAL:
            showIntLiteral(indentation, expression);
            break;
        case EXPRESSION_VARIABLEEXPRESSION:
            showVariableExpression(indentation, expression);
            break;
        default:
            errorUndefinedKind(expression->kind, "Expression");
    }
}

/*
 * Variables
 */

static void showNamedVariable(int indentation, Variable *variable) {
    indent(indentation, "NamedVariable(\n");
    showIdentifier(indentation + INDENTATION_INCREMENT, variable->u.namedVariable.name);
    printf(")");
}

static void showArrayAccess(int indentation, Variable *variable) {
    indent(indentation, "ArrayAccess(\n");
    showVariable(indentation + INDENTATION_INCREMENT, variable->u.arrayAccess.array);
    printf(",\n");
    showExpression(indentation + INDENTATION_INCREMENT, variable->u.arrayAccess.index);
    printf(")");
}

static void showVariable(int indentation, Variable *variable) {
    if (variable == NULL) {
        error("Variable is NULL!");
        return;
    }

    switch (variable->kind) {
        case VARIABLE_NAMEDVARIABLE:
            showNamedVariable(indentation, variable);
            break;
        case VARIABLE_ARRAYACCESS:
            showArrayAccess(indentation, variable);
            break;
        default:
            errorUndefinedKind(variable->kind, "Variable");
    }
}

/*
 * Statements
 */

static void showEmptyStatement(int indentation) {
    indent(indentation, "EmptyStatement()");
}

static void showCompoundStatement(int indentation, Statement *statement) {
    StatementList *statements;

    indent(indentation, "CompoundStatement(");
    statements = statement->u.compoundStatement.statements;
    if (!statements->isEmpty) {
        // Print only the head
        printf("\n");
        showStatement(indentation + INDENTATION_INCREMENT, statements->head);
        statements = statements->tail;

        // Print the tail of list
        while (!statements->isEmpty) {
            printf(",\n");
            showStatement(indentation + INDENTATION_INCREMENT, statements->head);
            statements = statements->tail;
        }
    }
    printf(")");
}

static void showAssignStatement(int indentation, Statement *statement) {
    indent(indentation, "AssignStatement(\n");
    showVariable(indentation + INDENTATION_INCREMENT, statement->u.assignStatement.target);
    printf(",\n");
    showExpression(indentation + INDENTATION_INCREMENT, statement->u.assignStatement.value);
    printf(")");
}

static void showIfStatement(int indentation, Statement *statement) {
    indent(indentation, "IfStatement(\n");
    showExpression(indentation + INDENTATION_INCREMENT, statement->u.ifStatement.condition);
    printf(",\n");
    showStatement(indentation + INDENTATION_INCREMENT, statement->u.ifStatement.thenPart);
    printf(",\n");
    showStatement(indentation + INDENTATION_INCREMENT, statement->u.ifStatement.elsePart);
    printf(")");
}

static void showWhileStatement(int indentation, Statement *statement) {
    indent(indentation, "WhileStatement(\n");
    showExpression(indentation + INDENTATION_INCREMENT, statement->u.whileStatement.condition);
    printf(",\n");
    showStatement(indentation + INDENTATION_INCREMENT, statement->u.whileStatement.body);
    printf(")");
}

static void showCallStatement(int indentation, Statement *statement) {
    ExpressionList *arguments;

    indent(indentation, "CallStatement(\n");
    showIdentifier(indentation + INDENTATION_INCREMENT, statement->u.callStatement.procedureName);
    printf(",\n");

    indent(indentation + INDENTATION_INCREMENT, "Arguments(");
    arguments = statement->u.callStatement.argumentList;
    if (!arguments->isEmpty) {
        // Print only the head
        printf("\n");
        showExpression(indentation + 2 * INDENTATION_INCREMENT, arguments->head);
        arguments = arguments->tail;

        // Print the tail of list
        while (!arguments->isEmpty) {
            printf(",\n");
            showExpression(indentation + 2 * INDENTATION_INCREMENT, arguments->head);
            arguments = arguments->tail;
        }
    }
    printf("))");
}

static void showStatement(int indentation, Statement *statement) {
    if (statement == NULL) {
        error("Statement is NULL!");
        return;
    }

    switch (statement->kind) {
        case STATEMENT_ASSIGNSTATEMENT:
            showAssignStatement(indentation, statement);
            break;
        case STATEMENT_CALLSTATEMENT:
            showCallStatement(indentation, statement);
            break;
        case STATEMENT_COMPOUNDSTATEMENT:
            showCompoundStatement(indentation, statement);
            break;
        case STATEMENT_EMPTYSTATEMENT:
            showEmptyStatement(indentation);
            break;
        case STATEMENT_IFSTATEMENT:
            showIfStatement(indentation, statement);
            break;
        case STATEMENT_WHILESTATEMENT:
            showWhileStatement(indentation, statement);
            break;
        default:
            errorUndefinedKind(statement->kind, "Statement");
    }
}

/*
 * Type Expressions
 */

static void showNamedTypeExpression(int indentation, TypeExpression *typeExpression) {
    indent(indentation, "NamedTypeExpression(\n");
    showIdentifier(indentation + INDENTATION_INCREMENT, typeExpression->u.namedTypeExpression.name);
    printf(")");
}

static void showArrayTypeExpression(int indentation, TypeExpression *typeExpression) {
    indent(indentation, "ArrayTypeExpression(\n");
    showTypeExpression(indentation + INDENTATION_INCREMENT, typeExpression->u.arrayTypeExpression.baseType);
    printf(",\n");
    showInteger(indentation + INDENTATION_INCREMENT, typeExpression->u.arrayTypeExpression.arraySize);
    printf(")");
}

static void showTypeExpression(int indentation, TypeExpression *typeExpression) {
    if (typeExpression == NULL) {
        error("Type Expression is NULL!");
        return;
    }

    switch (typeExpression->kind) {
        case TYPEEXPRESSION_ARRAYTYPEEXPRESSION:
            showArrayTypeExpression(indentation, typeExpression);
            break;
        case TYPEEXPRESSION_NAMEDTYPEEXPRESSION:
            showNamedTypeExpression(indentation, typeExpression);
            break;
        default:
            errorUndefinedKind(typeExpression->kind, "Type Expression");
    }
}

/*
 * Local Declarations
 */

static void showParameterDeclaration(int indentation, ParameterDeclaration *parameterDeclaration) {
    indent(indentation, "ParameterDeclaration(\n");
    showIdentifier(indentation + INDENTATION_INCREMENT, parameterDeclaration->name);
    printf(",\n");
    showTypeExpression(indentation + INDENTATION_INCREMENT, parameterDeclaration->typeExpression);
    printf(",\n");
    showBoolean(indentation + INDENTATION_INCREMENT, parameterDeclaration->isReference);
    printf(")");
}

static void showVariableDeclaration(int indentation, VariableDeclaration *variableDeclaration) {
    indent(indentation, "VariableDeclaration(\n");
    showIdentifier(indentation + INDENTATION_INCREMENT, variableDeclaration->name);
    printf(",\n");
    showTypeExpression(indentation + INDENTATION_INCREMENT, variableDeclaration->typeExpression);
    printf(")");
}

/*
 * Global Declarations
 */

static void showTypeDeclaration(int indentation, GlobalDeclaration *globalDeclaration) {
    indent(indentation, "TypeDeclaration(\n");
    showIdentifier(indentation + INDENTATION_INCREMENT, globalDeclaration->name);
    printf(",\n");
    showTypeExpression(indentation + INDENTATION_INCREMENT, globalDeclaration->u.typeDeclaration.typeExpression);
    printf(")");
}

static void showProcedureDeclaration(int indentation, GlobalDeclaration *globalDeclaration) {
    ParameterList *params;
    VariableDeclarationList *variables;
    StatementList *statements;

    indent(indentation, "ProcedureDeclaration(\n");
    showIdentifier(indentation + INDENTATION_INCREMENT, globalDeclaration->name);
    printf(",\n");

    indent(indentation + INDENTATION_INCREMENT, "Parameters(");
    params = globalDeclaration->u.procedureDeclaration.parameters;
    if (!params->isEmpty) {
        // Print only the head
        printf("\n");
        showParameterDeclaration(indentation + 2 * INDENTATION_INCREMENT, params->head);
        params = params->tail;

        // Print the tail of list
        while (!params->isEmpty) {
            printf(",\n");
            showParameterDeclaration(indentation + 2 * INDENTATION_INCREMENT, params->head);
            params = params->tail;
        }
    }
    printf("),\n");

    indent(indentation + INDENTATION_INCREMENT, "Variables(");
    variables = globalDeclaration->u.procedureDeclaration.variables;
    if (!variables->isEmpty) {
        // Print only the head
        printf("\n");
        showVariableDeclaration(indentation + 2 * INDENTATION_INCREMENT, variables->head);
        variables = variables->tail;

        // Print the tail of list
        while (!variables->isEmpty) {
            printf(",\n");
            showVariableDeclaration(indentation + 2 * INDENTATION_INCREMENT, variables->head);
            variables = variables->tail;
        }
    }
    printf("),\n");

    indent(indentation + INDENTATION_INCREMENT, "Body(");
    statements = globalDeclaration->u.procedureDeclaration.body;
    if (!statements->isEmpty) {
        // Print only the head
        printf("\n");
        showStatement(indentation + 2 * INDENTATION_INCREMENT, statements->head);
        statements = statements->tail;

        // Print the tail of list
        while (!statements->isEmpty) {
            printf(",\n");
            showStatement(indentation + 2 * INDENTATION_INCREMENT, statements->head);
            statements = statements->tail;
        }
    }
    printf(")");

    // End of ProcedureDeclaration
    printf(")");
}

static void showGlobalDeclaration(int indentation, GlobalDeclaration *globalDeclaration) {
    if (globalDeclaration == NULL) {
        error("Global Declaration is NULL!");
        return;
    }

    switch (globalDeclaration->kind) {
        case DECLARATION_PROCEDUREDECLARATION:
            showProcedureDeclaration(indentation, globalDeclaration);
            break;
        case DECLARATION_TYPEDECLARATION:
            showTypeDeclaration(indentation, globalDeclaration);
            break;
        default:
            errorUndefinedKind(globalDeclaration->kind, "Global Declaration");
    }
}
