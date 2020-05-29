#ifndef CPSL_CC_GRAMMAR_HH
#define CPSL_CC_GRAMMAR_HH

#include <cstdint>    // uint64_t
#include <variant>    // std::variant

#include "lexer.hh"   // Lexeme
#include "util.h"     // STRCAT*

/*
 * Exceptions types.
 */

class GrammarError : public std::runtime_error {
public:
	GrammarError();
	GrammarError(const std::string &message);
};

/*
 * Grammar types.
 */

class NonterminalSymbol {
public:
};

class Branch {
public:
};

// | DECLARE_SYMBOL_CLASS* macros:
//
// DECLARE_SYMBOL_CLASS_X:
// 	Declare a non-branching symbol class with X components.
// DECLARE_SYMBOL_CLASS_X_Y:
// 	Declare a symbol class with 2 branches.  The first has X components, and
// 	the second has Y components.
// DECLARE_SYMBOL_CLASS_X_Y_Z:
// 	Declare a symbol class with 3 branches.
#define DECLARE_SYMBOL_CLASS_1( \
	S, s, c0 \
) \
class S : public NonterminalSymbol { \
public: \
	S(uint64_t c0); \
	uint64_t c0; \
};

#define DECLARE_SYMBOL_CLASS_3( \
	S, s, c0, c1, c2 \
) \
class S : public NonterminalSymbol { \
public: \
	S(uint64_t c0, uint64_t c1, uint64_t c2); \
	uint64_t c0; \
	uint64_t c1; \
	uint64_t c2; \
};

#define DECLARE_SYMBOL_CLASS_4( \
	S, s, c0, c1, c2, c3 \
) \
class S : public NonterminalSymbol { \
public: \
	S(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3); \
	uint64_t c0; \
	uint64_t c1; \
	uint64_t c2; \
	uint64_t c3; \
};

#define DECLARE_SYMBOL_CLASS_6( \
	S, s, c0, c1, c2, c3, c4, c5 \
) \
class S : public NonterminalSymbol { \
public: \
	S(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3, uint64_t c4, uint64_t c5); \
	uint64_t c0; \
	uint64_t c1; \
	uint64_t c2; \
	uint64_t c3; \
	uint64_t c4; \
	uint64_t c5; \
};

#define DECLARE_SYMBOL_CLASS_0_1( \
	S, s, B0, B1, b0, b1, b1c0 \
) \
class S : public NonterminalSymbol { \
public: \
	S(uint64_t branch, uint64_t data); \
	uint64_t branch; \
	uint64_t data; \
 \
 	enum branch_e { \
		null_branch           = 0, \
		STRCAT_2(b0, _branch) = 1, \
		STRCAT_2(b1, _branch) = 2, \
		num_branches          = 2, \
	}; \
 \
 	class B0 : public Branch { \
	public: \
		B0(); \
	}; \
 \
 	class B1 : public Branch { \
	public: \
		B1(uint64_t b1c0); \
		uint64_t b1c0; \
	}; \
};

#define DECLARE_SYMBOL_CLASS_0_2( \
	S, s, B0, B1, b0, b1, b1c0, b1c1 \
) \
class S : public NonterminalSymbol { \
public: \
	S(uint64_t branch, uint64_t data); \
	uint64_t branch; \
	uint64_t data; \
 \
 	enum branch_e { \
		null_branch           = 0, \
		STRCAT_2(b0, _branch) = 1, \
		STRCAT_2(b1, _branch) = 2, \
		num_branches          = 2, \
	}; \
 \
 	class B0 : public Branch { \
	public: \
		B0(); \
	}; \
 \
 	class B1 : public Branch { \
	public: \
		B1(uint64_t b1c0, uint64_t b1c1); \
		uint64_t b1c0; \
		uint64_t b1c1; \
	}; \
};

