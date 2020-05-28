#ifndef CPSL_CC_SCANNER_HH
#define CPSL_CC_SCANNER_HH

#include <string>     // std::string
#include <vector>     // std::vector

#include "lexer.hh"   // Lexeme

std::vector<Lexeme> scanlines(const std::vector<std::string> &lines);

#endif /* #ifndef CPSL_CC_SCANNER_HH */
