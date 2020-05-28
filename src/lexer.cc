#include <algorithm>  // std::all_of, std::count
#include <cctype>     // std::isupper, std::tolower
#include <cstdint>    // uint8_t, uint64_t
#include <iterator>   // std::back_inserter
#include <map>        // std::map
#include <sstream>    // std::ostringstream
#include <stdexcept>  // std::runtime_error
#include <string>     // std::string
#include <utility>    // std::as_const, std::pair
#include <variant>    // std::bad_variant_access, std::get, std::monostate
#include <vector>     // std::vector

#include "lexer.hh"

/*
 * Exception types.
 */

LexerError::LexerError()
	: runtime_error("A lexer error occurred.")
	{}

LexerError::LexerError(std::string message)
	: runtime_error(std::move(message))
	{}

/*
 * Lexeme types.
 */

LexemeBase::LexemeBase(uint64_t line, uint64_t column, const std::string &text)
	: line(line)
	, column(column)
	, text(text)
	{}

LexemeBase::LexemeBase(uint64_t line, uint64_t column, std::string &&text)
	: line(line)
	, column(column)
	, text(std::move(text))
	{}

// | Calculate the end line and column value for the next lexeme.
LexemeBase::LexemeBase(const LexemeBase &previous_lexeme_base, const std::string &text)
	: line(previous_lexeme_base.get_line_end())
	, column(previous_lexeme_base.get_column_end())
	, text(text)
	{}

LexemeBase::LexemeBase(const LexemeBase &previous_lexeme_base, std::string &&text)
	: line(previous_lexeme_base.get_line_end())
	, column(previous_lexeme_base.get_column_end())
	, text(std::move(text))
	{}

uint64_t LexemeBase::get_line_end() const {
	return line + static_cast<uint64_t>(std::count(text.cbegin(), text.cend(), '\n'));
}

uint64_t LexemeBase::get_column_end() const {
	std::string::size_type last_newline_pos = text.rfind('\n');
	std::string::size_type last_line_pos;
	if (last_newline_pos == std::string::npos) {
		last_line_pos = 0;
	} else {
		last_line_pos = last_newline_pos + 1;
	}

	return text.substr(last_line_pos).size() + 1;  // "+ 1": Columns begin at 1.
}

LexemeIdentifier::LexemeIdentifier(const LexemeBase &lexeme_base)
	: LexemeBase(lexeme_base)
	{}

LexemeKeyword::LexemeKeyword(const LexemeBase &lexeme_base, keyword_t keyword)
	: LexemeBase(lexeme_base)
	, keyword(keyword)
	{}

// | Automatically find the keyword from the text, raising an exception if
// it isn't recognized.
LexemeKeyword::LexemeKeyword(const LexemeBase &lexeme_base)
	: LexemeBase(lexeme_base)
	, keyword(LexemeKeyword::get_keyword(lexeme_base.text).first)
	, uppercase(LexemeKeyword::get_keyword(lexeme_base.text).second)
	{}

const std::map<std::string, keyword_t> LexemeKeyword::keyword_map {
	{"array",     array_keyword},
	{"begin",     begin_keyword},
	{"chr",       chr_keyword},
	{"const",     const_keyword},
	{"do",        do_keyword},
	{"downto",    downto_keyword},
	{"else",      else_keyword},
	{"elseif",    elseif_keyword},
	{"end",       end_keyword},
	{"for",       for_keyword},
	{"forward",   forward_keyword},
	{"function",  function_keyword},
	{"if",        if_keyword},
	{"of",        of_keyword},
	{"ord",       ord_keyword},
	{"pred",      pred_keyword},
	{"procedure", procedure_keyword},
	{"read",      read_keyword},
	{"record",    record_keyword},
	{"ref",       ref_keyword},
	{"repeat",    repeat_keyword},
	{"return",    return_keyword},
	{"stop",      stop_keyword},
	{"succ",      succ_keyword},
	{"then",      then_keyword},
	{"to",        to_keyword},
	{"type",      type_keyword},
	{"until",     until_keyword},
	{"var",       var_keyword},
	{"while",     while_keyword},
};
std::pair<keyword_t, bool> LexemeKeyword::get_keyword(std::string text) {
	std::string key;
	bool uppercase;

	// All uppercase?
	if (text.size() > 0 && std::all_of(text.cbegin(), text.cend(), [](char c){return std::isupper(static_cast<unsigned char>(c));})) {
		uppercase = true;
		// c.f. https://stackoverflow.com/q/1489313
		std::transform(text.cbegin(), text.cend(), std::back_inserter(key), [](char c){return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));});
	} else {
		uppercase = false;
		key = text;
	}

	std::map<std::string, keyword_t>::const_iterator search = keyword_map.find(key);
	if (search == keyword_map.cend()) {
		// No match found.
		std::ostringstream sstr;
		sstr << "LexemeKeyword::get_keyword: unrecognized keyword: " << text << "";
		throw LexerError(sstr.str());
	} else {
		// Match found; return the value.
		return std::pair<keyword_t, bool>(search->second, uppercase);
	}
}

