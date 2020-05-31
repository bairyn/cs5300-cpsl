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
/* Enable optional tracing: c.f. https://www.gnu.org/software/bison/manual/html_node/Enabling-Traces.html */
%define parse.trace

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

%token PLUS_OPERATOR             42
%token MINUS_OPERATOR            43
%token TIMES_OPERATOR            44
%token SLASH_OPERATOR            45
%token AMPERSAND_OPERATOR        46
%token PIPE_OPERATOR             47
%token TILDE_OPERATOR            48
%token EQUALS_OPERATOR           49
%token LT_OR_GT_OPERATOR         50
%token LT_OPERATOR               51
%token LE_OPERATOR               52
%token GT_OPERATOR               53
%token GE_OPERATOR               54
%token DOT_OPERATOR              55
%token COMMA_OPERATOR            56
%token COLON_OPERATOR            57
%token SEMICOLON_OPERATOR        58
%token LEFTPARENTHESIS_OPERATOR  59
%token RIGHTPARENTHESIS_OPERATOR 60
%token LEFTBRACKET_OPERATOR      61
%token RIGHTBRACKET_OPERATOR     62
%token COLONEQUALS_OPERATOR      63
%token PERCENT_OPERATOR          64

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

/* Definitions and macros. */
%{
#define pg parser_state->grammar
%}

%%

	/* Comment out "keyword" and "operator" symbols to hide the unused symbol warnings. */

	/*
keyword:
	  ARRAY_KEYWORD     {$$ = pg.new_keyword($1);}
	| BEGIN_KEYWORD     {$$ = pg.new_keyword($1);}
	| CHR_KEYWORD       {$$ = pg.new_keyword($1);}
	| CONST_KEYWORD     {$$ = pg.new_keyword($1);}
	| DO_KEYWORD        {$$ = pg.new_keyword($1);}
	| DOWNTO_KEYWORD    {$$ = pg.new_keyword($1);}
	| ELSE_KEYWORD      {$$ = pg.new_keyword($1);}
	| ELSEIF_KEYWORD    {$$ = pg.new_keyword($1);}
	| END_KEYWORD       {$$ = pg.new_keyword($1);}
	| FOR_KEYWORD       {$$ = pg.new_keyword($1);}
	| FORWARD_KEYWORD   {$$ = pg.new_keyword($1);}
	| FUNCTION_KEYWORD  {$$ = pg.new_keyword($1);}
	| IF_KEYWORD        {$$ = pg.new_keyword($1);}
	| OF_KEYWORD        {$$ = pg.new_keyword($1);}
	| ORD_KEYWORD       {$$ = pg.new_keyword($1);}
	| PRED_KEYWORD      {$$ = pg.new_keyword($1);}
	| PROCEDURE_KEYWORD {$$ = pg.new_keyword($1);}
	| READ_KEYWORD      {$$ = pg.new_keyword($1);}
	| RECORD_KEYWORD    {$$ = pg.new_keyword($1);}
	| REF_KEYWORD       {$$ = pg.new_keyword($1);}
	| REPEAT_KEYWORD    {$$ = pg.new_keyword($1);}
	| RETURN_KEYWORD    {$$ = pg.new_keyword($1);}
	| STOP_KEYWORD      {$$ = pg.new_keyword($1);}
	| SUCC_KEYWORD      {$$ = pg.new_keyword($1);}
	| THEN_KEYWORD      {$$ = pg.new_keyword($1);}
	| TO_KEYWORD        {$$ = pg.new_keyword($1);}
	| TYPE_KEYWORD      {$$ = pg.new_keyword($1);}
	| UNTIL_KEYWORD     {$$ = pg.new_keyword($1);}
	| VAR_KEYWORD       {$$ = pg.new_keyword($1);}
	| WHILE_KEYWORD     {$$ = pg.new_keyword($1);}
	| WRITE_KEYWORD     {$$ = pg.new_keyword($1);}
;
	*/

	/*
operator:
	  PLUS_OPERATOR             {$$ = pg.new_operator($1);}
	| MINUS_OPERATOR            {$$ = pg.new_operator($1);}
	| TIMES_OPERATOR            {$$ = pg.new_operator($1);}
	| SLASH_OPERATOR            {$$ = pg.new_operator($1);}
	| AMPERSAND_OPERATOR        {$$ = pg.new_operator($1);}
	| PIPE_OPERATOR             {$$ = pg.new_operator($1);}
	| TILDE_OPERATOR            {$$ = pg.new_operator($1);}
	| EQUALS_OPERATOR           {$$ = pg.new_operator($1);}
	| LT_OR_GT_OPERATOR         {$$ = pg.new_operator($1);}
	| LT_OPERATOR               {$$ = pg.new_operator($1);}
	| LE_OPERATOR               {$$ = pg.new_operator($1);}
	| GT_OPERATOR               {$$ = pg.new_operator($1);}
	| GE_OPERATOR               {$$ = pg.new_operator($1);}
	| DOT_OPERATOR              {$$ = pg.new_operator($1);}
	| COMMA_OPERATOR            {$$ = pg.new_operator($1);}
	| COLON_OPERATOR            {$$ = pg.new_operator($1);}
	| SEMICOLON_OPERATOR        {$$ = pg.new_operator($1);}
	| LEFTPARENTHESIS_OPERATOR  {$$ = pg.new_operator($1);}
	| RIGHTPARENTHESIS_OPERATOR {$$ = pg.new_operator($1);}
	| LEFTBRACKET_OPERATOR      {$$ = pg.new_operator($1);}
	| RIGHTBRACKET_OPERATOR     {$$ = pg.new_operator($1);}
	| COLONEQUALS_OPERATOR      {$$ = pg.new_operator($1);}
	| PERCENT_OPERATOR          {$$ = pg.new_operator($1);}
;
	*/

