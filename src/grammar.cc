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

ConstantDecl::ConstantDecl(uint64_t const_keyword0, uint64_t constant_assignment, uint64_t constant_assignment_list)
	: const_keyword0(const_keyword0)
	, constant_assignment(constant_assignment)
	, constant_assignment_list(constant_assignment_list)
	{}

ConstantAssignmentList::ConstantAssignmentList(uint64_t branch, uint64_t data)
	: branch(branch)
	, data(data)
	{}

ConstantAssignmentList::Empty::Empty()
	{}

ConstantAssignmentList::Cons::Cons(uint64_t constant_assignment_list, uint64_t constant_assignment)
	: constant_assignment_list(constant_assignment_list)
	, constant_assignment(constant_assignment)
	{}

ConstantAssignment::ConstantAssignment(uint64_t identifier, uint64_t equals_operator0, uint64_t expression, uint64_t semicolon_operator0)
	: identifier(identifier)
	, equals_operator0(equals_operator0)
	, expression(expression)
	, semicolon_operator0(semicolon_operator0)
	{}

ProcedureDecl::ProcedureDecl(uint64_t branch, uint64_t data)
	: branch(branch)
	, data(data)
	{}

ProcedureDecl::Forward::Forward(uint64_t procedure_keyword0, uint64_t identifier, uint64_t leftparenthesis_operator0, uint64_t formal_parameters, uint64_t rightparenthesis_operator0, uint64_t semicolon_operator0, uint64_t forward_keyword0, uint64_t semicolon_operator1)
	: procedure_keyword0(procedure_keyword0)
	, identifier(identifier)
	, leftparenthesis_operator0(leftparenthesis_operator0)
	, formal_parameters(formal_parameters)
	, rightparenthesis_operator0(rightparenthesis_operator0)
	, semicolon_operator0(semicolon_operator0)
	, forward_keyword0(forward_keyword0)
	, semicolon_operator1(semicolon_operator1)
	{}

ProcedureDecl::Definition::Definition(uint64_t procedure_keyword0, uint64_t identifier, uint64_t leftparenthesis_operator0, uint64_t formal_parameters, uint64_t rightparenthesis_operator0, uint64_t semicolon_operator0, uint64_t body, uint64_t semicolon_operator1)
	: procedure_keyword0(procedure_keyword0)
	, identifier(identifier)
	, leftparenthesis_operator0(leftparenthesis_operator0)
	, formal_parameters(formal_parameters)
	, rightparenthesis_operator0(rightparenthesis_operator0)
	, semicolon_operator0(semicolon_operator0)
	, body(body)
	, semicolon_operator1(semicolon_operator1)
	{}

FunctionDecl::FunctionDecl(uint64_t branch, uint64_t data)
	: branch(branch)
	, data(data)
	{}

FunctionDecl::Forward::Forward(uint64_t function_keyword0, uint64_t identifier, uint64_t leftparenthesis_operator0, uint64_t formal_parameters, uint64_t rightparenthesis_operator0, uint64_t colon_operator0, uint64_t type, uint64_t semicolon_operator0, uint64_t forward_keyword0, uint64_t semicolon_operator1)
	: function_keyword0(function_keyword0)
	, identifier(identifier)
	, leftparenthesis_operator0(leftparenthesis_operator0)
	, formal_parameters(formal_parameters)
	, rightparenthesis_operator0(rightparenthesis_operator0)
	, colon_operator0(colon_operator0)
	, type(type)
	, semicolon_operator0(semicolon_operator0)
	, forward_keyword0(forward_keyword0)
	, semicolon_operator1(semicolon_operator1)
	{}

FunctionDecl::Definition::Definition(uint64_t function_keyword0, uint64_t identifier, uint64_t leftparenthesis_operator0, uint64_t formal_parameters, uint64_t rightparenthesis_operator0, uint64_t colon_operator0, uint64_t type, uint64_t semicolon_operator0, uint64_t body, uint64_t semicolon_operator1)
	: function_keyword0(function_keyword0)
	, identifier(identifier)
	, leftparenthesis_operator0(leftparenthesis_operator0)
	, formal_parameters(formal_parameters)
	, rightparenthesis_operator0(rightparenthesis_operator0)
	, colon_operator0(colon_operator0)
	, type(type)
	, semicolon_operator0(semicolon_operator0)
	, body(body)
	, semicolon_operator1(semicolon_operator1)
	{}