bool LexemeKeyword::is_keyword(std::string text) {
	std::string key;
	bool uppercase;
	(void) uppercase;  // Unused, except for writing.

	// All uppercase?
	if (text.size() > 0 && std::all_of(text.cbegin(), text.cend(), [](char c){return std::isupper(static_cast<unsigned char>(c));})) {
		uppercase = true;
		// c.f. https://stackoverflow.com/q/1489313
		std::transform(text.cbegin(), text.cend(), std::back_inserter(key), [](char c){return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));});
	} else {
		uppercase = false;
		key = text;
	}

	std::map<std::string, keyword_t>::const_iterator search = keyword_map.find(key);
	if (search == keyword_map.cend()) {
		// No match found.
		return false;
	} else {
		// Match found; this is a keyword.
		return true;
	}
}

LexemeOperator::LexemeOperator(const LexemeBase &lexeme_base, operator_t operator_)
	: LexemeBase(lexeme_base)
	, operator_(operator_)
	{}

// | Automatically find the operator from the text, raising an exception if
// it isn't recognized.
LexemeOperator::LexemeOperator(const LexemeBase &lexeme_base)
	: LexemeBase(lexeme_base)
	, operator_(LexemeOperator::get_operator(lexeme_base.text))
	{}

const std::map<std::string, operator_t> LexemeOperator::operator_map {
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
	{":=", colonequals_operator},
	{"%", percent_operator},
};
operator_t LexemeOperator::get_operator(std::string text) {
	std::map<std::string, operator_t>::const_iterator search = operator_map.find(text);
	if (search == operator_map.cend()) {
		// No match found.
		std::ostringstream sstr;
		sstr << "LexemeOperator::get_operator: unrecognized operator: " << text << "";
		throw LexerError(sstr.str());
	} else {
		// Match found; return the value.
		return search->second;
	}
}

LexemeInteger::LexemeInteger(const LexemeBase &lexeme_base, lexeme_integer_base_t integer_base, uint64_t first_digits, const std::vector<uint64_t> &remaining_digits)
	: LexemeBase(lexeme_base)
	, integer_base(integer_base)
	, first_digits(first_digits)
	, remaining_digits(remaining_digits)
	{}

LexemeInteger::LexemeInteger(const LexemeBase &lexeme_base, lexeme_integer_base_t integer_base, uint64_t first_digits, std::vector<uint64_t> &&remaining_digits)
	: LexemeBase(lexeme_base)
	, integer_base(integer_base)
	, first_digits(first_digits)
	, remaining_digits(std::move(remaining_digits))
	{}

