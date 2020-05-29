#ifndef CPSL_CC_GRAMMAR_HH
#define CPSL_CC_GRAMMAR_HH

#include <cstdint>    // uint64_t
#include <variant>    // std::variant

#include "lexer.hh"   // Lexeme

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

class Keyword : public NonterminalSymbol {
public:
	Keyword(uint64_t lexeme);
	uint64_t lexeme;
};

class Operator : public NonterminalSymbol {
public:
	Operator(uint64_t lexeme);
	uint64_t lexeme;
};

class Start : public NonterminalSymbol {
public:
	Start(uint64_t program);
	uint64_t program;
};

class Program : public NonterminalSymbol {
public:
	Program(uint64_t constant_decl_opt, uint64_t type_decl_opt, uint64_t var_decl_opt, uint64_t procedure_decl_or_function_decl_list, uint64_t block, uint64_t dot_operator0);
	uint64_t constant_decl_opt;
	uint64_t type_decl_opt;
	uint64_t var_decl_opt;
	uint64_t procedure_decl_or_function_decl_list;
	uint64_t block;
	uint64_t dot_operator0;
};

class ConstantDeclOpt : public NonterminalSymbol {
public:
	ConstantDeclOpt(uint64_t branch, uint64_t data);
	uint64_t branch;
	uint64_t data;

	enum branch_e {
		null_branch  = 0,
		empty_branch = 1,
		value_branch = 2,
		num_branches = 2,
	};
	typedef enum branch_e branch_t;

	class Empty : public Branch {
	public:
		Empty();
	};

	class Value : public Branch {
	public:
		Value(uint64_t constant_decl);
		uint64_t constant_decl;
	};
};

class TypeDeclOpt : public NonterminalSymbol {
public:
	TypeDeclOpt(uint64_t branch, uint64_t data);
	uint64_t branch;
	uint64_t data;

	enum branch_e {
		null_branch  = 0,
		empty_branch = 1,
		value_branch = 2,
		num_branches = 2,
	};
	typedef enum branch_e branch_t;

	class Empty : public Branch {
	public:
		Empty();
	};

	class Value : public Branch {
	public:
		Value(uint64_t type_decl);
		uint64_t type_decl;
	};
};

class VarDeclOpt : public NonterminalSymbol {
public:
	VarDeclOpt(uint64_t branch, uint64_t data);
	uint64_t branch;
	uint64_t data;

	enum branch_e {
		null_branch  = 0,
		empty_branch = 1,
		value_branch = 2,
		num_branches = 2,
	};
	typedef enum branch_e branch_t;

	class Empty : public Branch {
	public:
		Empty();
	};

	class Value : public Branch {
	public:
		Value(uint64_t var_decl);
		uint64_t var_decl;
	};
};

class ProcedureDeclOrFunctionDeclList : public NonterminalSymbol {
public:
	ProcedureDeclOrFunctionDeclList(uint64_t branch, uint64_t data);
	uint64_t branch;
	uint64_t data;

	enum branch_e {
		null_branch  = 0,
		empty_branch = 1,
		cons_branch  = 2,
		num_branches = 2,
	};
	typedef enum branch_e branch_t;

	class Empty : public Branch {
	public:
		Empty();
	};

	class Cons : public Branch {
	public:
		Cons(uint64_t procedure_decl_or_function_decl_list, uint64_t procedure_decl_or_function_decl);
		uint64_t procedure_decl_or_function_decl_list;
		uint64_t procedure_decl_or_function_decl;
	};
};

class ProcedureDeclOrFunctionDecl : public NonterminalSymbol {
public:
	ProcedureDeclOrFunctionDecl(uint64_t branch, uint64_t data);
	uint64_t branch;
	uint64_t data;

	enum branch_e {
		null_branch      = 0,
		procedure_branch = 1,
		function_branch  = 2,
		num_branches     = 2,
	};
	typedef enum branch_e branch_t;

	class Procedure : public Branch {
	public:
		Procedure(uint64_t procedure_decl);
		uint64_t procedure_decl;
	};

	class Function : public Branch {
	public:
		Function(uint64_t function_decl);
		uint64_t function_decl;
	};
};

class ConstantDecl : public NonterminalSymbol {
public:
	ConstantDecl(uint64_t const_keyword0, uint64_t constant_assignment, uint64_t constant_assignment_list);
	uint64_t const_keyword0;
	uint64_t constant_assignment;
	uint64_t constant_assignment_list;
};

