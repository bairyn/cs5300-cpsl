#include "grammar.hh"

/*
 * Exception types.
 */

GrammarError::GrammarError()
	: runtime_error("A grammar error occurred.")
	{}

GrammarError::GrammarError(const std::string &message)
	: runtime_error(message)
	{}

/*
 * Grammar types.
 */


/*
 * Grammar class.
 */

// | A complete parse tree of a vector of lexemes.
Grammar::Grammar()
	{}