#define DECLARE_SYMBOL_CLASS_0_3( \
	S, s, B0, B1, b0, b1, b1c0, b1c1, b1c2 \
) \
class S : public NonterminalSymbol { \
public: \
	S(uint64_t branch, uint64_t data); \
	uint64_t branch; \
	uint64_t data; \
 \
 	enum branch_e { \
		null_branch           = 0, \
		STRCAT_2(b0, _branch) = 1, \
		STRCAT_2(b1, _branch) = 2, \
		num_branches          = 2, \
	}; \
 \
 	class B0 : public Branch { \
	public: \
		B0(); \
	}; \
 \
 	class B1 : public Branch { \
	public: \
		B1(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2); \
		uint64_t b1c0; \
		uint64_t b1c1; \
		uint64_t b1c2; \
	}; \
};

#define DECLARE_SYMBOL_CLASS_1_1( \
	S, s, B0, B1, b0, b1, b0c0, b1c0 \
) \
class S : public NonterminalSymbol { \
public: \
	S(uint64_t branch, uint64_t data); \
	uint64_t branch; \
	uint64_t data; \
 \
 	enum branch_e { \
		null_branch           = 0, \
		STRCAT_2(b0, _branch) = 1, \
		STRCAT_2(b1, _branch) = 2, \
		num_branches          = 2, \
	}; \
 \
 	class B0 : public Branch { \
	public: \
		B0(uint64_t b0c0); \
		uint64_t b0c0; \
	}; \
 \
 	class B1 : public Branch { \
	public: \
		B1(uint64_t b1c0); \
		uint64_t b1c0; \
	}; \
};

#define DECLARE_SYMBOL_CLASS_2_3( \
	S, s, B0, B1, b0, b1, b0c0, b0c1, b1c0, b1c1, b1c2 \
) \
class S : public NonterminalSymbol { \
public: \
	S(uint64_t branch, uint64_t data); \
	uint64_t branch; \
	uint64_t data; \
 \
 	enum branch_e { \
		null_branch           = 0, \
		STRCAT_2(b0, _branch) = 1, \
		STRCAT_2(b1, _branch) = 2, \
		num_branches          = 2, \
	}; \
 \
 	class B0 : public Branch { \
	public: \
		B0(uint64_t b0c0, uint64_t b0c1); \
		uint64_t b0c0; \
		uint64_t b0c1; \
	}; \
 \
 	class B1 : public Branch { \
	public: \
		B1(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2); \
		uint64_t b1c0; \
		uint64_t b1c1; \
		uint64_t b1c2; \
	}; \
};

#define DECLARE_SYMBOL_CLASS_8_8( \
	S, s, B0, B1, b0, b1, b0c0, b0c1, b0c2, b0c3, b0c4, b0c5, b0c6, b0c7, b1c0, b1c1, b1c2, b1c3, b1c4, b1c5, b1c6, b1c7 \
) \
class S : public NonterminalSymbol { \
public: \
	S(uint64_t branch, uint64_t data); \
	uint64_t branch; \
	uint64_t data; \
 \
 	enum branch_e { \
		null_branch           = 0, \
		STRCAT_2(b0, _branch) = 1, \
		STRCAT_2(b1, _branch) = 2, \
		num_branches          = 2, \
	}; \
 \
 	class B0 : public Branch { \
	public: \
		B0(uint64_t b0c0, uint64_t b0c1, uint64_t b0c2, uint64_t b0c3, uint64_t b0c4, uint64_t b0c5, uint64_t b0c6, uint64_t b0c7); \
		uint64_t b0c0; \
		uint64_t b0c1; \
		uint64_t b0c2; \
		uint64_t b0c3; \
		uint64_t b0c4; \
		uint64_t b0c5; \
		uint64_t b0c6; \
		uint64_t b0c7; \
	}; \
 \
 	class B1 : public Branch { \
	public: \
		B1(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2, uint64_t b1c3, uint64_t b1c4, uint64_t b1c5, uint64_t b1c6, uint64_t b1c7); \
		uint64_t b1c0; \
		uint64_t b1c1; \
		uint64_t b1c2; \
		uint64_t b1c3; \
		uint64_t b1c4; \
		uint64_t b1c5; \
		uint64_t b1c6; \
		uint64_t b1c7; \
	}; \
};

