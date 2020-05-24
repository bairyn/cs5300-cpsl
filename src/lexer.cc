#include "lexer.hh"

#include <map>      // std::map
#include <sstream>  // std::ostringstream
#include <string>   // std::string

LexerError::LexerError()
	: runtime_error("A lexer error occurred.")
	{}

LexerError::LexerError(std::string message)
	: runtime_error(message)
	{}

Lexeme::Lexeme(lexeme_tag_t tag, const lexeme_data_t &data)
	: tag(tag) {
	switch (tag) {
		case identifier_tag:
			new (&this->data.identifier_lexeme) IdentifierLexeme(data.IdentifierLexeme);
			break;

		case operator_tag:
			new (&this->data.operator_lexeme) OperatorLexeme(data.OperatorLexeme);
			break;

		case integer_tag:
			new (&this->data.integer_lexeme) IntegerLexeme(data.IntegerLexeme);
			break;

		case char_tag:
			new (&this->data.char_lexeme) CharLexeme(data.CharLexeme);
			break;

		case string_tag:
			new (&this->data.string_lexeme) StringLexeme(data.StringLexeme);
			break;

		case null_lexeme_tag:
		default:
			std::ostringstream sstr;
			sstr << "Lexeme::Lexeme: invalid tag: " << tag << ".";
			throw LexerException(sstr.str());
			break;
	}
}

Lexeme::Lexeme(lexeme_tag_t tag, lexeme_data_t &&data)
	: tag(tag) {
	switch (tag) {
		case identifier_tag:
			new (&this->data.identifier_lexeme) IdentifierLexeme(data.IdentifierLexeme);
			break;

		case operator_tag:
			new (&this->data.operator_lexeme) OperatorLexeme(data.OperatorLexeme);
			break;

		case integer_tag:
			new (&this->data.integer_lexeme) IntegerLexeme(data.IntegerLexeme);
			break;

		case char_tag:
			new (&this->data.char_lexeme) CharLexeme(data.CharLexeme);
			break;

		case string_tag:
			new (&this->data.string_lexeme) StringLexeme(data.StringLexeme);
			break;

		case null_lexeme_tag:
		default:
			std::ostringstream sstr;
			sstr << "Lexeme::Lexeme: invalid tag: " << tag << ".";
			throw LexerException(sstr.str());
			break;
	}
}

Lexeme::~Lexeme()
	: tag(tag) {
	switch (tag) {
		case identifier_tag:
			new (&this->data.identifier_lexeme) IdentifierLexeme(data.IdentifierLexeme);
			break;

		case operator_tag:
			new (&this->data.operator_lexeme) OperatorLexeme(data.OperatorLexeme);
			break;

		case integer_tag:
			new (&this->data.integer_lexeme) IntegerLexeme(data.IntegerLexeme);
			break;

		case char_tag:
			new (&this->data.char_lexeme) CharLexeme(data.CharLexeme);
			break;

		case string_tag:
			new (&this->data.string_lexeme) StringLexeme(data.StringLexeme);
			break;

		case null_lexeme_tag:
		default:
			std::ostringstream sstr;
			sstr << "Lexeme::~Lexeme: invalid tag: " << tag << ".";
			throw LexerException(sstr.str());
			break;
	}
}

Lexeme::Lexeme(const Lexeme &val)
	: tag(val.tag) {
	switch (tag) {
		case identifier_tag:
			new (&this->data.identifier_lexeme) IdentifierLexeme(val.data.IdentifierLexeme);
			break;

		case operator_tag:
			new (&this->data.operator_lexeme) OperatorLexeme(val.data.OperatorLexeme);
			break;

		case integer_tag:
			new (&this->data.integer_lexeme) IntegerLexeme(val.data.IntegerLexeme);
			break;

		case char_tag:
			new (&this->data.char_lexeme) CharLexeme(val.data.CharLexeme);
			break;

		case string_tag:
			new (&this->data.string_lexeme) StringLexeme(val.data.StringLexeme);
			break;

		case null_lexeme_tag:
		default:
			std::ostringstream sstr;
			sstr << "Lexeme::Lexeme: invalid tag: " << tag << ".";
			throw LexerException(sstr.str());
			break;
	}
}

Lexeme::Lexeme &operator=(const Lexeme &val)
	: tag(val.tag) {
	switch (tag) {
		case identifier_tag:
			new (&this->data.identifier_lexeme) IdentifierLexeme(val.data.IdentifierLexeme);
			break;

		case operator_tag:
			new (&this->data.operator_lexeme) OperatorLexeme(val.data.OperatorLexeme);
			break;

		case integer_tag:
			new (&this->data.integer_lexeme) IntegerLexeme(val.data.IntegerLexeme);
			break;

		case char_tag:
			new (&this->data.char_lexeme) CharLexeme(val.data.CharLexeme);
			break;

		case string_tag:
			new (&this->data.string_lexeme) StringLexeme(val.data.StringLexeme);
			break;

		case null_lexeme_tag:
		default:
			std::ostringstream sstr;
			sstr << "Lexeme::operator=: invalid tag: " << tag << ".";
			throw LexerException(sstr.str());
			break;
	}
}

