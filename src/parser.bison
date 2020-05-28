%define api.pure full
/*
 * Use our own yylex variant to control feeding in of lexemes.
 * Flex references:
 * c.f.:
 * - https://stackoverflow.com/a/32473263
 * - https://westes.github.io/flex/manual/Code_002dLevel-And-API-Options.html#index-prefix
 * Bison references:
 * c.f.:
 * https://www.gnu.org/software/bison/manual/html_node/Multiple-Parsers.html
/* %option prefix="yy_cpsl_cc_parser" */
%define api.prefix {yy_cpsl_cc_parser}
/* Fallback: */
%{
#define yylex yy_cpsl_cc_parserlex
#define yyparse yy_cpsl_cc_parserparse
#define yyerror yy_cpsl_cc_parsererror
%}
/*
 * For the semantic value, use an index into the vector of lexemes.
 * c.f. https://www.gnu.org/software/bison/manual/html_node/Value-Type.html
 */
%define api.value.type {parser_yystype_t}
/*
 * Define YYLTYPE:
 * c.f. https://www.gnu.org/software/bison/manual/html_node/Location-Type.html
 */
%{
#define YYLTYPE parser_yyltype_t
%}
/*
 * Specify additional yy_cpsl_cc_parserlex and yy_cpsl_cc_parserparse parameters.
 */
%param {std::shared_ptr<ParserState> parser_state}
/* Produce verbose errors: c.f. https://www.gnu.org/software/bison/manual/html_node/Error-Reporting-Function.html */
%define parse.error detailed

/* The start symbol is "start".  c.f. https://www.gnu.org/software/bison/manual/html_node/Start-Decl.html#Start-Decl */
%start start

/* List the tokens. */

%token IDENTIFIER 2
%token INTEGER    4
%token CHAR       5
%token STRING     6
%token COMMENT    7
%token WHITESPACE 8

%token ARRAY_KEYWORD     10
%token BEGIN_KEYWORD     11
%token CHR_KEYWORD       12
%token CONST_KEYWORD     13
%token DO_KEYWORD        14
%token DOWNTO_KEYWORD    15
%token ELSE_KEYWORD      16
%token ELSEIF_KEYWORD    17
%token END_KEYWORD       18
%token FOR_KEYWORD       19
%token FORWARD_KEYWORD   20
%token FUNCTION_KEYWORD  21
%token IF_KEYWORD        22
%token OF_KEYWORD        23
%token ORD_KEYWORD       24
%token PRED_KEYWORD      25
%token PROCEDURE_KEYWORD 26
%token READ_KEYWORD      27
%token RECORD_KEYWORD    28
%token REF_KEYWORD       29
%token REPEAT_KEYWORD    30
%token RETURN_KEYWORD    31
%token STOP_KEYWORD      32
%token SUCC_KEYWORD      33
%token THEN_KEYWORD      34
%token TO_KEYWORD        35
%token TYPE_KEYWORD      36
%token UNTIL_KEYWORD     37
%token VAR_KEYWORD       38
%token WHILE_KEYWORD     39
%token WRITE_KEYWORD     40

%token PLUS_OPERATOR             41
%token MINUS_OPERATOR            42
%token TIMES_OPERATOR            43
%token SLASH_OPERATOR            44
%token AMPERSAND_OPERATOR        45
%token PIPE_OPERATOR             46
%token TILDE_OPERATOR            47
%token EQUALS_OPERATOR           48
%token LT_OR_GT_OPERATOR         49
%token LT_OPERATOR               50
%token LE_OPERATOR               51
%token GT_OPERATOR               52
%token GE_OPERATOR               53
%token DOT_OPERATOR              54
%token COMMA_OPERATOR            55
%token COLON_OPERATOR            56
%token SEMICOLON_OPERATOR        57
%token LEFTPARENTHESIS_OPERATOR  58
%token RIGHTPARENTHESIS_OPERATOR 59
%token LEFTBRACKET_OPERATOR      60
%token RIGHTBRACKET_OPERATOR     61
%token COLONEQUALS_OPERATOR      62
%token PERCENT_OPERATOR          63

