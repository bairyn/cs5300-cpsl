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
		case string_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::is_char: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::ConstantValue::is_string() const {
	switch(tag) {
		case dynamic_tag:
		case integer_tag:
		case char_tag:
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

void Semantics::ConstantValue::set_string(const std::string &string) {
	switch(tag) {
		case dynamic_tag:
		case integer_tag:
		case char_tag:
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
	const std::map<std::string, ConstantValue> &const_identifier_scope
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
		pipe_branch:
		ampersand_branch:
		equals_branch:
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
