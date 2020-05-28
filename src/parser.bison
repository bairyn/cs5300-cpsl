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
#include <sstream>     // std::ostringstream
#include <vector>      // std::vector

#include "lexer.hh"    // comment_tag, Lexeme, whitespace_tag
#include "grammar.hh"  // Grammar, GrammarError

#include "parser.hh"
%}

/*
 * Associativity and precedence.
 * c.f.:
 * - https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 * - https://www.gnu.org/software/bison/manual/html_node/Infix-Calc.html (c.f. "%prec")
 */

%right UNARY_MINUS
%left TIMES_OPERATOR SLASH_OPERATOR PERCENT_OPERATOR
%left PLUS_OPERATOR MINUS_OPERATOR
%nonassoc EQUALS_OPERATOR LT_OR_GT_OPERATOR LT_OPERATOR LE_OPERATOR GT_OPERATOR GE_OPERATOR
%left TILDE_OPERATOR
%left AMPERSAND_OPERATOR
%left PIPE_OPERATOR

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
	  PLUS_OPERATOR
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

	/* TODO: actions */

start:
	program
;

	/* Overall structure. */

program:
	constant_decl_opt type_decl_opt var_decl_opt procedure_decl_or_function_decl_list block DOT_OPERATOR
;

constant_decl_opt:
	  %empty
	| constant_decl
;

type_decl_opt:
	  %empty
	| type_decl
;

var_decl_opt:
	  %empty
	| var_decl
;

procedure_decl_or_function_decl_list:
	  %empty
	| procedure_decl_or_function_decl_list procedure_decl_or_function_decl
;

procedure_decl_or_function_decl:
	  procedure_decl
	| function_decl
;

	/* Declarations. */

constant_decl:
	CONST_KEYWORD constant_assignment constant_assignment_list
;

constant_assignment_list:
	  %empty
	| constant_assignment_list constant_assignment
;

constant_assignment:
	IDENTIFIER EQUALS_OPERATOR expression SEMICOLON_OPERATOR
;

procedure_decl:
	  PROCEDURE_KEYWORD IDENTIFIER LEFTPARENTHESIS_OPERATOR formal_parameters RIGHTPARENTHESIS_OPERATOR SEMICOLON_OPERATOR FORWARD_KEYWORD SEMICOLON_OPERATOR
	| PROCEDURE_KEYWORD IDENTIFIER LEFTPARENTHESIS_OPERATOR formal_parameters RIGHTPARENTHESIS_OPERATOR SEMICOLON_OPERATOR body SEMICOLON_OPERATOR
;

function_decl:
	  FUNCTION_KEYWORD IDENTIFIER LEFTPARENTHESIS_OPERATOR formal_parameters RIGHTPARENTHESIS_OPERATOR COLON_OPERATOR type SEMICOLON_OPERATOR FORWARD_KEYWORD SEMICOLON_OPERATOR
	| FUNCTION_KEYWORD IDENTIFIER LEFTPARENTHESIS_OPERATOR formal_parameters RIGHTPARENTHESIS_OPERATOR COLON_OPERATOR type SEMICOLON_OPERATOR body SEMICOLON_OPERATOR
;

	/*
	* The structure is equivalent to the documented one but the details differ a
	* bit.
	*/
formal_parameters:
	  %empty
	| formal_parameter formal_parameter_prefixed_list
;

formal_parameter:
	var_or_ref ident_list COLON_OPERATOR type
;

formal_parameter_prefixed_list:
	  %empty
	| formal_parameter_prefixed_list SEMICOLON_OPERATOR formal_parameter
;

var_or_ref:
	  VAR_KEYWORD
	| REF_KEYWORD
;

body:
	constant_decl_opt type_decl_opt block
;

block:
	BEGIN_KEYWORD statement_sequence END_KEYWORD
;

type_decl:
	TYPE_KEYWORD type_assignment type_assignment_list;
;

type_assignment_list:
	  %empty
	| type_assignment_list type_assignment
