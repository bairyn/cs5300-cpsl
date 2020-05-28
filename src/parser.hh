#ifndef CPSL_CC_PARSER_HH
#define CPSL_CC_PARSER_HH

#include <cstdint>     // uint64_t
#include <memory>      // std::shared_ptr
#include <vector>      // std::vector

#include "lexer.hh"    // Lexeme
#include "grammar.hh"  // Grammar

using parser_yystype_t = uint64_t;
// Currently unused.
using parser_yyltype_t = uint64_t;

class ParserState {
public:
	ParserState(const std::vector<Lexeme> &lexemes);

	const std::vector<Lexeme> &lexemes;
	uint64_t next_lexeme;
};

#if 0
/* int yy_cpsl_cc_parserlex(YYSTYPE *lvalp, YYLTYPE *llocp, std::shared_ptr<ParserState> parser_state); */
int yy_cpsl_cc_parserlex(parser_yystype_t *lvalp, parser_yyltype_t *llocp, std::shared_ptr<ParserState> parser_state);
#else
/* int yy_cpsl_cc_parserlex(YYSTYPE *lvalp, std::shared_ptr<ParserState> parser_state); */
int yy_cpsl_cc_parserlex(parser_yystype_t *lvalp, std::shared_ptr<ParserState> parser_state);
#endif

void yy_cpsl_cc_parsererror(std::shared_ptr<ParserState> parser_state, const char *s);

Grammar parse_lexemes(const std::vector<Lexeme> &lexemes);

#endif /* #ifndef CPSL_CC_PARSER_HH */