#define DECLARE_SYMBOL_CLASS_10_10( \
	S, s, B0, B1, b0, b1, b0c0, b0c1, b0c2, b0c3, b0c4, b0c5, b0c6, b0c7, b0c8, b0c9, b1c0, b1c1, b1c2, b1c3, b1c4, b1c5, b1c6, b1c7, b1c8, b1c9 \
) \
class S : public NonterminalSymbol { \
public: \
	S(uint64_t branch, uint64_t data); \
	uint64_t branch; \
	uint64_t data; \
 \
 	enum branch_e { \
		null_branch           = 0, \
		STRCAT_2(b0, _branch) = 1, \
		STRCAT_2(b1, _branch) = 2, \
		num_branches          = 2, \
	}; \
 \
 	class B0 : public Branch { \
	public: \
		B0(uint64_t b0c0, uint64_t b0c1, uint64_t b0c2, uint64_t b0c3, uint64_t b0c4, uint64_t b0c5, uint64_t b0c6, uint64_t b0c7, uint64_t b0c8, uint64_t b0c9); \
		uint64_t b0c0; \
		uint64_t b0c1; \
		uint64_t b0c2; \
		uint64_t b0c3; \
		uint64_t b0c4; \
		uint64_t b0c5; \
		uint64_t b0c6; \
		uint64_t b0c7; \
		uint64_t b0c8; \
		uint64_t b0c9; \
	}; \
 \
 	class B1 : public Branch { \
	public: \
		B1(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2, uint64_t b1c3, uint64_t b1c4, uint64_t b1c5, uint64_t b1c6, uint64_t b1c7, uint64_t b1c8, uint64_t b1c9); \
		uint64_t b1c0; \
		uint64_t b1c1; \
		uint64_t b1c2; \
		uint64_t b1c3; \
		uint64_t b1c4; \
		uint64_t b1c5; \
		uint64_t b1c6; \
		uint64_t b1c7; \
		uint64_t b1c8; \
		uint64_t b1c9; \
	}; \
};