// | Automatically determine the integer from the text.
//
// Only some validity checks are performed.
LexemeInteger::LexemeInteger(const LexemeBase &lexeme_base)
	: LexemeBase(lexeme_base)
{
	if (text.size() <= 0) {
		std::ostringstream sstr;
		sstr << "LexemeInteger::LexemeInteger: could not parse an empty string as an integer.";
		throw LexerError(sstr.str());
	} else if (text[0] != '0') {
		// This is a decimal-formatted integer.
		integer_base = lexeme_integer_base_10;

		// Parse the (natural) integer value.
		bool parsed_first_group = false;
		uint64_t this_group = 0;

		for (const char &c: std::as_const(text)) {
			if (c < '0' || c > '9') {
				std::ostringstream sstr;
				sstr << "LexemeInteger::LexemeInteger: unexpected decimal character '" << c << "' when parsing a decimal integer.";
				throw LexerError(sstr.str());
			}

			uint64_t digit = static_cast<uint64_t>(c - '0');

			// Can we fit another digit into the group?
			// The maximum value that can fit in the 64-bit group of digits is 2^64-1=18446744073709551615.
			// An overflow will occur iff
			//     10*this_group + digit > 18446744073709551615
			// <=>    this_group + digit/10 > 1844674407370955161.5
			// <=>    this_group > 1844674407370955161.5 - digit/10
			// <=>    this_group > 1844674407370955161 if digit <= 5
			//        this_group > 1844674407370955160 otherwise
			if (
				(digit <= 5 && this_group > 1844674407370955161) ||
				(digit >  5 && this_group > 1844674407370955160)
			) {
				// We need another uint64_t to hold more digits.
				if (!parsed_first_group) {
					// This is the first group of digits; store it in
					// first_digits.
					first_digits = this_group;
					parsed_first_group = true;
					this_group = 0;
				} else {
					// This is not the first group of digits.
					remaining_digits.push_back(this_group);
					this_group = 0;
				}
			}

			// We can now parse the additional digit into "this_group".
			this_group = 10 * this_group + digit;
		}
	} else if (text.size() >= 2 && text[1] == 'x') {
		// This is a hex value.
		integer_base = lexeme_integer_base_16;

		// Make sure there is at least one hex digit.
		if (text.size() <= 2) {
			std::ostringstream sstr;
			sstr << "Lexeme::Lexeme: expecting hex value after parsing only ``0x\".  Are the digits missing?";
			throw LexerError(sstr.str());
		}

		// Parse the (natural) integer value.
		bool parsed_first_group = false;
		uint64_t this_group = 0;

		for (const char &c: text.substr(2)) {
			uint64_t digit;

			// Is this a decimal or a hex digit?
			if        (c >= '0' && c <= '9') {
				digit = static_cast<uint64_t>(c - '0');
			} else if (c >= 'A' && c <= 'F') {
				digit = static_cast<uint64_t>(c - 'A') + 10;
			} else if (c >= 'a' && c <= 'f') {
				digit = static_cast<uint64_t>(c - 'a') + 10;
			} else {
				std::ostringstream sstr;
				sstr << "LexemeInteger::LexemeInteger: unexpected hex character '" << c << "' when parsing a hex integer.";
				throw LexerError(sstr.str());
			}

			// Can we fit another digit into the group?
			if (this_group >= 0x1000000000000000) {
				// We need another uint64_t to hold more digits.
				if (!parsed_first_group) {
					// This is the first group of digits; store it in
					// first_digits.
					first_digits = this_group;
					parsed_first_group = true;
					this_group = 0;
				} else {
					// This is not the first group of digits.
					remaining_digits.push_back(this_group);
					this_group = 0;
				}
			}

			// We can now parse the additional digit into "this_group".
			this_group = 16 * this_group + digit;
		}
	} else if(text.size() <= 1) {
		// It's just 0.
		integer_base = lexeme_integer_base_10;
		first_digits = 0;
		remaining_digits.clear();
	} else {
		// It's an octal value.  The first character is 0.
		integer_base = lexeme_integer_base_8;

		// Make sure there is at least one octal digit; the case of only "0"
		// should already be covered.
		if (text.size() <= 1) {
			std::ostringstream sstr;
			sstr << "Lexeme::Lexeme: expecting hex value after parsing only ``0\"; there is an implementation error, since this should resolve to just 0.";
			throw LexerError(sstr.str());
		}

		// Parse the (natural) integer value.
		bool parsed_first_group = false;
		uint64_t this_group = 0;

		for (const char &c: text.substr(1)) {
			if (c < '0' || c > '7') {
				std::ostringstream sstr;
				sstr << "LexemeInteger::LexemeInteger: unexpected octal character '" << c << "' when parsing an octal integer.";
				throw LexerError(sstr.str());
			}

			uint64_t digit = static_cast<uint64_t>(c - '0');

			// Can we fit another digit into the group?
			if (this_group >= 0x8000000000000000) {
				// We need another uint64_t to hold more digits.
				if (!parsed_first_group) {
					// This is the first group of digits; store it in
					// first_digits.
					first_digits = this_group;
					parsed_first_group = true;
					this_group = 0;
				} else {
					// This is not the first group of digits.
					remaining_digits.push_back(this_group);
					this_group = 0;
				}
			}

			// We can now parse the additional digit into "this_group".
			this_group = 8 * this_group + digit;
		}
	}
}

