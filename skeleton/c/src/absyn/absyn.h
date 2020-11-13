/*
 * absyn.h -- abstract syntax
 */

#ifndef _ABSYN_H_
#define _ABSYN_H_

#include <stdbool.h>
#include "types/types.h"
#include "table/identifier.h"

/**
 * This enum represents the possible operators for binary expressions in SPL.
 */
typedef enum {
    ABSYN_OP_EQU,
    ABSYN_OP_NEQ,
    ABSYN_OP_LST,
    ABSYN_OP_LSE,
    ABSYN_OP_GRT,
    ABSYN_OP_GRE,
    ABSYN_OP_ADD,
    ABSYN_OP_SUB,
    ABSYN_OP_MUL,
    ABSYN_OP_DIV
} binary_operator;

/**
 * This array holds the short-codes of the operators in an order correspond the binary_operator enum.
 */
extern const char *BINARY_OPERATOR_NAMES[];

/**
 * Used to differentiate between type expression kinds.
 */
typedef enum {
    TYPEEXPRESSION_NAMEDTYPEEXPRESSION,
    TYPEEXPRESSION_ARRAYTYPEEXPRESSION
} type_expression_kind;
/**
 * Used to differentiate between expression kinds.
 */
typedef enum {
    EXPRESSION_BINARYEXPRESSION,
    EXPRESSION_INTLITERAL,
    EXPRESSION_VARIABLEEXPRESSION
} expression_kind;
/**
 * Used to differentiate between variable kinds.
 */
typedef enum {
    VARIABLE_NAMEDVARIABLE,
    VARIABLE_ARRAYACCESS
} variable_kind;
/**
 * Used to differentiate between statement kinds.
 */
typedef enum {
    STATEMENT_EMPTYSTATEMENT,
    STATEMENT_COMPOUNDSTATEMENT,
    STATEMENT_ASSIGNSTATEMENT,
    STATEMENT_IFSTATEMENT,
    STATEMENT_WHILESTATEMENT,
    STATEMENT_CALLSTATEMENT
} statement_kind;
/**
 * Used to differentiate between global declaration kinds.
 */
typedef enum {
    DECLARATION_TYPEDECLARATION,
    DECLARATION_PROCEDUREDECLARATION
} global_declaration_kind;

struct expression;
struct variable;
struct statement;
struct type_expression;

typedef struct global_declaration_list GlobalDeclarationList;
typedef struct parameter_list ParameterList;
typedef struct variable_declaration_list VariableDeclarationList;
typedef struct statement_list StatementList;
typedef struct expression_list ExpressionList;

/**
 * This struct represents an expression in SPL.
 *
 * Expressions are used as a value assigned to a variable or in the condition of conditional statements in SPL.
 * Every expression has a semantic type that has to be set in phase 4.
 * There are three kinds of expressions:
 *
 * 1. Binary expressions combining a left and a right expression with an operator.
 * The type of binary expressions is dependent on the kind of the operator.
 *
 * 2. Integer literals representing a literal value from the source code.
 * Every time a number is used in an SPL program (decimal, hexadecimal or as a character), this is represented as an
 * integer literal.
 *
 * 3. Variable expressions representing the value of a variable.
 * The semantic type of a variable expression depends on the type of its variable.
 */
typedef struct expression {
    int line;
    expression_kind kind;
    Type *dataType;
    union {
        struct {
            binary_operator operator;
            struct expression *leftOperand, *rightOperand;
        } binaryExpression;
        struct {
            int value;
        } intLiteral;
        struct {
            struct variable *variable;
        } variableExpression;
    } u;
} Expression;

/**
 * This struct represents variables in SPL.
 *
 * Variables are comparable to pointers to a certain value.
 * Every variable has a semantic type, that has to be set in phase 4.
 * There are two kinds of variables in SPL:
 *
 * 1. Named variables which represent a simple variable, that is identified by its name.
 *
 * 2. Array accesses that represent an array accessed at a certain index. The accessed array itself may be any variable.
 */
typedef struct variable {
    int line;
    variable_kind kind;
    Type *dataType;
    union {
        struct {
            Identifier *name;
        } namedVariable;
        struct {
            struct variable *array;
            struct expression *index;
        } arrayAccess;
    } u;
} Variable;

