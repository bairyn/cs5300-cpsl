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
 * Grammar types.
 */

Keyword::Keyword(uint64_t lexeme)
	: lexeme(lexeme)
	{}

Operator::Operator(uint64_t lexeme)
	: lexeme(lexeme)
	{}

Start::Start(uint64_t program)
	: program(program)
	{}

Program::Program(uint64_t constant_decl_opt, uint64_t type_decl_opt, uint64_t var_decl_opt, uint64_t procedure_decl_or_function_decl_list, uint64_t block, uint64_t dot_operator0)
	: constant_decl_opt(constant_decl_opt)
	, type_decl_opt(type_decl_opt)
	, var_decl_opt(var_decl_opt)
	, procedure_decl_or_function_decl_list(procedure_decl_or_function_decl_list)
	, block(block)
	, dot_operator0(dot_operator0)
	{}

ConstantDeclOpt::ConstantDeclOpt(uint64_t branch, uint64_t data)
	: branch(branch)
	, data(data)
	{}

ConstantDeclOpt::Empty::Empty()
	{}

ConstantDeclOpt::Value::Value(uint64_t constant_decl)
	: constant_decl(constant_decl)
	{}

TypeDeclOpt::TypeDeclOpt(uint64_t branch, uint64_t data)
	: branch(branch)
	, data(data)
	{}

TypeDeclOpt::Empty::Empty()
	{}

TypeDeclOpt::Value::Value(uint64_t type_decl)
	: type_decl(type_decl)
	{}

VarDeclOpt::VarDeclOpt(uint64_t branch, uint64_t data)
	: branch(branch)
	, data(data)
	{}

VarDeclOpt::Empty::Empty()
	{}

VarDeclOpt::Value::Value(uint64_t var_decl)
	: var_decl(var_decl)
	{}

ProcedureDeclOrFunctionDeclList::ProcedureDeclOrFunctionDeclList(uint64_t branch, uint64_t data)
	: branch(branch)
	, data(data)
	{}

ProcedureDeclOrFunctionDeclList::Empty::Empty()
	{}

ProcedureDeclOrFunctionDeclList::Cons::Cons(uint64_t procedure_decl_or_function_decl_list, uint64_t procedure_decl_or_function_decl)
	: procedure_decl_or_function_decl_list(procedure_decl_or_function_decl_list)
	, procedure_decl_or_function_decl(procedure_decl_or_function_decl)
	{}

ProcedureDeclOrFunctionDecl::ProcedureDeclOrFunctionDecl(uint64_t branch, uint64_t data)
	: branch(branch)
	, data(data)
	{}

ProcedureDeclOrFunctionDecl::Procedure::Procedure(uint64_t procedure_decl)
	: procedure_decl(procedure_decl)
	{}

ProcedureDeclOrFunctionDecl::Function::Function(uint64_t function_decl)
	: function_decl(function_decl)
	{}

// ... (TODO)

LvalueAccessorClause::LvalueAccessorClause(uint64_t branch, uint64_t data)
	: branch(branch)
	, data(data)
	{}

LvalueAccessorClause::Index::Index(uint64_t dot_operator0, uint64_t identifier)
	: dot_operator0(dot_operator0)
	, identifier(identifier)
	{}

LvalueAccessorClause::Array::Array(uint64_t leftbracket_operator0, uint64_t expression, uint64_t rightbracket_operator0)
	: leftbracket_operator0(leftbracket_operator0)
	, expression(expression)
	, rightbracket_operator0(rightbracket_operator0)
	{}

