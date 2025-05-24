%option noyywrap

%{
#include <string>
#include "bison.tab.hh"
#include "header/compiler_error.h"

extern int yylineno;
%}

%%

[ \t]   ; /* ignore whitespace */
\n      yylineno++; /* count newlines */
"//".*  ; /* ignore comments */

"int"   { return INT_TYPE; }
"float" { return FLOAT_TYPE; }
"print" { return PRINT; }
"read"  { return READ; }
"if"    { return IF; }

"+"     { return PLUS; }
"-"     { return MINUS; }
"*"     { return MUL; }
"/"     { return DIV; }

"("     { return LPAREN; }
")"     { return RPAREN; }
"!"     { return EXCL; }
"="     { return ASSIGN; }
"=="    { return EQ_OP; }
"~="    { return NE_OP; }
"<"     { return LT_OP; }
"<="    { return LE_OP; }
">"     { return GT_OP; }
">="    { return GE_OP; }

-?[0-9]+          { yylval.ival = atoi(yytext); return INT_LITERAL; }
-?[0-9]+\.[0-9]+  { yylval.fval = atof(yytext); return FLOAT_LITERAL; }
[a-zA-Z][a-zA-Z0-9_]* { yylval.name = strdup(yytext); return ID; }

.       { throwCompilerError(CompilerError::InvalidChar, "Invalid character"); }

%%

