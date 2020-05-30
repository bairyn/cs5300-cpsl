#include <sstream>       // std::ostringstream
#include <string>        // std::string
#include <variant>       // std::get, std::monostate

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
	const std::map<std::string, ConstantValue> &const_identifiers
) {
	// TODO
	return ConstantValue();  // TODO
}

// | Force a re-analysis of the semantics data.
void Semantics::analyze() {
	// TODO
}
