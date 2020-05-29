#include "grammar.hh"

/*
 * Exception types.
 */

GrammarError::GrammarError()
	: runtime_error("A grammar error occurred.")
	{}

GrammarError::GrammarError(const std::string &message)
	: runtime_error(message)
	{}

/*
 * Grammar types and class.
 */

// | A complete parse tree of a vector of lexemes.
Grammar::Grammar()
	{}

Grammar::Grammar(const std::vector<Lexeme> &lexemes)
	: lexemes(lexemes)
	{}

Grammar::Grammar(std::vector<Lexeme> &&lexemes)
	: lexemes(std::move(lexemes))
	{}

#define DEFINE_SYMBOL_1( \
	S, s, c0 \
) \
S::S(uint64_t c0) \
	: c0(c0) \
	{} \
 \
uint64_t Grammar::STRCAT_2(new_, s)(uint64_t c0) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	STRCAT_2(s, _storage).emplace_back(c0); \
	return index; \
}

#define DEFINE_SYMBOL_3( \
	S, s, c0, c1, c2 \
) \
S::S(uint64_t c0, uint64_t c1, uint64_t c2) \
	: c0(c0) \
	, c1(c1) \
	, c2(c2) \
	{} \
 \
uint64_t Grammar::STRCAT_2(new_, s)(uint64_t c0, uint64_t c1, uint64_t c2) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	STRCAT_2(s, _storage).emplace_back(c0, c1, c2); \
	return index; \
}

#define DEFINE_SYMBOL_4( \
	S, s, c0, c1, c2, c3 \
) \
S::S(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3) \
	: c0(c0) \
	, c1(c1) \
	, c2(c2) \
	, c3(c3) \
	{} \
 \
uint64_t Grammar::STRCAT_2(new_, s)(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	STRCAT_2(s, _storage).emplace_back(c0, c1, c2, c3); \
	return index; \
}

#define DEFINE_SYMBOL_6( \
	S, s, c0, c1, c2, c3, c4, c5 \
) \
S::S(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3, uint64_t c4, uint64_t c5) \
	: c0(c0) \
	, c1(c1) \
	, c2(c2) \
	, c3(c3) \
	, c4(c4) \
	, c5(c5) \
	{} \
 \
uint64_t Grammar::STRCAT_2(new_, s)(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3, uint64_t c4, uint64_t c5) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	STRCAT_2(s, _storage).emplace_back(c0, c1, c2, c3, c4, c5); \
	return index; \
}

#define DEFINE_SYMBOL_0_1( \
	S, s, B0, B1, b0, b1, b1c0 \
) \
S::S(uint64_t branch, uint64_t data) \
	: branch(branch) \
	, data(data) \
	{} \
 \
S::B0::B0() \
	{} \
 \
S::B1::B1(uint64_t b1c0) \
	: b1c0(b1c0) \
	{} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b0)() { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b0, _storage).size()); \
	STRCAT_4(s, _, b0, _storage).emplace_back(); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b0, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b1)(uint64_t b1c0) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b1, _storage).size()); \
	STRCAT_4(s, _, b1, _storage).emplace_back(b1c0); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b1, _branch), branch_index); \
	return index; \
}

#define DEFINE_SYMBOL_0_2( \
	S, s, B0, B1, b0, b1, b1c0, b1c1 \
) \
S::S(uint64_t branch, uint64_t data) \
	: branch(branch) \
	, data(data) \
	{} \
 \
S::B0::B0() \
	{} \
 \
S::B1::B1(uint64_t b1c0, uint64_t b1c1) \
	: b1c0(b1c0) \
	, b1c1(b1c1) \
	{} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b0)() { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b0, _storage).size()); \
	STRCAT_4(s, _, b0, _storage).emplace_back(); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b0, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b1)(uint64_t b1c0, uint64_t b1c1) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b1, _storage).size()); \
	STRCAT_4(s, _, b1, _storage).emplace_back(b1c0, b1c1); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b1, _branch), branch_index); \
	return index; \
}

#define DEFINE_SYMBOL_0_3( \
	S, s, B0, B1, b0, b1, b1c0, b1c1, b1c2 \
) \
S::S(uint64_t branch, uint64_t data) \
	: branch(branch) \
	, data(data) \
	{} \
 \
S::B0::B0() \
	{} \
 \
S::B1::B1(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2) \
	: b1c0(b1c0) \
	, b1c1(b1c1) \
	, b1c2(b1c2) \
	{} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b0)() { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b0, _storage).size()); \
	STRCAT_4(s, _, b0, _storage).emplace_back(); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b0, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b1)(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b1, _storage).size()); \
	STRCAT_4(s, _, b1, _storage).emplace_back(b1c0, b1c1, b1c2); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b1, _branch), branch_index); \
	return index; \
}

#define DEFINE_SYMBOL_1_1( \
	S, s, B0, B1, b0, b1, b0c0, b1c0 \
) \
S::S(uint64_t branch, uint64_t data) \
	: branch(branch) \
	, data(data) \
	{} \
 \
