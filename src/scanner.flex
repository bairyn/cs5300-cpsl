/* The lexer for the CS-5300 CPSL compiler. */

/* Make compatible with C++: c.f. https://stackoverflow.com/a/57061573 */
%option noyywrap
/* Create a reentrant scanner: c.f. https://www.cs.virginia.edu/~cr4bd/flex-manual/Reentrant.html#Reentrant */
%option reentrant
/* Allow the current lexeme to be stored by rules' actions: c.f. https://www.cs.virginia.edu/~cr4bd/flex-manual/Extra-Data.html#Extra-Data */
%option extra-type="Lexeme"

/* Start conditions: c.f. https://www.cs.virginia.edu/~cr4bd/flex-manual/Start-Conditions.html */
%x COMMENT

%{
#include <iostream>   // std::endl
#include <sstream>    // std::ostringstream
#include <string>     // std::string
#include <vector>     // std::vector

#include "lexer.hh"

#include "scanner.hh"
%}

DIGIT           [0-9]
HEXDIGIT        [0-9]|[a-f]|[A-F]
OCTALDIGIT      [0-7]
LOWER           [a-z]
UPPER           [A-Z]
LETTER          {UPPER}|{LOWER}
/* {-} operator: c.f. https://westes.github.io/flex/manual/Patterns.html */
INNERCHAR       [[:print:]]{-}[\\]|"\\"[[:print:]]

OCTAL "0"{OCTALDIGIT}*
HEX "0x"{HEXDIGIT}*
/*
 * Just fail on non-octal numbers beginning with "0", as opposed to treating it
 * as a decimal value, at the lexer/scanner phase.
 */
DECIMAL [1-9]{DIGIT}*

COMMENT_PREFIX "$"
COMMENT_NOPREFIX [^\n]*

/*
 * Note: flex orders rules by longest match, then order; not order first!
 * Handle keywords through identifier matches and LexemeKeyword::is_keyword.
 * c.f. https://stackoverflow.com/a/34895995
 */
IDENTIFIER {LETTER}({LETTER}|{DIGIT}|"_")*
KEYWORD array|begin|chr|const|do|downto|else|elseif|end|for|forward|function|if|of|ord|pred|procedure|read|record|ref|repeat|return|stop|succ|then|to|type|until|var|while|write
OPERATOR "+"|"-"|"*"|"/"|"&"|"|"|"~"|"="|"<>"|"<"|"<="|">"|">="|"."|","|":"|";"|"("|")"|"["|"]"|":="|"%"
INTEGER {OCTAL}|{HEX}|{DECIMAL}
COMMENT {COMMENT_PREFIX}{COMMENT_NOPREFIX}
CHAR "'"{INNERCHAR}"'"
STRING "\""{INNERCHAR}*"\""
WHITESPACE [ \n\t]+

%%

{IDENTIFIER} {
	const std::string text(yytext);
	const Lexeme last_lexeme(yyget_extra(yyscanner));

	if (LexemeKeyword::is_keyword(text)) {
		Lexeme current_lexeme(
			keyword_tag,
			LexemeKeyword(LexemeBase(last_lexeme.get_base(), text))
		);
		yyset_extra(current_lexeme, yyscanner);
		return current_lexeme.tag;
	} else {
		Lexeme current_lexeme(
			identifier_tag,
			LexemeIdentifier(LexemeBase(last_lexeme.get_base(), text))
		);
		yyset_extra(current_lexeme, yyscanner);
		return current_lexeme.tag;
	}
}

{OPERATOR} {
	const std::string text(yytext);
	const Lexeme last_lexeme(yyget_extra(yyscanner));

	Lexeme current_lexeme(
		operator_tag,
		LexemeOperator(LexemeBase(last_lexeme.get_base(), text))
	);
	yyset_extra(current_lexeme, yyscanner);
	return current_lexeme.tag;
}