class ConstantAssignmentList : public NonterminalSymbol {
public:
	ConstantAssignmentList(uint64_t branch, uint64_t data);
	uint64_t branch;
	uint64_t data;

	enum branch_e {
		null_branch  = 0,
		empty_branch = 1,
		cons_branch  = 2,
		num_branches = 2,
	};
	typedef enum branch_e branch_t;

	class Empty : public Branch {
	public:
		Empty();
	};

	class Cons : public Branch {
	public:
		Cons(uint64_t constant_assignment_list, uint64_t constant_assignment);
		uint64_t constant_assignment_list;
		uint64_t constant_assignment;
	};
};

class ConstantAssignment : public NonterminalSymbol {
public:
	ConstantAssignment(uint64_t identifier, uint64_t equals_operator0, uint64_t expression, uint64_t semicolon_operator0);
	uint64_t identifier;
	uint64_t equals_operator0;
	uint64_t expression;
	uint64_t semicolon_operator0;
};

class ProcedureDecl : public NonterminalSymbol {
public:
	ProcedureDecl(uint64_t branch, uint64_t data);
	uint64_t branch;
	uint64_t data;

	enum branch_e {
		null_branch       = 0,
		forward_branch    = 1,
		definition_branch = 2,
		num_branches      = 2,
	};
	typedef enum branch_e branch_t;

	class Forward : public Branch {
	public:
		Forward(uint64_t procedure_keyword0, uint64_t identifier, uint64_t leftparenthesis_operator0, uint64_t formal_parameters, uint64_t rightparenthesis_operator0, uint64_t semicolon_operator0, uint64_t forward_keyword0, uint64_t semicolon_operator1);
		uint64_t procedure_keyword0;
		uint64_t identifier;
		uint64_t leftparenthesis_operator0;
		uint64_t formal_parameters;
		uint64_t rightparenthesis_operator0;
		uint64_t semicolon_operator0;
		uint64_t forward_keyword0;
		uint64_t semicolon_operator1;
	};

	class Definition : public Branch {
	public:
		Definition(uint64_t procedure_keyword0, uint64_t identifier, uint64_t leftparenthesis_operator0, uint64_t formal_parameters, uint64_t rightparenthesis_operator0, uint64_t semicolon_operator0, uint64_t body, uint64_t semicolon_operator1);
		uint64_t procedure_keyword0;
		uint64_t identifier;
		uint64_t leftparenthesis_operator0;
		uint64_t formal_parameters;
		uint64_t rightparenthesis_operator0;
		uint64_t semicolon_operator0;
		uint64_t body;
		uint64_t semicolon_operator1;
	};
};

class FunctionDecl : public NonterminalSymbol {
public:
	FunctionDecl(uint64_t branch, uint64_t data);
	uint64_t branch;
	uint64_t data;

	enum branch_e {
		null_branch       = 0,
		forward_branch    = 1,
		definition_branch = 2,
		num_branches      = 2,
	};
	typedef enum branch_e branch_t;

	class Forward : public Branch {
	public:
		Forward(uint64_t function_keyword0, uint64_t identifier, uint64_t leftparenthesis_operator0, uint64_t formal_parameters, uint64_t rightparenthesis_operator0, uint64_t colon_operator0, uint64_t type, uint64_t semicolon_operator0, uint64_t forward_keyword0, uint64_t semicolon_operator1);
		uint64_t function_keyword0;
		uint64_t identifier;
		uint64_t leftparenthesis_operator0;
		uint64_t formal_parameters;
		uint64_t rightparenthesis_operator0;
		uint64_t colon_operator0;
		uint64_t type;
		uint64_t semicolon_operator0;
		uint64_t forward_keyword0;
		uint64_t semicolon_operator1;
	};

	class Definition : public Branch {
	public:
		Definition(uint64_t function_keyword0, uint64_t identifier, uint64_t leftparenthesis_operator0, uint64_t formal_parameters, uint64_t rightparenthesis_operator0, uint64_t colon_operator0, uint64_t type, uint64_t semicolon_operator0, uint64_t body, uint64_t semicolon_operator1);
		uint64_t function_keyword0;
		uint64_t identifier;
		uint64_t leftparenthesis_operator0;
		uint64_t formal_parameters;
		uint64_t rightparenthesis_operator0;
		uint64_t colon_operator0;
		uint64_t type;
		uint64_t semicolon_operator0;
		uint64_t body;
		uint64_t semicolon_operator1;
	};
};

class FormalParameters : public NonterminalSymbol {
public:
	FormalParameters(uint64_t branch, uint64_t data);
	uint64_t branch;
	uint64_t data;