S::B0::B0(uint64_t b0c0) \
	: b0c0(b0c0) \
	{} \
 \
S::B1::B1(uint64_t b1c0) \
	: b1c0(b1c0) \
	{} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b0)(uint64_t b0c0) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b0, _storage).size()); \
	STRCAT_4(s, _, b0, _storage).emplace_back(b0c0); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b0, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b1)(uint64_t b1c0) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b1, _storage).size()); \
	STRCAT_4(s, _, b1, _storage).emplace_back(b1c0); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b1, _branch), branch_index); \
	return index; \
}

#define DEFINE_SYMBOL_2_3( \
	S, s, B0, B1, b0, b1, b0c0, b0c1, b1c0, b1c1, b1c2 \
) \
S::S(uint64_t branch, uint64_t data) \
	: branch(branch) \
	, data(data) \
	{} \
 \
S::B0::B0(uint64_t b0c0, uint64_t b0c1) \
	: b0c0(b0c0) \
	, b0c1(b0c1) \
	{} \
 \
S::B1::B1(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2) \
	: b1c0(b1c0) \
	, b1c1(b1c1) \
	, b1c2(b1c2) \
	{} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b0)(uint64_t b0c0, uint64_t b0c1) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b0, _storage).size()); \
	STRCAT_4(s, _, b0, _storage).emplace_back(b0c0, b0c1); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b0, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b1)(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b1, _storage).size()); \
	STRCAT_4(s, _, b1, _storage).emplace_back(b1c0, b1c1, b1c2); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b1, _branch), branch_index); \
	return index; \
}

#define DEFINE_SYMBOL_8_8( \
	S, s, B0, B1, b0, b1, b0c0, b0c1, b0c2, b0c3, b0c4, b0c5, b0c6, b0c7, b1c0, b1c1, b1c2, b1c3, b1c4, b1c5, b1c6, b1c7 \
) \
S::S(uint64_t branch, uint64_t data) \
	: branch(branch) \
	, data(data) \
	{} \
 \
S::B0::B0(uint64_t b0c0, uint64_t b0c1, uint64_t b0c2, uint64_t b0c3, uint64_t b0c4, uint64_t b0c5, uint64_t b0c6, uint64_t b0c7) \
	: b0c0(b0c0) \
	, b0c1(b0c1) \
	, b0c2(b0c2) \
	, b0c3(b0c3) \
	, b0c4(b0c4) \
	, b0c5(b0c5) \
	, b0c6(b0c6) \
	, b0c7(b0c7) \
	{} \
 \
S::B1::B1(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2, uint64_t b1c3, uint64_t b1c4, uint64_t b1c5, uint64_t b1c6, uint64_t b1c7) \
	: b1c0(b1c0) \
	, b1c1(b1c1) \
	, b1c2(b1c2) \
	, b1c3(b1c3) \
	, b1c4(b1c4) \
	, b1c5(b1c5) \
	, b1c6(b1c6) \
	, b1c7(b1c7) \
	{} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b0)(uint64_t b0c0, uint64_t b0c1, uint64_t b0c2, uint64_t b0c3, uint64_t b0c4, uint64_t b0c5, uint64_t b0c6, uint64_t b0c7) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b0, _storage).size()); \
	STRCAT_4(s, _, b0, _storage).emplace_back(b0c0, b0c1, b0c2, b0c3, b0c4, b0c5, b0c6, b0c7); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b0, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b1)(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2, uint64_t b1c3, uint64_t b1c4, uint64_t b1c5, uint64_t b1c6, uint64_t b1c7) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b1, _storage).size()); \
	STRCAT_4(s, _, b1, _storage).emplace_back(b1c0, b1c1, b1c2, b1c3, b1c4, b1c5, b1c6, b1c7); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b1, _branch), branch_index); \
	return index; \
}

#define DEFINE_SYMBOL_10_10( \
	S, s, B0, B1, b0, b1, b0c0, b0c1, b0c2, b0c3, b0c4, b0c5, b0c6, b0c7, b0c8, b0c9, b1c0, b1c1, b1c2, b1c3, b1c4, b1c5, b1c6, b1c7, b1c8, b1c9 \
) \
S::S(uint64_t branch, uint64_t data) \
	: branch(branch) \
	, data(data) \
	{} \
 \
S::B0::B0(uint64_t b0c0, uint64_t b0c1, uint64_t b0c2, uint64_t b0c3, uint64_t b0c4, uint64_t b0c5, uint64_t b0c6, uint64_t b0c7, uint64_t b0c8, uint64_t b0c9) \
	: b0c0(b0c0) \
	, b0c1(b0c1) \
	, b0c2(b0c2) \
	, b0c3(b0c3) \
	, b0c4(b0c4) \
	, b0c5(b0c5) \
	, b0c6(b0c6) \
	, b0c7(b0c7) \
	, b0c8(b0c8) \
	, b0c9(b0c9) \
	{} \
 \