start:
	program  {$$ = pg.new_start($1);}
;

	/* Overall structure. */

program:
	constant_decl_opt type_decl_opt var_decl_opt procedure_decl_or_function_decl_list block DOT_OPERATOR {$$ = pg.new_program($1, $2, $3, $4, $5, $6);}
;

constant_decl_opt:
	  %empty        {$$ = pg.new_constant_decl_opt_empty();}
	| constant_decl {$$ = pg.new_constant_decl_opt_value($1);}
;

type_decl_opt:
	  %empty    {$$ = pg.new_type_decl_opt_empty();}
	| type_decl {$$ = pg.new_type_decl_opt_value($1);}
;

var_decl_opt:
	  %empty   {$$ = pg.new_var_decl_opt_empty();}
	| var_decl {$$ = pg.new_var_decl_opt_value($1);}
;

procedure_decl_or_function_decl_list:
	  %empty                                                               {$$ = pg.new_procedure_decl_or_function_decl_list_empty();}
	| procedure_decl_or_function_decl_list procedure_decl_or_function_decl {$$ = pg.new_procedure_decl_or_function_decl_list_cons($1, $2);}
;

procedure_decl_or_function_decl:
	  procedure_decl {$$ = pg.new_procedure_decl_or_function_decl_procedure($1);}
	| function_decl  {$$ = pg.new_procedure_decl_or_function_decl_function($1);}
;

	/* Declarations. */

constant_decl:
	CONST_KEYWORD constant_assignment constant_assignment_list {$$ = pg.new_constant_decl($1, $2, $3);}
;

constant_assignment_list:
	  %empty                                       {$$ = pg.new_constant_assignment_list_empty();}
	| constant_assignment_list constant_assignment {$$ = pg.new_constant_assignment_list_cons($1, $2);}
;

constant_assignment:
	IDENTIFIER EQUALS_OPERATOR expression SEMICOLON_OPERATOR {$$ = pg.new_constant_assignment($1, $2, $3, $4);}
;