%{
#include <memory>      // std::shared_ptr
#include <vector>      // std::vector

#include "lexer.hh"    // Lexeme
#include "grammar.hh"  // Grammar

#include "parser.hh"
%}

%%

keyword:
	  ARRAY_KEYWORD
	| BEGIN_KEYWORD
	| CHR_KEYWORD
	| CONST_KEYWORD
	| DO_KEYWORD
	| DOWNTO_KEYWORD
	| ELSE_KEYWORD
	| ELSEIF_KEYWORD
	| END_KEYWORD
	| FOR_KEYWORD
	| FORWARD_KEYWORD
	| FUNCTION_KEYWORD
	| IF_KEYWORD
	| OF_KEYWORD
	| ORD_KEYWORD
	| PRED_KEYWORD
	| PROCEDURE_KEYWORD
	| READ_KEYWORD
	| RECORD_KEYWORD
	| REF_KEYWORD
	| REPEAT_KEYWORD
	| RETURN_KEYWORD
	| STOP_KEYWORD
	| SUCC_KEYWORD
	| THEN_KEYWORD
	| TO_KEYWORD
	| TYPE_KEYWORD
	| UNTIL_KEYWORD
	| VAR_KEYWORD
	| WHILE_KEYWORD
	| WRITE_KEYWORD
;

operator:
	| PLUS_OPERATOR
	| MINUS_OPERATOR
	| TIMES_OPERATOR
	| SLASH_OPERATOR
	| AMPERSAND_OPERATOR
	| PIPE_OPERATOR
	| TILDE_OPERATOR
	| EQUALS_OPERATOR
	| LT_OR_GT_OPERATOR
	| LT_OPERATOR
	| LE_OPERATOR
	| GT_OPERATOR
	| GE_OPERATOR
	| DOT_OPERATOR
	| COMMA_OPERATOR
	| COLON_OPERATOR
	| SEMICOLON_OPERATOR
	| LEFTPARENTHESIS_OPERATOR
	| RIGHTPARENTHESIS_OPERATOR
	| LEFTBRACKET_OPERATOR
	| RIGHTBRACKET_OPERATOR
	| COLONEQUALS_OPERATOR
	| PERCENT_OPERATOR
;

start:
	IDENTIFIER | keyword {
		/* TODO */
	}
;

	/* TODO: rules */

%%

ParserState::ParserState(const std::vector<Lexeme> &lexemes)
	: lexemes(lexemes)
	, next_lexeme(0)
	{}

/*
 * Feed in a lexeme: *lvalp = semantic value; return token type;
 */
int yy_cpsl_cc_parserlex(parser_yystype_t *lvalp, std::shared_ptr<ParserState> parser_state) {
	if (parser_state->next_lexeme >= parser_state->lexemes.size()) {
		// Done traversing lexemes.
		return 0;
	} else {
		const Lexeme &lexeme = parser_state->lexemes[parser_state->next_lexeme];
		*lvalp = parser_state->next_lexeme;
		++parser_state->next_lexeme;
		return lexeme.get_enumerated_token_kind();
	}
}

void yy_cpsl_cc_parsererror(std::shared_ptr<ParserState> parser_state, const char *s) {
	// TODO
}

Grammar parse_lexemes(const std::vector<Lexeme> &lexemes) {
	std::shared_ptr<ParserState> parser_state(new ParserState(lexemes));
	int status = yy_cpsl_cc_parserparse(parser_state);
	// status: c.f. https://www.gnu.org/software/bison/manual/html_node/Parser-Function.html
	switch (status) {
		case 0:
			// Success.
			break;

		case 1:
			// Invalid input.
			break;

		case 2:
			// Out of memory.
			break;

		default:
			// Unknown status.
			break;
	}

	// TODO
	return Grammar();
}