const bool LexemeChar::permit_omitted_quotes = LEXEME_CHAR_PERMIT_OMITTED_QUOTES;

LexemeChar::LexemeChar(const LexemeBase &lexeme_base, uint8_t char_)
	: LexemeBase(lexeme_base)
	, char_(char_)
	{}

LexemeChar::LexemeChar(const LexemeBase &lexeme_base)
	: LexemeBase(lexeme_base)
{
	if        (text.size() <= 0) {
		std::ostringstream sstr;
		sstr << "LexemeChar::LexemeChar: cannot parse an empty string as a char.";
		throw LexerError(sstr.str());
	} else if (text.size() == 1) {
		if (!permit_omitted_quotes) {
			std::ostringstream sstr;
			sstr << "LexemeChar::LexemeChar: cannot parse a single character without quotes as a char.";
			throw LexerError(sstr.str());
		}

		if (text[0] == '\\') {
			std::ostringstream sstr;
			sstr << "LexemeChar::LexemeChar: cannot parse a single backslash ('\\') as a char.";
			throw LexerError(sstr.str());
		}

		char_ = static_cast<uint8_t>(text[0]);
	} else if (text.size() == 2) {
		if (!permit_omitted_quotes) {
			std::ostringstream sstr;
			sstr << "LexemeChar::LexemeChar: cannot parse two characters unquoted as a char.";
			throw LexerError(sstr.str());
		}

		if (text[0] != '\\') {
			std::ostringstream sstr;
			sstr << "LexemeChar::LexemeChar: cannot parse two characters without an escape as a char.";
			throw LexerError(sstr.str());
		}

		switch (text[1]) {
			case 'n':
				char_ = '\n';
				break;

			case 'r':
				char_ = '\r';
				break;

			case 'b':
				char_ = '\b';
				break;

			case 't':
				char_ = '\t';
				break;

			case 'f':
				char_ = '\f';
				break;

			default:
				char_ = static_cast<uint8_t>(text[1]);
				break;
		}
	} else if (text.size() == 3) {
		if (text[0] != '\'' || text[2] != '\'') {
			std::ostringstream sstr;
			sstr << "LexemeChar::LexemeChar: cannot parse string as a character; it should contain a character surrounded by single quotes.";
			throw LexerError(sstr.str());
		}

		if (text[1] == '\\') {
			std::ostringstream sstr;
			sstr << "LexemeChar::LexemeChar: cannot parse only a single backslash in quotes.  Escape a backslash with another, e.g. ``\\\\\".";
			throw LexerError(sstr.str());
		}

		char_ = static_cast<uint8_t>(text[1]);
	} else if (text.size() > 4) {
		std::ostringstream sstr;
		sstr << "LexemeChar::LexemeChar: too many characters for a valid char parse (`" << text << "').";
		throw LexerError(sstr.str());
	} else {  // text.size() == 4
		if (text[0] != '\'' || text[3] != '\'') {
			std::ostringstream sstr;
			sstr << "LexemeChar::LexemeChar: cannot parse string as a character (with an optional escape); it should contain a character surrounded by single quotes.";
			throw LexerError(sstr.str());
		}

		if (text[1] != '\\') {
			std::ostringstream sstr;
			sstr << "LexemeChar::LexemeChar: two characters inside single quotes is only a valid character if there is an escape with a backslash ('\\').";
			throw LexerError(sstr.str());
		}

		switch (text[2]) {
			case 'n':
				char_ = '\n';
				break;

			case 'r':
				char_ = '\r';
				break;

			case 'b':
				char_ = '\b';
				break;

			case 't':
				char_ = '\t';
				break;

			case 'f':
				char_ = '\f';
				break;

			default:
				char_ = static_cast<uint8_t>(text[2]);
				break;
		}
	}
}

LexemeComment::LexemeComment(const LexemeBase &lexeme_base)
	: LexemeBase(lexeme_base)
	{}

LexemeWhitespace::LexemeWhitespace(const LexemeBase &lexeme_base)
	: LexemeBase(lexeme_base)
	{}

const bool LexemeString::permit_omitted_quotes = LEXEME_STRING_PERMIT_OMITTED_QUOTES;

LexemeString::LexemeString(const LexemeBase &lexeme_base, std::string expanded)
	: LexemeBase(lexeme_base)
	, expanded(expanded)
	{}