FormalParameters::FormalParameters(uint64_t branch, uint64_t data)
	: branch(branch)
	, data(data)
	{}

FormalParameters::Empty::Empty()
	{}

FormalParameters::First::First(uint64_t formal_parameter, uint64_t formal_parameter_prefixed_list)
	: formal_parameter(formal_parameter)
	, formal_parameter_prefixed_list(formal_parameter_prefixed_list)
	{}

FormalParameterPrefixedList::FormalParameterPrefixedList(uint64_t branch, uint64_t data)
	: branch(branch)
	, data(data)
	{}

FormalParameterPrefixedList::Empty::Empty()
	{}

FormalParameterPrefixedList::Cons::Cons(uint64_t formal_parameter_prefixed_list, uint64_t semicolon_operator0, uint64_t formal_parameter)
	: formal_parameter_prefixed_list(formal_parameter_prefixed_list)
	, semicolon_operator0(semicolon_operator0)
	, formal_parameter(formal_parameter)
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

uint64_t Grammar::new_constant_decl(uint64_t const_keyword0, uint64_t constant_assignment, uint64_t constant_assignment_list) {
	uint64_t index = static_cast<uint64_t>(constant_decls.size());
	constant_decls.emplace_back(const_keyword0, constant_assignment, constant_assignment_list);
	return index;
}

uint64_t Grammar::new_constant_assignment_list_empty() {
	uint64_t index = static_cast<uint64_t>(constant_assignment_lists.size());
	uint64_t empty_index = static_cast<uint64_t>(constant_assignment_list_empties.size());
	constant_assignment_list_empties.emplace_back();
	constant_assignment_lists.emplace_back(ConstantAssignmentList::empty_branch, empty_index);
	return index;
}

uint64_t Grammar::new_constant_assignment_list_cons(uint64_t constant_assignment_list, uint64_t constant_assignment) {
	uint64_t index = static_cast<uint64_t>(constant_assignment_lists.size());
	uint64_t cons_index = static_cast<uint64_t>(constant_assignment_list_conses.size());
	constant_assignment_list_conses.emplace_back(constant_assignment_list, constant_assignment);
	constant_assignment_lists.emplace_back(ConstantAssignmentList::cons_branch, cons_index);
	return index;
}

uint64_t Grammar::new_constant_assignment(uint64_t identifier, uint64_t equals_operator0, uint64_t expression, uint64_t semicolon_operator0) {
	uint64_t index = static_cast<uint64_t>(constant_assignments.size());
	constant_assignments.emplace_back(identifier, equals_operator0, expression, semicolon_operator0);
	return index;
}

uint64_t Grammar::new_procedure_decl_forward(uint64_t procedure_keyword0, uint64_t identifier, uint64_t leftparenthesis_operator0, uint64_t formal_parameters, uint64_t rightparenthesis_operator0, uint64_t semicolon_operator0, uint64_t forward_keyword0, uint64_t semicolon_operator1) {
	uint64_t index = static_cast<uint64_t>(procedure_decls.size());
	uint64_t forward_index = static_cast<uint64_t>(procedure_decl_forwards.size());
	procedure_decl_forwards.emplace_back(procedure_keyword0, identifier, leftparenthesis_operator0, formal_parameters, rightparenthesis_operator0, semicolon_operator0, forward_keyword0, semicolon_operator1);
	procedure_decls.emplace_back(ProcedureDecl::forward_branch, forward_index);
	return index;
}

uint64_t Grammar::new_procedure_decl_definition(uint64_t procedure_keyword0, uint64_t identifier, uint64_t leftparenthesis_operator0, uint64_t formal_parameters, uint64_t rightparenthesis_operator0, uint64_t semicolon_operator0, uint64_t body, uint64_t semicolon_operator1) {
	uint64_t index = static_cast<uint64_t>(procedure_decls.size());
	uint64_t definition_index = static_cast<uint64_t>(procedure_decl_definitions.size());
	procedure_decl_definitions.emplace_back(procedure_keyword0, identifier, leftparenthesis_operator0, formal_parameters, rightparenthesis_operator0, semicolon_operator0, body, semicolon_operator1);
	procedure_decls.emplace_back(ProcedureDecl::definition_branch, definition_index);
	return index;
}

