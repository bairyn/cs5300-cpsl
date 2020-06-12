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
 * Grammar types and class.
 */

// | A complete parse tree of a vector of lexemes.
Grammar::Grammar()
	{}

Grammar::Grammar(const std::vector<Lexeme> &lexemes)
	: lexemes(lexemes)
	{}

Grammar::Grammar(std::vector<Lexeme> &&lexemes)
	: lexemes(std::move(lexemes))
	{}

#define DEFINE_SYMBOL_0( \
	S, s \
) \
S::S() \
	{} \
 \
uint64_t Grammar::STRCAT_2(new_, s)() { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	STRCAT_2(s, _storage).emplace_back(); \
	return index; \
}

#define DEFINE_SYMBOL_1( \
	S, s, c0 \
) \
S::S(uint64_t c0) \
	: c0(c0) \
	{} \
 \
uint64_t Grammar::STRCAT_2(new_, s)(uint64_t c0) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	STRCAT_2(s, _storage).emplace_back(c0); \
	return index; \
}

#define DEFINE_SYMBOL_2( \
	S, s, c0, c1 \
) \
S::S(uint64_t c0, uint64_t c1) \
	: c0(c0) \
	, c1(c1) \
	{} \
 \
uint64_t Grammar::STRCAT_2(new_, s)(uint64_t c0, uint64_t c1) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	STRCAT_2(s, _storage).emplace_back(c0, c1); \
	return index; \
}

#define DEFINE_SYMBOL_3( \
	S, s, c0, c1, c2 \
) \
S::S(uint64_t c0, uint64_t c1, uint64_t c2) \
	: c0(c0) \
	, c1(c1) \
	, c2(c2) \
	{} \
 \
uint64_t Grammar::STRCAT_2(new_, s)(uint64_t c0, uint64_t c1, uint64_t c2) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	STRCAT_2(s, _storage).emplace_back(c0, c1, c2); \
	return index; \
}

#define DEFINE_SYMBOL_4( \
	S, s, c0, c1, c2, c3 \
) \
S::S(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3) \
	: c0(c0) \
	, c1(c1) \
	, c2(c2) \
	, c3(c3) \
	{} \
 \
uint64_t Grammar::STRCAT_2(new_, s)(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	STRCAT_2(s, _storage).emplace_back(c0, c1, c2, c3); \
	return index; \
}

#define DEFINE_SYMBOL_5( \
	S, s, c0, c1, c2, c3, c4 \
) \
S::S(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3, uint64_t c4) \
	: c0(c0) \
	, c1(c1) \
	, c2(c2) \
	, c3(c3) \
	, c4(c4) \
	{} \
 \
uint64_t Grammar::STRCAT_2(new_, s)(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3, uint64_t c4) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	STRCAT_2(s, _storage).emplace_back(c0, c1, c2, c3, c4); \
	return index; \
}

#define DEFINE_SYMBOL_6( \
	S, s, c0, c1, c2, c3, c4, c5 \
) \
S::S(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3, uint64_t c4, uint64_t c5) \
	: c0(c0) \
	, c1(c1) \
	, c2(c2) \
	, c3(c3) \
	, c4(c4) \
	, c5(c5) \
	{} \
 \
uint64_t Grammar::STRCAT_2(new_, s)(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3, uint64_t c4, uint64_t c5) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	STRCAT_2(s, _storage).emplace_back(c0, c1, c2, c3, c4, c5); \
	return index; \
}

#define DEFINE_SYMBOL_7( \
	S, s, c0, c1, c2, c3, c4, c5, c6 \
) \
S::S(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3, uint64_t c4, uint64_t c5, uint64_t c6) \
	: c0(c0) \
	, c1(c1) \
	, c2(c2) \
	, c3(c3) \
	, c4(c4) \
	, c5(c5) \
	, c6(c6) \
	{} \
 \
uint64_t Grammar::STRCAT_2(new_, s)(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3, uint64_t c4, uint64_t c5, uint64_t c6) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	STRCAT_2(s, _storage).emplace_back(c0, c1, c2, c3, c4, c5, c6); \
	return index; \
}

#define DEFINE_SYMBOL_8( \
	S, s, c0, c1, c2, c3, c4, c5, c6, c7 \
) \
S::S(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3, uint64_t c4, uint64_t c5, uint64_t c6, uint64_t c7) \
	: c0(c0) \
	, c1(c1) \
	, c2(c2) \
	, c3(c3) \
	, c4(c4) \
	, c5(c5) \
	, c6(c6) \
	, c7(c7) \
	{} \
 \
uint64_t Grammar::STRCAT_2(new_, s)(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3, uint64_t c4, uint64_t c5, uint64_t c6, uint64_t c7) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	STRCAT_2(s, _storage).emplace_back(c0, c1, c2, c3, c4, c5, c6, c7); \
	return index; \
}

#define DEFINE_SYMBOL_9( \
	S, s, c0, c1, c2, c3, c4, c5, c6, c7, c8 \
) \
S::S(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3, uint64_t c4, uint64_t c5, uint64_t c6, uint64_t c7, uint64_t c8) \
	: c0(c0) \
	, c1(c1) \
	, c2(c2) \
	, c3(c3) \
	, c4(c4) \
	, c5(c5) \
	, c6(c6) \
	, c7(c7) \
	, c8(c8) \
	{} \
 \
uint64_t Grammar::STRCAT_2(new_, s)(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3, uint64_t c4, uint64_t c5, uint64_t c6, uint64_t c7, uint64_t c8) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	STRCAT_2(s, _storage).emplace_back(c0, c1, c2, c3, c4, c5, c6, c7, c8); \
	return index; \
}