LexemeString::LexemeString(const LexemeBase &lexeme_base)
	: LexemeBase(lexeme_base)
{
	// Skip the first and last characters, which should be quotes?  This should be true, since omitting quotes to the constructor is disabled.
	bool skip_ends;

	if (
		text.size() < 2 ||
		text.back() != '"' ||
		text.front() != '"'
	) {
		if (!permit_omitted_quotes) {
			std::ostringstream sstr;
			sstr << "LexemeString::LexemeString: the constructor must be called with the quotes present.  The text does not contain both a beginning and an end quote.";
			throw LexerError(sstr.str());
		}

		skip_ends = false;
	} else {
		skip_ends = true;
	}

	// Get a copy of the text string without the double quotes.
	std::string text_unquoted;
	if (skip_ends) {
		// Note: this branch cannot be reached if the length is < 2.
		text_unquoted = text.substr(1, text.size() - 1 - 1);
	} else {
		text_unquoted = std::move(std::string(text));
	}

	// Traverse the string, expanding all escapes.  Raise an error if there is
	// a trailing backslash that escapes nothing afterward.
	bool is_escape = false;
	for (const char &c: std::as_const(text_unquoted)) {
		if (is_escape) {
			switch (c) {
				case 'n':
					expanded.push_back('\n');
					break;

				case 'r':
					expanded.push_back('\r');
					break;

				case 'b':
					expanded.push_back('\b');
					break;

				case 't':
					expanded.push_back('\t');
					break;

				case 'f':
					expanded.push_back('\f');
					break;

				default:
					expanded.push_back(c);
					break;
			}
		} else {
			if (c == '\\') {
				is_escape = true;
			} else {
				expanded.push_back(c);
			}
		}
	}

	if (is_escape) {
		std::ostringstream sstr;
		sstr << "LexemeString::LexemeString: the string ends with a trailing backslash with nothing to follow it.";
		throw LexerError(sstr.str());
	}
}

/*
 * Lexeme class.
 */

Lexeme::Lexeme()
	: tag(null_lexeme_tag)
	, data(std::monostate())
	{}

Lexeme::Lexeme(lexeme_tag_t tag, const lexeme_data_t &data)
	: tag(tag)
	, data(data)
{
	switch (tag) {
		case null_lexeme_tag:
			try {
				std::get<std::monostate>(data);
			} catch (const std::bad_variant_access &ex) {
				std::ostringstream sstr;
				sstr << "Lexeme::Lexeme: the tag does not correspond to the data's std::variant tag.";
				throw LexerError(sstr.str());
			}
			break;

		case keyword_tag:
			try {
				std::get<LexemeKeyword>(data);
			} catch (const std::bad_variant_access &ex) {
				std::ostringstream sstr;
				sstr << "Lexeme::Lexeme: the tag does not correspond to the data's std::variant tag.";
				throw LexerError(sstr.str());
			}
			break;

		case identifier_tag:
			try {
				std::get<LexemeIdentifier>(data);
			} catch (const std::bad_variant_access &ex) {
				std::ostringstream sstr;
				sstr << "Lexeme::Lexeme: the tag does not correspond to the data's std::variant tag.";
				throw LexerError(sstr.str());
			}
			break;

		case operator_tag:
			try {
				std::get<LexemeOperator>(data);
			} catch (const std::bad_variant_access &ex) {
				std::ostringstream sstr;
				sstr << "Lexeme::Lexeme: the tag does not correspond to the data's std::variant tag.";
				throw LexerError(sstr.str());
			}
			break;

		case integer_tag:
			try {
				std::get<LexemeInteger>(data);
			} catch (const std::bad_variant_access &ex) {
				std::ostringstream sstr;
				sstr << "Lexeme::Lexeme: the tag does not correspond to the data's std::variant tag.";
				throw LexerError(sstr.str());
			}
			break;

		case char_tag:
			try {
				std::get<LexemeChar>(data);
			} catch (const std::bad_variant_access &ex) {
				std::ostringstream sstr;
				sstr << "Lexeme::Lexeme: the tag does not correspond to the data's std::variant tag.";
				throw LexerError(sstr.str());
			}
			break;

		case string_tag:
			try {
				std::get<LexemeString>(data);
			} catch (const std::bad_variant_access &ex) {
				std::ostringstream sstr;
				sstr << "Lexeme::Lexeme: the tag does not correspond to the data's std::variant tag.";
				throw LexerError(sstr.str());
			}
			break;

		case comment_tag:
			try {
				std::get<LexemeComment>(data);
			} catch (const std::bad_variant_access &ex) {
				std::ostringstream sstr;
				sstr << "Lexeme::Lexeme: the tag does not correspond to the data's std::variant tag.";
				throw LexerError(sstr.str());
			}
			break;

		case whitespace_tag:
			try {
				std::get<LexemeWhitespace>(data);
			} catch (const std::bad_variant_access &ex) {
				std::ostringstream sstr;
				sstr << "Lexeme::Lexeme: the tag does not correspond to the data's std::variant tag.";
				throw LexerError(sstr.str());
			}
			break;

		default:
			std::ostringstream sstr;
			sstr << "Lexeme::Lexeme: invalid tag: " << tag << ".";
			throw LexerError(sstr.str());
			break;
	}
}