	enum branch_e {
		null_branch  = 0,
		empty_branch = 1,
		first_branch = 2,
		num_branches = 2,
	};
	typedef enum branch_e branch_t;

	class Empty : public Branch {
	public:
		Empty();
	};

	class First : public Branch {
	public:
		First(uint64_t formal_parameter, uint64_t formal_parameter_prefixed_list);
		uint64_t formal_parameter;
		uint64_t formal_parameter_prefixed_list;
	};
};

class FormalParameterPrefixedList : public NonterminalSymbol {
public:
	FormalParameterPrefixedList(uint64_t branch, uint64_t data);
	uint64_t branch;
	uint64_t data;

	enum branch_e {
		null_branch  = 0,
		empty_branch = 1,
		cons_branch  = 2,
		num_branches = 2,
	};
	typedef enum branch_e branch_t;

	class Empty : public Branch {
	public:
		Empty();
	};

	class Cons : public Branch {
	public:
		Cons(uint64_t formal_parameter_prefixed_list, uint64_t semicolon_operator0, uint64_t formal_parameter);
		uint64_t formal_parameter_prefixed_list;
		uint64_t semicolon_operator0;
		uint64_t formal_parameter;
	};
};

// ... (TODO)

class LvalueAccessorClause : public NonterminalSymbol {
public:
	LvalueAccessorClause(uint64_t branch, uint64_t data);
	uint64_t branch;
	uint64_t data;

	enum branch_e {
		null_branch  = 0,
		index_branch = 1,
		array_branch = 2,
		num_branches = 2,
	};
	typedef enum branch_e branch_t;

	class Index : public Branch {
	public:
		Index(uint64_t dot_operator0, uint64_t identifier);
		uint64_t dot_operator0;
		uint64_t identifier;
	};

	class Array : public Branch {
	public:
		Array(uint64_t leftbracket_operator0, uint64_t expression, uint64_t rightbracket_operator0);
		uint64_t leftbracket_operator0;
		uint64_t expression;
		uint64_t rightbracket_operator0;
	};
};