#define DEFINE_SYMBOL_0_1( \
	S, s, B0, B1, b0, b1, b1c0 \
) \
S::S(uint64_t branch, uint64_t data) \
	: branch(branch) \
	, data(data) \
	{} \
 \
S::B0::B0() \
	{} \
 \
S::B1::B1(uint64_t b1c0) \
	: b1c0(b1c0) \
	{} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b0)() { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b0, _storage).size()); \
	STRCAT_4(s, _, b0, _storage).emplace_back(); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b0, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b1)(uint64_t b1c0) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b1, _storage).size()); \
	STRCAT_4(s, _, b1, _storage).emplace_back(b1c0); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b1, _branch), branch_index); \
	return index; \
}

#define DEFINE_SYMBOL_0_2( \
	S, s, B0, B1, b0, b1, b1c0, b1c1 \
) \
S::S(uint64_t branch, uint64_t data) \
	: branch(branch) \
	, data(data) \
	{} \
 \
S::B0::B0() \
	{} \
 \
S::B1::B1(uint64_t b1c0, uint64_t b1c1) \
	: b1c0(b1c0) \
	, b1c1(b1c1) \
	{} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b0)() { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b0, _storage).size()); \
	STRCAT_4(s, _, b0, _storage).emplace_back(); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b0, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b1)(uint64_t b1c0, uint64_t b1c1) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b1, _storage).size()); \
	STRCAT_4(s, _, b1, _storage).emplace_back(b1c0, b1c1); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b1, _branch), branch_index); \
	return index; \
}

#define DEFINE_SYMBOL_0_3( \
	S, s, B0, B1, b0, b1, b1c0, b1c1, b1c2 \
) \
S::S(uint64_t branch, uint64_t data) \
	: branch(branch) \
	, data(data) \
	{} \
 \
S::B0::B0() \
	{} \
 \
S::B1::B1(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2) \
	: b1c0(b1c0) \
	, b1c1(b1c1) \
	, b1c2(b1c2) \
	{} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b0)() { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b0, _storage).size()); \
	STRCAT_4(s, _, b0, _storage).emplace_back(); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b0, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b1)(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b1, _storage).size()); \
	STRCAT_4(s, _, b1, _storage).emplace_back(b1c0, b1c1, b1c2); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b1, _branch), branch_index); \
	return index; \
}

#define DEFINE_SYMBOL_1_1( \
	S, s, B0, B1, b0, b1, b0c0, b1c0 \
) \
S::S(uint64_t branch, uint64_t data) \
	: branch(branch) \
	, data(data) \
	{} \
 \
S::B0::B0(uint64_t b0c0) \
	: b0c0(b0c0) \
	{} \
 \
S::B1::B1(uint64_t b1c0) \
	: b1c0(b1c0) \
	{} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b0)(uint64_t b0c0) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b0, _storage).size()); \
	STRCAT_4(s, _, b0, _storage).emplace_back(b0c0); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b0, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b1)(uint64_t b1c0) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b1, _storage).size()); \
	STRCAT_4(s, _, b1, _storage).emplace_back(b1c0); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b1, _branch), branch_index); \
	return index; \
}

#define DEFINE_SYMBOL_2_3( \
	S, s, B0, B1, b0, b1, b0c0, b0c1, b1c0, b1c1, b1c2 \
) \
S::S(uint64_t branch, uint64_t data) \
	: branch(branch) \
	, data(data) \
	{} \
 \
S::B0::B0(uint64_t b0c0, uint64_t b0c1) \
	: b0c0(b0c0) \
	, b0c1(b0c1) \
	{} \
 \
S::B1::B1(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2) \
	: b1c0(b1c0) \
	, b1c1(b1c1) \
	, b1c2(b1c2) \
	{} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b0)(uint64_t b0c0, uint64_t b0c1) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b0, _storage).size()); \
	STRCAT_4(s, _, b0, _storage).emplace_back(b0c0, b0c1); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b0, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b1)(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b1, _storage).size()); \
	STRCAT_4(s, _, b1, _storage).emplace_back(b1c0, b1c1, b1c2); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b1, _branch), branch_index); \
	return index; \
}

#define DEFINE_SYMBOL_8_8( \
	S, s, B0, B1, b0, b1, b0c0, b0c1, b0c2, b0c3, b0c4, b0c5, b0c6, b0c7, b1c0, b1c1, b1c2, b1c3, b1c4, b1c5, b1c6, b1c7 \
) \
S::S(uint64_t branch, uint64_t data) \
	: branch(branch) \
	, data(data) \
	{} \
 \
S::B0::B0(uint64_t b0c0, uint64_t b0c1, uint64_t b0c2, uint64_t b0c3, uint64_t b0c4, uint64_t b0c5, uint64_t b0c6, uint64_t b0c7) \
	: b0c0(b0c0) \
	, b0c1(b0c1) \
	, b0c2(b0c2) \
	, b0c3(b0c3) \
	, b0c4(b0c4) \
	, b0c5(b0c5) \
	, b0c6(b0c6) \
	, b0c7(b0c7) \
	{} \
 \
