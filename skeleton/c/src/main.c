/*
 * main.c -- SPL compiler
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <util/errors.h>
#include <absyn/absyn.h>
#include "phases/_01_scanner/scanner.h"
#include <phases/_04a_tablebuild/tablebuild.h>
#include <phases/_02_03_parser/parser.h>
#include "phases/_04b_semant/procedurebodycheck.h"
#include "phases/_05_varalloc/varalloc.h"
#include "phases/_06_codegen/codegen.h"

#define VERSION        "1.1"

void showToken(int token) {
    printf("TOKEN = ");
    switch (token) {
        case 0:
            printf("EOF in line %d", yylval.noVal.line);
            break;
        case ARRAY:
            printf("ARRAY in line %d", yylval.noVal.line);
            break;
        case ELSE:
            printf("ELSE in line %d", yylval.noVal.line);
            break;
        case IF:
            printf("IF in line %d", yylval.noVal.line);
            break;
        case OF:
            printf("OF in line %d", yylval.noVal.line);
            break;
        case PROC:
            printf("PROC in line %d", yylval.noVal.line);
            break;
        case REF:
            printf("REF in line %d", yylval.noVal.line);
            break;
        case TYPE:
            printf("TYPE in line %d", yylval.noVal.line);
            break;
        case VAR:
            printf("VAR in line %d", yylval.noVal.line);
            break;
        case WHILE:
            printf("WHILE in line %d", yylval.noVal.line);
            break;
        case LPAREN:
            printf("LPAREN in line %d", yylval.noVal.line);
            break;
        case RPAREN:
            printf("RPAREN in line %d", yylval.noVal.line);
            break;
        case LBRACK:
            printf("LBRACK in line %d", yylval.noVal.line);
            break;
        case RBRACK:
            printf("RBRACK in line %d", yylval.noVal.line);
            break;
        case LCURL:
            printf("LCURL in line %d", yylval.noVal.line);
            break;
        case RCURL:
            printf("RCURL in line %d", yylval.noVal.line);
            break;
        case EQ:
            printf("EQ in line %d", yylval.noVal.line);
            break;
        case NE:
            printf("NE in line %d", yylval.noVal.line);
            break;
        case LT:
            printf("LT in line %d", yylval.noVal.line);
            break;
        case LE:
            printf("LE in line %d", yylval.noVal.line);
            break;
        case GT:
            printf("GT in line %d", yylval.noVal.line);
            break;
        case GE:
            printf("GE in line %d", yylval.noVal.line);
            break;
        case ASGN:
            printf("ASGN in line %d", yylval.noVal.line);
            break;
        case COLON:
            printf("COLON in line %d", yylval.noVal.line);
            break;
        case COMMA:
            printf("COMMA in line %d", yylval.noVal.line);
            break;
        case SEMIC:
            printf("SEMIC in line %d", yylval.noVal.line);
            break;
        case PLUS:
            printf("PLUS in line %d", yylval.noVal.line);
            break;
        case MINUS:
            printf("MINUS in line %d", yylval.noVal.line);
            break;
        case STAR:
            printf("STAR in line %d", yylval.noVal.line);
            break;
        case SLASH:
            printf("SLASH in line %d", yylval.noVal.line);
            break;
        case IDENT:
            printf("IDENT in line %d, value = \"%s\"",
                   yylval.stringVal.line, yylval.stringVal.val);
            break;
        case INTLIT:
            printf("INTLIT in line %d, value = %d",
                   yylval.intVal.line, yylval.intVal.val);
            break;
        default:
            /* this should never happen */
            error("unknown token %d in showToken", token);
    }
    printf("\n");
}

static void version(const char *myself) {
    printf("%s version %s (compiled %s)\n", myself, VERSION, __DATE__);
}

