#include <exception>  // std::runtime_error
#include <integer>    // uint64_t
#include <map>        // std::map
#include <string>     // std::string
#include <vector>     // std::vector

class LexerError : public std::runtime_error {
public:
	LexerError();
	LexerError(std::string message);
};

class Lexeme {
public:
	Lexeme(lexeme_tag_t tag, const lexeme_data_t &data);
	Lexeme(lexeme_tag_t tag, lexeme_data_t &&data);
	lexeme_tag_t tag;
	lexeme_data_t data;

	~Lexeme();
	Lexeme(const Lexeme &val);
	Lexeme &operator=(const Lexeme &val);
	Lexeme(Lexeme &&val);
	Lexeme &operator=(Lexeme &&val);
};

// Unrestricted unions: c.f. https://faouellet.github.io/unrestricted-unions/
typedef enum lexeme_tag_e lexeme_tag_t;
enum lexeme_tag_e {
	null_lexeme_tag = 0,
	identifier_tag  = 1,
	operator_tag    = 2,
	integer_tag     = 3,
	char_tag        = 4,
	string_tag      = 5,
	whitespace_tag  = 6,
	num_lexeme_tags = 6,
};

typedef union lexeme_data_u lexeme_data_t;
union lexeme_data_u {
	LexemeIdentifier identifier_lexeme;
	LexemeOperator operator_lexeme;
	LexemeInteger integer_lexeme;
	LexemeString string_lexeme;
	LexemeWhitespace whitespace_lexeme;
};

// TODO: add LexemeBase (or its arguments) to constructor arguments.
class LexemeBase {
public:
	LexemeBase();
	LexemeBase(uint64_t line, uint64_t column, std::string text);

	uint64_t line;
	uint64_t column;
	std::string text;
};

typedef enum identifier_e identifier_t;
enum identifier_e {
	null_identifier      = 0,
	array_identifier     = 1,
	begin_identifier     = 2,
	chr_identifier       = 3,
	const_identifier     = 4,
	do_identifier        = 5,
	downto_identifier    = 6,
	else_identifier      = 7,
	elseif_identifier    = 8,
	end_identifier       = 9,
	for_identifier       = 10,
	forward_identifier   = 11,
	function_identifier  = 12,
	if_identifier        = 13,
	of_identifier        = 14,
	ord_identifier       = 15,
	pred_identifier      = 16,
	procedure_identifier = 17,
	read_identifier      = 18,
	record_identifier    = 19,
	ref_identifier       = 20,
	repeat_identifier    = 21,
	return_identifier    = 22,
	stop_identifier      = 23,
	succ_identifier      = 24,
	then_identifier      = 25,
	to_identifier        = 26,
	type_identifier      = 27,
	until_identifier     = 28,
	var_identifier       = 29,
	while_identifier     = 30,
	write_identifier     = 31,
	num_identifiers      = 31,
};

class LexemeIdentifier : public LexemeBase {
public:
	LexemeIdentifier();
	LexemeIdentifier(identifier_t identifier);
	// | Construct an identifier lexeme by text, using "null_identifier" on an
	// unrecognized identifier.
	LexemeIdentifier(std::string text);

	identifier_t identifier;

	static const std::map<std::string, identifier_t> identifier_map;
	static identifier_t get_identifier(std::string text);
};

typedef enum operator_e operator_t;
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

class LexemeOperator : public LexemeBase {
public:
	LexemeOperator();
	LexemeOperator(operator_t operator_);
	// | Construct an operator lexeme by text, using "null_identifier" on an
	// unrecognized identifier.
	LexemeOperator(std::string text);

	identifier_t identifier;

	static const std::map<std::string, operator_t> operator_map;
	static operator_t get_operator(std::string text);
};

typedef enum lexeme_integer_base_e lexeme_integer_base_t;
enum lexeme_integer_base_e {
	lexeme_integer_base_null = 0,
	lexeme_integer_base_10 = 1,
	lexeme_integer_base_16 = 2,
	lexeme_integer_base_8 = 3,
	num_lexeme_integer_bases = 3,
};

class LexemeInteger : public LexemeBase {
public:
	LexemeInteger();
	LexemeInteger(lexeme_integer_base_t integer_base, uint64_t first_digits, std::vector<uint64_t> remaining_digits);
	LexemeInteger(std::string text);

	lexeme_integer_base_t integer_base;
	// | Read the greatest number of digits that will fit into a uint64.
	uint64_t first_digits;
	// | If there are too many digits, put the rest here in groups.
	std::vector<uint64_t> remaining_digits;
};

class LexemeChar : public LexemeBase {
public:
	LexemeChar();
	LexemeChar(uint8_t char_);
	uint8_t char_;
};


class LexemeString : public LexemeBase {
public:
	LexemeString();
	LexemeString(std::string expanded);
	// The string with escapes expanded.
	std::string expanded;
};

class LexemeWhitespace : public LexemeBase {
public:
	LexemeWhitespace();
};