S::B1::B1(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2, uint64_t b1c3, uint64_t b1c4, uint64_t b1c5, uint64_t b1c6, uint64_t b1c7) \
	: b1c0(b1c0) \
	, b1c1(b1c1) \
	, b1c2(b1c2) \
	, b1c3(b1c3) \
	, b1c4(b1c4) \
	, b1c5(b1c5) \
	, b1c6(b1c6) \
	, b1c7(b1c7) \
	{} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b0)(uint64_t b0c0, uint64_t b0c1, uint64_t b0c2, uint64_t b0c3, uint64_t b0c4, uint64_t b0c5, uint64_t b0c6, uint64_t b0c7) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b0, _storage).size()); \
	STRCAT_4(s, _, b0, _storage).emplace_back(b0c0, b0c1, b0c2, b0c3, b0c4, b0c5, b0c6, b0c7); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b0, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b1)(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2, uint64_t b1c3, uint64_t b1c4, uint64_t b1c5, uint64_t b1c6, uint64_t b1c7) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b1, _storage).size()); \
	STRCAT_4(s, _, b1, _storage).emplace_back(b1c0, b1c1, b1c2, b1c3, b1c4, b1c5, b1c6, b1c7); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b1, _branch), branch_index); \
	return index; \
}

#define DEFINE_SYMBOL_10_10( \
	S, s, B0, B1, b0, b1, b0c0, b0c1, b0c2, b0c3, b0c4, b0c5, b0c6, b0c7, b0c8, b0c9, b1c0, b1c1, b1c2, b1c3, b1c4, b1c5, b1c6, b1c7, b1c8, b1c9 \
) \
S::S(uint64_t branch, uint64_t data) \
	: branch(branch) \
	, data(data) \
	{} \
 \
S::B0::B0(uint64_t b0c0, uint64_t b0c1, uint64_t b0c2, uint64_t b0c3, uint64_t b0c4, uint64_t b0c5, uint64_t b0c6, uint64_t b0c7, uint64_t b0c8, uint64_t b0c9) \
	: b0c0(b0c0) \
	, b0c1(b0c1) \
	, b0c2(b0c2) \
	, b0c3(b0c3) \
	, b0c4(b0c4) \
	, b0c5(b0c5) \
	, b0c6(b0c6) \
	, b0c7(b0c7) \
	, b0c8(b0c8) \
	, b0c9(b0c9) \
	{} \
 \
S::B1::B1(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2, uint64_t b1c3, uint64_t b1c4, uint64_t b1c5, uint64_t b1c6, uint64_t b1c7, uint64_t b1c8, uint64_t b1c9) \
	: b1c0(b1c0) \
	, b1c1(b1c1) \
	, b1c2(b1c2) \
	, b1c3(b1c3) \
	, b1c4(b1c4) \
	, b1c5(b1c5) \
	, b1c6(b1c6) \
	, b1c7(b1c7) \
	, b1c8(b1c8) \
	, b1c9(b1c9) \
	{} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b0)(uint64_t b0c0, uint64_t b0c1, uint64_t b0c2, uint64_t b0c3, uint64_t b0c4, uint64_t b0c5, uint64_t b0c6, uint64_t b0c7, uint64_t b0c8, uint64_t b0c9) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b0, _storage).size()); \
	STRCAT_4(s, _, b0, _storage).emplace_back(b0c0, b0c1, b0c2, b0c3, b0c4, b0c5, b0c6, b0c7, b0c8, b0c9); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b0, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b1)(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2, uint64_t b1c3, uint64_t b1c4, uint64_t b1c5, uint64_t b1c6, uint64_t b1c7, uint64_t b1c8, uint64_t b1c9) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b1, _storage).size()); \
	STRCAT_4(s, _, b1, _storage).emplace_back(b1c0, b1c1, b1c2, b1c3, b1c4, b1c5, b1c6, b1c7, b1c8, b1c9); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b1, _branch), branch_index); \
	return index; \
}

#define DEFINE_SYMBOL_1_1_1( \
	S, s, B0, B1, B2, b0, b1, b2, b0c0, b1c0, b2c0 \
) \
S::S(uint64_t branch, uint64_t data) \
	: branch(branch) \
	, data(data) \
	{} \
 \
S::B0::B0(uint64_t b0c0) \
	: b0c0(b0c0) \
	{} \
 \
S::B1::B1(uint64_t b1c0) \
	: b1c0(b1c0) \
	{} \
 \
S::B2::B2(uint64_t b2c0) \
	: b2c0(b2c0) \
	{} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b0)(uint64_t b0c0) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b0, _storage).size()); \
	STRCAT_4(s, _, b0, _storage).emplace_back(b0c0); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b0, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b1)(uint64_t b1c0) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b1, _storage).size()); \
	STRCAT_4(s, _, b1, _storage).emplace_back(b1c0); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b1, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b2)(uint64_t b2c0) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b2, _storage).size()); \
	STRCAT_4(s, _, b2, _storage).emplace_back(b2c0); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b2, _branch), branch_index); \
	return index; \
}

#define DEFINE_SYMBOL_1_1_1_1_1_1_1_1_1_1_1( \
	S, s, B0, B1, B2, B3, B4, B5, B6, B7, B8, B9, B10, b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b0c0, b1c0, b2c0, b3c0, b4c0, b5c0, b6c0, b7c0, b8c0, b9c0, b10c0 \
) \
S::S(uint64_t branch, uint64_t data) \
	: branch(branch) \
	, data(data) \
	{} \
 \
S::B0::B0(uint64_t b0c0) \
	: b0c0(b0c0) \
	{} \
 \
S::B1::B1(uint64_t b1c0) \
	: b1c0(b1c0) \
	{} \
 \
S::B2::B2(uint64_t b2c0) \
	: b2c0(b2c0) \
	{} \
 \