procedure_decl:
	  PROCEDURE_KEYWORD IDENTIFIER LEFTPARENTHESIS_OPERATOR formal_parameters RIGHTPARENTHESIS_OPERATOR SEMICOLON_OPERATOR FORWARD_KEYWORD SEMICOLON_OPERATOR {$$ = pg.new_procedure_decl_forward($1, $2, $3, $4, $5, $6, $7, $8);}
	| PROCEDURE_KEYWORD IDENTIFIER LEFTPARENTHESIS_OPERATOR formal_parameters RIGHTPARENTHESIS_OPERATOR SEMICOLON_OPERATOR body SEMICOLON_OPERATOR            {$$ = pg.new_procedure_decl_definition($1, $2, $3, $4, $5, $6, $7, $8);}
;

function_decl:
	  FUNCTION_KEYWORD IDENTIFIER LEFTPARENTHESIS_OPERATOR formal_parameters RIGHTPARENTHESIS_OPERATOR COLON_OPERATOR type SEMICOLON_OPERATOR FORWARD_KEYWORD SEMICOLON_OPERATOR {$$ = pg.new_function_decl_forward($1, $2, $3, $4, $5, $6, $7, $8, $9, $10);}
	| FUNCTION_KEYWORD IDENTIFIER LEFTPARENTHESIS_OPERATOR formal_parameters RIGHTPARENTHESIS_OPERATOR COLON_OPERATOR type SEMICOLON_OPERATOR body SEMICOLON_OPERATOR            {$$ = pg.new_function_decl_definition($1, $2, $3, $4, $5, $6, $7, $8, $9, $10);}
;

	/*
	* The structure is equivalent to the documented one but the details differ a
	* bit.
	*/
formal_parameters:
	  %empty                                          {$$ = pg.new_formal_parameters_empty();}
	| formal_parameter formal_parameter_prefixed_list {$$ = pg.new_formal_parameters_first($1, $2);}
;

formal_parameter_prefixed_list:
	  %empty                                                             {$$ = pg.new_formal_parameter_prefixed_list_empty();}
	| formal_parameter_prefixed_list SEMICOLON_OPERATOR formal_parameter {$$ = pg.new_formal_parameter_prefixed_list_cons($1, $2, $3);}
;

formal_parameter:
	var_or_ref ident_list COLON_OPERATOR type {$$ = pg.new_formal_parameter($1, $2, $3, $4);}
;

var_or_ref:
	  VAR_KEYWORD {$$ = pg.new_var_or_ref_var($1);}
	| REF_KEYWORD {$$ = pg.new_var_or_ref_ref($1);}
;

body:
	constant_decl_opt type_decl_opt var_decl_opt block {$$ = pg.new_body($1, $2, $3, $4);}
;

block:
	BEGIN_KEYWORD statement_sequence END_KEYWORD {$$ = pg.new_block($1, $2, $3);}
;

type_decl:
	TYPE_KEYWORD type_assignment type_assignment_list {$$ = pg.new_type_decl($1, $2, $3);}
;

type_assignment_list:
	  %empty                               {$$ = pg.new_type_assignment_list_empty();}
	| type_assignment_list type_assignment {$$ = pg.new_type_assignment_list_cons($1, $2);}
;

type_assignment:
	IDENTIFIER EQUALS_OPERATOR type SEMICOLON_OPERATOR {$$ = pg.new_type_assignment($1, $2, $3, $4);}
;

type:
	  simple_type {$$ = pg.new_type_simple($1);}
	| record_type {$$ = pg.new_type_record($1);}
	| array_type  {$$ = pg.new_type_array($1);}
;

simple_type:
	IDENTIFIER {$$ = pg.new_simple_type($1);}
;

record_type:
	RECORD_KEYWORD typed_identifier_sequence_list END_KEYWORD {$$ = pg.new_record_type($1, $2, $3);}
;

typed_identifier_sequence_list:
	  %empty                                                   {$$ = pg.new_typed_identifier_sequence_list_empty();}
	| typed_identifier_sequence_list typed_identifier_sequence {$$ = pg.new_typed_identifier_sequence_list_cons($1, $2);}
