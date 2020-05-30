#include <cassert>       // assert
#include <limits>        // std::numeric_limits
#include <optional>      // std::optional
#include <sstream>       // std::ostringstream
#include <string>        // std::string
#include <utility>       // std::as_const, std::move
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

Semantics::ConstantValue::ConstantValue(int32_t integer)
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

int32_t Semantics::ConstantValue::get_integer() const {
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

	return std::get<int32_t>(data);
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

bool Semantics::ConstantValue::get_boolean() const {
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
			sstr << "Semantics::ConstantValue::get_boolean: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_boolean()) {
		std::ostringstream sstr;
		sstr << "Semantics::ConstantValue::get_boolean: constant value has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<bool>(data);
}

const std::string &Semantics::ConstantValue::get_string() const {
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

std::string &&Semantics::ConstantValue::get_string() {
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

	return std::move(std::get<std::string>(data));
}

void Semantics::ConstantValue::set_integer(int32_t integer) {
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

Semantics::IdentifierScope::IdentifierBinding::Static::Static()
	{}

Semantics::IdentifierScope::IdentifierBinding::Static::Static(const ConstantValue &constant_value)
	: constant_value(constant_value)
	{}

Semantics::IdentifierScope::IdentifierBinding::Static::Static(ConstantValue &&constant_value)
	: constant_value(std::move(constant_value))
	{}

Semantics::IdentifierScope::IdentifierBinding::IdentifierBinding()
	{}

Semantics::IdentifierScope::IdentifierBinding::IdentifierBinding(tag_t tag, const data_t &data)
	: tag(tag)
	, data(data)
	{}

Semantics::IdentifierScope::IdentifierBinding::IdentifierBinding(tag_t tag, data_t &&data)
	: tag(tag)
	, data(std::move(data))
	{}

bool Semantics::IdentifierScope::IdentifierBinding::is_static() const {
	switch(tag) {
		case static_tag:
			return true;
		case dynamic_tag:
		case type_tag:
		case var_tag:
		case ref_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::is_static: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::IdentifierScope::IdentifierBinding::is_dynamic() const {
	switch(tag) {
		case static_tag:
			return false;
		case dynamic_tag:
			return true;
		case type_tag:
		case var_tag:
		case ref_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::is_dynamic: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::IdentifierScope::IdentifierBinding::is_type() const {
	switch(tag) {
		case static_tag:
		case dynamic_tag:
			return false;
		case type_tag:
			return true;
		case var_tag:
		case ref_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::is_type: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::IdentifierScope::IdentifierBinding::is_var() const {
	switch(tag) {
		case static_tag:
		case dynamic_tag:
		case type_tag:
			return false;
		case var_tag:
			return true;
		case ref_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::is_var: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::IdentifierScope::IdentifierBinding::is_ref() const {
	switch(tag) {
		case static_tag:
		case dynamic_tag:
		case type_tag:
		case var_tag:
			return false;
		case ref_tag:
			return true;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::is_ref: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

// | The tags must be correct, or else an exception will be thrown, including for set_*.
const Semantics::IdentifierScope::IdentifierBinding::Static &Semantics::IdentifierScope::IdentifierBinding::get_static() const {
	switch(tag) {
		case static_tag:
		case dynamic_tag:
		case type_tag:
		case var_tag:
		case ref_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::get_static: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_static()) {
		std::ostringstream sstr;
		sstr << "Semantics::IdentifierScope::IdentifierBinding::get_static: binding has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<Static>(data);
}

const Semantics::IdentifierScope::IdentifierBinding::Dynamic &Semantics::IdentifierScope::IdentifierBinding::get_dynamic() const {
	switch(tag) {
		case static_tag:
		case dynamic_tag:
		case type_tag:
		case var_tag:
		case ref_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::get_dynamic: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_dynamic()) {
		std::ostringstream sstr;
		sstr << "Semantics::IdentifierScope::IdentifierBinding::get_dynamic: binding has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<Dynamic>(data);
}

const Semantics::IdentifierScope::IdentifierBinding::Type &Semantics::IdentifierScope::IdentifierBinding::get_type() const {
	switch(tag) {
		case static_tag:
		case dynamic_tag:
		case type_tag:
		case var_tag:
		case ref_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::get_type: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_type()) {
		std::ostringstream sstr;
		sstr << "Semantics::IdentifierScope::IdentifierBinding::get_type: binding has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<Type>(data);
}

const Semantics::IdentifierScope::IdentifierBinding::Var &Semantics::IdentifierScope::IdentifierBinding::get_var() const {
	switch(tag) {
		case static_tag:
		case dynamic_tag:
		case type_tag:
		case var_tag:
		case ref_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::get_var: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_var()) {
		std::ostringstream sstr;
		sstr << "Semantics::IdentifierScope::IdentifierBinding::get_var: binding has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<Var>(data);
}

const Semantics::IdentifierScope::IdentifierBinding::Ref &Semantics::IdentifierScope::IdentifierBinding::get_ref() const {
	switch(tag) {
		case static_tag:
		case dynamic_tag:
		case type_tag:
		case var_tag:
		case ref_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::get_ref: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_ref()) {
		std::ostringstream sstr;
		sstr << "Semantics::IdentifierScope::IdentifierBinding::get_ref: binding has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<Ref>(data);
}

Semantics::IdentifierScope::IdentifierBinding::Static &Semantics::IdentifierScope::IdentifierBinding::get_static() {
	switch(tag) {
		case static_tag:
		case dynamic_tag:
		case type_tag:
		case var_tag:
		case ref_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::get_static: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_static()) {
		std::ostringstream sstr;
		sstr << "Semantics::IdentifierScope::IdentifierBinding::get_static: binding has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<Static>(data);
}

Semantics::IdentifierScope::IdentifierBinding::Dynamic &Semantics::IdentifierScope::IdentifierBinding::get_dynamic() {
	switch(tag) {
		case static_tag:
		case dynamic_tag:
		case type_tag:
		case var_tag:
		case ref_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::get_dynamic: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_dynamic()) {
		std::ostringstream sstr;
		sstr << "Semantics::IdentifierScope::IdentifierBinding::get_dynamic: binding has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<Dynamic>(data);
}

Semantics::IdentifierScope::IdentifierBinding::Type &Semantics::IdentifierScope::IdentifierBinding::get_type() {
	switch(tag) {
		case static_tag:
		case dynamic_tag:
		case type_tag:
		case var_tag:
		case ref_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::get_type: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_type()) {
		std::ostringstream sstr;
		sstr << "Semantics::IdentifierScope::IdentifierBinding::get_type: binding has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<Type>(data);
}

Semantics::IdentifierScope::IdentifierBinding::Var &Semantics::IdentifierScope::IdentifierBinding::get_var() {
	switch(tag) {
		case static_tag:
		case dynamic_tag:
		case type_tag:
		case var_tag:
		case ref_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::get_var: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_var()) {
		std::ostringstream sstr;
		sstr << "Semantics::IdentifierScope::IdentifierBinding::get_var: binding has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<Var>(data);
}

Semantics::IdentifierScope::IdentifierBinding::Ref &Semantics::IdentifierScope::IdentifierBinding::get_ref() {
	switch(tag) {
		case static_tag:
		case dynamic_tag:
		case type_tag:
		case var_tag:
		case ref_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::get_ref: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_ref()) {
		std::ostringstream sstr;
		sstr << "Semantics::IdentifierScope::IdentifierBinding::get_ref: binding has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<Ref>(data);
}

// | Return "static", "dynamic", "type", "var", or "ref".
std::string Semantics::IdentifierScope::IdentifierBinding::get_tag_repr(tag_t tag) {
	switch(tag) {
		case static_tag:
			return "static";
		case dynamic_tag:
			return "dynamic";
		case type_tag:
			return "type";
		case var_tag:
			return "var";
		case ref_tag:
			return "ref";

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::get_tag_repr: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

std::string Semantics::IdentifierScope::IdentifierBinding::get_tag_repr() const {
	return get_tag_repr(tag);
}

Semantics::IdentifierScope::IdentifierScope()
	{}

Semantics::IdentifierScope::IdentifierScope(const std::map<std::string, IdentifierBinding> &scope)
	: scope(scope)
	{}

Semantics::IdentifierScope::IdentifierScope(std::map<std::string, IdentifierBinding> &&scope)
	: scope(std::move(scope))
	{}

bool Semantics::IdentifierScope::has(std::string identifier) const {
	std::map<std::string, IdentifierBinding>::const_iterator identifier_binding_search = scope.find(identifier);
	if (identifier_binding_search == scope.cend()) {
		return false;
	} else {
		return true;
	}
}

const Semantics::IdentifierScope::IdentifierBinding &Semantics::IdentifierScope::get(std::string identifier) const {
	std::map<std::string, IdentifierBinding>::const_iterator identifier_binding_search = scope.find(identifier);
	if (identifier_binding_search == scope.cend()) {
		std::ostringstream sstr;
		sstr << "Semantics::IdentifierScope::get: the identifier is missing from scope: " << identifier;
		throw SemanticsError(sstr.str());
	} else {
		return identifier_binding_search->second;
	}
}

Semantics::IdentifierScope::IdentifierBinding &&Semantics::IdentifierScope::get(std::string identifier) {
	std::map<std::string, IdentifierBinding>::iterator identifier_binding_search = scope.find(identifier);
	if (identifier_binding_search == scope.end()) {
		std::ostringstream sstr;
		sstr << "Semantics::IdentifierScope::get: the identifier is missing from scope: " << identifier;
		throw SemanticsError(sstr.str());
	} else {
		return std::move(identifier_binding_search->second);
	}
}

const Semantics::IdentifierScope::IdentifierBinding &Semantics::IdentifierScope::operator[](std::string identifier) const {
	return get(identifier);
}

Semantics::IdentifierScope::IdentifierBinding &&Semantics::IdentifierScope::operator[](std::string identifier) {
	return std::move(get(identifier));
}

std::optional<Semantics::IdentifierScope::IdentifierBinding> Semantics::IdentifierScope::lookup_copy(std::string identifier) const {
	std::map<std::string, IdentifierBinding>::const_iterator identifier_binding_search = scope.find(identifier);
	if (identifier_binding_search == scope.cend()) {
		return std::optional<IdentifierBinding>();
	} else {
		return std::optional<IdentifierBinding>(std::move(IdentifierBinding(std::as_const(identifier_binding_search->second))));
	}
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
		case Expression::pipe_branch: {
			const Expression::Pipe &pipe           = grammar.expression_pipe_storage.at(expression_symbol.data);
			const Expression       &expression0    = grammar.expression_storage.at(pipe.expression0); (void) expression0;
			const LexemeOperator   &pipe_operator0 = grammar.lexemes.at(pipe.pipe_operator0).get_operator();
			const Expression       &expression1    = grammar.expression_storage.at(pipe.expression1); (void) expression1;

			// Is either subexpression dynamic?  If so, this expression is also dynamic.
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
					<< "): refusing to OR values of different types, for "
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
			if        (left.is_integer()) {
				expression_constant_value = ConstantValue(static_cast<int32_t>(static_cast<int32_t>(left.get_integer()) | static_cast<int32_t>(right.get_integer())));
				break;
			} else if (left.is_char()) {
				expression_constant_value = ConstantValue(static_cast<char>(static_cast<char>(left.get_char()) | static_cast<char>(right.get_char())));
				break;
			} else if (left.is_boolean()) {
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
		} case Expression::ampersand_branch: {
			const Expression::Ampersand &ampersand           = grammar.expression_ampersand_storage.at(expression_symbol.data);
			const Expression            &expression0         = grammar.expression_storage.at(ampersand.expression0); (void) expression0;
			const LexemeOperator        &ampersand_operator0 = grammar.lexemes.at(ampersand.ampersand_operator0).get_operator();
			const Expression            &expression1         = grammar.expression_storage.at(ampersand.expression1); (void) expression1;

			// Is either subexpression dynamic?  If so, this expression is also dynamic.
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
					<< "): refusing to AND values of different types, for "
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
			if        (left.is_integer()) {
				expression_constant_value = ConstantValue(static_cast<int32_t>(static_cast<int32_t>(left.get_integer()) & static_cast<int32_t>(right.get_integer())));
				break;
			} else if (left.is_char()) {
				expression_constant_value = ConstantValue(static_cast<char>(static_cast<char>(left.get_char()) & static_cast<char>(right.get_char())));
				break;
			} else if (left.is_boolean()) {
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
		} case Expression::equals_branch: {
			const Expression::Equals &equals           = grammar.expression_equals_storage.at(expression_symbol.data);
			const Expression         &expression0      = grammar.expression_storage.at(equals.expression0); (void) expression0;
			const LexemeOperator     &equals_operator0 = grammar.lexemes.at(equals.equals_operator0).get_operator();
			const Expression         &expression1      = grammar.expression_storage.at(equals.expression1); (void) expression1;

			// Is either subexpression dynamic?  If so, this expression is also dynamic.
			ConstantValue right = is_expression_constant(equals.expression1, expression_scope);
			if (right.is_dynamic()) {
				expression_constant_value = right;
				break;
			}
			ConstantValue left  = is_expression_constant(equals.expression0, expression_scope);
			if (left.is_dynamic()) {
				expression_constant_value = left;
				break;
			}

			// Are the expressions of the same type?
			if (left.tag != right.tag) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< equals_operator0.line << " col " << equals_operator0.column
					<< "): refusing to compare values of different types for =, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Apply = comparison depending on the type.
			if        (left.is_integer()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_integer() == right.get_integer()));
				break;
			} else if (left.is_char()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_char() == right.get_char()));
				break;
			} else if (left.is_boolean()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_boolean() == right.get_boolean()));
				break;
			} else if (left.is_string()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_string() == right.get_string()));
				break;
			} else {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: internal error (line "
					<< equals_operator0.line << " col " << equals_operator0.column
					<< "): unhandled constant expression type for = comparison: "
					<< left.get_tag_repr()
					;
				throw SemanticsError(sstr.str());
			}
		} case Expression::lt_or_gt_branch: {
			const Expression::LtOrGt &lt_or_gt           = grammar.expression_lt_or_gt_storage.at(expression_symbol.data);
			const Expression         &expression0        = grammar.expression_storage.at(lt_or_gt.expression0); (void) expression0;
			const LexemeOperator     &lt_or_gt_operator0 = grammar.lexemes.at(lt_or_gt.lt_or_gt_operator0).get_operator();
			const Expression         &expression1        = grammar.expression_storage.at(lt_or_gt.expression1); (void) expression1;

			// Is either subexpression dynamic?  If so, this expression is also dynamic.
			ConstantValue right = is_expression_constant(lt_or_gt.expression1, expression_scope);
			if (right.is_dynamic()) {
				expression_constant_value = right;
				break;
			}
			ConstantValue left  = is_expression_constant(lt_or_gt.expression0, expression_scope);
			if (left.is_dynamic()) {
				expression_constant_value = left;
				break;
			}

			// Are the expressions of the same type?
			if (left.tag != right.tag) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< lt_or_gt_operator0.line << " col " << lt_or_gt_operator0.column
					<< "): refusing to compare values of different types for <>, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Apply <> comparison depending on the type.
			if        (left.is_integer()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_integer() != right.get_integer()));
				break;
			} else if (left.is_char()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_char() != right.get_char()));
				break;
			} else if (left.is_boolean()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_boolean() != right.get_boolean()));
				break;
			} else if (left.is_string()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_string() != right.get_string()));
				break;
			} else {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: internal error (line "
					<< lt_or_gt_operator0.line << " col " << lt_or_gt_operator0.column
					<< "): unhandled constant expression type for <> comparison: "
					<< left.get_tag_repr()
					;
				throw SemanticsError(sstr.str());
			}
		} case Expression::le_branch: {
			const Expression::Le &le           = grammar.expression_le_storage.at(expression_symbol.data);
			const Expression     &expression0  = grammar.expression_storage.at(le.expression0); (void) expression0;
			const LexemeOperator &le_operator0 = grammar.lexemes.at(le.le_operator0).get_operator();
			const Expression     &expression1  = grammar.expression_storage.at(le.expression1); (void) expression1;

			// Is either subexpression dynamic?  If so, this expression is also dynamic.
			ConstantValue right = is_expression_constant(le.expression1, expression_scope);
			if (right.is_dynamic()) {
				expression_constant_value = right;
				break;
			}
			ConstantValue left  = is_expression_constant(le.expression0, expression_scope);
			if (left.is_dynamic()) {
				expression_constant_value = left;
				break;
			}

			// Are the expressions of the same type?
			if (left.tag != right.tag) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< le_operator0.line << " col " << le_operator0.column
					<< "): refusing to compare values of different types for <=, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Apply <= comparison depending on the type.
			if        (left.is_integer()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_integer() <= right.get_integer()));
				break;
			} else if (left.is_char()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_char() <= right.get_char()));
				break;
			} else if (left.is_boolean()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_boolean() <= right.get_boolean()));
				break;
			} else if (left.is_string()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_string() <= right.get_string()));
				break;
			} else {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: internal error (line "
					<< le_operator0.line << " col " << le_operator0.column
					<< "): unhandled constant expression type for <= comparison: "
					<< left.get_tag_repr()
					;
				throw SemanticsError(sstr.str());
			}
		} case Expression::ge_branch: {
			const Expression::Ge &ge           = grammar.expression_ge_storage.at(expression_symbol.data);
			const Expression     &expression0  = grammar.expression_storage.at(ge.expression0); (void) expression0;
			const LexemeOperator &ge_operator0 = grammar.lexemes.at(ge.ge_operator0).get_operator();
			const Expression     &expression1  = grammar.expression_storage.at(ge.expression1); (void) expression1;

			// Is either subexpression dynamic?  If so, this expression is also dynamic.
			ConstantValue right = is_expression_constant(ge.expression1, expression_scope);
			if (right.is_dynamic()) {
				expression_constant_value = right;
				break;
			}
			ConstantValue left  = is_expression_constant(ge.expression0, expression_scope);
			if (left.is_dynamic()) {
				expression_constant_value = left;
				break;
			}

			// Are the expressions of the same type?
			if (left.tag != right.tag) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< ge_operator0.line << " col " << ge_operator0.column
					<< "): refusing to compare values of different types for >=, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Apply >= comparison depending on the type.
			if        (left.is_integer()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_integer() >= right.get_integer()));
				break;
			} else if (left.is_char()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_char() >= right.get_char()));
				break;
			} else if (left.is_boolean()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_boolean() >= right.get_boolean()));
				break;
			} else if (left.is_string()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_string() >= right.get_string()));
				break;
			} else {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: internal error (line "
					<< ge_operator0.line << " col " << ge_operator0.column
					<< "): unhandled constant expression type for >= comparison: "
					<< left.get_tag_repr()
					;
				throw SemanticsError(sstr.str());
			}
		} case Expression::lt_branch: {
			const Expression::Lt &lt           = grammar.expression_lt_storage.at(expression_symbol.data);
			const Expression     &expression0  = grammar.expression_storage.at(lt.expression0); (void) expression0;
			const LexemeOperator &lt_operator0 = grammar.lexemes.at(lt.lt_operator0).get_operator();
			const Expression     &expression1  = grammar.expression_storage.at(lt.expression1); (void) expression1;

			// Is either subexpression dynamic?  If so, this expression is also dynamic.
			ConstantValue right = is_expression_constant(lt.expression1, expression_scope);
			if (right.is_dynamic()) {
				expression_constant_value = right;
				break;
			}
			ConstantValue left  = is_expression_constant(lt.expression0, expression_scope);
			if (left.is_dynamic()) {
				expression_constant_value = left;
				break;
			}

			// Are the expressions of the same type?
			if (left.tag != right.tag) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< lt_operator0.line << " col " << lt_operator0.column
					<< "): refusing to compare values of different types for <, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Apply < comparison depending on the type.
			if        (left.is_integer()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_integer() < right.get_integer()));
				break;
			} else if (left.is_char()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_char() < right.get_char()));
				break;
			} else if (left.is_boolean()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_boolean() < right.get_boolean()));
				break;
			} else if (left.is_string()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_string() < right.get_string()));
				break;
			} else {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: internal error (line "
					<< lt_operator0.line << " col " << lt_operator0.column
					<< "): unhandled constant expression type for < comparison: "
					<< left.get_tag_repr()
					;
				throw SemanticsError(sstr.str());
			}
		} case Expression::gt_branch: {
			const Expression::Gt &gt           = grammar.expression_gt_storage.at(expression_symbol.data);
			const Expression     &expression0  = grammar.expression_storage.at(gt.expression0); (void) expression0;
			const LexemeOperator &gt_operator0 = grammar.lexemes.at(gt.gt_operator0).get_operator();
			const Expression     &expression1  = grammar.expression_storage.at(gt.expression1); (void) expression1;

			// Is either subexpression dynamic?  If so, this expression is also dynamic.
			ConstantValue right = is_expression_constant(gt.expression1, expression_scope);
			if (right.is_dynamic()) {
				expression_constant_value = right;
				break;
			}
			ConstantValue left  = is_expression_constant(gt.expression0, expression_scope);
			if (left.is_dynamic()) {
				expression_constant_value = left;
				break;
			}

			// Are the expressions of the same type?
			if (left.tag != right.tag) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< gt_operator0.line << " col " << gt_operator0.column
					<< "): refusing to compare values of different types for >, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Apply > comparison depending on the type.
			if        (left.is_integer()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_integer() > right.get_integer()));
				break;
			} else if (left.is_char()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_char() > right.get_char()));
				break;
			} else if (left.is_boolean()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_boolean() > right.get_boolean()));
				break;
			} else if (left.is_string()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_string() > right.get_string()));
				break;
			} else {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: internal error (line "
					<< gt_operator0.line << " col " << gt_operator0.column
					<< "): unhandled constant expression type for > comparison: "
					<< left.get_tag_repr()
					;
				throw SemanticsError(sstr.str());
			}
		} case Expression::plus_branch: {
			const Expression::Plus &plus           = grammar.expression_plus_storage.at(expression_symbol.data);
			const Expression       &expression0    = grammar.expression_storage.at(plus.expression0); (void) expression0;
			const LexemeOperator   &plus_operator0 = grammar.lexemes.at(plus.plus_operator0).get_operator();
			const Expression       &expression1    = grammar.expression_storage.at(plus.expression1); (void) expression1;

			// Is either subexpression dynamic?  If so, this expression is also dynamic.
			ConstantValue right = is_expression_constant(plus.expression1, expression_scope);
			if (right.is_dynamic()) {
				expression_constant_value = right;
				break;
			}
			ConstantValue left  = is_expression_constant(plus.expression0, expression_scope);
			if (left.is_dynamic()) {
				expression_constant_value = left;
				break;
			}

			// Are the expressions of the same type?
			if (left.tag != right.tag) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< plus_operator0.line << " col " << plus_operator0.column
					<< "): refusing to add different types, for "
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
					<< plus_operator0.line << " col " << plus_operator0.column
					<< "): cannot apply addition on a string expression, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Are we attempting to operate on a non-integer?
			if (left.is_char() || left.is_boolean() || right.is_char() || right.is_boolean()) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< plus_operator0.line << " col " << plus_operator0.column
					<< "): refusing to apply addition on a non-integer, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Apply addition depending on the integer type.
			if (left.is_integer()) {
				// Detect overflow in constant expression.
				if (would_addition_overflow(left.get_integer(), right.get_integer())) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::is_expression_constant: error (line "
						<< plus_operator0.line << " col " << plus_operator0.column
						<< "): addition would result in an overflow, for "
						<< left.get_integer() << " + " << right.get_integer()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				expression_constant_value = ConstantValue(static_cast<int32_t>(static_cast<int32_t>(left.get_integer()) + static_cast<int32_t>(right.get_integer())));
				break;
			} else {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: internal error (line "
					<< plus_operator0.line << " col " << plus_operator0.column
					<< "): unhandled constant expression type for addition: "
					<< left.get_tag_repr()
					;
				throw SemanticsError(sstr.str());
			}
		} case Expression::minus_branch: {
			const Expression::Minus &minus           = grammar.expression_minus_storage.at(expression_symbol.data);
			const Expression        &expression0     = grammar.expression_storage.at(minus.expression0); (void) expression0;
			const LexemeOperator    &minus_operator0 = grammar.lexemes.at(minus.minus_operator0).get_operator();
			const Expression        &expression1     = grammar.expression_storage.at(minus.expression1); (void) expression1;

			// Is either subexpression dynamic?  If so, this expression is also dynamic.
			ConstantValue right = is_expression_constant(minus.expression1, expression_scope);
			if (right.is_dynamic()) {
				expression_constant_value = right;
				break;
			}
			ConstantValue left  = is_expression_constant(minus.expression0, expression_scope);
			if (left.is_dynamic()) {
				expression_constant_value = left;
				break;
			}

			// Are the expressions of the same type?
			if (left.tag != right.tag) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< minus_operator0.line << " col " << minus_operator0.column
					<< "): refusing to substract values of different types, for "
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
					<< minus_operator0.line << " col " << minus_operator0.column
					<< "): cannot apply subtraction on a string expression, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Are we attempting to operate on a non-integer?
			if (left.is_char() || left.is_boolean() || right.is_char() || right.is_boolean()) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< minus_operator0.line << " col " << minus_operator0.column
					<< "): refusing to apply subtraction on a non-integer, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Apply subtraction depending on the integer type.
			if (left.is_integer()) {
				// Detect overflow in constant expression.
				if (would_addition_overflow(left.get_integer(), -right.get_integer())) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::is_expression_constant: error (line "
						<< minus_operator0.line << " col " << minus_operator0.column
						<< "): subtraction would result in an overflow, for "
						<< left.get_integer() << " - " << right.get_integer()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				expression_constant_value = ConstantValue(static_cast<int32_t>(static_cast<int32_t>(left.get_integer()) - static_cast<int32_t>(right.get_integer())));
				break;
			} else {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: internal error (line "
					<< minus_operator0.line << " col " << minus_operator0.column
					<< "): unhandled constant expression type for subtraction: "
					<< left.get_tag_repr()
					;
				throw SemanticsError(sstr.str());
			}
		} case Expression::times_branch: {
			const Expression::Times &times           = grammar.expression_times_storage.at(expression_symbol.data);
			const Expression        &expression0     = grammar.expression_storage.at(times.expression0); (void) expression0;
			const LexemeOperator    &times_operator0 = grammar.lexemes.at(times.times_operator0).get_operator();
			const Expression        &expression1     = grammar.expression_storage.at(times.expression1); (void) expression1;

			// Is either subexpression dynamic?  If so, this expression is also dynamic.
			ConstantValue right = is_expression_constant(times.expression1, expression_scope);
			if (right.is_dynamic()) {
				expression_constant_value = right;
				break;
			}
			ConstantValue left  = is_expression_constant(times.expression0, expression_scope);
			if (left.is_dynamic()) {
				expression_constant_value = left;
				break;
			}

			// Are the expressions of the same type?
			if (left.tag != right.tag) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< times_operator0.line << " col " << times_operator0.column
					<< "): refusing to multiply values of different types, for "
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
					<< times_operator0.line << " col " << times_operator0.column
					<< "): cannot apply multiplication on a string expression, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Are we attempting to operate on a non-integer?
			if (left.is_char() || left.is_boolean() || right.is_char() || right.is_boolean()) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< times_operator0.line << " col " << times_operator0.column
					<< "): refusing to apply multiplication on a non-integer, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Apply multiplication depending on the integer type.
			if (left.is_integer()) {
				// Detect overflow in constant expression.
				if (would_multiplication_overflow(left.get_integer(), right.get_integer())) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::is_expression_constant: error (line "
						<< times_operator0.line << " col " << times_operator0.column
						<< "): multiplication would result in an overflow, for "
						<< left.get_integer() << " * " << right.get_integer()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				expression_constant_value = ConstantValue(static_cast<int32_t>(static_cast<int32_t>(left.get_integer()) * static_cast<int32_t>(right.get_integer())));
				break;
			} else {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: internal error (line "
					<< times_operator0.line << " col " << times_operator0.column
					<< "): unhandled constant expression type for multiplication: "
					<< left.get_tag_repr()
					;
				throw SemanticsError(sstr.str());
			}
		} case Expression::slash_branch: {
			const Expression::Slash &slash           = grammar.expression_slash_storage.at(expression_symbol.data);
			const Expression        &expression0     = grammar.expression_storage.at(slash.expression0); (void) expression0;
			const LexemeOperator    &slash_operator0 = grammar.lexemes.at(slash.slash_operator0).get_operator();
			const Expression        &expression1     = grammar.expression_storage.at(slash.expression1); (void) expression1;

			// Is either subexpression dynamic?  If so, this expression is also dynamic.
			ConstantValue right = is_expression_constant(slash.expression1, expression_scope);
			if (right.is_dynamic()) {
				expression_constant_value = right;
				break;
			}
			ConstantValue left  = is_expression_constant(slash.expression0, expression_scope);
			if (left.is_dynamic()) {
				expression_constant_value = left;
				break;
			}

			// Are the expressions of the same type?
			if (left.tag != right.tag) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< slash_operator0.line << " col " << slash_operator0.column
					<< "): refusing to divide values of different types, for "
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
					<< slash_operator0.line << " col " << slash_operator0.column
					<< "): cannot apply division on a string expression, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Are we attempting to operate on a non-integer?
			if (left.is_char() || left.is_boolean() || right.is_char() || right.is_boolean()) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< slash_operator0.line << " col " << slash_operator0.column
					<< "): refusing to apply division on a non-integer, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Apply division depending on the integer type.
			if (left.is_integer()) {
				// Detect overflow in constant expression.
				if (would_division_overflow(left.get_integer(), right.get_integer())) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::is_expression_constant: error (line "
						<< slash_operator0.line << " col " << slash_operator0.column
						<< "): division would result in an overflow, for "
						<< left.get_integer() << " / " << right.get_integer()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Detect division by zero in constant expression.
				if (right.get_integer() == 0) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::is_expression_constant: error (line "
						<< slash_operator0.line << " col " << slash_operator0.column
						<< "): division by zero, for "
						<< left.get_integer() << " / " << right.get_integer()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Of three standard division/mod algorithms:
				// - QuotRem: C-style division and mod that truncates toward 0.
				// - DivMod: Division truncates toward negative infinite.
				// - Euclidian division: the remainder is non-negative.
				// I prefer Euclidian division, even though this deviates from
				// traditional division and modding in C.  So I'll use it.
				//expression_constant_value = ConstantValue(static_cast<int32_t>(static_cast<int32_t>(left.get_integer()) / static_cast<int32_t>(right.get_integer())));
				expression_constant_value = ConstantValue(static_cast<int32_t>(euclidian_div(static_cast<int32_t>(left.get_integer()), static_cast<int32_t>(right.get_integer()))));
				break;
			} else {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: internal error (line "
					<< slash_operator0.line << " col " << slash_operator0.column
					<< "): unhandled constant expression type for division: "
					<< left.get_tag_repr()
					;
				throw SemanticsError(sstr.str());
			}
		} case Expression::percent_branch: {
			const Expression::Percent &percent           = grammar.expression_percent_storage.at(expression_symbol.data);
			const Expression          &expression0       = grammar.expression_storage.at(percent.expression0); (void) expression0;
			const LexemeOperator      &percent_operator0 = grammar.lexemes.at(percent.percent_operator0).get_operator();
			const Expression          &expression1       = grammar.expression_storage.at(percent.expression1); (void) expression1;

			// Is either subexpression dynamic?  If so, this expression is also dynamic.
			ConstantValue right = is_expression_constant(percent.expression1, expression_scope);
			if (right.is_dynamic()) {
				expression_constant_value = right;
				break;
			}
			ConstantValue left  = is_expression_constant(percent.expression0, expression_scope);
			if (left.is_dynamic()) {
				expression_constant_value = left;
				break;
			}

			// Are the expressions of the same type?
			if (left.tag != right.tag) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< percent_operator0.line << " col " << percent_operator0.column
					<< "): refusing to mod values of different types, for "
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
					<< percent_operator0.line << " col " << percent_operator0.column
					<< "): cannot apply mod on a string expression, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Are we attempting to operate on a non-integer?
			if (left.is_char() || left.is_boolean() || right.is_char() || right.is_boolean()) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< percent_operator0.line << " col " << percent_operator0.column
					<< "): refusing to apply mod on a non-integer, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Apply mod depending on the integer type.
			if (left.is_integer()) {
				// Detect division by zero in constant expression.
				if (right.get_integer() == 0) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::is_expression_constant: error (line "
						<< percent_operator0.line << " col " << percent_operator0.column
						<< "): division by zero, for "
						<< left.get_integer() << " % " << right.get_integer()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				//expression_constant_value = ConstantValue(static_cast<int32_t>(static_cast<int32_t>(left.get_integer()) % static_cast<int32_t>(right.get_integer())));
				expression_constant_value = ConstantValue(static_cast<int32_t>(euclidian_mod(static_cast<int32_t>(left.get_integer()), static_cast<int32_t>(right.get_integer()))));
				break;
			} else {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: internal error (line "
					<< percent_operator0.line << " col " << percent_operator0.column
					<< "): unhandled constant expression type for mod: "
					<< left.get_tag_repr()
					;
				throw SemanticsError(sstr.str());
			}
		} case Expression::tilde_branch: {
			const Expression::Tilde &tilde           = grammar.expression_tilde_storage.at(expression_symbol.data);
			const Expression        &expression0     = grammar.expression_storage.at(tilde.expression); (void) expression0;
			const LexemeOperator    &tilde_operator0 = grammar.lexemes.at(tilde.tilde_operator0).get_operator();

			// Is the subexpression dynamic?  If so, this expression is also dynamic.
			ConstantValue value = is_expression_constant(tilde.expression, expression_scope);
			if (value.is_dynamic()) {
				expression_constant_value = value;
				break;
			}

			// Are we attempting to operate on a string?
			if (value.is_string()) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< tilde_operator0.line << " col " << tilde_operator0.column
					<< "): cannot apply bitwise NOT on a string expression."
					;
				throw SemanticsError(sstr.str());
			}

			// Apply bitwise NOT depending on the integer type.
			if        (value.is_integer()) {
				expression_constant_value = ConstantValue(static_cast<int32_t>(~static_cast<int32_t>(value.get_integer())));
				break;
			} else if (value.is_char()) {
				expression_constant_value = ConstantValue(static_cast<char>(~static_cast<char>(value.get_integer())));
				break;
			} else if (value.is_boolean()) {
				expression_constant_value = ConstantValue(static_cast<bool>(~static_cast<bool>(value.get_integer())));
				break;
			} else {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: internal error (line "
					<< tilde_operator0.line << " col " << tilde_operator0.column
					<< "): unhandled constant expression type for bitwise NOT: "
					<< value.get_tag_repr()
					;
				throw SemanticsError(sstr.str());
			}
		} case Expression::unary_minus_branch: {
			const Expression::UnaryMinus &unary_minus     = grammar.expression_unary_minus_storage.at(expression_symbol.data);
			const Expression             &expression0     = grammar.expression_storage.at(unary_minus.expression); (void) expression0;
			const LexemeOperator         &minus_operator0 = grammar.lexemes.at(unary_minus.minus_operator0).get_operator();

			// Is the subexpression dynamic?  If so, this expression is also dynamic.
			ConstantValue value = is_expression_constant(unary_minus.expression, expression_scope);
			if (value.is_dynamic()) {
				expression_constant_value = value;
				break;
			}

			// Are we attempting to operate on a string?
			if (value.is_string()) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< minus_operator0.line << " col " << minus_operator0.column
					<< "): cannot apply unary minus on a string expression."
					;
				throw SemanticsError(sstr.str());
			}

			// Are we attempting to operate on a non-integer?
			if (value.is_char() || value.is_boolean()) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< minus_operator0.line << " col " << minus_operator0.column
					<< "): refusing to apply unary minus on a non-integer, for "
					<< value.get_tag_repr() << "."
					;
				throw SemanticsError(sstr.str());
			}

			// Apply unary minus depending on the integer type.
			if (value.is_integer()) {
				// Detect overflow in constant expression.
				if (would_multiplication_overflow(-1, value.get_integer())) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::is_expression_constant: error (line "
						<< minus_operator0.line << " col " << minus_operator0.column
						<< "): unary minus would result in an overflow, for "
						<< value.get_integer() << "."
						;
					throw SemanticsError(sstr.str());
				}

				expression_constant_value = ConstantValue(static_cast<int32_t>(-static_cast<int32_t>(value.get_integer())));
				break;
			} else {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: internal error (line "
					<< minus_operator0.line << " col " << minus_operator0.column
					<< "): unhandled constant expression type for unary minus: "
					<< value.get_tag_repr()
					;
				throw SemanticsError(sstr.str());
			}
		} case Expression::parentheses_branch: {
			const Expression::Parentheses &parentheses                = grammar.expression_parentheses_storage.at(expression_symbol.data);
			const LexemeOperator          &leftparenthesis_operator0  = grammar.lexemes.at(parentheses.leftparenthesis_operator0).get_operator(); (void) leftparenthesis_operator0;
			const Expression              &expression0                = grammar.expression_storage.at(parentheses.expression); (void) expression0;
			const LexemeOperator          &rightparenthesis_operator0 = grammar.lexemes.at(parentheses.rightparenthesis_operator0).get_operator(); (void) rightparenthesis_operator0;

			// Is the subexpression dynamic?  If so, this expression is also dynamic.
			ConstantValue value = is_expression_constant(parentheses.expression, expression_scope);
			if (value.is_dynamic()) {
				expression_constant_value = value;
				break;
			}

			// The constant value of this expression is equivalent to the
			// constant value of the subexpression.  Copy the subexpression's
			// constant value.
			expression_constant_value = std::move(ConstantValue(value));
			break;
		}

		// These 5 branches are dynamic.
		case Expression::call_branch:
		case Expression::chr_branch:
		case Expression::ord_branch:
		case Expression::pred_branch:
		case Expression::succ_branch:
			expression_constant_value = ConstantValue::dynamic;
			break;

		// lvalue_branch may or may not be dynamic, depending on whether it is
		// a simple identifier (without .foo or [bar]) that refers to a known
		// constant value in the scope of the expression.
		case Expression::lvalue_branch: {
			const Expression::Lvalue       &lvalue                      = grammar.expression_lvalue_storage.at(expression_symbol.data);
			const Lvalue                   &lvalue_symbol               = grammar.lvalue_storage.at(lvalue.lvalue);
			const LexemeIdentifier         &lexeme_identifier           = grammar.lexemes.at(lvalue_symbol.identifier).get_identifier();
			const LvalueAccessorClauseList &lvalue_accessor_clause_list = grammar.lvalue_accessor_clause_list_storage.at(lvalue_symbol.lvalue_accessor_clause_list);

			// According to the documentation, only lvalues without
			// accessors can be constant (static) expressions.  So check the
			// lvalue type, to see if it's just an identifier.
			if (lvalue_accessor_clause_list.branch != LvalueAccessorClauseList::empty_branch) {
				// It's not just an identifier.  This lvalue expression is not
				// a constant expression.
				expression_constant_value = ConstantValue::dynamic;
				break;
			}

			// Lookup the identifier binding.
			std::optional<IdentifierScope::IdentifierBinding> identifier_binding_search = expression_scope.lookup_copy(lexeme_identifier.text);
			if (!identifier_binding_search) {
				std::ostringstream sstr;
				sstr << "Semantics::is_expression_constant: error (line " << lexeme_identifier.line << " col " << lexeme_identifier.column << "): identifier out of scope when checking for constant lvalue: " << lexeme_identifier.text;
				throw SemanticsError(sstr.str());
			}

			if (!identifier_binding_search->is_static()) {
				// The identifier does not refer to a constant expression.
				expression_constant_value = ConstantValue::dynamic;
				break;
			} else {  // identifier_binding_search->is_static()
				const IdentifierScope::IdentifierBinding::Static &static_ = identifier_binding_search->get_static();
				// Copy the constant value.
				expression_constant_value = std::move(ConstantValue(static_.constant_value));
				break;
			}
		}

		// These 3 branches are static.
		case Expression::integer_branch: {
			const Expression::Integer &integer        = grammar.expression_integer_storage.at(expression_symbol.data);
			const LexemeInteger       &lexeme_integer = grammar.lexemes.at(integer.integer).get_integer();
			if (lexeme_integer.first_digits > std::numeric_limits<int32_t>::max() || lexeme_integer.remaining_digits.size() > 0) {
				std::ostringstream sstr;
				sstr << "Semantics::is_expression_constant: error (line " << lexeme_integer.line << " col " << lexeme_integer.column << "): integer is too large to encode in 32 bits: " << lexeme_integer.text;
				throw SemanticsError(sstr.str());
			}
			expression_constant_value = ConstantValue(static_cast<int32_t>(lexeme_integer.first_digits));
			break;
		} case Expression::char__branch: {
			const Expression::Char_ &char_       = grammar.expression_char__storage.at(expression_symbol.data);
			const LexemeChar        &lexeme_char = grammar.lexemes.at(char_.char_).get_char();
			expression_constant_value = ConstantValue(static_cast<char>(lexeme_char.char_));
			break;
		} case Expression::string_branch: {
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

bool Semantics::would_addition_overflow(int32_t a, int32_t b) {
	int32_t smaller, larger;
	if (a <= b) {
		smaller = a;
		larger  = b;
	} else {
		smaller = b;
		larger  = a;
	}

	if (smaller >= 0) {
		// positive + positive
		return std::numeric_limits<int32_t>::max() - smaller > larger;
	} else if (larger < 0) {
		// negative + negative
		return std::numeric_limits<int32_t>::min() - larger < smaller;
	} else {
		// positive + negative or negative + positive
		return false;
	}
}

bool Semantics::would_multiplication_overflow(int32_t a, int32_t b) {
	// Handle cases where the result does not increase in magnitude, while
	// preventing division by zero and enabling the next check's validity.
	if (a == 0 || b == 0) {
		return false;
	}
	if (a == 1 || b == 1) {
		return false;
	}
	if (a == -1 || b == -1) {
		return a != std::numeric_limits<int32_t>::min() && b != std::numeric_limits<int32_t>::min();
	}

	// Since the result is increasing in magnitude, fail at the greatest
	// possible magnitude, while also protecting us from overflows occurring
	// during our checks themselves.
	if (a == std::numeric_limits<int32_t>::min() || b == std::numeric_limits<int32_t>::min()) {
		return true;
	}

	// Get sign of result and absolute values of inputs.
	bool result_nat = (a >= 0) == (b >= 0);
	int32_t a_abs = a >= 0 ? a : -a;
	int32_t b_abs = b >= 0 ? b : -b;

	// Check.
	if (result_nat) {
		// Result is zero or positive (by now, only positive).
		return (std::numeric_limits<int32_t>::max()-1)/a_abs >= b_abs;
	} else {
		// Result is zero or negative (by now, only negative).
		assert(-(std::numeric_limits<int32_t>::min()+1) == std::numeric_limits<int32_t>::max());
		return std::numeric_limits<int32_t>::max()/a_abs >= b_abs;
	}
}

bool Semantics::would_division_overflow(int32_t a, int32_t b) {
	if        (a == -1 && b == std::numeric_limits<int32_t>::min()) {
		return true;
	} else if (b == -1 && a == std::numeric_limits<int32_t>::min()) {
		return true;
	} else {
		return false;
	}
}

int32_t Semantics::euclidian_div(int32_t a, int32_t b) {
	int32_t quotient  = a / b;
	int32_t remainder = a % b;
	if (remainder < 0) {
		return quotient + 1;
	} else {
		return quotient;
	}
}

int32_t Semantics::euclidian_mod(int32_t a, int32_t b) {
	int32_t remainder = a % b;
	if (remainder < 0) {
		return remainder + b;
	} else {
		return remainder;
	}
}

// | Clear memoization caches and calculated output values.
void Semantics::clear_output() {
	is_expression_constant_calculations.clear();
}

// | Force a re-analysis of the semantics data.
void Semantics::analyze() {
	// It's possible the grammar was reset.  Clear caches and outputs just in
	// case.
	clear_output();

	// If there are no parsed start symbols, then the grammar probably has not
	// yet been set up.  Just return, in this case.  The Semantics value might
	// remain null.
	if (grammar.start_storage.size() <= 0) {
		return;
	}

	// Make sure there is a single start symbol.  Get it.
	if (grammar.start_storage.size() != 1) {
		std::ostringstream sstr;
		sstr << "Semantics::analyze: internal error: the number of parsed start symbols should be exactly 1 but isn't: " << grammar.start_storage.size() << ".";
		throw SemanticsError(sstr.str());
	}
	const Start &start = grammar.start_storage[0];

	// Unpack the program symbol.
	const Program                         &program                              = grammar.program_storage.at(start.program);
	const ConstantDeclOpt                 &constant_decl_opt                    = grammar.constant_decl_opt_storage.at(program.constant_decl_opt);
	const TypeDeclOpt                     &type_decl_opt                        = grammar.type_decl_opt_storage.at(program.type_decl_opt);
	const VarDeclOpt                      &var_decl_opt                         = grammar.var_decl_opt_storage.at(program.var_decl_opt);
	const ProcedureDeclOrFunctionDeclList &procedure_decl_or_function_decl_list = grammar.procedure_decl_or_function_decl_list_storage.at(program.procedure_decl_or_function_decl_list);
	const Block                           &block                                = grammar.block_storage.at(program.block);
	const LexemeOperator                  &dot_operator0                        = grammar.lexemes.at(program.dot_operator0).get_operator(); (void) dot_operator0;

	// First, parse top-level constants.  These are at the beginning of the
	// parsed program grammar tree.
	switch (constant_decl_opt.branch) {
		case ConstantDeclOpt::empty_branch: {
			// No constant declarations.  Nothing to do here.
			break;
		}

		case ConstantDeclOpt::value_branch: {
			// TODO
			break;
		}

		// Unrecognized branch.
		default: {
			std::ostringstream sstr;
			sstr << "Semantics::analyze: internal error: invalid constant_decl_opt branch at index " << program.constant_decl_opt << ": " << constant_decl_opt.branch;
			throw SemanticsError(sstr.str());
		}
	}

	// TODO
}