S::B3::B3(uint64_t b3c0) \
	: b3c0(b3c0) \
	{} \
 \
S::B4::B4(uint64_t b4c0) \
	: b4c0(b4c0) \
	{} \
 \
S::B5::B5(uint64_t b5c0) \
	: b5c0(b5c0) \
	{} \
 \
S::B6::B6(uint64_t b6c0) \
	: b6c0(b6c0) \
	{} \
 \
S::B7::B7(uint64_t b7c0) \
	: b7c0(b7c0) \
	{} \
 \
S::B8::B8(uint64_t b8c0) \
	: b8c0(b8c0) \
	{} \
 \
S::B9::B9(uint64_t b9c0) \
	: b9c0(b9c0) \
	{} \
 \
S::B10::B10(uint64_t b10c0) \
	: b10c0(b10c0) \
	{} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b0)(uint64_t b0c0) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b0, _storage).size()); \
	STRCAT_4(s, _, b0, _storage).emplace_back(b0c0); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b0, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b1)(uint64_t b1c0) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b1, _storage).size()); \
	STRCAT_4(s, _, b1, _storage).emplace_back(b1c0); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b1, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b2)(uint64_t b2c0) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b2, _storage).size()); \
	STRCAT_4(s, _, b2, _storage).emplace_back(b2c0); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b2, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b3)(uint64_t b3c0) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b3, _storage).size()); \
	STRCAT_4(s, _, b3, _storage).emplace_back(b3c0); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b3, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b4)(uint64_t b4c0) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b4, _storage).size()); \
	STRCAT_4(s, _, b4, _storage).emplace_back(b4c0); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b4, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b5)(uint64_t b5c0) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b5, _storage).size()); \
	STRCAT_4(s, _, b5, _storage).emplace_back(b5c0); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b5, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b6)(uint64_t b6c0) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b6, _storage).size()); \
	STRCAT_4(s, _, b6, _storage).emplace_back(b6c0); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b6, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b7)(uint64_t b7c0) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b7, _storage).size()); \
	STRCAT_4(s, _, b7, _storage).emplace_back(b7c0); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b7, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b8)(uint64_t b8c0) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b8, _storage).size()); \
	STRCAT_4(s, _, b8, _storage).emplace_back(b8c0); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b8, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b9)(uint64_t b9c0) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b9, _storage).size()); \
	STRCAT_4(s, _, b9, _storage).emplace_back(b9c0); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b9, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b10)(uint64_t b10c0) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b10, _storage).size()); \
	STRCAT_4(s, _, b10, _storage).emplace_back(b10c0); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b10, _branch), branch_index); \
	return index; \
}

#define DEFINE_SYMBOL_3_3_3_3_3_3_3_3_3_3_3_3_3_2_2_3_4_4_4_4_4_1_1_1_1( \
	S, s, \
	B0,  B1,  B2,  B3,  B4,  B5,  B6,  B7,  B8,  B9, \
	B10, B11, B12, B13, B14, B15, B16, B17, B18, B19, \
	B20, B21, B22, B23, B24, \
	b0,  b1,  b2,  b3,  b4,  b5,  b6,  b7,  b8,  b9, \
	b10, b11, b12, b13, b14, b15, b16, b17, b18, b19, \
	b20, b21, b22, b23, b24, \
	b0c0, b0c1, b0c2, \
	b1c0, b1c1, b1c2, \
	b2c0, b2c1, b2c2, \
	b3c0, b3c1, b3c2, \
	b4c0, b4c1, b4c2, \
	b5c0, b5c1, b5c2, \
	b6c0, b6c1, b6c2, \
	b7c0, b7c1, b7c2, \
	b8c0, b8c1, b8c2, \
	b9c0, b9c1, b9c2, \
	b10c0, b10c1, b10c2, \
	b11c0, b11c1, b11c2, \
	b12c0, b12c1, b12c2, \
	b13c0, b13c1, \
	b14c0, b14c1, \
	b15c0, b15c1, b15c2, \
	b16c0, b16c1, b16c2, b16c3, \
	b17c0, b17c1, b17c2, b17c3, \
	b18c0, b18c1, b18c2, b18c3, \
	b19c0, b19c1, b19c2, b19c3, \
	b20c0, b20c1, b20c2, b20c3, \
	b21c0, \
	b22c0, \
	b23c0, \
	b24c0 \
) \
S::S(uint64_t branch, uint64_t data) \
	: branch(branch) \
	, data(data) \
	{} \
 \
S::B0::B0(uint64_t b0c0, uint64_t b0c1, uint64_t b0c2) \
	: b0c0(b0c0) \
	, b0c1(b0c1) \
	, b0c2(b0c2) \
	{} \
 \
S::B1::B1(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2) \
	: b1c0(b1c0) \
	, b1c1(b1c1) \
	, b1c2(b1c2) \
	{} \
 \
S::B2::B2(uint64_t b2c0, uint64_t b2c1, uint64_t b2c2) \
	: b2c0(b2c0) \
	, b2c1(b2c1) \
	, b2c2(b2c2) \
	{} \
 \
S::B3::B3(uint64_t b3c0, uint64_t b3c1, uint64_t b3c2) \
	: b3c0(b3c0) \
	, b3c1(b3c1) \
	, b3c2(b3c2) \
	{} \
 \
S::B4::B4(uint64_t b4c0, uint64_t b4c1, uint64_t b4c2) \
	: b4c0(b4c0) \
	, b4c1(b4c1) \
	, b4c2(b4c2) \
	{} \
 \