Lexeme::Lexeme(lexeme_tag_t tag, lexeme_data_t &&data)
	: tag(tag)
	, data(data)
{
	switch (tag) {
		case null_lexeme_tag:
			try {
				std::get<std::monostate>(data);
			} catch (const std::bad_variant_access &ex) {
				std::ostringstream sstr;
				sstr << "Lexeme::Lexeme: the tag does not correspond to the data's std::variant tag.";
				throw LexerError(sstr.str());
			}
			break;

		case keyword_tag:
			try {
				std::get<LexemeKeyword>(data);
			} catch (const std::bad_variant_access &ex) {
				std::ostringstream sstr;
				sstr << "Lexeme::Lexeme: the tag does not correspond to the data's std::variant tag.";
				throw LexerError(sstr.str());
			}
			break;

		case identifier_tag:
			try {
				std::get<LexemeIdentifier>(data);
			} catch (const std::bad_variant_access &ex) {
				std::ostringstream sstr;
				sstr << "Lexeme::Lexeme: the tag does not correspond to the data's std::variant tag.";
				throw LexerError(sstr.str());
			}
			break;

		case operator_tag:
			try {
				std::get<LexemeOperator>(data);
			} catch (const std::bad_variant_access &ex) {
				std::ostringstream sstr;
				sstr << "Lexeme::Lexeme: the tag does not correspond to the data's std::variant tag.";
				throw LexerError(sstr.str());
			}
			break;

		case integer_tag:
			try {
				std::get<LexemeInteger>(data);
			} catch (const std::bad_variant_access &ex) {
				std::ostringstream sstr;
				sstr << "Lexeme::Lexeme: the tag does not correspond to the data's std::variant tag.";
				throw LexerError(sstr.str());
			}
			break;

		case char_tag:
			try {
				std::get<LexemeChar>(data);
			} catch (const std::bad_variant_access &ex) {
				std::ostringstream sstr;
				sstr << "Lexeme::Lexeme: the tag does not correspond to the data's std::variant tag.";
				throw LexerError(sstr.str());
			}
			break;

		case string_tag:
			try {
				std::get<LexemeString>(data);
			} catch (const std::bad_variant_access &ex) {
				std::ostringstream sstr;
				sstr << "Lexeme::Lexeme: the tag does not correspond to the data's std::variant tag.";
				throw LexerError(sstr.str());
			}
			break;

		case comment_tag:
			try {
				std::get<LexemeComment>(data);
			} catch (const std::bad_variant_access &ex) {
				std::ostringstream sstr;
				sstr << "Lexeme::Lexeme: the tag does not correspond to the data's std::variant tag.";
				throw LexerError(sstr.str());
			}
			break;

		case whitespace_tag:
			try {
				std::get<LexemeWhitespace>(data);
			} catch (const std::bad_variant_access &ex) {
				std::ostringstream sstr;
				sstr << "Lexeme::Lexeme: the tag does not correspond to the data's std::variant tag.";
				throw LexerError(sstr.str());
			}
			break;

		default:
			std::ostringstream sstr;
			sstr << "Lexeme::Lexeme: invalid tag: " << tag << ".";
			throw LexerError(sstr.str());
			break;
	}
}