DECLARE_SYMBOL_CLASS_1(Keyword, keyword, lexeme)
DECLARE_SYMBOL_CLASS_1(Operator, operator_, lexeme)
DECLARE_SYMBOL_CLASS_1(Start, start, program)
DECLARE_SYMBOL_CLASS_6(Program, program, constant_decl_opt, type_decl_opt, var_decl_opt, procedure_decl_or_function_decl_list, block, dot_operator0)
DECLARE_SYMBOL_CLASS_0_1(ConstantDeclOpt, constant_decl_opt, Empty, Value, empty, value, constant_decl)
DECLARE_SYMBOL_CLASS_0_1(TypeDeclOpt, type_decl_opt, Empty, Value, empty, value, type_decl)
DECLARE_SYMBOL_CLASS_0_1(VarDeclOpt, var_decl_opt, Empty, Value, empty, value, var_decl)
DECLARE_SYMBOL_CLASS_0_2(ProcedureDeclOrFunctionDeclList, procedure_decl_or_function_decl_list, Empty, Cons, empty, cons, procedure_decl_or_function_decl_list, procedure_decl_or_function_decl)
DECLARE_SYMBOL_CLASS_1_1(ProcedureDeclOrFunctionDecl, procedure_decl_or_function_decl, Procedure, Function, procedure, function, procedure_decl, function_decl)
DECLARE_SYMBOL_CLASS_3(ConstantDecl, constant_decl, const_keyword0, constant_assignment, constant_assignment_list)
DECLARE_SYMBOL_CLASS_0_2(ConstantAssignmentList, constant_assignment_list, Empty, Cons, empty, cons, constant_assignment_list, constant_assignment)
DECLARE_SYMBOL_CLASS_4(ConstantAssignment, constant_assignment, identifier, equals_operator0, expression, semicolon_operator0)
DECLARE_SYMBOL_CLASS_8_8(
	ProcedureDecl, procedure_decl, Forward, Definition, forward, definition,
	procedure_keyword0, identifier, leftparenthesis_operator0, formal_parameters, rightparenthesis_operator0, semicolon_operator0, forward_keyword0, semicolon_operator1,
	procedure_keyword0, identifier, leftparenthesis_operator0, formal_parameters, rightparenthesis_operator0, semicolon_operator0, body, semicolon_operator1
)
DECLARE_SYMBOL_CLASS_10_10(
	FunctionDecl, function_decl, Forward, Definition, forward, definition,
	function_keyword0, identifier, leftparenthesis_operator0, formal_parameters, rightparenthesis_operator0, colon_operator0, type, semicolon_operator0, forward_keyword0, semicolon_operator1,
	function_keyword0, identifier, leftparenthesis_operator0, formal_parameters, rightparenthesis_operator0, colon_operator0, type, semicolon_operator0, body, semicolon_operator1
)
DECLARE_SYMBOL_CLASS_0_2(FormalParameters, formal_parameters, Empty, First, empty, first, formal_parameter, formal_parameter_prefixed_list)
DECLARE_SYMBOL_CLASS_0_3(FormalParameterPrefixedList, formal_parameter_prefixed_list, Empty, Cons, empty, cons, formal_parameter_prefixed_list, semicolon_operator0, formal_parameter)
// ... (TODO)
DECLARE_SYMBOL_CLASS_2_3(
	LvalueAccessorClause, lvalue_accessor_clause, Index, Array, index, array,
	dot_operator0, identifier,
	leftbracket_operator0, expression, rightbracket_operator0
)

/*
 * Grammar class.
 */

#define DECLARE_SYMBOL_FIELDS_1( \
	S, s, c0 \
) \
std::vector<S> STRCAT_2(s, _storage); \
uint64_t STRCAT_2(new_, s)(uint64_t c0);

#define DECLARE_SYMBOL_FIELDS_3( \
	S, s, c0, c1, c2 \
) \
std::vector<S> STRCAT_2(s, _storage); \
uint64_t STRCAT_2(new_, s)(uint64_t c0, uint64_t c1, uint64_t c2);

#define DECLARE_SYMBOL_FIELDS_4( \
	S, s, c0, c1, c2, c3 \
) \
std::vector<S> STRCAT_2(s, _storage); \
uint64_t STRCAT_2(new_, s)(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3);

#define DECLARE_SYMBOL_FIELDS_6( \
	S, s, c0, c1, c2, c3, c4, c5 \
) \
std::vector<S> STRCAT_2(s, _storage); \
uint64_t STRCAT_2(new_, s)(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3, uint64_t c4, uint64_t c5);

#define DECLARE_SYMBOL_FIELDS_0_1( \
	S, s, B0, B1, b0, b1, b1c0 \
) \
std::vector<S> STRCAT_2(s, _storage); \
std::vector<S::B0> STRCAT_4(s, _, b0, _storage); \
std::vector<S::B1> STRCAT_4(s, _, b1, _storage); \
uint64_t STRCAT_4(new_, s, _, b0)(); \
uint64_t STRCAT_4(new_, s, _, b1)(uint64_t b1c0);

#define DECLARE_SYMBOL_FIELDS_0_2( \
	S, s, B0, B1, b0, b1, b1c0, b1c1 \
) \
std::vector<S> STRCAT_2(s, _storage); \
std::vector<S::B0> STRCAT_4(s, _, b0, _storage); \
std::vector<S::B1> STRCAT_4(s, _, b1, _storage); \
uint64_t STRCAT_4(new_, s, _, b0)(); \
uint64_t STRCAT_4(new_, s, _, b1)(uint64_t b1c0, uint64_t b1c1);