S::B5::B5(uint64_t b5c0, uint64_t b5c1, uint64_t b5c2) \
	: b5c0(b5c0) \
	, b5c1(b5c1) \
	, b5c2(b5c2) \
	{} \
 \
S::B6::B6(uint64_t b6c0, uint64_t b6c1, uint64_t b6c2) \
	: b6c0(b6c0) \
	, b6c1(b6c1) \
	, b6c2(b6c2) \
	{} \
 \
S::B7::B7(uint64_t b7c0, uint64_t b7c1, uint64_t b7c2) \
	: b7c0(b7c0) \
	, b7c1(b7c1) \
	, b7c2(b7c2) \
	{} \
 \
S::B8::B8(uint64_t b8c0, uint64_t b8c1, uint64_t b8c2) \
	: b8c0(b8c0) \
	, b8c1(b8c1) \
	, b8c2(b8c2) \
	{} \
 \
S::B9::B9(uint64_t b9c0, uint64_t b9c1, uint64_t b9c2) \
	: b9c0(b9c0) \
	, b9c1(b9c1) \
	, b9c2(b9c2) \
	{} \
 \
S::B10::B10(uint64_t b10c0, uint64_t b10c1, uint64_t b10c2) \
	: b10c0(b10c0) \
	, b10c1(b10c1) \
	, b10c2(b10c2) \
	{} \
 \
S::B11::B11(uint64_t b11c0, uint64_t b11c1, uint64_t b11c2) \
	: b11c0(b11c0) \
	, b11c1(b11c1) \
	, b11c2(b11c2) \
	{} \
 \
S::B12::B12(uint64_t b12c0, uint64_t b12c1, uint64_t b12c2) \
	: b12c0(b12c0) \
	, b12c1(b12c1) \
	, b12c2(b12c2) \
	{} \
 \
S::B13::B13(uint64_t b13c0, uint64_t b13c1) \
	: b13c0(b13c0) \
	, b13c1(b13c1) \
	{} \
 \
S::B14::B14(uint64_t b14c0, uint64_t b14c1) \
	: b14c0(b14c0) \
	, b14c1(b14c1) \
	{} \
 \
S::B15::B15(uint64_t b15c0, uint64_t b15c1, uint64_t b15c2) \
	: b15c0(b15c0) \
	, b15c1(b15c1) \
	, b15c2(b15c2) \
	{} \
 \
S::B16::B16(uint64_t b16c0, uint64_t b16c1, uint64_t b16c2, uint64_t b16c3) \
	: b16c0(b16c0) \
	, b16c1(b16c1) \
	, b16c2(b16c2) \
	, b16c3(b16c3) \
	{} \
 \
S::B17::B17(uint64_t b17c0, uint64_t b17c1, uint64_t b17c2, uint64_t b17c3) \
	: b17c0(b17c0) \
	, b17c1(b17c1) \
	, b17c2(b17c2) \
	, b17c3(b17c3) \
	{} \
 \
S::B18::B18(uint64_t b18c0, uint64_t b18c1, uint64_t b18c2, uint64_t b18c3) \
	: b18c0(b18c0) \
	, b18c1(b18c1) \
	, b18c2(b18c2) \
	, b18c3(b18c3) \
	{} \
 \
S::B19::B19(uint64_t b19c0, uint64_t b19c1, uint64_t b19c2, uint64_t b19c3) \
	: b19c0(b19c0) \
	, b19c1(b19c1) \
	, b19c2(b19c2) \
	, b19c3(b19c3) \
	{} \
 \
S::B20::B20(uint64_t b20c0, uint64_t b20c1, uint64_t b20c2, uint64_t b20c3) \
	: b20c0(b20c0) \
	, b20c1(b20c1) \
	, b20c2(b20c2) \
	, b20c3(b20c3) \
	{} \
 \
S::B21::B21(uint64_t b21c0) \
	: b21c0(b21c0) \
	{} \
 \
S::B22::B22(uint64_t b22c0) \
	: b22c0(b22c0) \
	{} \
 \
S::B23::B23(uint64_t b23c0) \
	: b23c0(b23c0) \
	{} \
 \