;

typed_identifier_sequence:
	ident_list COLON_OPERATOR type SEMICOLON_OPERATOR {$$ = pg.new_typed_identifier_sequence($1, $2, $3, $4);}
;

array_type:
	ARRAY_KEYWORD LEFTBRACKET_OPERATOR expression COLON_OPERATOR expression RIGHTBRACKET_OPERATOR OF_KEYWORD type {$$ = pg.new_array_type($1, $2, $3, $4, $5, $6, $7, $8);}
;

	/*
	 * Non-empty, unlike other _list s; do be consistent with the
	 * documentation.
	 */
ident_list:
	IDENTIFIER identifier_prefixed_list {$$ = pg.new_ident_list($1, $2);}
;

identifier_prefixed_list:
	  %empty                                             {$$ = pg.new_identifier_prefixed_list_empty();}
	| identifier_prefixed_list COMMA_OPERATOR IDENTIFIER {$$ = pg.new_identifier_prefixed_list_cons($1, $2, $3);}
;

var_decl:
	VAR_KEYWORD typed_identifier_sequence typed_identifier_sequence_list {$$ = pg.new_var_decl($1, $2, $3);}
;

	/* Statements. */

statement_sequence:
	statement statement_prefixed_list {$$ = pg.new_statement_sequence($1, $2);}
;

statement_prefixed_list:
	  %empty                                               {$$ = pg.new_statement_prefixed_list_empty();}
	| statement_prefixed_list SEMICOLON_OPERATOR statement {$$ = pg.new_statement_prefixed_list_cons($1, $2, $3);}
;

statement:
	  assignment       {$$ = pg.new_statement_assignment($1);}
	| if_statement     {$$ = pg.new_statement_if($1);}
	| while_statement  {$$ = pg.new_statement_while($1);}
	| repeat_statement {$$ = pg.new_statement_repeat($1);}
	| for_statement    {$$ = pg.new_statement_for($1);}
	| stop_statement   {$$ = pg.new_statement_stop($1);}
	| return_statement {$$ = pg.new_statement_return($1);}
	| read_statement   {$$ = pg.new_statement_read($1);}
	| write_statement  {$$ = pg.new_statement_write($1);}
	| procedure_call   {$$ = pg.new_statement_call($1);}
	| null_statement   {$$ = pg.new_statement_null_($1);}
;

assignment:
	lvalue COLONEQUALS_OPERATOR expression {$$ = pg.new_assignment($1, $2, $3);}
;

if_statement:
	IF_KEYWORD expression THEN_KEYWORD statement_sequence elseif_clause_list else_clause_opt END_KEYWORD {$$ = pg.new_if_statement($1, $2, $3, $4, $5, $6, $7);}
;

elseif_clause_list:
	  %empty                           {$$ = pg.new_elseif_clause_list_empty();}
	| elseif_clause_list elseif_clause {$$ = pg.new_elseif_clause_list_cons($1, $2);}
;

elseif_clause:
	ELSEIF_KEYWORD expression THEN_KEYWORD statement_sequence {$$ = pg.new_elseif_clause($1, $2, $3, $4);}
;

else_clause_opt:
	  %empty      {$$ = pg.new_else_clause_opt_empty();}
	| else_clause {$$ = pg.new_else_clause_opt_value($1);}
;

else_clause:
	ELSE_KEYWORD statement_sequence {$$ = pg.new_else_clause($1, $2);}
;

while_statement:
	WHILE_KEYWORD expression DO_KEYWORD statement_sequence END_KEYWORD {$$ = pg.new_while_statement($1, $2, $3, $4, $5);}
;

repeat_statement:
	REPEAT_KEYWORD statement_sequence UNTIL_KEYWORD expression {$$ = pg.new_repeat_statement($1, $2, $3, $4);}
;