/**
 * This structs represent a statement in SPL.
 *
 * There exist many kinds of statements which, may all occur in a procedures body in SPL:
 *
 * 1. Empty statements are statements with no effect.
 *
 * 2. Compound statements are statements that combine different statements into a single one.
 * These are used whenever it is semantically necessary for many statements to act as one, for example the
 * body of a while-statement may only hold a single statement, which can be a compound statement
 * containing many statements itself.
 *
 * 3. Assign statements evaluates its right hand side expression and stores the value in the
 * variable on the left hand side of the assignment operator.
 *
 * 4. If-statements consisting of two branches and a conditional expression. Which of the two branches
 * is executed depends on the value of the condition.
 *
 * 5. While-statements consisting of a conditional expression and a body. The statement in the body is executed
 * until the condition evaluates to false.
 *
 * 6. Call statements represent a call of another procedure in SPL.
 * Whenever a procedure is called, additionally to the called procedures name, a list of expressions
 * has to be provided, whose types match the parameters of the called procedure.
 * Those expressions are then evaluated and passed as arguments to the called procedure, which is executed.
 * The execution of the current procedure is halted until the called procedure returns.
 */
typedef struct statement {
    int line;
    statement_kind kind;
    union {
        struct {
            int dummy;        /* empty struct not allowed in C */
        } emptyStatement;
        struct {
            struct statement_list *statements;
        } compoundStatement;
        struct {
            Variable *target;
            Expression *value;
        } assignStatement;
        struct {
            Expression *condition;
            struct statement *thenPart;
            struct statement *elsePart;
        } ifStatement;
        struct {
            Expression *condition;
            struct statement *body;
        } whileStatement;
        struct {
            Identifier *procedureName;
            struct expression_list *argumentList;
        } callStatement;
    } u;
} Statement;

/**
 * This struct represents type expressions in SPL.
 *
 * A type expression behaves like a formula representing a concrete semantic type, which has to be
 * calculated during phase 4.
 * There are two kinds of type expressions in SPL:
 *
 * 1. Named type expressions are type expressions, where the type is determined solely by its identifier.
 *
 * 2. Array type expressions which represent the type of an fixed-size array of another type expression.
 * They consist of the base type, which is the type of an arrays elements and a size, determining how many
 * of those elements an array of this type can hold.
 */
typedef struct type_expression {
    int line;
    type_expression_kind kind;
    Type *dataType;
    union {
        struct {
            Identifier *name;
        } namedTypeExpression;
        struct {
            int arraySize;
            struct type_expression *baseType;
        } arrayTypeExpression;
    } u;
} TypeExpression;

/**
 * This struct represents the local declaration of a parameter in SPL.
 *
 * Parameter are declared in the parameter list of a procedure. They combine an identifier with
 * a type expression, expressing the parameters type and additionally have to store
 * whether the parameter is passed as a reference.
 * Parameters are only visible in the scope of their procedure.
 */
typedef struct {
    int line;
    Identifier *name;
    TypeExpression *typeExpression;
    bool isReference;
} ParameterDeclaration;

/**
 * This struct represents the local declaration of a variable in SPL.
 *
 * Variables are declared inside a procedure and combine an identifier with a type expression,
 * expressing the variables type.
 * Variables are only visible in the local scope of their procedure.
 */
typedef struct {
    int line;
    Identifier *name;
    TypeExpression *typeExpression;
} VariableDeclaration;

/**
 * This struct represents a global declaration in SPL.
 *
 * Global declarations are all declarations done in the global scope.
 * There are two kinds of global declarations:
 *
 * 1. Type declarations representing the declaration of a new type. When declaring a new type it is necessary
 * to provide an identifier for the newly declared type and a type expression describing the type.
 *
 * 2. Procedure declarations representing the declaration of a new procedure. When declaring a procedure
 * an identifier has to be provided, that is used as the identifier of the declaration.
 * Additionally a procedure declaration requires a list of parameters, a list of local variables and
 * a list of statements in the body of the procedure.
 */