static void showUsage(FILE *out, const char *myself) {
    /* show some help how to use the program */
    fprintf(out, "Usage: %s [options] <input file> <output file>\n", myself);
    fprintf(out, "\n");
    fprintf(out, "Executes all compiler phases up to (and including) the specified one.\n");
    fprintf(out, "If no flag is specified, all phases are run and code is written to the output file.\n");
    fprintf(out, "Options:\n");
    fprintf(out, "  --tokens     Phase 1: Scans for tokens and prints them.\n");
    fprintf(out, "  --parse      Phase 2: Parses the stream of tokens to check for syntax errors.\n");
    fprintf(out, "  --absyn      Phase 3: Creates an abstract syntax tree from the input tokens and prints it.\n");
    fprintf(out, "  --tables     Phase 4a: Builds a symbol table and prints its entries.\n");
    fprintf(out, "  --semant     Phase 4b: Performs the semantic analysis.\n");
    fprintf(out, "  --vars       Phase 5: Allocates memory space for variables and prints the amount of allocated memory.\n");
    fprintf(out, "  --version    Show compiler version.\n");
    fprintf(out, "  --help       Show this help.\n");
}

static void usageError(const char *myself, const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    fprintf(stderr, "Usage: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);

    showUsage(stderr, myself);
    exit(1);
}


int main(int argc, char *argv[]) {
    int i;
    char *inFileName;
    char *outFileName;
    bool optionTokens;
    bool optionParse;
    bool optionAbsyn;
    bool optionTables;
    bool optionSemant;
    bool optionVars;
    int token;

    /* analyze command line */
    inFileName = NULL;
    outFileName = NULL;
    optionTokens = false;
    optionParse = false;
    optionAbsyn = false;
    optionTables = false;
    optionSemant = false;
    optionVars = false;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--tokens") == 0) {
            optionTokens = true;
        } else if (strcmp(argv[i], "--parse") == 0) {
            optionParse = true;
        } else if (strcmp(argv[i], "--absyn") == 0) {
            optionAbsyn = true;
        } else if (strcmp(argv[i], "--tables") == 0) {
            optionTables = true;
        } else if (strcmp(argv[i], "--semant") == 0) {
            optionSemant = true;
        } else if (strcmp(argv[i], "--vars") == 0) {
            optionVars = true;
        } else if (strcmp(argv[i], "--version") == 0) {
            version(argv[0]);
            exit(0);
        } else if (strcmp(argv[i], "--help") == 0) {
            showUsage(stdout, argv[0]);
            exit(0);
        } else {
            if (!(argv[i][0] == '-' && argv[i][1] == '-')) {
                if (inFileName == NULL) inFileName = argv[i];
                else if (outFileName == NULL) outFileName = argv[i];
                else usageError(argv[0], "Only one output file is allowed!");
            } else {
                usageError(argv[0], "Unknown option '%s'!", argv[i]);
            }
        }
    }

    if (inFileName == NULL)
        usageError(argv[0], "No input file");
    // Only display usage if compiler is expected to run the code-generation phase
    if (outFileName == NULL &&
        !(optionTokens || optionParse || optionAbsyn || optionTables || optionVars || optionSemant))
        usageError(argv[0], "No output file");

    yyin = fopen(inFileName, "r");
    if (yyin == NULL) {
        error("cannot open input file '%s'", inFileName);
    }

    if (optionTokens) {
        do {
            token = yylex();
            showToken(token);
        } while (token != 0);
        fclose(yyin);
        exit(0);
    }

    Program *program;
    if (yyparse(&program) != 0) error("Failed to parse input!");
    fclose(yyin);

    if (optionParse) {
        printf("Input parsed successfully!\n");
        exit(0);
    }

    if (optionAbsyn) {
        showAbsyn(program);
        exit(0);
    }

    SymbolTable *globalTable = buildSymbolTable(program, optionTables);
    if (optionTables) exit(0);

    check(program, globalTable);
    if (optionSemant) {
        printf("No semantic errors found!\n");
        exit(0);
    }

    allocVars(program, globalTable, optionVars);
    if (optionVars) exit(0);

    FILE *outFile = fopen(outFileName, "w");
    if (outFile == NULL) {
        error("Unable to open output file '%s'", outFileName);
    }
    genCode(program, globalTable, outFile);
    fclose(outFile);
    return 0;
}