for_statement:
	FOR_KEYWORD IDENTIFIER COLONEQUALS_OPERATOR expression to_or_downto expression DO_KEYWORD statement_sequence END_KEYWORD {$$ = pg.new_for_statement($1, $2, $4, $4, $5, $6, $7, $8, $9);}
;

to_or_downto:
	  TO_KEYWORD     {$$ = pg.new_to_or_downto_to($1);}
	| DOWNTO_KEYWORD {$$ = pg.new_to_or_downto_downto($1);}
;

stop_statement:
	STOP_KEYWORD {$$ = pg.new_stop_statement($1);}
;

return_statement:
	RETURN_KEYWORD expression_opt {$$ = pg.new_return_statement($1, $2);}
;

expression_opt:
	  %empty     {$$ = pg.new_expression_opt_empty();}
	| expression {$$ = pg.new_expression_opt_value($1);}
;

read_statement:
	READ_KEYWORD LEFTPARENTHESIS_OPERATOR lvalue_sequence RIGHTPARENTHESIS_OPERATOR {$$ = pg.new_read_statement($1, $2, $3, $4);}
;

lvalue_sequence:
	lvalue lvalue_prefixed_list {$$ = pg.new_lvalue_sequence($1, $2);}
;

lvalue_prefixed_list:
	  %empty                                     {$$ = pg.new_lvalue_prefixed_list_empty();}
	| lvalue_prefixed_list COMMA_OPERATOR lvalue {$$ = pg.new_lvalue_prefixed_list_cons($1, $2, $3);}
;

write_statement:
	WRITE_KEYWORD LEFTPARENTHESIS_OPERATOR expression_sequence RIGHTPARENTHESIS_OPERATOR {$$ = pg.new_write_statement($1, $2, $3, $4);}
;

expression_sequence:
	expression expression_prefixed_list {$$ = pg.new_expression_sequence($1, $2);}
;

expression_prefixed_list:
	  %empty                                             {$$ = pg.new_expression_prefixed_list_empty();}
	| expression_prefixed_list COMMA_OPERATOR expression {$$ = pg.new_expression_prefixed_list_cons($1, $2, $3);}
;

procedure_call:
	IDENTIFIER LEFTPARENTHESIS_OPERATOR expression_sequence_opt RIGHTPARENTHESIS_OPERATOR {$$ = pg.new_procedure_call($1, $2, $3, $4);}
;

expression_sequence_opt:
	  %empty              {$$ = pg.new_expression_sequence_opt_empty();}
	| expression_sequence {$$ = pg.new_expression_sequence_opt_value($1);}
;

null_statement:
	  %empty {$$ = pg.new_null_statement();}
;

	/* Expressions. */

	/*
	 * Note: the int, char, and str branches are missing from the CPSL
	 * documentation, but the document says that ConstExpression is a subset of
	 * Expression, so I'm assuming these were meant to be in this symbol's
	 * branches.
	 */