S::B24::B24(uint64_t b24c0) \
	: b24c0(b24c0) \
	{} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b0)(uint64_t b0c0, uint64_t b0c1, uint64_t b0c2) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b0, _storage).size()); \
	STRCAT_4(s, _, b0, _storage).emplace_back(b0c0, b0c1, b0c2); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b0, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b1)(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b1, _storage).size()); \
	STRCAT_4(s, _, b1, _storage).emplace_back(b1c0, b1c1, b1c2); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b1, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b2)(uint64_t b2c0, uint64_t b2c1, uint64_t b2c2) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b2, _storage).size()); \
	STRCAT_4(s, _, b2, _storage).emplace_back(b2c0, b2c1, b2c2); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b2, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b3)(uint64_t b3c0, uint64_t b3c1, uint64_t b3c2) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b3, _storage).size()); \
	STRCAT_4(s, _, b3, _storage).emplace_back(b3c0, b3c1, b3c2); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b3, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b4)(uint64_t b4c0, uint64_t b4c1, uint64_t b4c2) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b4, _storage).size()); \
	STRCAT_4(s, _, b4, _storage).emplace_back(b4c0, b4c1, b4c2); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b4, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b5)(uint64_t b5c0, uint64_t b5c1, uint64_t b5c2) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b5, _storage).size()); \
	STRCAT_4(s, _, b5, _storage).emplace_back(b5c0, b5c1, b5c2); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b5, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b6)(uint64_t b6c0, uint64_t b6c1, uint64_t b6c2) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b6, _storage).size()); \
	STRCAT_4(s, _, b6, _storage).emplace_back(b6c0, b6c1, b6c2); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b6, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b7)(uint64_t b7c0, uint64_t b7c1, uint64_t b7c2) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b7, _storage).size()); \
	STRCAT_4(s, _, b7, _storage).emplace_back(b7c0, b7c1, b7c2); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b7, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b8)(uint64_t b8c0, uint64_t b8c1, uint64_t b8c2) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b8, _storage).size()); \
	STRCAT_4(s, _, b8, _storage).emplace_back(b8c0, b8c1, b8c2); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b8, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b9)(uint64_t b9c0, uint64_t b9c1, uint64_t b9c2) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b9, _storage).size()); \
	STRCAT_4(s, _, b9, _storage).emplace_back(b9c0, b9c1, b9c2); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b9, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b10)(uint64_t b10c0, uint64_t b10c1, uint64_t b10c2) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b10, _storage).size()); \
	STRCAT_4(s, _, b10, _storage).emplace_back(b10c0, b10c1, b10c2); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b10, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b11)(uint64_t b11c0, uint64_t b11c1, uint64_t b11c2) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b11, _storage).size()); \
	STRCAT_4(s, _, b11, _storage).emplace_back(b11c0, b11c1, b11c2); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b11, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b12)(uint64_t b12c0, uint64_t b12c1, uint64_t b12c2) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b12, _storage).size()); \
	STRCAT_4(s, _, b12, _storage).emplace_back(b12c0, b12c1, b12c2); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b12, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b13)(uint64_t b13c0, uint64_t b13c1) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b13, _storage).size()); \
	STRCAT_4(s, _, b13, _storage).emplace_back(b13c0, b13c1); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b13, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b14)(uint64_t b14c0, uint64_t b14c1) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b14, _storage).size()); \
	STRCAT_4(s, _, b14, _storage).emplace_back(b14c0, b14c1); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b14, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b15)(uint64_t b15c0, uint64_t b15c1, uint64_t b15c2) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b15, _storage).size()); \
	STRCAT_4(s, _, b15, _storage).emplace_back(b15c0, b15c1, b15c2); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b15, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b16)(uint64_t b16c0, uint64_t b16c1, uint64_t b16c2, uint64_t b16c3) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b16, _storage).size()); \
	STRCAT_4(s, _, b16, _storage).emplace_back(b16c0, b16c1, b16c2, b16c3); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b16, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b17)(uint64_t b17c0, uint64_t b17c1, uint64_t b17c2, uint64_t b17c3) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b17, _storage).size()); \
	STRCAT_4(s, _, b17, _storage).emplace_back(b17c0, b17c1, b17c2, b17c3); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b17, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b18)(uint64_t b18c0, uint64_t b18c1, uint64_t b18c2, uint64_t b18c3) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b18, _storage).size()); \
	STRCAT_4(s, _, b18, _storage).emplace_back(b18c0, b18c1, b18c2, b18c3); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b18, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b19)(uint64_t b19c0, uint64_t b19c1, uint64_t b19c2, uint64_t b19c3) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b19, _storage).size()); \
	STRCAT_4(s, _, b19, _storage).emplace_back(b19c0, b19c1, b19c2, b19c3); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b19, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b20)(uint64_t b20c0, uint64_t b20c1, uint64_t b20c2, uint64_t b20c3) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b20, _storage).size()); \
	STRCAT_4(s, _, b20, _storage).emplace_back(b20c0, b20c1, b20c2, b20c3); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b20, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b21)(uint64_t b21c0) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b21, _storage).size()); \
	STRCAT_4(s, _, b21, _storage).emplace_back(b21c0); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b21, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b22)(uint64_t b22c0) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b22, _storage).size()); \
	STRCAT_4(s, _, b22, _storage).emplace_back(b22c0); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b22, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b23)(uint64_t b23c0) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b23, _storage).size()); \
	STRCAT_4(s, _, b23, _storage).emplace_back(b23c0); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b23, _branch), branch_index); \
	return index; \
} \
 \
uint64_t Grammar::STRCAT_4(new_, s, _, b24)(uint64_t b24c0) { \
	uint64_t index = static_cast<uint64_t>(STRCAT_2(s, _storage).size()); \
	uint64_t branch_index = static_cast<uint64_t>(STRCAT_4(s, _, b24, _storage).size()); \
	STRCAT_4(s, _, b24, _storage).emplace_back(b24c0); \
	STRCAT_2(s, _storage).emplace_back(S::STRCAT_2(b24, _branch), branch_index); \
	return index; \
}