#define DECLARE_SYMBOL_FIELDS_0_3( \
	S, s, B0, B1, b0, b1, b1c0, b1c1, b1c2 \
) \
std::vector<S> STRCAT_2(s, _storage); \
std::vector<S::B0> STRCAT_4(s, _, b0, _storage); \
std::vector<S::B1> STRCAT_4(s, _, b1, _storage); \
uint64_t STRCAT_4(new_, s, _, b0)(); \
uint64_t STRCAT_4(new_, s, _, b1)(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2);

#define DECLARE_SYMBOL_FIELDS_1_1( \
	S, s, B0, B1, b0, b1, b0c0, b1c0 \
) \
std::vector<S> STRCAT_2(s, _storage); \
std::vector<S::B0> STRCAT_4(s, _, b0, _storage); \
std::vector<S::B1> STRCAT_4(s, _, b1, _storage); \
uint64_t STRCAT_4(new_, s, _, b0)(uint64_t b0c0); \
uint64_t STRCAT_4(new_, s, _, b1)(uint64_t b1c0);

#define DECLARE_SYMBOL_FIELDS_2_3( \
	S, s, B0, B1, b0, b1, b0c0, b0c1, b1c0, b1c1, b1c2 \
) \
std::vector<S> STRCAT_2(s, _storage); \
std::vector<S::B0> STRCAT_4(s, _, b0, _storage); \
std::vector<S::B1> STRCAT_4(s, _, b1, _storage); \
uint64_t STRCAT_4(new_, s, _, b0)(uint64_t b0c0, uint64_t b0c1); \
uint64_t STRCAT_4(new_, s, _, b1)(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2);

#define DECLARE_SYMBOL_FIELDS_8_8( \
	S, s, B0, B1, b0, b1, b0c0, b0c1, b0c2, b0c3, b0c4, b0c5, b0c6, b0c7, b1c0, b1c1, b1c2, b1c3, b1c4, b1c5, b1c6, b1c7 \
) \
std::vector<S> STRCAT_2(s, _storage); \
std::vector<S::B0> STRCAT_4(s, _, b0, _storage); \
std::vector<S::B1> STRCAT_4(s, _, b1, _storage); \
uint64_t STRCAT_4(new_, s, _, b0)(uint64_t b0c0, uint64_t b0c1, uint64_t b0c2, uint64_t b0c3, uint64_t b0c4, uint64_t b0c5, uint64_t b0c6, uint64_t b0c7); \
uint64_t STRCAT_4(new_, s, _, b1)(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2, uint64_t b1c3, uint64_t b1c4, uint64_t b1c5, uint64_t b1c6, uint64_t b1c7);

#define DECLARE_SYMBOL_FIELDS_10_10( \
	S, s, B0, B1, b0, b1, b0c0, b0c1, b0c2, b0c3, b0c4, b0c5, b0c6, b0c7, b0c8, b0c9, b1c0, b1c1, b1c2, b1c3, b1c4, b1c5, b1c6, b1c7, b1c8, b1c9 \
) \
std::vector<S> STRCAT_2(s, _storage); \
std::vector<S::B0> STRCAT_4(s, _, b0, _storage); \
std::vector<S::B1> STRCAT_4(s, _, b1, _storage); \
uint64_t STRCAT_4(new_, s, _, b0)(uint64_t b0c0, uint64_t b0c1, uint64_t b0c2, uint64_t b0c3, uint64_t b0c4, uint64_t b0c5, uint64_t b0c6, uint64_t b0c7, uint64_t b0c8, uint64_t b0c9); \
uint64_t STRCAT_4(new_, s, _, b1)(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2, uint64_t b1c3, uint64_t b1c4, uint64_t b1c5, uint64_t b1c6, uint64_t b1c7, uint64_t b1c8, uint64_t b1c9);