{INTEGER} {
	const std::string text(yytext);
	const Lexeme last_lexeme(yyget_extra(yyscanner));

	Lexeme current_lexeme(
		integer_tag,
		LexemeInteger(LexemeBase(last_lexeme.get_base(), text))
	);
	yyset_extra(current_lexeme, yyscanner);
	return current_lexeme.tag;
}

{COMMENT_PREFIX} {
	BEGIN(COMMENT);
}
<COMMENT>{COMMENT_NOPREFIX} {
	BEGIN(INITIAL);

	const std::string text(std::string("$") + std::string(yytext));
	const Lexeme last_lexeme(yyget_extra(yyscanner));

	Lexeme current_lexeme(
		comment_tag,
		LexemeComment(LexemeBase(last_lexeme.get_base(), text))
	);
	yyset_extra(current_lexeme, yyscanner);
	return current_lexeme.tag;
}

{CHAR} {
	const std::string text(yytext);
	const Lexeme last_lexeme(yyget_extra(yyscanner));

	Lexeme current_lexeme(
		char_tag,
		LexemeChar(LexemeBase(last_lexeme.get_base(), text))
	);
	yyset_extra(current_lexeme, yyscanner);
	return current_lexeme.tag;
}

{STRING} {
	const std::string text(yytext);
	const Lexeme last_lexeme(yyget_extra(yyscanner));

	Lexeme current_lexeme(
		string_tag,
		LexemeString(LexemeBase(last_lexeme.get_base(), text))
	);
	yyset_extra(current_lexeme, yyscanner);
	return current_lexeme.tag;
}

{WHITESPACE} {
	const std::string text(yytext);
	const Lexeme last_lexeme(yyget_extra(yyscanner));

	Lexeme current_lexeme(
		whitespace_tag,
		LexemeWhitespace(LexemeBase(last_lexeme.get_base(), text))
	);
	yyset_extra(current_lexeme, yyscanner);
	return current_lexeme.tag;
}

	/* c.f. https://stackoverflow.com/a/22713809 */
	/* [ \t\n] | */  /* Comment out to suppress warning, even though it is deliberately redundant. */
. {
	std::ostringstream sstr;
	sstr << "Unrecognized character: '" << yytext << "'";
	throw LexerError(sstr.str());
}

%%

std::vector<Lexeme> scanlines(const std::vector<std::string> &lines) {
	// Get a buffer containing the lines that will be alive for the duration of
	// the execution of this function, while we perform scanning with our
	// lexer.
	std::vector<Lexeme> lexemes;

	std::ostringstream sconcatenated;
	for (const std::string &line : lines) {
		sconcatenated << line << std::endl;
	}
	std::string concatenated = sconcatenated.str();

	const char *concatenated_c_str = concatenated.c_str();

	// Initialize a scanner.
	yyscan_t scanner;
	yylex_init(&scanner);

	// Tell flex to copy concatenated_c_str and use it.
	// c.f.
	// https://westes.github.io/flex/manual/Multiple-Input-Buffers.html#Scanning-Strings
	// https://www.cs.virginia.edu/~cr4bd/flex-manual/Reentrant-Uses.html#Reentrant-Uses
	YY_BUFFER_STATE buf;
	buf = yy_scan_bytes(concatenated_c_str, concatenated.size(), scanner);

	// Perform lexical scanning.
	try {
		int token;
		(void) token;  // Unused, except for setting.
		yyset_extra(Lexeme(), scanner);
		while((token = yylex(scanner)) > 0) {
			lexemes.push_back(Lexeme(yyget_extra(scanner)));
		}
	} catch (const LexerError &ex) {
		// Close the scanner.
		yy_delete_buffer(buf, scanner);
		yylex_destroy(scanner);

		throw ex;
	}

	// Close the scanner.
	yy_delete_buffer(buf, scanner);
	yylex_destroy(scanner);

	// Return the scanned lexemes.
	return lexemes;
}