/*
 * Grammar class.
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

uint64_t Grammar::new_keyword(uint64_t lexeme) {
	uint64_t index = static_cast<uint64_t>(keywords.size());
	keywords.emplace_back(lexeme);
	return index;
}

uint64_t Grammar::new_operator(uint64_t lexeme) {
	uint64_t index = static_cast<uint64_t>(operators.size());
	operators.emplace_back(lexeme);
	return index;
}

uint64_t Grammar::new_start(uint64_t program) {
	uint64_t index = static_cast<uint64_t>(starts.size());
	starts.emplace_back(program);
	return index;
}

uint64_t Grammar::new_program(uint64_t constant_decl_opt, uint64_t type_decl_opt, uint64_t var_decl_opt, uint64_t procedure_decl_or_function_decl_list, uint64_t block, uint64_t dot_operator0) {
	uint64_t index = static_cast<uint64_t>(programs.size());
	programs.emplace_back(constant_decl_opt, type_decl_opt, var_decl_opt, procedure_decl_or_function_decl_list, block, dot_operator0);
	return index;
}

uint64_t Grammar::new_constant_decl_opt_empty() {
	uint64_t index = static_cast<uint64_t>(constant_decl_opts.size());
	uint64_t empty_index = static_cast<uint64_t>(constant_decl_opt_empties.size());
	constant_decl_opt_empties.emplace_back();
	constant_decl_opts.emplace_back(ConstantDeclOpt::empty_branch, empty_index);
	return index;
}

uint64_t Grammar::new_constant_decl_opt_value(uint64_t constant_decl) {
	uint64_t index = static_cast<uint64_t>(constant_decl_opts.size());
	uint64_t value_index = static_cast<uint64_t>(constant_decl_opt_values.size());
	constant_decl_opt_values.emplace_back(constant_decl);
	constant_decl_opts.emplace_back(ConstantDeclOpt::value_branch, value_index);
	return index;
}

uint64_t Grammar::new_type_decl_opt_empty() {
	uint64_t index = static_cast<uint64_t>(type_decl_opts.size());
	uint64_t empty_index = static_cast<uint64_t>(type_decl_opt_empties.size());
	type_decl_opt_empties.emplace_back();
	type_decl_opts.emplace_back(TypeDeclOpt::empty_branch, empty_index);
	return index;
}

uint64_t Grammar::new_type_decl_opt_value(uint64_t type_decl) {
	uint64_t index = static_cast<uint64_t>(type_decl_opts.size());
	uint64_t value_index = static_cast<uint64_t>(type_decl_opt_values.size());
	type_decl_opt_values.emplace_back(type_decl);
	type_decl_opts.emplace_back(TypeDeclOpt::value_branch, value_index);
	return index;
}

uint64_t Grammar::new_var_decl_opt_empty() {
	uint64_t index = static_cast<uint64_t>(var_decl_opts.size());
	uint64_t empty_index = static_cast<uint64_t>(var_decl_opt_empties.size());
	var_decl_opt_empties.emplace_back();
	var_decl_opts.emplace_back(VarDeclOpt::empty_branch, empty_index);
	return index;
}

uint64_t Grammar::new_var_decl_opt_value(uint64_t var_decl) {
	uint64_t index = static_cast<uint64_t>(var_decl_opts.size());
	uint64_t value_index = static_cast<uint64_t>(var_decl_opt_values.size());
	var_decl_opt_values.emplace_back(var_decl);
	var_decl_opts.emplace_back(VarDeclOpt::value_branch, value_index);
	return index;
}

uint64_t Grammar::new_procedure_decl_or_function_decl_list_empty() {
	uint64_t index = static_cast<uint64_t>(procedure_decl_or_function_decl_lists.size());
	uint64_t empty_index = static_cast<uint64_t>(procedure_decl_or_function_decl_list_empties.size());
	procedure_decl_or_function_decl_list_empties.emplace_back();
	procedure_decl_or_function_decl_lists.emplace_back(ProcedureDeclOrFunctionDeclList::empty_branch, empty_index);
	return index;
}

uint64_t Grammar::new_procedure_decl_or_function_decl_list_cons(uint64_t procedure_decl_or_function_decl_list, uint64_t procedure_decl_or_function_decl) {
	uint64_t index = static_cast<uint64_t>(procedure_decl_or_function_decl_lists.size());
	uint64_t cons_index = static_cast<uint64_t>(procedure_decl_or_function_decl_list_conses.size());
	procedure_decl_or_function_decl_list_conses.emplace_back(procedure_decl_or_function_decl_list, procedure_decl_or_function_decl);
	procedure_decl_or_function_decl_lists.emplace_back(ProcedureDeclOrFunctionDeclList::cons_branch, cons_index);
	return index;
}

uint64_t Grammar::new_procedure_decl_or_function_decl_procedure(uint64_t procedure_decl) {
	uint64_t index = static_cast<uint64_t>(procedure_decl_or_function_decls.size());
	uint64_t procedure_index = static_cast<uint64_t>(procedure_decl_or_function_decl_procedures.size());
	procedure_decl_or_function_decl_procedures.emplace_back(procedure_decl);
	procedure_decl_or_function_decls.emplace_back(ProcedureDeclOrFunctionDecl::procedure_branch, procedure_index);
	return index;
}

uint64_t Grammar::new_procedure_decl_or_function_decl_function(uint64_t function_decl) {
	uint64_t index = static_cast<uint64_t>(procedure_decl_or_function_decls.size());
	uint64_t function_index = static_cast<uint64_t>(procedure_decl_or_function_decl_functions.size());
	procedure_decl_or_function_decl_functions.emplace_back(function_decl);
	procedure_decl_or_function_decls.emplace_back(ProcedureDeclOrFunctionDecl::function_branch, function_index);
	return index;
}

// ... (TODO)

uint64_t Grammar::new_lvalue_accessor_clause_index(uint64_t dot_operator0, uint64_t identifier) {
	uint64_t index = static_cast<uint64_t>(lvalue_accessor_clauses.size());
	uint64_t index_index = static_cast<uint64_t>(lvalue_accessor_clause_indexes.size());
	lvalue_accessor_clause_indexes.emplace_back(dot_operator0, identifier);
	lvalue_accessor_clauses.emplace_back(LvalueAccessorClause::index_branch, index_index);
	return index;
}

uint64_t Grammar::new_lvalue_accessor_clause_array(uint64_t leftbracket_operator0, uint64_t expression, uint64_t rightbracket_operator0) {
	uint64_t index = static_cast<uint64_t>(lvalue_accessor_clauses.size());
	uint64_t array_index = static_cast<uint64_t>(lvalue_accessor_clause_arrays.size());
	lvalue_accessor_clause_arrays.emplace_back(leftbracket_operator0, expression, rightbracket_operator0);
	lvalue_accessor_clauses.emplace_back(LvalueAccessorClause::array_branch, array_index);
	return index;
}