S::B1::B1(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2, uint64_t b1c3, uint64_t b1c4, uint64_t b1c5, uint64_t b1c6, uint64_t b1c7, uint64_t b1c8, uint64_t b1c9) \
	: b1c0(b1c0) \
	, b1c1(b1c1) \
	, b1c2(b1c2) \
	, b1c3(b1c3) \
	, b1c4(b1c4) \
	, b1c5(b1c5) \
	, b1c6(b1c6) \
	, b1c7(b1c7) \
	, b1c8(b1c8) \
	, b1c9(b1c9) \
	{} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b0)(uint64_t b0c0, uint64_t b0c1, uint64_t b0c2, uint64_t b0c3, uint64_t b0c4, uint64_t b0c5, uint64_t b0c6, uint64_t b0c7, uint64_t b0c8, uint64_t b0c9) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b0, _storage).size()); \
	STRCAT_4(s, _, b0, _storage).emplace_back(b0c0, b0c1, b0c2, b0c3, b0c4, b0c5, b0c6, b0c7, b0c8, b0c9); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b0, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b1)(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2, uint64_t b1c3, uint64_t b1c4, uint64_t b1c5, uint64_t b1c6, uint64_t b1c7, uint64_t b1c8, uint64_t b1c9) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b1, _storage).size()); \
	STRCAT_4(s, _, b1, _storage).emplace_back(b1c0, b1c1, b1c2, b1c3, b1c4, b1c5, b1c6, b1c7, b1c8, b1c9); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b1, _branch), branch_index); \
	return index; \
}

DEFINE_SYMBOL_1(Keyword, keyword, lexeme)
DEFINE_SYMBOL_1(Operator, operator_, lexeme)
DEFINE_SYMBOL_1(Start, start, program)
DEFINE_SYMBOL_6(Program, program, constant_decl_opt, type_decl_opt, var_decl_opt, procedure_decl_or_function_decl_list, block, dot_operator0)
DEFINE_SYMBOL_0_1(ConstantDeclOpt, constant_decl_opt, Empty, Value, empty, value, constant_decl)
DEFINE_SYMBOL_0_1(TypeDeclOpt, type_decl_opt, Empty, Value, empty, value, type_decl)
DEFINE_SYMBOL_0_1(VarDeclOpt, var_decl_opt, Empty, Value, empty, value, var_decl)
DEFINE_SYMBOL_0_2(ProcedureDeclOrFunctionDeclList, procedure_decl_or_function_decl_list, Empty, Cons, empty, cons, procedure_decl_or_function_decl_list, procedure_decl_or_function_decl)
DEFINE_SYMBOL_1_1(ProcedureDeclOrFunctionDecl, procedure_decl_or_function_decl, Procedure, Function, procedure, function, procedure_decl, function_decl)
DEFINE_SYMBOL_3(ConstantDecl, constant_decl, const_keyword0, constant_assignment, constant_assignment_list)
DEFINE_SYMBOL_0_2(ConstantAssignmentList, constant_assignment_list, Empty, Cons, empty, cons, constant_assignment_list, constant_assignment)
DEFINE_SYMBOL_4(ConstantAssignment, constant_assignment, identifier, equals_operator0, expression, semicolon_operator0)
DEFINE_SYMBOL_8_8(
	ProcedureDecl, procedure_decl, Forward, Definition, forward, definition,
	procedure_keyword0, identifier, leftparenthesis_operator0, formal_parameters, rightparenthesis_operator0, semicolon_operator0, forward_keyword0, semicolon_operator1,
	procedure_keyword0, identifier, leftparenthesis_operator0, formal_parameters, rightparenthesis_operator0, semicolon_operator0, body, semicolon_operator1
)
DEFINE_SYMBOL_10_10(
	FunctionDecl, function_decl, Forward, Definition, forward, definition,
	function_keyword0, identifier, leftparenthesis_operator0, formal_parameters, rightparenthesis_operator0, colon_operator0, type, semicolon_operator0, forward_keyword0, semicolon_operator1,
	function_keyword0, identifier, leftparenthesis_operator0, formal_parameters, rightparenthesis_operator0, colon_operator0, type, semicolon_operator0, body, semicolon_operator1
)
DEFINE_SYMBOL_0_2(FormalParameters, formal_parameters, Empty, First, empty, first, formal_parameter, formal_parameter_prefixed_list)
DEFINE_SYMBOL_0_3(FormalParameterPrefixedList, formal_parameter_prefixed_list, Empty, Cons, empty, cons, formal_parameter_prefixed_list, semicolon_operator0, formal_parameter)
// ... (TODO)
DEFINE_SYMBOL_2_3(
	LvalueAccessorClause, lvalue_accessor_clause, Index, Array, index, array,
	dot_operator0, identifier,
	leftbracket_operator0, expression, rightbracket_operator0
)