typedef struct {
    int line;
    global_declaration_kind kind;
    Identifier *name;
    union {
        struct {
            TypeExpression *typeExpression;
        } typeDeclaration;
        struct {
            ParameterList *parameters;
            VariableDeclarationList *variables;
            StatementList *body;
        } procedureDeclaration;
    } u;
} GlobalDeclaration;

typedef struct global_declaration_list {
    bool isEmpty;
    GlobalDeclaration *head;
    struct global_declaration_list *tail;
} GlobalDeclarationList;

typedef struct parameter_list {
    bool isEmpty;
    ParameterDeclaration *head;
    struct parameter_list *tail;
} ParameterList;

typedef struct variable_declaration_list {
    bool isEmpty;
    VariableDeclaration *head;
    struct variable_declaration_list *tail;
} VariableDeclarationList;

typedef struct statement_list {
    bool isEmpty;
    Statement *head;
    struct statement_list *tail;
} StatementList;

typedef struct expression_list {
    bool isEmpty;
    Expression *head;
    struct expression_list *tail;
} ExpressionList;

/**
 * The program type represents the root of the AST.
 * It consists of a list containing all global declarations of a SPL program.
 */
typedef GlobalDeclarationList Program;

/**
 * Creates a new node representing a named type expression.
 * @param line The position of the type expression in the source code.
 * @param name The identifier used to express this type.
 * @return The reference to a newly created node.
 */
TypeExpression *newNamedTypeExpression(int line, Identifier *name);
/**
 * Creates a new node representing the type expression for the type of a fixed-size array.
 * @param line The position of the type expression in the source code.
 * @param ty The type expression of the elements type.
 * @param size The amount of elements an array of this type can hold.
 * @return The reference to a newly created node.
 */
TypeExpression *newArrayTypeExpression(int line, TypeExpression *ty, int size);

/**
 * Creates a new node representing a type declaration.
 * @param line The position of the declaration in the source code.
 * @param name The declarations identifier.
 * @param ty The type expression associated with this declaration.
 * @return The reference to a newly created node.
 */
GlobalDeclaration *newTypeDeclaration(int line, Identifier *name, TypeExpression *ty);
/**
 * Creates a new node representing a procedure declaration.
 * @param line The position of the declaration in the source code.
 * @param name The procedures identifier.
 * @param params The procedures parameter list.
 * @param decls The procedures local variables.
 * @param body The statements in the body of the procedure.
 * @return The reference to a newly created node.
 */
GlobalDeclaration *newProcedureDeclaration(int line, Identifier *name, ParameterList *params, VariableDeclarationList *decls, StatementList *body);

/**
 * Creates a new node representing the declaration of a parameter in the head of a procedure.
 * @param line The position of the declaration in the source code.
 * @param name The identifier of the declared parameter.
 * @param ty The type expression used to express the parameters type.
 * @param isRef A boolean value used to represent whether the parameter is passed as a reference.
 * @return The reference to a newly created node.
 */
ParameterDeclaration *newParameterDeclaration(int line, Identifier *name, TypeExpression *ty, bool isRef);
/**
 * Creates a new node representing the declaration of a local variable in a procedures body.
 * @param line The position of the declaration in the source code.
 * @param name The identifier of the declared local variable.
 * @param ty The type expression used to express the type of the local variable.
 * @return The reference to a newly created node.
 */
VariableDeclaration *newVariableDeclaration(int line, Identifier *name, TypeExpression *ty);

/**
 * Creates a new node representing an empty statement.
 * @param line The position of the statement in the source code.
 * @return The reference to a newly created node.
 */
Statement *newEmptyStatement(int line);
/**
 * Creates a new node representing a compound statement.
 * @param line The position of the statement in the source code.
 * @param stms The list of statements that this statement combines.
 * @return The reference to a newly created node.
 */
Statement *newCompoundStatement(int line, StatementList *stms);
/**
 * Creates a new node representing an assignment.
 * @param line The position of the statement in the source code.
 * @param var The variable where the value is assigned to.
 * @param exp The value to be assigned.
 * @return The reference to a newly created node.
 */
