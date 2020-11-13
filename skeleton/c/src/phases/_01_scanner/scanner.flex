%{

/*
 * scanner.flex -- SPL scanner specification
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <util/errors.h>
#include <util/memory.h>
#include <table/identifier.h>
#include <types/types.h>
#include <absyn/absyn.h>
#include <phases/_01_scanner/scanner.h>
#include <phases/_02_03_parser/parser.h>

static int lineNumber = 1;

%}

 /* Disables the ability to read multiple files (which is not required in SPL). */
%option noyywrap
 /* Sets the caching behavior of the generated scanner. */
%option never-interactive
 /* This header may not always exist on Windows systems. */
%option nounistd
 /* Disables unused functions, so no warnings are displayed during compilation. */
%option nounput
%option noinput

%%

    // TODO (assignment 1): The regular expressions for all tokens need to be defined here.

.|\n		{
              illegalCharacter(lineNumber, yytext[0]);
			}


%%