// | A complete parse tree of a vector of lexemes.
//
// This class also represents a storage without partial deletion, but nothing
// will need to be deleted until the entire storage is freed.
class Grammar {
public:
	Grammar();
	Grammar(const std::vector<Lexeme> &lexemes);
	Grammar(std::vector<Lexeme> &&lexemes);

	std::vector<Lexeme> lexemes;

	DECLARE_SYMBOL_FIELDS_1(Keyword, keyword, lexeme)
	DECLARE_SYMBOL_FIELDS_1(Operator, operator_, lexeme)
	DECLARE_SYMBOL_FIELDS_1(Start, start, program)
	DECLARE_SYMBOL_FIELDS_6(Program, program, constant_decl_opt, type_decl_opt, var_decl_opt, procedure_decl_or_function_decl_list, block, dot_operator0)
	DECLARE_SYMBOL_FIELDS_0_1(ConstantDeclOpt, constant_decl_opt, Empty, Value, empty, value, constant_decl)
	DECLARE_SYMBOL_FIELDS_0_1(TypeDeclOpt, type_decl_opt, Empty, Value, empty, value, type_decl)
	DECLARE_SYMBOL_FIELDS_0_1(VarDeclOpt, var_decl_opt, Empty, Value, empty, value, var_decl)
	DECLARE_SYMBOL_FIELDS_0_2(ProcedureDeclOrFunctionDeclList, procedure_decl_or_function_decl_list, Empty, Cons, empty, cons, procedure_decl_or_function_decl_list, procedure_decl_or_function_decl)
	DECLARE_SYMBOL_FIELDS_1_1(ProcedureDeclOrFunctionDecl, procedure_decl_or_function_decl, Procedure, Function, procedure, function, procedure_decl, function_decl)
	DECLARE_SYMBOL_FIELDS_3(ConstantDecl, constant_decl, const_keyword0, constant_assignment, constant_assignment_list)
	DECLARE_SYMBOL_FIELDS_0_2(ConstantAssignmentList, constant_assignment_list, Empty, Cons, empty, cons, constant_assignment_list, constant_assignment)
	DECLARE_SYMBOL_FIELDS_4(ConstantAssignment, constant_assignment, identifier, equals_operator0, expression, semicolon_operator0)
	DECLARE_SYMBOL_FIELDS_8_8(
		ProcedureDecl, procedure_decl, Forward, Definition, forward, definition,
		procedure_keyword0, identifier, leftparenthesis_operator0, formal_parameters, rightparenthesis_operator0, semicolon_operator0, forward_keyword0, semicolon_operator1,
		procedure_keyword0, identifier, leftparenthesis_operator0, formal_parameters, rightparenthesis_operator0, semicolon_operator0, body, semicolon_operator1
	)
	DECLARE_SYMBOL_FIELDS_10_10(
		FunctionDecl, function_decl, Forward, Definition, forward, definition,
		function_keyword0, identifier, leftparenthesis_operator0, formal_parameters, rightparenthesis_operator0, colon_operator0, type, semicolon_operator0, forward_keyword0, semicolon_operator1,
		function_keyword0, identifier, leftparenthesis_operator0, formal_parameters, rightparenthesis_operator0, colon_operator0, type, semicolon_operator0, body, semicolon_operator1
	)
	DECLARE_SYMBOL_FIELDS_0_2(FormalParameters, formal_parameters, Empty, First, empty, first, formal_parameter, formal_parameter_prefixed_list)
	DECLARE_SYMBOL_FIELDS_0_3(FormalParameterPrefixedList, formal_parameter_prefixed_list, Empty, Cons, empty, cons, formal_parameter_prefixed_list, semicolon_operator0, formal_parameter)
	// ... (TODO)
	DECLARE_SYMBOL_FIELDS_2_3(
		LvalueAccessorClause, lvalue_accessor_clause, Index, Array, index, array,
		dot_operator0, identifier,
		leftbracket_operator0, expression, rightbracket_operator0
	)
};

#endif /* #ifndef CPSL_CC_GRAMMAR_HH */