;

type_assignment:
	IDENTIFIER EQUALS_OPERATOR type SEMICOLON_OPERATOR
;

type:
	  simple_type
	| record_type
	| array_type
;

simple_type:
	IDENTIFIER
;

record_type:
	RECORD_KEYWORD record_entry_list END_KEYWORD
;

record_entry_list:
	  %empty
	| record_entry_list typed_identifier_sequence_list
;

typed_identifier_sequence:
	ident_list COLON_OPERATOR type SEMICOLON_OPERATOR
;

typed_identifier_sequence_list:
	  %empty
	| typed_identifier_sequence_list typed_identifier_sequence
;

array_type:
	ARRAY_KEYWORD LEFTBRACKET_OPERATOR expression COLON_OPERATOR expression RIGHTBRACKET_OPERATOR OF_KEYWORD type
;

	/*
	 * Non-empty, unlike other _list s; do be consistent with the
	 * documentation.
	 */
ident_list:
	IDENTIFIER identifier_prefixed_list
;

identifier_prefixed_list:
	  %empty
	| identifier_prefixed_list COMMA_OPERATOR IDENTIFIER
;

var_decl:
	VAR_KEYWORD typed_identifier_sequence typed_identifier_sequence_list
;

	/* Statements. */

statement_sequence:
	statement statement_prefixed_list
;

statement_prefixed_list:
	  %empty
	| statement_prefixed_list SEMICOLON_OPERATOR statement
;

statement:
	  assignment
	| if_statement
	| while_statement
	| repeat_statement
	| for_statement
	| stop_statement
	| return_statement
	| read_statement
	| write_statement
	| procedure_call
	| null_statement;
;

assignment:
	lvalue COLONEQUALS_OPERATOR expression
;

if_statement:
	IF_KEYWORD expression THEN_KEYWORD statement_sequence elseif_clause_list else_clause_opt END_KEYWORD
;

elseif_clause_list:
	  %empty
	| elseif_clause_list elseif_clause
;

elseif_clause:
	ELSEIF_KEYWORD expression THEN_KEYWORD statement_sequence
;

else_clause_opt:
	  %empty
	| else_clause
;

else_clause:
	ELSE_KEYWORD statement_sequence
;

while_statement:
	WHILE_KEYWORD expression DO_KEYWORD statement_sequence END_KEYWORD
;

repeat_statement:
	REPEAT_KEYWORD statement_sequence UNTIL_KEYWORD expression
;

for_statement:
	FOR_KEYWORD IDENTIFIER COLONEQUALS_OPERATOR expression to_or_downto expression DO_KEYWORD statement_sequence END_KEYWORD
;

to_or_downto:
	  TO_KEYWORD
	| DOWNTO_KEYWORD
;

stop_statement:
	STOP_KEYWORD
;

return_statement:
	RETURN_KEYWORD expression_opt
;

expression_opt:
	  %empty
	| expression
;

read_statement:
	READ_KEYWORD LEFTPARENTHESIS_OPERATOR lvalue_sequence RIGHTPARENTHESIS_OPERATOR
;

lvalue_sequence:
	lvalue lvalue_prefixed_list
;

lvalue_prefixed_list:
	  %empty
	| lvalue_prefixed_list COMMA_OPERATOR lvalue
;

write_statement:
	WRITE_KEYWORD LEFTPARENTHESIS_OPERATOR expression_sequence RIGHTPARENTHESIS_OPERATOR
;

expression_sequence:
	expression expression_prefixed_list
;

expression_prefixed_list:
	  %empty
	| expression_prefixed_list COMMA_OPERATOR expression
;

procedure_call:
	IDENTIFIER LEFTPARENTHESIS_OPERATOR expression_sequence_opt RIGHTPARENTHESIS_OPERATOR
;

expression_sequence_opt:
	  %empty
	| expression_sequence
;

null_statement:
	  %empty
;

	/* Expressions. */