std::string Lexeme::tag_repr() const {
	switch (tag) {
		case keyword_tag:
			return "KEYWORD";

		case identifier_tag:
			return "IDENTIFIER";

		case operator_tag:
			return "OPERATOR";

		case integer_tag:
			return "INTEGER";

		case char_tag:
			return "CHAR";

		case string_tag:
			return "TAG";

		case comment_tag:
			return "COMMENT";

		case whitespace_tag:
			return "WHITESPACE";

		case null_lexeme_tag:
		default:
			std::ostringstream sstr;
			sstr << "Lexeme::tag_repr: invalid tag: " << tag << ".";
			throw LexerError(sstr.str());
			break;
	}
}

// | Get the base values of the lexeme.
LexemeBase Lexeme::get_base() const {
	switch (tag) {
		case null_lexeme_tag:
			// For convenience, this is the initial base state available for
			// the lexer.
			return LexemeBase(1, 0, "");

		case keyword_tag:
			try {
				const LexemeKeyword &lexeme_keyword = std::get<LexemeKeyword>(data);
				return LexemeBase(lexeme_keyword);
			} catch (const std::bad_variant_access &ex) {
				std::ostringstream sstr;
				sstr << "Lexeme::get_base: the tag does not correspond to the data's std::variant tag.";
				throw LexerError(sstr.str());
			}
			break;

		case identifier_tag:
			try {
				const LexemeIdentifier &lexeme_identifier = std::get<LexemeIdentifier>(data);
				return LexemeBase(lexeme_identifier);
			} catch (const std::bad_variant_access &ex) {
				std::ostringstream sstr;
				sstr << "Lexeme::get_base: the tag does not correspond to the data's std::variant tag.";
				throw LexerError(sstr.str());
			}
			break;

		case operator_tag:
			try {
				const LexemeOperator &lexeme_operator = std::get<LexemeOperator>(data);
				return LexemeBase(lexeme_operator);
			} catch (const std::bad_variant_access &ex) {
				std::ostringstream sstr;
				sstr << "Lexeme::get_base: the tag does not correspond to the data's std::variant tag.";
				throw LexerError(sstr.str());
			}
			break;

		case integer_tag:
			try {
				const LexemeInteger &lexeme_integer = std::get<LexemeInteger>(data);
				return LexemeBase(lexeme_integer);
			} catch (const std::bad_variant_access &ex) {
				std::ostringstream sstr;
				sstr << "Lexeme::get_base: the tag does not correspond to the data's std::variant tag.";
				throw LexerError(sstr.str());
			}
			break;

		case char_tag:
			try {
				const LexemeChar &lexeme_char = std::get<LexemeChar>(data);
				return LexemeBase(lexeme_char);
			} catch (const std::bad_variant_access &ex) {
				std::ostringstream sstr;
				sstr << "Lexeme::get_base: the tag does not correspond to the data's std::variant tag.";
				throw LexerError(sstr.str());
			}
			break;

		case string_tag:
			try {
				const LexemeString &lexeme_string = std::get<LexemeString>(data);
				return LexemeBase(lexeme_string);
			} catch (const std::bad_variant_access &ex) {
				std::ostringstream sstr;
				sstr << "Lexeme::get_base: the tag does not correspond to the data's std::variant tag.";
				throw LexerError(sstr.str());
			}
			break;

		case comment_tag:
			try {
				const LexemeComment &lexeme_comment = std::get<LexemeComment>(data);
				return LexemeBase(lexeme_comment);
			} catch (const std::bad_variant_access &ex) {
				std::ostringstream sstr;
				sstr << "Lexeme::get_base: the tag does not correspond to the data's std::variant tag.";
				throw LexerError(sstr.str());
			}
			break;

		case whitespace_tag:
			try {
				const LexemeWhitespace &lexeme_whitespace = std::get<LexemeWhitespace>(data);
				return LexemeBase(lexeme_whitespace);
			} catch (const std::bad_variant_access &ex) {
				std::ostringstream sstr;
				sstr << "Lexeme::get_base: the tag does not correspond to the data's std::variant tag.";
				throw LexerError(sstr.str());
			}
			break;

		default:
			std::ostringstream sstr;
			sstr << "Lexeme::Lexeme: invalid tag: " << tag << ".";
			throw LexerError(sstr.str());
			break;
	}
}

// | Get the line of the lexeme.
uint64_t Lexeme::get_line() const {
	return get_base().line;
}

// | Get the column of the lexeme.
uint64_t Lexeme::get_column() const {
	return get_base().column;
}

// | Get a copy of the text of the lexeme.
std::string Lexeme::get_text() const {
	return std::string(get_base().text);
}