uint64_t Grammar::new_function_decl_forward(uint64_t function_keyword0, uint64_t identifier, uint64_t leftparenthesis_operator0, uint64_t formal_parameters, uint64_t rightparenthesis_operator0, uint64_t colon_operator0, uint64_t type, uint64_t semicolon_operator0, uint64_t forward_keyword0, uint64_t semicolon_operator1) {
	uint64_t index = static_cast<uint64_t>(function_decls.size());
	uint64_t forward_index = static_cast<uint64_t>(function_decl_forwards.size());
	function_decl_forwards.emplace_back(function_keyword0, identifier, leftparenthesis_operator0, formal_parameters, rightparenthesis_operator0, colon_operator0, type, semicolon_operator0, forward_keyword0, semicolon_operator1);
	function_decls.emplace_back(FunctionDecl::forward_branch, forward_index);
	return index;
}

uint64_t Grammar::new_function_decl_definition(uint64_t function_keyword0, uint64_t identifier, uint64_t leftparenthesis_operator0, uint64_t formal_parameters, uint64_t rightparenthesis_operator0, uint64_t colon_operator0, uint64_t type, uint64_t semicolon_operator0, uint64_t body, uint64_t semicolon_operator1) {
	uint64_t index = static_cast<uint64_t>(function_decls.size());
	uint64_t definition_index = static_cast<uint64_t>(function_decl_definitions.size());
	function_decl_definitions.emplace_back(function_keyword0, identifier, leftparenthesis_operator0, formal_parameters, rightparenthesis_operator0, colon_operator0, type, semicolon_operator0, body, semicolon_operator1);
	function_decls.emplace_back(FunctionDecl::definition_branch, definition_index);
	return index;
}

uint64_t Grammar::new_formal_parameters_empty() {
	uint64_t index = static_cast<uint64_t>(formal_parameterses.size());
	uint64_t empty_index = static_cast<uint64_t>(formal_parameters_empties.size());
	formal_parameters_empties.emplace_back();
	formal_parameterses.emplace_back(FormalParameters::empty_branch, empty_index);
	return index;
}

uint64_t Grammar::new_formal_parameters_first(uint64_t formal_parameter, uint64_t formal_parameter_prefixed_list) {
	uint64_t index = static_cast<uint64_t>(formal_parameterses.size());
	uint64_t first_index = static_cast<uint64_t>(formal_parameters_firsts.size());
	formal_parameters_firsts.emplace_back(formal_parameter, formal_parameter_prefixed_list);
	formal_parameterses.emplace_back(FormalParameters::first_branch, first_index);
	return index;
}

uint64_t Grammar::new_formal_parameter_prefixed_list_empty() {
	uint64_t index = static_cast<uint64_t>(formal_parameter_prefixed_lists.size());
	uint64_t empty_index = static_cast<uint64_t>(formal_parameter_prefixed_list_empties.size());
	formal_parameter_prefixed_list_empties.emplace_back();
	formal_parameter_prefixed_lists.emplace_back(FormalParameterPrefixedList::empty_branch, empty_index);
	return index;
}

uint64_t Grammar::new_formal_parameter_prefixed_list_cons(uint64_t formal_parameter_prefixed_list, uint64_t semicolon_operator0, uint64_t formal_parameter) {
	uint64_t index = static_cast<uint64_t>(formal_parameter_prefixed_lists.size());
	uint64_t cons_index = static_cast<uint64_t>(formal_parameter_prefixed_list_conses.size());
	formal_parameter_prefixed_list_conses.emplace_back(formal_parameter_prefixed_list, semicolon_operator0, formal_parameter);
	formal_parameter_prefixed_lists.emplace_back(FormalParameterPrefixedList::cons_branch, cons_index);
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