/*
 * Grammar class.
 */

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

	std::vector<Keyword>                                keywords;
	std::vector<Operator>                               operators;
	std::vector<Start>                                  starts;
	std::vector<Program>                                programs;
	std::vector<ConstantDeclOpt>                        constant_decl_opts;
	std::vector<ConstantDeclOpt::Empty>                 constant_decl_opt_empties;
	std::vector<ConstantDeclOpt::Value>                 constant_decl_opt_values;
	std::vector<TypeDeclOpt>                            type_decl_opts;
	std::vector<TypeDeclOpt::Empty>                     type_decl_opt_empties;
	std::vector<TypeDeclOpt::Value>                     type_decl_opt_values;
	std::vector<VarDeclOpt>                             var_decl_opts;
	std::vector<VarDeclOpt::Empty>                      var_decl_opt_empties;
	std::vector<VarDeclOpt::Value>                      var_decl_opt_values;
	std::vector<ProcedureDeclOrFunctionDeclList>        procedure_decl_or_function_decl_lists;
	std::vector<ProcedureDeclOrFunctionDeclList::Empty> procedure_decl_or_function_decl_list_empties;
	std::vector<ProcedureDeclOrFunctionDeclList::Cons>  procedure_decl_or_function_decl_list_conses;
	std::vector<ProcedureDeclOrFunctionDecl>            procedure_decl_or_function_decls;
	std::vector<ProcedureDeclOrFunctionDecl::Procedure> procedure_decl_or_function_decl_procedures;
	std::vector<ProcedureDeclOrFunctionDecl::Function>  procedure_decl_or_function_decl_functions;
	std::vector<ConstantDecl>                           constant_decls;
	std::vector<ConstantAssignmentList>                 constant_assignment_lists;
	std::vector<ConstantAssignmentList::Empty>          constant_assignment_list_empties;
	std::vector<ConstantAssignmentList::Cons>           constant_assignment_list_conses;
	std::vector<ConstantAssignment>                     constant_assignments;
	std::vector<ProcedureDecl>                          procedure_decls;
	std::vector<ProcedureDecl::Forward>                 procedure_decl_forwards;
	std::vector<ProcedureDecl::Definition>              procedure_decl_definitions;
	std::vector<FunctionDecl>                           function_decls;
	std::vector<FunctionDecl::Forward>                  function_decl_forwards;
	std::vector<FunctionDecl::Definition>               function_decl_definitions;
	std::vector<FormalParameters>                       formal_parameterses;
	std::vector<FormalParameters::Empty>                formal_parameters_empties;
	std::vector<FormalParameters::First>                formal_parameters_firsts;
	std::vector<FormalParameterPrefixedList>            formal_parameter_prefixed_lists;
	std::vector<FormalParameterPrefixedList::Empty>     formal_parameter_prefixed_list_empties;
	std::vector<FormalParameterPrefixedList::Cons>      formal_parameter_prefixed_list_conses;
	// ... (TODO)
	std::vector<LvalueAccessorClause>                   lvalue_accessor_clauses;
	std::vector<LvalueAccessorClause::Index>            lvalue_accessor_clause_indexes;
	std::vector<LvalueAccessorClause::Array>            lvalue_accessor_clause_arrays;

	uint64_t new_keyword(uint64_t lexeme);
	uint64_t new_operator(uint64_t lexeme);
	uint64_t new_start(uint64_t program);
	uint64_t new_program(uint64_t constant_decl_opt, uint64_t type_decl_opt, uint64_t var_decl_opt, uint64_t procedure_decl_or_function_decl_list, uint64_t block, uint64_t dot_operator0);
	uint64_t new_constant_decl_opt_empty();
	uint64_t new_constant_decl_opt_value(uint64_t constant_decl);
	uint64_t new_type_decl_opt_empty();
	uint64_t new_type_decl_opt_value(uint64_t type_decl);
	uint64_t new_var_decl_opt_empty();
	uint64_t new_var_decl_opt_value(uint64_t var_decl);
	uint64_t new_procedure_decl_or_function_decl_list_empty();
	uint64_t new_procedure_decl_or_function_decl_list_cons(uint64_t procedure_decl_or_function_decl_list, uint64_t procedure_decl_or_function_decl);
	uint64_t new_procedure_decl_or_function_decl_procedure(uint64_t procedure_decl);
	uint64_t new_procedure_decl_or_function_decl_function(uint64_t function_decl);
	uint64_t new_constant_decl(uint64_t const_keyword0, uint64_t constant_assignment, uint64_t constant_assignment_list);
	uint64_t new_constant_assignment_list_empty();
	uint64_t new_constant_assignment_list_cons(uint64_t constant_assignment_list, uint64_t constant_assignment);
	uint64_t new_constant_assignment(uint64_t identifier, uint64_t equals_operator0, uint64_t expression, uint64_t semicolon_operator0);
	uint64_t new_procedure_decl_forward(uint64_t procedure_keyword0, uint64_t identifier, uint64_t leftparenthesis_operator0, uint64_t formal_parameters, uint64_t rightparenthesis_operator0, uint64_t semicolon_operator0, uint64_t forward_keyword0, uint64_t semicolon_operator1);
	uint64_t new_procedure_decl_definition(uint64_t procedure_keyword0, uint64_t identifier, uint64_t leftparenthesis_operator0, uint64_t formal_parameters, uint64_t rightparenthesis_operator0, uint64_t semicolon_operator0, uint64_t body, uint64_t semicolon_operator1);
	uint64_t new_function_decl_forward(uint64_t function_keyword0, uint64_t identifier, uint64_t leftparenthesis_operator0, uint64_t formal_parameters, uint64_t rightparenthesis_operator0, uint64_t colon_operator0, uint64_t type, uint64_t semicolon_operator0, uint64_t forward_keyword0, uint64_t semicolon_operator1);
	uint64_t new_function_decl_definition(uint64_t function_keyword0, uint64_t identifier, uint64_t leftparenthesis_operator0, uint64_t formal_parameters, uint64_t rightparenthesis_operator0, uint64_t colon_operator0, uint64_t type, uint64_t semicolon_operator0, uint64_t body, uint64_t semicolon_operator1);
	uint64_t new_formal_parameters_empty();
	uint64_t new_formal_parameters_first(uint64_t formal_parameter, uint64_t formal_parameter_prefixed_list);
	uint64_t new_formal_parameter_prefixed_list_empty();
	uint64_t new_formal_parameter_prefixed_list_cons(uint64_t formal_parameter_prefixed_list, uint64_t semicolon_operator0, uint64_t formal_parameter);
	// ... (TODO)
	uint64_t new_lvalue_accessor_clause_index(uint64_t dot_operator0, uint64_t identifier);
	uint64_t new_lvalue_accessor_clause_array(uint64_t leftbracket_operator0, uint64_t expression, uint64_t rightbracket_operator0);
};

#endif /* #ifndef CPSL_CC_GRAMMAR_HH */