Statement *newAssignStatement(int line, Variable *var, Expression *exp);
/**
 * Creates a new node representing an if-statement.
 * @param line The position of the statement in the source code.
 * @param condition The expression deciding which branch to execute.
 * @param thenPart The executed statement if the expression evaluates to true.
 * @param elsePart The executed statement if the expression evaluates to false.
 * @return The reference to a newly created node.
 */
Statement *newIfStatement(int line, Expression *condition, Statement *thenPart, Statement *elsePart);
/**
 * Creates a new node representing a while-statement.
 * @param line The position of the statement in the source code.
 * @param condition The expression used to determine whether the while-loop should continue.
 * @param body The statement executed until the condition evaluates to false.
 * @return The reference to a newly created node.
 */
Statement *newWhileStatement(int line, Expression *condition, Statement *body);
/**
 * Creates a new node representing a procedure call.
 * @param line The position of the call in the source code.
 * @param name The identifier of the called procedure.
 * @param args The list of expressions, whose values will be passed to the procedure.
 * @return The reference to a newly created node.
 */
Statement *newCallStatement(int line, Identifier *name, ExpressionList *args);

/**
 * Creates a new node representing an expression combining two expressions with an operator.
 * @param line The position of the expression in the source code.
 * @param op The operator used in this expression.
 * @param left The operand on the left hand side of the operator.
 * @param right The operand on the right hand side of the operator.
 * @return The reference to a newly created node.
 */
Expression *newBinaryExpression(int line, binary_operator op, Expression *left, Expression *right);
/**
 * Creates a new node representing the value of a variable.
 * @param line The position of the variable in the source code.
 * @param var The variable whose value is used as a value for this expression.
 * @return The reference to a newly created node.
 */
Expression *newVariableExpression(int line, Variable *var);
/**
 * Creates a new node representing an integer literal.
 * @param line The position of the literal in the source code.
 * @param val The value the literal holds.
 * @return The reference to a newly created node.
 */
Expression *newIntLiteral(int line, int val);

/**
 * Creates a new node representing a named variable.
 * @param line The position of the variable in the source code.
 * @param name The identifier of the variable.
 * @return The reference to a newly created node.
 */
Variable *newNamedVariable(int line, Identifier *name);
/**
 * Creates a new node representing an array access.
 * @param line The position of the array access in the source code.
 * @param var The variable representing the accessed array.
 * @param index The expression representing the index of the access.
 * @return The reference to a newly created node.
 */
Variable *newArrayAccess(int line, Variable *var, Expression *index);

/**
 * Returns an empty list for global declarations.
 */
GlobalDeclarationList *emptyGlobalDeclarationList(void);
/**
 * Prepends a global declaration to a list.
 * @param head The element to add.
 * @param tail The list where the element is added.
 * @return A list with the element added.
 */
GlobalDeclarationList *newGlobalDeclarationList(GlobalDeclaration *head, GlobalDeclarationList *tail);

/**
 * Returns an empty list for variables.
 */
VariableDeclarationList *emptyVariableList(void);
/**
 * Prepends a variable declaration to a list.
 * @param head The element to add.
 * @param tail The list where the element is added.
 * @return A list with the element added.
 */
VariableDeclarationList *newVariableList(VariableDeclaration *head, VariableDeclarationList *tail);

/**
 * Returns an empty list for parameters.
 */
ParameterList *emptyParameterList(void);
/**
 * Prepends a parameter declaration to a list.
 * @param head The element to add.
 * @param tail The list where the element is added.
 * @return A list with the element added.
 */
ParameterList *newParameterList(ParameterDeclaration *head, ParameterList *tail);

/**
 * Returns an empty list for statements.
 */
StatementList *emptyStatementList(void);
/**
 * Prepends a statement to a list.
 * @param head The element to add.
 * @param tail The list where the element is added.
 * @return A list with the element added.
 */
StatementList *newStatementList(Statement *head, StatementList *tail);

/**
 * Returns an empty list for expressions.
 */
ExpressionList *emptyExpressionList(void);
/**
 * Prepends an expression to a list.
 * @param head The element to add.
 * @param tail The list where the element is added.
 * @return A list with the element added.
 */
ExpressionList *newExpressionList(Expression *head, ExpressionList *tail);

void showAbsyn(Program *program);

#endif /* _ABSYN_H_ */