expression:
	  expression PIPE_OPERATOR      expression
	| expression AMPERSAND_OPERATOR expression
	| expression EQUALS_OPERATOR    expression
	| expression LT_OR_GT_OPERATOR  expression
	| expression LE_OPERATOR        expression
	| expression GE_OPERATOR        expression
	| expression LT_OPERATOR        expression
	| expression GT_OPERATOR        expression
	| expression PLUS_OPERATOR      expression
	| expression MINUS_OPERATOR     expression
	| expression TIMES_OPERATOR     expression
	| expression SLASH_OPERATOR     expression
	| expression PERCENT_OPERATOR   expression
	| TILDE_OPERATOR expression
	| MINUS_OPERATOR expression %prec UNARY_MINUS
	| LEFTPARENTHESIS_OPERATOR expression RIGHTPARENTHESIS_OPERATOR
	| IDENTIFIER LEFTPARENTHESIS_OPERATOR expression_sequence_opt RIGHTPARENTHESIS_OPERATOR
	| CHR_KEYWORD LEFTPARENTHESIS_OPERATOR expression RIGHTPARENTHESIS_OPERATOR
	| ORD_KEYWORD LEFTPARENTHESIS_OPERATOR expression RIGHTPARENTHESIS_OPERATOR
	| PRED_KEYWORD LEFTPARENTHESIS_OPERATOR expression RIGHTPARENTHESIS_OPERATOR
	| SUCC_KEYWORD LEFTPARENTHESIS_OPERATOR expression RIGHTPARENTHESIS_OPERATOR
	| lvalue
;

lvalue:
	IDENTIFIER lvalue_accessor_clause_list
;

lvalue_accessor_clause_list:
	  %empty
	| lvalue_accessor_clause_list lvalue_accessor_clause
;

lvalue_accessor_clause:
	  DOT_OPERATOR IDENTIFIER
	| LEFTBRACKET_OPERATOR expression RIGHTBRACKET_OPERATOR
;

%%

ParserState::ParserState(const std::vector<Lexeme> &lexemes)
	: lexemes(lexemes)
	, next_lexeme(0)
	{}

/*
 * Feed in a lexeme: *lvalp = semantic value; return token type;
 */
int yy_cpsl_cc_parserlex(parser_yystype_t *lvalp, std::shared_ptr<ParserState> parser_state) {
	for (; parser_state->next_lexeme < parser_state->lexemes.size(); ++parser_state->next_lexeme) {
		const Lexeme &lexeme = parser_state->lexemes[parser_state->next_lexeme];

		// Skip whitespace and comments when feeding the parser.
		if (lexeme.tag == comment_tag || lexeme.tag == whitespace_tag) {
			continue;
		} else {
			*lvalp = parser_state->next_lexeme++;
			return lexeme.get_enumerated_token_kind();
		}
	}

	// Done traversing lexemes.
	return 0;
}

void yy_cpsl_cc_parsererror(std::shared_ptr<ParserState> parser_state, const char *s) {
	std::ostringstream sstr;
	sstr << "parser error: " << s;
	throw GrammarError(sstr.str());
}

Grammar parse_lexemes(const std::vector<Lexeme> &lexemes) {
	std::shared_ptr<ParserState> parser_state(new ParserState(lexemes));
	int status = yy_cpsl_cc_parserparse(parser_state);
	// status: c.f. https://www.gnu.org/software/bison/manual/html_node/Parser-Function.html
	switch (status) {
		case 0: {
			// Success.
			break;
		} case 1: {
			// Invalid input.
			std::ostringstream sstr;
			sstr << "parser: Invalid input.";
			throw GrammarError(sstr.str());
		} case 2: {
			// Out of memory.
			std::ostringstream sstr;
			sstr << "parser: out of memory.";
			throw GrammarError(sstr.str());
		} default: {
			// Unknown status.
			std::ostringstream sstr;
			sstr << "parser: unrecognized status from parse: " << status;
			throw GrammarError(sstr.str());
		}
	}

	// TODO
	return Grammar();
}
