#include <limits>        // std::numeric_limits
#include <sstream>       // std::ostringstream
#include <string>        // std::string
#include <utility>       // std::move
#include <variant>       // std::get, std::monostate

#include "grammar.hh"

#include "semantics.hh"

/*
 * Exception types.
 */

SemanticsError::SemanticsError()
	: runtime_error("A semantics error occurred.")
	{}

SemanticsError::SemanticsError(const std::string &message)
	: runtime_error(message)
	{}

/*
 * Semantics types.
 */

Semantics::ConstantValue::ConstantValue()
	: tag(null_tag)
	, data(std::monostate())
	{}

Semantics::ConstantValue::ConstantValue(tag_t tag, const data_t &data)
	: tag(tag)
	, data(data)
	{}

Semantics::ConstantValue::ConstantValue(tag_t tag, data_t &&data)
	: tag(tag)
	, data(std::move(data))
	{}

const Semantics::ConstantValue Semantics::ConstantValue::dynamic(Semantics::ConstantValue::dynamic_tag, Semantics::ConstantValue::Dynamic());

Semantics::ConstantValue::ConstantValue(uint32_t integer)
	: tag(integer_tag)
	, data(integer)
	{}

Semantics::ConstantValue::ConstantValue(char char_)
	: tag(char_tag)
	, data(char_)
	{}

Semantics::ConstantValue::ConstantValue(bool boolean)
	: tag(integer_tag)
	, data(boolean)
	{}

Semantics::ConstantValue::ConstantValue(const std::string &string)
	: tag(string_tag)
	, data(string)
	{}

Semantics::ConstantValue::ConstantValue(std::string &&string)
	: tag(string_tag)
	, data(std::move(string))
	{}