expression:
	  expression PIPE_OPERATOR      expression                                              {$$ = pg.new_expression_pipe($1, $2, $3);}
	| expression AMPERSAND_OPERATOR expression                                              {$$ = pg.new_expression_ampersand($1, $2, $3);}
	| expression EQUALS_OPERATOR    expression                                              {$$ = pg.new_expression_equals($1, $2, $3);}
	| expression LT_OR_GT_OPERATOR  expression                                              {$$ = pg.new_expression_lt_or_gt($1, $2, $3);}
	| expression LE_OPERATOR        expression                                              {$$ = pg.new_expression_le($1, $2, $3);}
	| expression GE_OPERATOR        expression                                              {$$ = pg.new_expression_ge($1, $2, $3);}
	| expression LT_OPERATOR        expression                                              {$$ = pg.new_expression_lt($1, $2, $3);}
	| expression GT_OPERATOR        expression                                              {$$ = pg.new_expression_gt($1, $2, $3);}
	| expression PLUS_OPERATOR      expression                                              {$$ = pg.new_expression_plus($1, $2, $3);}
	| expression MINUS_OPERATOR     expression                                              {$$ = pg.new_expression_minus($1, $2, $3);}
	| expression TIMES_OPERATOR     expression                                              {$$ = pg.new_expression_times($1, $2, $3);}
	| expression SLASH_OPERATOR     expression                                              {$$ = pg.new_expression_slash($1, $2, $3);}
	| expression PERCENT_OPERATOR   expression                                              {$$ = pg.new_expression_percent($1, $2, $3);}
	| TILDE_OPERATOR expression                                                             {$$ = pg.new_expression_tilde($1, $2);}
	| MINUS_OPERATOR expression  %prec UNARY_MINUS                                          {$$ = pg.new_expression_unary_minus($1, $2);}
	| LEFTPARENTHESIS_OPERATOR expression RIGHTPARENTHESIS_OPERATOR                         {$$ = pg.new_expression_parentheses($1, $2, $3);}
	| IDENTIFIER LEFTPARENTHESIS_OPERATOR expression_sequence_opt RIGHTPARENTHESIS_OPERATOR {$$ = pg.new_expression_call($1, $2, $3, $4);}
	| CHR_KEYWORD LEFTPARENTHESIS_OPERATOR expression RIGHTPARENTHESIS_OPERATOR             {$$ = pg.new_expression_chr($1, $2, $3, $4);}
	| ORD_KEYWORD LEFTPARENTHESIS_OPERATOR expression RIGHTPARENTHESIS_OPERATOR             {$$ = pg.new_expression_ord($1, $2, $3, $4);}
	| PRED_KEYWORD LEFTPARENTHESIS_OPERATOR expression RIGHTPARENTHESIS_OPERATOR            {$$ = pg.new_expression_pred($1, $2, $3, $4);}
	| SUCC_KEYWORD LEFTPARENTHESIS_OPERATOR expression RIGHTPARENTHESIS_OPERATOR            {$$ = pg.new_expression_succ($1, $2, $3, $4);}
	| lvalue                                                                                {$$ = pg.new_expression_lvalue($1);}
	| INTEGER                                                                               {$$ = pg.new_expression_integer($1);}
	| CHAR                                                                                  {$$ = pg.new_expression_char_($1);}
	| STRING                                                                                {$$ = pg.new_expression_string($1);}
;

lvalue:
	IDENTIFIER lvalue_accessor_clause_list {$$ = pg.new_lvalue($1, $2);}
;

lvalue_accessor_clause_list:
	  %empty                                             {$$ = pg.new_lvalue_accessor_clause_list_empty();}
	| lvalue_accessor_clause_list lvalue_accessor_clause {$$ = pg.new_lvalue_accessor_clause_list_cons($1, $2);}
;

lvalue_accessor_clause:
	  DOT_OPERATOR IDENTIFIER                               {$$ = pg.new_lvalue_accessor_clause_index($1, $2);}
	| LEFTBRACKET_OPERATOR expression RIGHTBRACKET_OPERATOR {$$ = pg.new_lvalue_accessor_clause_array($1, $2, $3);}
;

%%

ParserState::ParserState(Grammar &grammar)
	: grammar(grammar)
	, next_lexeme(0)
	{}

/*
 * Feed in a lexeme: *lvalp = semantic value; return token type;
 */
int yy_cpsl_cc_parserlex(parser_yystype_t *lvalp, std::shared_ptr<ParserState> parser_state) {
	for (; parser_state->next_lexeme < parser_state->grammar.lexemes.size(); ++parser_state->next_lexeme) {
		const Lexeme &lexeme = parser_state->grammar.lexemes[parser_state->next_lexeme];

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

Grammar parse_lexemes(const std::vector<Lexeme> &lexemes, bool parser_trace) {
	// Create a new storage for an AST.
	Grammar grammar(lexemes);

	if (parser_trace) {
		yy_cpsl_cc_parserdebug = 1;
	}

	std::shared_ptr<ParserState> parser_state(new ParserState(grammar));
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

	return grammar;
}