DEFINE_SYMBOL_1(Keyword, keyword, lexeme)
DEFINE_SYMBOL_1(Operator, operator, lexeme)
DEFINE_SYMBOL_1(Start, start, program)
DEFINE_SYMBOL_6(Program, program, constant_decl_opt, type_decl_opt, var_decl_opt, procedure_decl_or_function_decl_list, block, dot_operator0)
DEFINE_SYMBOL_0_1(ConstantDeclOpt, constant_decl_opt, Empty, Value, empty, value, constant_decl)
DEFINE_SYMBOL_0_1(TypeDeclOpt, type_decl_opt, Empty, Value, empty, value, type_decl)
DEFINE_SYMBOL_0_1(VarDeclOpt, var_decl_opt, Empty, Value, empty, value, var_decl)
DEFINE_SYMBOL_0_2(ProcedureDeclOrFunctionDeclList, procedure_decl_or_function_decl_list, Empty, Cons, empty, cons, procedure_decl_or_function_decl_list, procedure_decl_or_function_decl)
DEFINE_SYMBOL_1_1(ProcedureDeclOrFunctionDecl, procedure_decl_or_function_decl, Procedure, Function, procedure, function, procedure_decl, function_decl)
DEFINE_SYMBOL_3(ConstantDecl, constant_decl, const_keyword0, constant_assignment, constant_assignment_list)
DEFINE_SYMBOL_0_2(ConstantAssignmentList, constant_assignment_list, Empty, Cons, empty, cons, constant_assignment_list, constant_assignment)
DEFINE_SYMBOL_4(ConstantAssignment, constant_assignment, identifier, equals_operator0, expression, semicolon_operator0)
DEFINE_SYMBOL_8_8(
	ProcedureDecl, procedure_decl, Forward, Definition, forward, definition,
	procedure_keyword0, identifier, leftparenthesis_operator0, formal_parameters, rightparenthesis_operator0, semicolon_operator0, forward_keyword0, semicolon_operator1,
	procedure_keyword0, identifier, leftparenthesis_operator0, formal_parameters, rightparenthesis_operator0, semicolon_operator0, body, semicolon_operator1
)
DEFINE_SYMBOL_10_10(
	FunctionDecl, function_decl, Forward, Definition, forward, definition,
	function_keyword0, identifier, leftparenthesis_operator0, formal_parameters, rightparenthesis_operator0, colon_operator0, type, semicolon_operator0, forward_keyword0, semicolon_operator1,
	function_keyword0, identifier, leftparenthesis_operator0, formal_parameters, rightparenthesis_operator0, colon_operator0, type, semicolon_operator0, body, semicolon_operator1
)
DEFINE_SYMBOL_0_2(FormalParameters, formal_parameters, Empty, First, empty, first, formal_parameter, formal_parameter_prefixed_list)
DEFINE_SYMBOL_0_3(FormalParameterPrefixedList, formal_parameter_prefixed_list, Empty, Cons, empty, cons, formal_parameter_prefixed_list, semicolon_operator0, formal_parameter)
DEFINE_SYMBOL_4(FormalParameter, formal_parameter, var_or_ref, ident_list, colon_operator0, type)
DEFINE_SYMBOL_1_1(VarOrRef, var_or_ref, Var, Ref, var, ref, var_keyword0, ref_keyword0)
DEFINE_SYMBOL_4(Body, body, constant_decl_opt, type_decl_opt, var_decl_opt, block)
DEFINE_SYMBOL_3(Block, block, begin_keyword0, statement_sequence, end_keyword0)
DEFINE_SYMBOL_3(TypeDecl, type_decl, type_keyword0, type_assignment, type_assignment_list)
DEFINE_SYMBOL_0_2(TypeAssignmentList, type_assignment_list, Empty, Cons, empty, cons, type_assignment_list, type_assignment)
DEFINE_SYMBOL_4(TypeAssignment, type_assignment, identifier, equals_operator0, type, semicolon_operator0)
DEFINE_SYMBOL_1_1_1(Type, type, Simple, Record, Array, simple, record, array, simple_type, record_type, array_type)
DEFINE_SYMBOL_1(SimpleType, simple_type, identifier)
DEFINE_SYMBOL_3(RecordType, record_type, record_keyword0, typed_identifier_sequence_list, end_keyword0)
DEFINE_SYMBOL_0_2(TypedIdentifierSequenceList, typed_identifier_sequence_list, Empty, Cons, empty, cons, typed_identifier_sequence_list, typed_identifier_sequence)
DEFINE_SYMBOL_4(TypedIdentifierSequence, typed_identifier_sequence, ident_list, colon_operator0, type, semicolon_operator0)
DEFINE_SYMBOL_8(ArrayType, array_type, array_keyword0, leftbracket_operator0, expression0, colon_operator0, expression1, rightbracket_operator0, of_keyword0, type)
DEFINE_SYMBOL_2(IdentList, ident_list, identifier, identifier_prefixed_list)
DEFINE_SYMBOL_0_3(IdentifierPrefixedList, identifier_prefixed_list, Empty, Cons, empty, cons, identifier_prefixed_list, comma_operator0, identifier)
DEFINE_SYMBOL_3(VarDecl, var_decl, var_keyword0, typed_identifier_sequence, typed_identifier_sequence_list)
DEFINE_SYMBOL_2(StatementSequence, statement_sequence, statement, statement_prefixed_list)
DEFINE_SYMBOL_0_3(StatementPrefixedList, statement_prefixed_list, Empty, Cons, empty, cons, statement_prefixed_list, semicolon_operator0, statement)
DEFINE_SYMBOL_1_1_1_1_1_1_1_1_1_1_1(
	Statement, statement,
	Assignment, If, While, Repeat, For, Stop, Return, Read, Write, Call, Null_,
	assignment, if, while, repeat, for, stop, return, read, write, call, null_,
	assignment, if_statement, while_statement, repeat_statement, for_statement, stop_statement, return_statement, read_statement, write_statement, procedure_call, null_statement
)
DEFINE_SYMBOL_3(Assignment, assignment, lvalue, colonequals_operator0, expression)
DEFINE_SYMBOL_7(IfStatement, if_statement, if_keyword0, expression, then_keyword0, statement_sequence, elseif_clause_list, else_clause_opt, end_keyword0)
DEFINE_SYMBOL_0_2(ElseifClauseList, elseif_clause_list, Empty, Cons, empty, cons, elseif_clause_list, elseif_clause)
DEFINE_SYMBOL_4(ElseifClause, elseif_clause, elseif_keyword0, expression, then_keyword0, statement_sequence)
DEFINE_SYMBOL_0_1(ElseClauseOpt, else_clause_opt, Empty, Value, empty, value, else_clause)
DEFINE_SYMBOL_2(ElseClause, else_clause, else_keyword0, statement_sequence)
DEFINE_SYMBOL_5(WhileStatement, while_statement, while_keyword0, expression, do_keyword0, statement_sequence, end_keyword0)
DEFINE_SYMBOL_4(RepeatStatement, repeat_statement, repeat_keyword0, statement_sequence, until_keyword0, expression)
DEFINE_SYMBOL_9(ForStatement, for_statement, for_keyword0, identifier, colonequals_operator0, expression0, to_or_downto, expression1, do_keyword0, statement_sequence, end_keyword0)
DEFINE_SYMBOL_1_1(ToOrDownto, to_or_downto, To, Downto, to, downto, to_keyword0, downto_keyword0)
DEFINE_SYMBOL_1(StopStatement, stop_statement, stop_keyword0)
DEFINE_SYMBOL_2(ReturnStatement, return_statement, return_keyword0, expression_opt)
DEFINE_SYMBOL_0_1(ExpressionOpt, expression_opt, Empty, Value, empty, value, expression)
DEFINE_SYMBOL_4(ReadStatement, read_statement, read_keyword0, leftparenthesis_operator0, lvalue_sequence, rightparenthesis_operator0)
DEFINE_SYMBOL_2(LvalueSequence, lvalue_sequence, lvalue, lvalue_prefixed_list)
DEFINE_SYMBOL_0_3(LvaluePrefixedList, lvalue_prefixed_list, Empty, Cons, empty, cons, lvalue_prefixed_list, comman_operator0, lvalue)
DEFINE_SYMBOL_4(WriteStatement, write_statement, write_keyword0, leftparenthesis_operator0, expression_sequence, rightparenthesis_operator0)
DEFINE_SYMBOL_2(ExpressionSequence, expression_sequence, expression, expression_prefixed_list)
DEFINE_SYMBOL_0_3(ExpressionPrefixedList, expression_prefixed_list, Empty, Cons, empty, cons, expression_prefixed_list, comma_operator0, expression)
DEFINE_SYMBOL_4(ProcedureCall, procedure_call, identifier, leftparenthesis_operator0, expression_sequence_opt, rightparenthesis_operator0)
DEFINE_SYMBOL_0_1(ExpressionSequenceOpt, expression_sequence_opt, Empty, Value, empty, value, expression_sequence)
DEFINE_SYMBOL_0(NullStatement, null_statement)
DEFINE_SYMBOL_3_3_3_3_3_3_3_3_3_3_3_3_3_2_2_3_4_4_4_4_4_1_1_1_1(
	Expression, expression,
	Pipe, Ampersand, Equals, LtOrGt, Le, Ge, Lt, Gt, Plus, Minus, Times, Slash, Percent, Tilde, UnaryMinus, Parentheses, Call, Chr, Ord, Pred, Succ, Lvalue, Integer, Char_, String,
	pipe, ampersand, equals, lt_or_gt, le, ge, lt, gt, plus, minus, times, slash, percent, tilde, unary_minus, parentheses, call, chr, ord, pred, succ, lvalue, integer, char_, string,
	expression0, pipe_operator0, expression1,
	expression0, ampersand_operator0, expression1,
	expression0, equals_operator0, expression1,
	expression0, lt_or_gt_operator0, expression1,
	expression0, le_operator0, expression1,
	expression0, ge_operator0, expression1,
	expression0, lt_operator0, expression1,
	expression0, gt_operator0, expression1,
	expression0, plus_operator0, expression1,
	expression0, minus_operator0, expression1,
	expression0, times_operator0, expression1,
	expression0, slash_operator0, expression1,
	expression0, percent_operator0, expression1,
	tilde_operator0, expression,
	minus_operator0, expression,
	leftparenthesis_operator0, expression, rightparenthesis_operator0,
	identifier, leftparenthesis_operator0, expression_sequence_opt, rightparenthesis_operator0,
	chr_keyword0, leftparenthesis_operator0, expression, rightparenthesis_operator0,
	ord_keyword0, leftparenthesis_operator0, expression, rightparenthesis_operator0,
	pred_keyword0, leftparenthesis_operator0, expression, rightparenthesis_operator0,
	succ_keyword0, leftparenthesis_operator0, expression, rightparenthesis_operator0,
	lvalue,
	integer,
	char_,
	string
)
DEFINE_SYMBOL_2(Lvalue, lvalue, identifier, lvalue_accessor_clause_list)
DEFINE_SYMBOL_0_2(LvalueAccessorClauseList, lvalue_accessor_clause_list, Empty, Cons, empty, cons, lvalue_accessor_clause_list, lvalue_accessor_clause)
DEFINE_SYMBOL_2_3(
	LvalueAccessorClause, lvalue_accessor_clause, Index, Array, index, array,
	dot_operator0, identifier,
	leftbracket_operator0, expression, rightbracket_operator0
)

std::string Grammar::lexemes_text(uint64_t begin, uint64_t end) const {
	std::string concatenated;

	for (uint64_t index = begin; index < end; ++index) {
		const Lexeme &lexeme = lexemes.at(index);
		concatenated += lexeme.get_text();
	}

	return concatenated;
}