bool Semantics::ConstantValue::is_static() const {
	switch(tag) {
		case dynamic_tag:
			return false;
		case integer_tag:
		case char_tag:
		case boolean_tag:
		case string_tag:
			return true;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::is_static: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::ConstantValue::is_dynamic() const {
	switch(tag) {
		case dynamic_tag:
			return true;
		case integer_tag:
		case char_tag:
		case boolean_tag:
		case string_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::is_dynamic: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::ConstantValue::is_integer() const {
	switch(tag) {
		case dynamic_tag:
			return false;
		case integer_tag:
			return true;
		case char_tag:
		case boolean_tag:
		case string_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::is_integer: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::ConstantValue::is_char() const {
	switch(tag) {
		case dynamic_tag:
		case integer_tag:
			return false;
		case char_tag:
			return true;
		case boolean_tag:
		case string_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::is_char: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::ConstantValue::is_boolean() const {
	switch(tag) {
		case dynamic_tag:
		case integer_tag:
		case char_tag:
			return false;
		case boolean_tag:
			return true;
		case string_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::is_boolean: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::ConstantValue::is_string() const {
	switch(tag) {
		case dynamic_tag:
		case integer_tag:
		case char_tag:
		case boolean_tag:
			return false;
		case string_tag:
			return true;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::is_string: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

uint32_t Semantics::ConstantValue::get_integer() const {
	switch(tag) {
		case dynamic_tag:
		case integer_tag:
		case char_tag:
		case boolean_tag:
		case string_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::get_integer: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_integer()) {
		std::ostringstream sstr;
		sstr << "Semantics::ConstantValue::get_integer: constant value has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<uint32_t>(data);
}

char Semantics::ConstantValue::get_char() const {
	switch(tag) {
		case dynamic_tag:
		case integer_tag:
		case char_tag:
		case boolean_tag:
		case string_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::get_char: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_char()) {
		std::ostringstream sstr;
		sstr << "Semantics::ConstantValue::get_char: constant value has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<char>(data);
}

std::string Semantics::ConstantValue::get_string() const {
	switch(tag) {
		case dynamic_tag:
		case integer_tag:
		case char_tag:
		case boolean_tag:
		case string_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::get_string: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_string()) {
		std::ostringstream sstr;
		sstr << "Semantics::ConstantValue::get_string: constant value has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<std::string>(data);
}

void Semantics::ConstantValue::set_integer(uint32_t integer) {
	switch(tag) {
		case dynamic_tag:
		case integer_tag:
		case char_tag:
		case boolean_tag:
		case string_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::set_integer: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_integer()) {
		std::ostringstream sstr;
		sstr << "Semantics::ConstantValue::set_integer: constant value has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	data = integer;
}

void Semantics::ConstantValue::set_char(char char_) {
	switch(tag) {
		case dynamic_tag:
		case integer_tag:
		case char_tag:
		case boolean_tag:
		case string_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::set_char: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_char()) {
		std::ostringstream sstr;
		sstr << "Semantics::ConstantValue::set_char: constant value has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	data = char_;
}

void Semantics::ConstantValue::set_boolean(bool boolean) {
	switch(tag) {
		case dynamic_tag:
		case integer_tag:
		case char_tag:
		case boolean_tag:
		case string_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::set_boolean: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_boolean()) {
		std::ostringstream sstr;
		sstr << "Semantics::ConstantValue::set_boolean: constant value has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	data = boolean;
}

void Semantics::ConstantValue::set_string(const std::string &string) {
	switch(tag) {
		case dynamic_tag:
		case integer_tag:
		case char_tag:
		case boolean_tag:
		case string_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::set_string: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_string()) {
		std::ostringstream sstr;
		sstr << "Semantics::ConstantValue::set_string: constant value has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	data = string;
}

void Semantics::ConstantValue::set_string(std::string &&string) {
	switch(tag) {
		case dynamic_tag:
		case integer_tag:
		case char_tag:
		case boolean_tag:
		case string_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::set_string: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_string()) {
		std::ostringstream sstr;
		sstr << "Semantics::ConstantValue::set_string: constant value has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	data = std::string(string);
}

std::string Semantics::ConstantValue::get_tag_repr(tag_t tag) {
	switch(tag) {
		case dynamic_tag:
			return "dynamic";
		case integer_tag:
			return "integer";
		case char_tag:
			return "char";
		case boolean_tag:
			return "boolean";
		case string_tag:
			return "string";

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::get_tag_repr: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

std::string Semantics::ConstantValue::get_tag_repr() const {
	return get_tag_repr(tag);
}

Semantics::Semantics()
	: auto_analyze(true)
{
	if (auto_analyze) {
		analyze();
	}
}

Semantics::Semantics(bool auto_analyze)
	: auto_analyze(auto_analyze)
{
	if (auto_analyze) {
		analyze();
	}
}

Semantics::Semantics(const Grammar &grammar, bool auto_analyze)
	: grammar(grammar)
	, auto_analyze(auto_analyze)
{
	if (auto_analyze) {
		analyze();
	}
}

Semantics::Semantics(Grammar &&grammar, bool auto_analyze)
	: grammar(std::move(grammar))
	, auto_analyze(auto_analyze)
{
	if (auto_analyze) {
		analyze();
	}
}

const Grammar Semantics::get_grammar() const {
	return grammar;
}

void Semantics::set_grammar(const Grammar &grammar) {
	this->grammar = grammar;

	if (auto_analyze) {
		analyze();
	}
}

void Semantics::set_grammar(Grammar &&grammar) {
	grammar = std::move(grammar);

	if (auto_analyze) {
		analyze();
	}
}

// | Determine whether the expression in the grammar tree is a constant expression.
// The result will be memoized in is_expression_constant_calculations.
Semantics::ConstantValue Semantics::is_expression_constant(
	// | Reference to the expression in the grammar tree.
	uint64_t expression,
	// | A collection of identifiers of constants available to the scope of the expression.
	const IdentifierScope &expression_scope
) {
	// TODO: assert() or assert(this->verify()) and configure macros to enable
	// assertions only when debugging is enabled (DEBUG=1 is defined).
	if (expression > grammar.expression_storage.size()) {
		std::ostringstream sstr;
		sstr << "Semantics::is_expression_constant: out of bounds expression reference: " << expression << " >= " << grammar.expression_storage.size() << ".";
		throw SemanticsError(sstr.str());
	}

	// Have we already calculated this value?
	std::map<uint64_t, ConstantValue>::const_iterator expression_search = is_expression_constant_calculations.find(expression);
	if (expression_search != is_expression_constant_calculations.cend()) {
		return expression_search->second;
	}

	// Lookup the expression from the grammar tree.
	const Expression &expression_symbol = grammar.expression_storage[expression];

	// Prepare the calculated constant value.
	ConstantValue expression_constant_value;

	// Branch according to the expression type.
	switch (expression_symbol.branch) {
		// These 16 branches are static iff all subexpressions are static.
		pipe_branch: {
			const Expression::Pipe &pipe           = grammar.expression_pipe_storage.at(expression_symbol.data);
			const Expression       &expression0    = grammar.expression_storage.at(pipe.expression0); (void) expression0;
			const LexemeOperator   &pipe_operator0 = grammar.lexemes.at(pipe.pipe_operator0).get_operator();
			const Expression       &expression1    = grammar.expression_storage.at(pipe.expression1); (void) expression1;

			// Is either expression dynamic?  If so, this expression is also dynamic.
			// (Normally we'd operate on the left side first, but since order
			// of evaluation is referentially transparent and the parser tree
			// is left-recursive, check the expression on the right first,
			// which is more efficient.)
			ConstantValue right = is_expression_constant(pipe.expression1, expression_scope);
			if (right.is_dynamic()) {
				expression_constant_value = right;
				break;
			}
			ConstantValue left  = is_expression_constant(pipe.expression0, expression_scope);
			if (left.is_dynamic()) {
				expression_constant_value = left;
				break;
			}

			// Are the expressions of the same type?
			if (left.tag != right.tag) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< pipe_operator0.line << " col " << pipe_operator0.column
					<< "): refusing to OR different types, "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Are we attempting to operate on a string?
			if (left.is_string() || right.is_string()) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< pipe_operator0.line << " col " << pipe_operator0.column
					<< "): cannot apply bitwise OR on a string expression, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Apply bitwise OR depending on the integer type.
			if       (left.is_integer()) {
				expression_constant_value = ConstantValue(static_cast<uint32_t>(static_cast<uint32_t>(left.get_integer()) | static_cast<uint32_t>(right.get_integer())));
				break;
			} else if(left.is_char()) {
				expression_constant_value = ConstantValue(static_cast<char>(static_cast<char>(left.get_char()) | static_cast<char>(right.get_char())));
				break;
			} else if(left.is_boolean()) {
				expression_constant_value = ConstantValue(static_cast<bool>(static_cast<bool>(left.get_boolean()) | static_cast<bool>(right.get_boolean())));
				break;
			} else {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: internal error (line "
					<< pipe_operator0.line << " col " << pipe_operator0.column
					<< "): unhandled constant expression type for bitwise OR: "
					<< left.get_tag_repr()
					;
				throw SemanticsError(sstr.str());
			}
		} ampersand_branch: {
			const Expression::Ampersand &ampersand           = grammar.expression_ampersand_storage.at(expression_symbol.data);
			const Expression            &expression0         = grammar.expression_storage.at(ampersand.expression0); (void) expression0;
			const LexemeOperator        &ampersand_operator0 = grammar.lexemes.at(ampersand.ampersand_operator0).get_operator();
			const Expression            &expression1         = grammar.expression_storage.at(ampersand.expression1); (void) expression1;

			// Is either expression dynamic?  If so, this expression is also dynamic.
			// (Normally we'd operate on the left side first, but since order
			// of evaluation is referentially transparent and the parser tree
			// is left-recursive, check the expression on the right first,
			// which is more efficient.)
			ConstantValue right = is_expression_constant(ampersand.expression1, expression_scope);
			if (right.is_dynamic()) {
				expression_constant_value = right;
				break;
			}
			ConstantValue left  = is_expression_constant(ampersand.expression0, expression_scope);
			if (left.is_dynamic()) {
				expression_constant_value = left;
				break;
			}

			// Are the expressions of the same type?
			if (left.tag != right.tag) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< ampersand_operator0.line << " col " << ampersand_operator0.column
					<< "): refusing to AND different types, "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Are we attempting to operate on a string?
			if (left.is_string() || right.is_string()) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< ampersand_operator0.line << " col " << ampersand_operator0.column
					<< "): cannot apply bitwise AND on a string expression, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Apply bitwise AND depending on the integer type.
			if       (left.is_integer()) {
				expression_constant_value = ConstantValue(static_cast<uint32_t>(static_cast<uint32_t>(left.get_integer()) & static_cast<uint32_t>(right.get_integer())));
				break;
			} else if(left.is_char()) {
				expression_constant_value = ConstantValue(static_cast<char>(static_cast<char>(left.get_char()) & static_cast<char>(right.get_char())));
				break;
			} else if(left.is_boolean()) {
				expression_constant_value = ConstantValue(static_cast<bool>(static_cast<bool>(left.get_boolean()) & static_cast<bool>(right.get_boolean())));
				break;
			} else {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: internal error (line "
					<< ampersand_operator0.line << " col " << ampersand_operator0.column
					<< "): unhandled constant expression type for bitwise AND: "
					<< left.get_tag_repr()
					;
				throw SemanticsError(sstr.str());
			}
		} equals_branch:
		lt_or_gt_branch:
		le_branch:
		ge_branch:
		lt_branch:
		gt_branch:
		plus_branch:
		minus_branch:
		times_branch:
		slash_branch:
		percent_branch:
		tilde_branch:
		unary_minus_branch:
		parentheses_branch:
			// TODO
			break;

		// These 5 branches are dynamic.
		call_branch:
		chr_branch:
		ord_branch:
		pred_branch:
		succ_branch:
			expression_constant_value = ConstantValue::dynamic;
			break;

		// lvalue_branch may or may not be dynamic, depending on whether it is
		// a simple identifier (without .foo or [bar]) that refers to a known
		// constant value in the scope of the expression.
		lvalue_branch:
			// TODO
			break;

		// These 3 branches are static.
		integer_branch: {
			const Expression::Integer &integer        = grammar.expression_integer_storage.at(expression_symbol.data);
			// TODO: why doesn't get_integer() cause a linker error now?
			const LexemeInteger       &lexeme_integer = grammar.lexemes.at(integer.integer).get_integer();
			if (lexeme_integer.first_digits > std::numeric_limits<uint32_t>::max() || lexeme_integer.remaining_digits.size() > 0) {
				std::ostringstream sstr;
				sstr << "Semantics::is_expression_constant: error (line " << lexeme_integer.line << " col " << lexeme_integer.column << "): integer is too large to encode in 32 bits: " << lexeme_integer.text;
				throw SemanticsError(sstr.str());
			}
			expression_constant_value = ConstantValue(static_cast<uint32_t>(lexeme_integer.first_digits));
			break;
		} char__branch: {
			const Expression::Char_ &char_       = grammar.expression_char__storage.at(expression_symbol.data);
			const LexemeChar        &lexeme_char = grammar.lexemes.at(char_.char_).get_char();
			expression_constant_value = ConstantValue(static_cast<char>(lexeme_char.char_));
			break;
		} string_branch: {
			const Expression::String &string        = grammar.expression_string_storage.at(expression_symbol.data);
			const LexemeString       &lexeme_string = grammar.lexemes.at(string.string).get_string();
			expression_constant_value = ConstantValue(std::move(std::string(lexeme_string.expanded)));
			break;
		}

		// Unrecognized branch.
		default: {
			std::ostringstream sstr;
			sstr << "Semantics::is_expression_constant: internal error: invalid expression branch at index " << expression << ": " << expression_symbol.branch;
			throw SemanticsError(sstr.str());
		}
	}

	// Cache and return the calculated constant value.
	is_expression_constant_calculations.insert({expression, ConstantValue(expression_constant_value)});
	return expression_constant_value;
}

// | Force a re-analysis of the semantics data.
void Semantics::analyze() {
	// TODO
}
