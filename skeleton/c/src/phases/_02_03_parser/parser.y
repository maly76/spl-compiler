%{

/*
 * parser.y -- SPL parser specification
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/errors.h>
#include <table/identifier.h>
#include <types/types.h>
#include <absyn/absyn.h>
#include <phases/_01_scanner/scanner.h>
#include <phases/_02_03_parser/parser.h>

void yyerror(Program**, char *);

%}

%expect 0 //TODO Change?
%parse-param {Program** program}

%union {
  NoVal noVal;
  IntVal intVal;
  StringVal stringVal;

  Expression *expression;
  Variable *variable;
  Statement *statement;
  TypeExpression *typeExpression;
  GlobalDeclaration *globalDeclaration;
  VariableDeclaration *variableDeclaration;
  ParameterDeclaration *parameterDeclaration;

  StatementList *statementList;
  ExpressionList *expressionList;
  VariableDeclarationList *variableList;
  ParameterList *parameterList;
  GlobalDeclarationList *globalDeclarationList;
}


%token	<noVal>		ARRAY ELSE IF OF PROC
%token	<noVal>		REF TYPE VAR WHILE
%token	<noVal>		LPAREN RPAREN LBRACK
%token	<noVal>		RBRACK LCURL RCURL
%token	<noVal>		EQ NE LT LE GT GE
%token	<noVal>		ASGN COLON COMMA SEMIC
%token	<noVal>		PLUS MINUS STAR SLASH
%token	<stringVal>	IDENT
%token	<intVal>	INTLIT

%start			program


%%

program			: INTLIT; //TODO (assignment 2 and 3): Just a dummy, needs to be replaced by the actual spl grammar.

%%

void yyerror(Program** program, char *msg) {
    // The first parameter is needed because of '%parse-param {Program** program}'.
    // However, it is unused and gcc emits a warning for it. The following line "uses" the parameter,
    // but does nothing. It is used to silence the warning.
    (void)program;
    syntaxError(yylval.noVal.line, msg);
}
