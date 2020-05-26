/* The lexer for the CS-5300 CPSL compiler. */

%{
#include "lexer.hh"

#include "scanner.hh"
%}

DIGIT       [0-9]
HEXDIGIT    [0-9]|[a-f]|[A-F]
OCTALDIGIT  [0-7]
LOWER       [a-z]
UPPER       [A-Z]
LETTER      {UPPER}|{LOWER}
INNERCHAR   [^\[^:print:]]|"\\"[[:print:]]

OCTAL "0"{OCTALDIGIT}*
HEX "0x"{HEXDIGIT}*
/*
 * Just fail on non-octal numbers beginning with "0", as opposed to treating it
 * as a decimal value, at the lexer/scanner phase.
 */
DECIMAL [1-9]{DIGIT}*

IDENTIFIER {LETTER}({LETTER}|{DIGIT}|"_")*
KEYWORD array|begin|chr|const|do|downto|else|elseif|end|for|forward|function|if|of|ord|pred|procedure|read|record|ref|repeat|return|stop|succ|then|to|type|until|var|while|write
OPERATOR "+"|"-"|"*"|"/"|"&"|"|"|"~"|"="|"<>"|"<"|"<="|">"|">="|"."|","|":"|";"|"("|")"|"["|"]"|":="|"%"
INTEGER {OCTAL}|{HEX}|{DECIMAL}
CHAR "'"{INNERCHAR}"'"
STRING "\""{INNERCHAR}*"\""
WHITESPACE "\n"|"\t"|" "

%%

/* Handlers. */

IDENTIFIER  {
	/* ... */
}

.  printf( "Unrecognized character: %s\n", yytext );

%%

void scan(FILE *fh) {
	yyin = fh;
	yylex();
}
