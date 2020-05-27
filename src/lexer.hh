#ifndef CPSL_CC_LEXER_HH
#define CPSL_CC_LEXER_HH

#include <cstdint>    // uint8_t, uint64_t
#include <map>        // std::map
#include <stdexcept>  // std::runtime_error
#include <string>     // std::string
#include <utility>    // std::pair
#include <variant>    // std::monostate, std::variant
#include <vector>     // std::vector

/*
 * Exceptions types.
 */

class LexerError : public std::runtime_error {
public:
	LexerError();
	LexerError(std::string message);
};

/*
 * Lexeme types.
 */

class LexemeBase {
public:
	LexemeBase(uint64_t line, uint64_t column, std::string text);

	uint64_t line;
	uint64_t column;
	std::string text;
};

class LexemeIdentifier : public LexemeBase {
public:
	LexemeIdentifier(const LexemeBase &lexeme_base);
};

enum keyword_e {
	null_keyword      = 0,
	array_keyword     = 1,
	begin_keyword     = 2,
	chr_keyword       = 3,
	const_keyword     = 4,
	do_keyword        = 5,
	downto_keyword    = 6,
	else_keyword      = 7,
	elseif_keyword    = 8,
	end_keyword       = 9,
	for_keyword       = 10,
	forward_keyword   = 11,
	function_keyword  = 12,
	if_keyword        = 13,
	of_keyword        = 14,
	ord_keyword       = 15,
	pred_keyword      = 16,
	procedure_keyword = 17,
	read_keyword      = 18,
	record_keyword    = 19,
	ref_keyword       = 20,
	repeat_keyword    = 21,
	return_keyword    = 22,
	stop_keyword      = 23,
	succ_keyword      = 24,
	then_keyword      = 25,
	to_keyword        = 26,
	type_keyword      = 27,
	until_keyword     = 28,
	var_keyword       = 29,
	while_keyword     = 30,
	write_keyword     = 31,
	num_keywords      = 31,
};
typedef enum keyword_e keyword_t;

class LexemeKeyword : public LexemeBase {
public:
	LexemeKeyword(const LexemeBase &lexeme_base, keyword_t keyword);
	// | Automatically find the keyword from the text, raising an exception if
	// it isn't recognized.
	LexemeKeyword(const LexemeBase &lexeme_base);

	keyword_t keyword;
	bool uppercase;

	static const std::map<std::string, keyword_t> keyword_map;
	static std::pair<keyword_t, bool> get_keyword(std::string text);
};

enum operator_e {
	null_operator                               = 0,
	plus_operator                               = 1,
	minus_operator                              = 2,
	times_operator                              = 3,
	slash_operator                              = 4,
	ampersand_operator                          = 5,
	pipe_operator                               = 6,
	tilde_operator                              = 7,
	equals_operator                             = 8,
	lt_or_gt_operator                           = 9,
	lt_operator                                 = 10,
	le_operator                                 = 11,
	gt_operator                                 = 12,
	ge_operator                                 = 13,
	dot_operator                                = 14,
	comma_operator                              = 15,
	colon_operator                              = 16,
	semicolon_operator                          = 17,
	leftparenthesis_operator                    = 18,
	rightparenthesis_operator                   = 19,
	leftbracket_operator                        = 20,
	rightbracket_operator                       = 21,
	colonequals_operator                        = 22,
	percent_operator                            = 23,
	num_operators                               = 23,
};
typedef enum operator_e operator_t;

class LexemeOperator : public LexemeBase {
public:
	LexemeOperator(const LexemeBase &lexeme_base, operator_t operator_);
	// | Automatically find the operator from the text, raising an exception if
	// it isn't recognized.
	LexemeOperator(const LexemeBase &lexeme_base);

	operator_t operator_;

	static const std::map<std::string, operator_t> operator_map;
	static operator_t get_operator(std::string text);
};

enum lexeme_integer_base_e {
	lexeme_integer_base_null = 0,
	lexeme_integer_base_10 = 1,
	lexeme_integer_base_16 = 2,
	lexeme_integer_base_8 = 3,
	num_lexeme_integer_bases = 3,
};
typedef enum lexeme_integer_base_e lexeme_integer_base_t;

class LexemeInteger : public LexemeBase {
public:
	LexemeInteger(const LexemeBase &lexeme_base, lexeme_integer_base_t integer_base, uint64_t first_digits, const std::vector<uint64_t> &remaining_digits);
	LexemeInteger(const LexemeBase &lexeme_base, lexeme_integer_base_t integer_base, uint64_t first_digits, std::vector<uint64_t> &&remaining_digits);
	// | Automatically determine the integer from the text.
	LexemeInteger(const LexemeBase &lexeme_base);

	lexeme_integer_base_t integer_base;
	// | Read the greatest number of digits that will fit into a uint64.
	uint64_t first_digits;
	// | If there are too many digits, put the rest here in groups.
	std::vector<uint64_t> remaining_digits;
};

#define LEXEME_CHAR_PERMIT_OMITTED_QUOTES false
class LexemeChar : public LexemeBase {
public:
	LexemeChar(const LexemeBase &lexeme_base, uint8_t char_);
	LexemeChar(const LexemeBase &lexeme_base);
	uint8_t char_;

	static const bool permit_omitted_quotes;
};

class LexemeComment : public LexemeBase {
public:
	LexemeComment(const LexemeBase &lexeme_base);
};

#define LEXEME_STRING_PERMIT_OMITTED_QUOTES false
class LexemeString : public LexemeBase {
public:
	LexemeString(const LexemeBase &lexeme_base, std::string expanded);
	LexemeString(const LexemeBase &lexeme_base);
	// The string with escapes expanded.
	std::string expanded;

	static const bool permit_omitted_quotes;
};

class LexemeWhitespace : public LexemeBase {
public:
	LexemeWhitespace();
};

/*
 * Lexeme class.
 */

enum lexeme_tag_e {
	null_lexeme_tag = 0,
	keyword_tag     = 1,
	identifier_tag  = 2,
	operator_tag    = 3,
	integer_tag     = 4,
	char_tag        = 5,
	string_tag      = 6,
	comment_tag     = 7,
	whitespace_tag  = 8,
	num_lexeme_tags = 8,
};
typedef enum lexeme_tag_e lexeme_tag_t;

using lexeme_data_t = std::variant<
	std::monostate,
	LexemeKeyword,
	LexemeIdentifier,
	LexemeOperator,
	LexemeInteger,
	LexemeChar,
	LexemeString,
	LexemeComment,
	LexemeWhitespace
>;

class Lexeme {
public:
	Lexeme();
	Lexeme(lexeme_tag_t tag, const lexeme_data_t &data);
	Lexeme(lexeme_tag_t tag, lexeme_data_t &&data);
	lexeme_tag_t tag;
	lexeme_data_t data;

	// | Get a string representation of this lexeme's tag.
	std::string tag_repr() const;

	// | Get the base values of the lexeme.
	LexemeBase get_base() const;
	// | Get the line of the lexeme.
	uint64_t get_line() const;
	// | Get the column of the lexeme.
	uint64_t get_column() const;
	// | Get a copy of the text of the lexeme.
	std::string get_text() const;
};

#endif /* #ifndef CPSL_CC_LEXER_HH */