Lexeme::Lexeme(Lexeme &&val)
	: tag(val.tag) {
	switch (tag) {
		case identifier_tag:
			new (&this->data.identifier_lexeme) IdentifierLexeme(std::move(val.data.IdentifierLexeme));
			break;

		case operator_tag:
			new (&this->data.operator_lexeme) OperatorLexeme(std::move(val.data.OperatorLexeme));
			break;

		case integer_tag:
			new (&this->data.integer_lexeme) IntegerLexeme(std::move(val.data.IntegerLexeme));
			break;

		case char_tag:
			new (&this->data.char_lexeme) CharLexeme(std::move(val.data.CharLexeme));
			break;

		case string_tag:
			new (&this->data.string_lexeme) StringLexeme(std::move(val.data.StringLexeme));
			break;

		case null_lexeme_tag:
		default:
			std::ostringstream sstr;
			sstr << "Lexeme::Lexeme: invalid tag: " << tag << ".";
			throw LexerException(sstr.str());
			break;
	}
}

Lexeme::Lexeme &operator=(Lexeme &&val)
	: tag(val.tag) {
	switch (tag) {
		case identifier_tag:
			new (&this->data.identifier_lexeme) IdentifierLexeme(std::move(val.data.IdentifierLexeme));
			break;

		case operator_tag:
			new (&this->data.operator_lexeme) OperatorLexeme(std::move(val.data.OperatorLexeme));
			break;

		case integer_tag:
			new (&this->data.integer_lexeme) IntegerLexeme(std::move(val.data.IntegerLexeme));
			break;

		case char_tag:
			new (&this->data.char_lexeme) CharLexeme(std::move(val.data.CharLexeme));
			break;

		case string_tag:
			new (&this->data.string_lexeme) StringLexeme(std::move(val.data.StringLexeme));
			break;

		case null_lexeme_tag:
		default:
			std::ostringstream sstr;
			sstr << "Lexeme::operator=: invalid tag: " << tag << ".";
			throw LexerException(sstr.str());
			break;
	}
}

LexemeBase::LexemeBase()
	{}

LexemeBase::LexemeBase(uint64_t line, uint64_t column, std::string text)
	: line(line)
	, column(column)
	, text(text)
	{}

LexemeIdentifier::LexemeIdentifier()
	{}

LexemeIdentifier::LexemeIdentifier(identifier_t identifier)
	: identifier(identifier)
	{}

// | Construct an identifier by text, using "null_identifier" on an
// unrecognized identifier.
LexemeIdentifier::LexemeIdentifier(std::string text)
	: identifier(text)
	{}

static const std::map<std::string, identifier_t> LexemeIdentifier::identifier_map {
	{"array",     array_identifier},
	{"begin",     begin_identifier},
	{"chr",       chr_identifier},
	{"const",     const_identifier},
	{"do",        do_identifier},
	{"downto",    downto_identifier},
	{"else",      else_identifier},
	{"elseif",    elseif_identifier},
	{"end",       end_identifier},
	{"for",       for_identifier},
	{"forward",   forward_identifier},
	{"function",  function_identifier},
	{"if",        if_identifier},
	{"of",        of_identifier},
	{"ord",       ord_identifier},
	{"pred",      pred_identifier},
	{"procedure", procedure_identifier},
	{"read",      read_identifier},
	{"record",    record_identifier},
	{"ref",       ref_identifier},
	{"repeat",    repeat_identifier},
	{"return",    return_identifier},
	{"stop",      stop_identifier},
	{"succ",      succ_identifier},
	{"then",      then_identifier},
	{"to",        to_identifier},
	{"type",      type_identifier},
	{"until",     until_identifier},
	{"var",       var_identifier},
	{"while",     while_identifier},
};
static identifier_t LexemeIdentifier::get_identifier(std::string text) {
	std::map<std::string, identifier_t>::const_iterator search = identifier_map.find(text);
	if (search == identifier_map.cend()) {
		// No match found.
		if (false) {
			// Just return "null_identifier".
			return null_identifier;
		} else {
			std::ostringstream sstr;
			sstr << "LexemeIdentifier::get_identifier: unrecognized identifier: " << text << "";
			throw LexerException(sstr.str());
		}
	} else {
		// Match found; return the value.
		return search->second;
	}
}

LexemeOperator::LexemeOperator()
	{}

LexemeOperator::LexemeOperator(operator_t operator)
	: operator(operator)
	{}

// | Construct an operator by text, using "null_operator" on an
// unrecognized operator.
LexemeOperator::LexemeOperator(std::string text)
	: operator(text)
	{}

static const std::map<std::string, operator_t> LexemeOperator::operator_map {
	{"+", plus_operator},
	{"-", minus_operator},
	{"*", times_operator},
	{"/", slash_operator},
	{"&", ampersand_operator},
	{"|", slash_operator},
	{"~", tilde_operator},
	{"=", equals_operator},
	{"<>", lt_or_gt_operator},
	{"<", lt_operator},
	{"<=", le_operator},
	{">", gt_operator},
	{">=", ge_operator},
	{".", dot_operator},
	{",", comma_operator},
	{":", colon_operator},
	{";", semicolon_operator},
	{"(", leftparenthesis_operator},
	{")", rightparenthesis_operator},
	{"[", leftbracket_operator},
	{"]", rightbracket_operator},
	{":", colonequals_operator},
	{"%", percent_operator},
};
static operator_t LexemeOperator::get_operator(std::string text) {
	std::map<std::string, operator_t>::const_iterator search = operator_map.find(text);
	if (search == operator_map.cend()) {
		// No match found.
		if (false) {
			// Just return "null_operator".
			return null_operator;
		} else {
			std::ostringstream sstr;
			sstr << "LexemeOperator::get_operator: unrecognized identifier: " << text << "";
			throw LexerException(sstr.str());
		}
	} else {
		// Match found; return the value.
		return search->second;
	}
}
