#ifndef CPSL_CC_GRAMMAR_HH
#define CPSL_CC_GRAMMAR_HH

#include "lexer.hh"   // Lexeme

/*
 * Exceptions types.
 */

class GrammarError : public std::runtime_error {
public:
	GrammarError();
	GrammarError(const std::string &message);
};

/*
 * Grammar types.
 */


/*
 * Grammar class.
 */

// | A complete parse tree of a vector of lexemes.
class Grammar {
public:
	Grammar();

	//std::vector<foo> foos;
	//std::vector<bar> bars;
};

#endif /* #ifndef CPSL_CC_GRAMMAR_HH */
