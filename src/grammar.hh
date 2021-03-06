#ifndef CPSL_CC_GRAMMAR_HH
#define CPSL_CC_GRAMMAR_HH

#include <cstdint>    // uint64_t
#include <variant>    // std::variant

#include "lexer.hh"   // Lexeme
#include "util.h"     // STRCAT*

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

class NonterminalSymbol {
public:
};

class Branch {
public:
};

// | DECLARE_SYMBOL_CLASS* macros:
//
// DECLARE_SYMBOL_CLASS_X:
// 	Declare a non-branching symbol class with X components.
// DECLARE_SYMBOL_CLASS_X_Y:
// 	Declare a symbol class with 2 branches.  The first has X components, and
// 	the second has Y components.
// DECLARE_SYMBOL_CLASS_X_Y_Z:
// 	Declare a symbol class with 3 branches.
#define DECLARE_SYMBOL_CLASS_0( \
	S, s \
) \
class S : public NonterminalSymbol { \
public: \
	S(); \
};

#define DECLARE_SYMBOL_CLASS_1( \
	S, s, c0 \
) \
class S : public NonterminalSymbol { \
public: \
	S(uint64_t c0); \
	uint64_t c0; \
};

#define DECLARE_SYMBOL_CLASS_2( \
	S, s, c0, c1 \
) \
class S : public NonterminalSymbol { \
public: \
	S(uint64_t c0, uint64_t c1); \
	uint64_t c0; \
	uint64_t c1; \
};

#define DECLARE_SYMBOL_CLASS_3( \
	S, s, c0, c1, c2 \
) \
class S : public NonterminalSymbol { \
public: \
	S(uint64_t c0, uint64_t c1, uint64_t c2); \
	uint64_t c0; \
	uint64_t c1; \
	uint64_t c2; \
};

#define DECLARE_SYMBOL_CLASS_4( \
	S, s, c0, c1, c2, c3 \
) \
class S : public NonterminalSymbol { \
public: \
	S(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3); \
	uint64_t c0; \
	uint64_t c1; \
	uint64_t c2; \
	uint64_t c3; \
};

#define DECLARE_SYMBOL_CLASS_5( \
	S, s, c0, c1, c2, c3, c4 \
) \
class S : public NonterminalSymbol { \
public: \
	S(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3, uint64_t c4); \
	uint64_t c0; \
	uint64_t c1; \
	uint64_t c2; \
	uint64_t c3; \
	uint64_t c4; \
};

#define DECLARE_SYMBOL_CLASS_6( \
	S, s, c0, c1, c2, c3, c4, c5 \
) \
class S : public NonterminalSymbol { \
public: \
	S(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3, uint64_t c4, uint64_t c5); \
	uint64_t c0; \
	uint64_t c1; \
	uint64_t c2; \
	uint64_t c3; \
	uint64_t c4; \
	uint64_t c5; \
};

#define DECLARE_SYMBOL_CLASS_7( \
	S, s, c0, c1, c2, c3, c4, c5, c6 \
) \
class S : public NonterminalSymbol { \
public: \
	S(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3, uint64_t c4, uint64_t c5, uint64_t c6); \
	uint64_t c0; \
	uint64_t c1; \
	uint64_t c2; \
	uint64_t c3; \
	uint64_t c4; \
	uint64_t c5; \
	uint64_t c6; \
};

#define DECLARE_SYMBOL_CLASS_8( \
	S, s, c0, c1, c2, c3, c4, c5, c6, c7 \
) \
class S : public NonterminalSymbol { \
public: \
	S(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3, uint64_t c4, uint64_t c5, uint64_t c6, uint64_t c7); \
	uint64_t c0; \
	uint64_t c1; \
	uint64_t c2; \
	uint64_t c3; \
	uint64_t c4; \
	uint64_t c5; \
	uint64_t c6; \
	uint64_t c7; \
};

#define DECLARE_SYMBOL_CLASS_9( \
	S, s, c0, c1, c2, c3, c4, c5, c6, c7, c8 \
) \
class S : public NonterminalSymbol { \
public: \
	S(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3, uint64_t c4, uint64_t c5, uint64_t c6, uint64_t c7, uint64_t c8); \
	uint64_t c0; \
	uint64_t c1; \
	uint64_t c2; \
	uint64_t c3; \
	uint64_t c4; \
	uint64_t c5; \
	uint64_t c6; \
	uint64_t c7; \
	uint64_t c8; \
};

#define DECLARE_SYMBOL_CLASS_0_1( \
	S, s, B0, B1, b0, b1, b1c0 \
) \
class S : public NonterminalSymbol { \
public: \
	S(uint64_t branch, uint64_t data); \
	uint64_t branch; \
	uint64_t data; \
 \
 	enum branch_e { \
		null_branch           = 0, \
		STRCAT_2(b0, _branch) = 1, \
		STRCAT_2(b1, _branch) = 2, \
		num_branches          = 2, \
	}; \
 \
 	class B0 : public Branch { \
	public: \
		B0(); \
	}; \
 \
 	class B1 : public Branch { \
	public: \
		B1(uint64_t b1c0); \
		uint64_t b1c0; \
	}; \
};

#define DECLARE_SYMBOL_CLASS_0_2( \
	S, s, B0, B1, b0, b1, b1c0, b1c1 \
) \
class S : public NonterminalSymbol { \
public: \
	S(uint64_t branch, uint64_t data); \
	uint64_t branch; \
	uint64_t data; \
 \
 	enum branch_e { \
		null_branch           = 0, \
		STRCAT_2(b0, _branch) = 1, \
		STRCAT_2(b1, _branch) = 2, \
		num_branches          = 2, \
	}; \
 \
 	class B0 : public Branch { \
	public: \
		B0(); \
	}; \
 \
 	class B1 : public Branch { \
	public: \
		B1(uint64_t b1c0, uint64_t b1c1); \
		uint64_t b1c0; \
		uint64_t b1c1; \
	}; \
};

#define DECLARE_SYMBOL_CLASS_0_3( \
	S, s, B0, B1, b0, b1, b1c0, b1c1, b1c2 \
) \
class S : public NonterminalSymbol { \
public: \
	S(uint64_t branch, uint64_t data); \
	uint64_t branch; \
	uint64_t data; \
 \
 	enum branch_e { \
		null_branch           = 0, \
		STRCAT_2(b0, _branch) = 1, \
		STRCAT_2(b1, _branch) = 2, \
		num_branches          = 2, \
	}; \
 \
 	class B0 : public Branch { \
	public: \
		B0(); \
	}; \
 \
 	class B1 : public Branch { \
	public: \
		B1(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2); \
		uint64_t b1c0; \
		uint64_t b1c1; \
		uint64_t b1c2; \
	}; \
};

#define DECLARE_SYMBOL_CLASS_1_1( \
	S, s, B0, B1, b0, b1, b0c0, b1c0 \
) \
class S : public NonterminalSymbol { \
public: \
	S(uint64_t branch, uint64_t data); \
	uint64_t branch; \
	uint64_t data; \
 \
 	enum branch_e { \
		null_branch           = 0, \
		STRCAT_2(b0, _branch) = 1, \
		STRCAT_2(b1, _branch) = 2, \
		num_branches          = 2, \
	}; \
 \
 	class B0 : public Branch { \
	public: \
		B0(uint64_t b0c0); \
		uint64_t b0c0; \
	}; \
 \
 	class B1 : public Branch { \
	public: \
		B1(uint64_t b1c0); \
		uint64_t b1c0; \
	}; \
};

#define DECLARE_SYMBOL_CLASS_2_3( \
	S, s, B0, B1, b0, b1, b0c0, b0c1, b1c0, b1c1, b1c2 \
) \
class S : public NonterminalSymbol { \
public: \
	S(uint64_t branch, uint64_t data); \
	uint64_t branch; \
	uint64_t data; \
 \
 	enum branch_e { \
		null_branch           = 0, \
		STRCAT_2(b0, _branch) = 1, \
		STRCAT_2(b1, _branch) = 2, \
		num_branches          = 2, \
	}; \
 \
 	class B0 : public Branch { \
	public: \
		B0(uint64_t b0c0, uint64_t b0c1); \
		uint64_t b0c0; \
		uint64_t b0c1; \
	}; \
 \
 	class B1 : public Branch { \
	public: \
		B1(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2); \
		uint64_t b1c0; \
		uint64_t b1c1; \
		uint64_t b1c2; \
	}; \
};

#define DECLARE_SYMBOL_CLASS_8_8( \
	S, s, B0, B1, b0, b1, b0c0, b0c1, b0c2, b0c3, b0c4, b0c5, b0c6, b0c7, b1c0, b1c1, b1c2, b1c3, b1c4, b1c5, b1c6, b1c7 \
) \
class S : public NonterminalSymbol { \
public: \
	S(uint64_t branch, uint64_t data); \
	uint64_t branch; \
	uint64_t data; \
 \
 	enum branch_e { \
		null_branch           = 0, \
		STRCAT_2(b0, _branch) = 1, \
		STRCAT_2(b1, _branch) = 2, \
		num_branches          = 2, \
	}; \
 \
 	class B0 : public Branch { \
	public: \
		B0(uint64_t b0c0, uint64_t b0c1, uint64_t b0c2, uint64_t b0c3, uint64_t b0c4, uint64_t b0c5, uint64_t b0c6, uint64_t b0c7); \
		uint64_t b0c0; \
		uint64_t b0c1; \
		uint64_t b0c2; \
		uint64_t b0c3; \
		uint64_t b0c4; \
		uint64_t b0c5; \
		uint64_t b0c6; \
		uint64_t b0c7; \
	}; \
 \
 	class B1 : public Branch { \
	public: \
		B1(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2, uint64_t b1c3, uint64_t b1c4, uint64_t b1c5, uint64_t b1c6, uint64_t b1c7); \
		uint64_t b1c0; \
		uint64_t b1c1; \
		uint64_t b1c2; \
		uint64_t b1c3; \
		uint64_t b1c4; \
		uint64_t b1c5; \
		uint64_t b1c6; \
		uint64_t b1c7; \
	}; \
};

#define DECLARE_SYMBOL_CLASS_10_10( \
	S, s, B0, B1, b0, b1, b0c0, b0c1, b0c2, b0c3, b0c4, b0c5, b0c6, b0c7, b0c8, b0c9, b1c0, b1c1, b1c2, b1c3, b1c4, b1c5, b1c6, b1c7, b1c8, b1c9 \
) \
class S : public NonterminalSymbol { \
public: \
	S(uint64_t branch, uint64_t data); \
	uint64_t branch; \
	uint64_t data; \
 \
 	enum branch_e { \
		null_branch           = 0, \
		STRCAT_2(b0, _branch) = 1, \
		STRCAT_2(b1, _branch) = 2, \
		num_branches          = 2, \
	}; \
 \
 	class B0 : public Branch { \
	public: \
		B0(uint64_t b0c0, uint64_t b0c1, uint64_t b0c2, uint64_t b0c3, uint64_t b0c4, uint64_t b0c5, uint64_t b0c6, uint64_t b0c7, uint64_t b0c8, uint64_t b0c9); \
		uint64_t b0c0; \
		uint64_t b0c1; \
		uint64_t b0c2; \
		uint64_t b0c3; \
		uint64_t b0c4; \
		uint64_t b0c5; \
		uint64_t b0c6; \
		uint64_t b0c7; \
		uint64_t b0c8; \
		uint64_t b0c9; \
	}; \
 \
 	class B1 : public Branch { \
	public: \
		B1(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2, uint64_t b1c3, uint64_t b1c4, uint64_t b1c5, uint64_t b1c6, uint64_t b1c7, uint64_t b1c8, uint64_t b1c9); \
		uint64_t b1c0; \
		uint64_t b1c1; \
		uint64_t b1c2; \
		uint64_t b1c3; \
		uint64_t b1c4; \
		uint64_t b1c5; \
		uint64_t b1c6; \
		uint64_t b1c7; \
		uint64_t b1c8; \
		uint64_t b1c9; \
	}; \
};

#define DECLARE_SYMBOL_CLASS_1_1_0( \
	S, s, B0, B1, B2, b0, b1, b2, b0c0, b1c0 \
) \
class S : public NonterminalSymbol { \
public: \
	S(uint64_t branch, uint64_t data); \
	uint64_t branch; \
	uint64_t data; \
 \
 	enum branch_e { \
		null_branch           = 0, \
		STRCAT_2(b0, _branch) = 1, \
		STRCAT_2(b1, _branch) = 2, \
		STRCAT_2(b2, _branch) = 3, \
		num_branches          = 3, \
	}; \
 \
 	class B0 : public Branch { \
	public: \
		B0(uint64_t b0c0); \
		uint64_t b0c0; \
	}; \
 \
 	class B1 : public Branch { \
	public: \
		B1(uint64_t b1c0); \
		uint64_t b1c0; \
	}; \
 \
 	class B2 : public Branch { \
	public: \
		B2(); \
	}; \
};

#define DECLARE_SYMBOL_CLASS_1_1_1( \
	S, s, B0, B1, B2, b0, b1, b2, b0c0, b1c0, b2c0 \
) \
class S : public NonterminalSymbol { \
public: \
	S(uint64_t branch, uint64_t data); \
	uint64_t branch; \
	uint64_t data; \
 \
 	enum branch_e { \
		null_branch           = 0, \
		STRCAT_2(b0, _branch) = 1, \
		STRCAT_2(b1, _branch) = 2, \
		STRCAT_2(b2, _branch) = 3, \
		num_branches          = 3, \
	}; \
 \
 	class B0 : public Branch { \
	public: \
		B0(uint64_t b0c0); \
		uint64_t b0c0; \
	}; \
 \
 	class B1 : public Branch { \
	public: \
		B1(uint64_t b1c0); \
		uint64_t b1c0; \
	}; \
 \
 	class B2 : public Branch { \
	public: \
		B2(uint64_t b2c0); \
		uint64_t b2c0; \
	}; \
};

#define DECLARE_SYMBOL_CLASS_1_1_1_1_1_1_1_1_1_1_1( \
	S, s, B0, B1, B2, B3, B4, B5, B6, B7, B8, B9, B10, b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b0c0, b1c0, b2c0, b3c0, b4c0, b5c0, b6c0, b7c0, b8c0, b9c0, b10c0 \
) \
class S : public NonterminalSymbol { \
public: \
	S(uint64_t branch, uint64_t data); \
	uint64_t branch; \
	uint64_t data; \
 \
 	enum branch_e { \
		null_branch            = 0, \
		STRCAT_2(b0,  _branch) = 1, \
		STRCAT_2(b1,  _branch) = 2, \
		STRCAT_2(b2,  _branch) = 3, \
		STRCAT_2(b3,  _branch) = 4, \
		STRCAT_2(b4,  _branch) = 5, \
		STRCAT_2(b5,  _branch) = 6, \
		STRCAT_2(b6,  _branch) = 7, \
		STRCAT_2(b7,  _branch) = 8, \
		STRCAT_2(b8,  _branch) = 9, \
		STRCAT_2(b9,  _branch) = 10, \
		STRCAT_2(b10, _branch) = 11, \
		num_branches           = 11, \
	}; \
 \
 	class B0 : public Branch { \
	public: \
		B0(uint64_t b0c0); \
		uint64_t b0c0; \
	}; \
 \
 	class B1 : public Branch { \
	public: \
		B1(uint64_t b1c0); \
		uint64_t b1c0; \
	}; \
 \
 	class B2 : public Branch { \
	public: \
		B2(uint64_t b2c0); \
		uint64_t b2c0; \
	}; \
 \
 	class B3 : public Branch { \
	public: \
		B3(uint64_t b3c0); \
		uint64_t b3c0; \
	}; \
 \
 	class B4 : public Branch { \
	public: \
		B4(uint64_t b4c0); \
		uint64_t b4c0; \
	}; \
 \
 	class B5 : public Branch { \
	public: \
		B5(uint64_t b5c0); \
		uint64_t b5c0; \
	}; \
 \
 	class B6 : public Branch { \
	public: \
		B6(uint64_t b6c0); \
		uint64_t b6c0; \
	}; \
 \
 	class B7 : public Branch { \
	public: \
		B7(uint64_t b7c0); \
		uint64_t b7c0; \
	}; \
 \
 	class B8 : public Branch { \
	public: \
		B8(uint64_t b8c0); \
		uint64_t b8c0; \
	}; \
 \
 	class B9 : public Branch { \
	public: \
		B9(uint64_t b9c0); \
		uint64_t b9c0; \
	}; \
 \
 	class B10 : public Branch { \
	public: \
		B10(uint64_t b10c0); \
		uint64_t b10c0; \
	}; \
};

#define DECLARE_SYMBOL_CLASS_3_3_3_3_3_3_3_3_3_3_3_3_3_2_2_3_4_4_4_4_4_1_1_1_1( \
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
class S : public NonterminalSymbol { \
public: \
	S(uint64_t branch, uint64_t data); \
	uint64_t branch; \
	uint64_t data; \
 \
 	enum branch_e { \
		null_branch            = 0, \
		STRCAT_2(b0, _branch)  = 1, \
		STRCAT_2(b1, _branch)  = 2, \
		STRCAT_2(b2, _branch)  = 3, \
		STRCAT_2(b3, _branch)  = 4, \
		STRCAT_2(b4, _branch)  = 5, \
		STRCAT_2(b5, _branch)  = 6, \
		STRCAT_2(b6, _branch)  = 7, \
		STRCAT_2(b7, _branch)  = 8, \
		STRCAT_2(b8, _branch)  = 9, \
		STRCAT_2(b9, _branch)  = 10, \
		STRCAT_2(b10, _branch) = 11, \
		STRCAT_2(b11, _branch) = 12, \
		STRCAT_2(b12, _branch) = 13, \
		STRCAT_2(b13, _branch) = 14, \
		STRCAT_2(b14, _branch) = 15, \
		STRCAT_2(b15, _branch) = 16, \
		STRCAT_2(b16, _branch) = 17, \
		STRCAT_2(b17, _branch) = 18, \
		STRCAT_2(b18, _branch) = 19, \
		STRCAT_2(b19, _branch) = 20, \
		STRCAT_2(b20, _branch) = 21, \
		STRCAT_2(b21, _branch) = 22, \
		STRCAT_2(b22, _branch) = 23, \
		STRCAT_2(b23, _branch) = 24, \
		STRCAT_2(b24, _branch) = 25, \
		num_branches           = 25, \
	}; \
 \
 	class B0 : public Branch { \
	public: \
		B0(uint64_t b0c0, uint64_t b0c1, uint64_t b0c2); \
		uint64_t b0c0; \
		uint64_t b0c1; \
		uint64_t b0c2; \
	}; \
 \
 	class B1 : public Branch { \
	public: \
		B1(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2); \
		uint64_t b1c0; \
		uint64_t b1c1; \
		uint64_t b1c2; \
	}; \
 \
 	class B2 : public Branch { \
	public: \
		B2(uint64_t b2c0, uint64_t b2c1, uint64_t b2c2); \
		uint64_t b2c0; \
		uint64_t b2c1; \
		uint64_t b2c2; \
	}; \
 \
 	class B3 : public Branch { \
	public: \
		B3(uint64_t b3c0, uint64_t b3c1, uint64_t b3c2); \
		uint64_t b3c0; \
		uint64_t b3c1; \
		uint64_t b3c2; \
	}; \
 \
 	class B4 : public Branch { \
	public: \
		B4(uint64_t b4c0, uint64_t b4c1, uint64_t b4c2); \
		uint64_t b4c0; \
		uint64_t b4c1; \
		uint64_t b4c2; \
	}; \
 \
 	class B5 : public Branch { \
	public: \
		B5(uint64_t b5c0, uint64_t b5c1, uint64_t b5c2); \
		uint64_t b5c0; \
		uint64_t b5c1; \
		uint64_t b5c2; \
	}; \
 \
 	class B6 : public Branch { \
	public: \
		B6(uint64_t b6c0, uint64_t b6c1, uint64_t b6c2); \
		uint64_t b6c0; \
		uint64_t b6c1; \
		uint64_t b6c2; \
	}; \
 \
 	class B7 : public Branch { \
	public: \
		B7(uint64_t b7c0, uint64_t b7c1, uint64_t b7c2); \
		uint64_t b7c0; \
		uint64_t b7c1; \
		uint64_t b7c2; \
	}; \
 \
 	class B8 : public Branch { \
	public: \
		B8(uint64_t b8c0, uint64_t b8c1, uint64_t b8c2); \
		uint64_t b8c0; \
		uint64_t b8c1; \
		uint64_t b8c2; \
	}; \
 \
 	class B9 : public Branch { \
	public: \
		B9(uint64_t b9c0, uint64_t b9c1, uint64_t b9c2); \
		uint64_t b9c0; \
		uint64_t b9c1; \
		uint64_t b9c2; \
	}; \
 \
 	class B10 : public Branch { \
	public: \
		B10(uint64_t b10c0, uint64_t b10c1, uint64_t b10c2); \
		uint64_t b10c0; \
		uint64_t b10c1; \
		uint64_t b10c2; \
	}; \
 \
 	class B11 : public Branch { \
	public: \
		B11(uint64_t b11c0, uint64_t b11c1, uint64_t b11c2); \
		uint64_t b11c0; \
		uint64_t b11c1; \
		uint64_t b11c2; \
	}; \
 \
 	class B12 : public Branch { \
	public: \
		B12(uint64_t b12c0, uint64_t b12c1, uint64_t b12c2); \
		uint64_t b12c0; \
		uint64_t b12c1; \
		uint64_t b12c2; \
	}; \
 \
 	class B13 : public Branch { \
	public: \
		B13(uint64_t b13c0, uint64_t b13c1); \
		uint64_t b13c0; \
		uint64_t b13c1; \
	}; \
 \
 	class B14 : public Branch { \
	public: \
		B14(uint64_t b14c0, uint64_t b14c1); \
		uint64_t b14c0; \
		uint64_t b14c1; \
	}; \
 \
 	class B15 : public Branch { \
	public: \
		B15(uint64_t b15c0, uint64_t b15c1, uint64_t b15c2); \
		uint64_t b15c0; \
		uint64_t b15c1; \
		uint64_t b15c2; \
	}; \
 \
 	class B16 : public Branch { \
	public: \
		B16(uint64_t b16c0, uint64_t b16c1, uint64_t b16c2, uint64_t b16c3); \
		uint64_t b16c0; \
		uint64_t b16c1; \
		uint64_t b16c2; \
		uint64_t b16c3; \
	}; \
 \
 	class B17 : public Branch { \
	public: \
		B17(uint64_t b17c0, uint64_t b17c1, uint64_t b17c2, uint64_t b17c3); \
		uint64_t b17c0; \
		uint64_t b17c1; \
		uint64_t b17c2; \
		uint64_t b17c3; \
	}; \
 \
 	class B18 : public Branch { \
	public: \
		B18(uint64_t b18c0, uint64_t b18c1, uint64_t b18c2, uint64_t b18c3); \
		uint64_t b18c0; \
		uint64_t b18c1; \
		uint64_t b18c2; \
		uint64_t b18c3; \
	}; \
 \
 	class B19 : public Branch { \
	public: \
		B19(uint64_t b19c0, uint64_t b19c1, uint64_t b19c2, uint64_t b19c3); \
		uint64_t b19c0; \
		uint64_t b19c1; \
		uint64_t b19c2; \
		uint64_t b19c3; \
	}; \
 \
 	class B20 : public Branch { \
	public: \
		B20(uint64_t b20c0, uint64_t b20c1, uint64_t b20c2, uint64_t b20c3); \
		uint64_t b20c0; \
		uint64_t b20c1; \
		uint64_t b20c2; \
		uint64_t b20c3; \
	}; \
 \
 	class B21 : public Branch { \
	public: \
		B21(uint64_t b21c0); \
		uint64_t b21c0; \
	}; \
 \
 	class B22 : public Branch { \
	public: \
		B22(uint64_t b22c0); \
		uint64_t b22c0; \
	}; \
 \
 	class B23 : public Branch { \
	public: \
		B23(uint64_t b23c0); \
		uint64_t b23c0; \
	}; \
 \
 	class B24 : public Branch { \
	public: \
		B24(uint64_t b24c0); \
		uint64_t b24c0; \
	}; \
};

DECLARE_SYMBOL_CLASS_1(Keyword, keyword, lexeme)
DECLARE_SYMBOL_CLASS_1(Operator, operator, lexeme)
DECLARE_SYMBOL_CLASS_1(Start, start, program)
DECLARE_SYMBOL_CLASS_6(Program, program, constant_decl_opt, type_decl_opt, var_decl_opt, procedure_decl_or_function_decl_list, block, dot_operator0)
DECLARE_SYMBOL_CLASS_0_1(ConstantDeclOpt, constant_decl_opt, Empty, Value, empty, value, constant_decl)
DECLARE_SYMBOL_CLASS_0_1(TypeDeclOpt, type_decl_opt, Empty, Value, empty, value, type_decl)
DECLARE_SYMBOL_CLASS_0_1(VarDeclOpt, var_decl_opt, Empty, Value, empty, value, var_decl)
DECLARE_SYMBOL_CLASS_0_2(ProcedureDeclOrFunctionDeclList, procedure_decl_or_function_decl_list, Empty, Cons, empty, cons, procedure_decl_or_function_decl_list, procedure_decl_or_function_decl)
DECLARE_SYMBOL_CLASS_1_1(ProcedureDeclOrFunctionDecl, procedure_decl_or_function_decl, Procedure, Function, procedure, function, procedure_decl, function_decl)
DECLARE_SYMBOL_CLASS_3(ConstantDecl, constant_decl, const_keyword0, constant_assignment, constant_assignment_list)
DECLARE_SYMBOL_CLASS_0_2(ConstantAssignmentList, constant_assignment_list, Empty, Cons, empty, cons, constant_assignment_list, constant_assignment)
DECLARE_SYMBOL_CLASS_4(ConstantAssignment, constant_assignment, identifier, equals_operator0, expression, semicolon_operator0)
DECLARE_SYMBOL_CLASS_8_8(
	ProcedureDecl, procedure_decl, Forward, Definition, forward, definition,
	procedure_keyword0, identifier, leftparenthesis_operator0, formal_parameters, rightparenthesis_operator0, semicolon_operator0, forward_keyword0, semicolon_operator1,
	procedure_keyword0, identifier, leftparenthesis_operator0, formal_parameters, rightparenthesis_operator0, semicolon_operator0, body, semicolon_operator1
)
DECLARE_SYMBOL_CLASS_10_10(
	FunctionDecl, function_decl, Forward, Definition, forward, definition,
	function_keyword0, identifier, leftparenthesis_operator0, formal_parameters, rightparenthesis_operator0, colon_operator0, type, semicolon_operator0, forward_keyword0, semicolon_operator1,
	function_keyword0, identifier, leftparenthesis_operator0, formal_parameters, rightparenthesis_operator0, colon_operator0, type, semicolon_operator0, body, semicolon_operator1
)
DECLARE_SYMBOL_CLASS_0_2(FormalParameters, formal_parameters, Empty, First, empty, first, formal_parameter, formal_parameter_prefixed_list)
DECLARE_SYMBOL_CLASS_0_3(FormalParameterPrefixedList, formal_parameter_prefixed_list, Empty, Cons, empty, cons, formal_parameter_prefixed_list, semicolon_operator0, formal_parameter)
DECLARE_SYMBOL_CLASS_4(FormalParameter, formal_parameter, var_or_ref, ident_list, colon_operator0, type)
DECLARE_SYMBOL_CLASS_1_1_0(VarOrRef, var_or_ref, Var, Ref, Empty, var, ref, empty, var_keyword0, ref_keyword0)
DECLARE_SYMBOL_CLASS_4(Body, body, constant_decl_opt, type_decl_opt, var_decl_opt, block)
DECLARE_SYMBOL_CLASS_3(Block, block, begin_keyword0, statement_sequence, end_keyword0)
DECLARE_SYMBOL_CLASS_3(TypeDecl, type_decl, type_keyword0, type_assignment, type_assignment_list)
DECLARE_SYMBOL_CLASS_0_2(TypeAssignmentList, type_assignment_list, Empty, Cons, empty, cons, type_assignment_list, type_assignment)
DECLARE_SYMBOL_CLASS_4(TypeAssignment, type_assignment, identifier, equals_operator0, type, semicolon_operator0)
DECLARE_SYMBOL_CLASS_1_1_1(Type, type, Simple, Record, Array, simple, record, array, simple_type, record_type, array_type)
DECLARE_SYMBOL_CLASS_1(SimpleType, simple_type, identifier)
DECLARE_SYMBOL_CLASS_3(RecordType, record_type, record_keyword0, typed_identifier_sequence_list, end_keyword0)
DECLARE_SYMBOL_CLASS_0_2(TypedIdentifierSequenceList, typed_identifier_sequence_list, Empty, Cons, empty, cons, typed_identifier_sequence_list, typed_identifier_sequence)
DECLARE_SYMBOL_CLASS_4(TypedIdentifierSequence, typed_identifier_sequence, ident_list, colon_operator0, type, semicolon_operator0)
DECLARE_SYMBOL_CLASS_8(ArrayType, array_type, array_keyword0, leftbracket_operator0, expression0, colon_operator0, expression1, rightbracket_operator0, of_keyword0, type)
DECLARE_SYMBOL_CLASS_2(IdentList, ident_list, identifier, identifier_prefixed_list)
DECLARE_SYMBOL_CLASS_0_3(IdentifierPrefixedList, identifier_prefixed_list, Empty, Cons, empty, cons, identifier_prefixed_list, comma_operator0, identifier)
DECLARE_SYMBOL_CLASS_3(VarDecl, var_decl, var_keyword0, typed_identifier_sequence, typed_identifier_sequence_list)
DECLARE_SYMBOL_CLASS_2(StatementSequence, statement_sequence, statement, statement_prefixed_list)
DECLARE_SYMBOL_CLASS_0_3(StatementPrefixedList, statement_prefixed_list, Empty, Cons, empty, cons, statement_prefixed_list, semicolon_operator0, statement)
DECLARE_SYMBOL_CLASS_1_1_1_1_1_1_1_1_1_1_1(
	Statement, statement,
	Assignment, If, While, Repeat, For, Stop, Return, Read, Write, Call, Null_,
	assignment, if, while, repeat, for, stop, return, read, write, call, null_,
	assignment, if_statement, while_statement, repeat_statement, for_statement, stop_statement, return_statement, read_statement, write_statement, procedure_call, null_statement
)
DECLARE_SYMBOL_CLASS_3(Assignment, assignment, lvalue, colonequals_operator0, expression)
DECLARE_SYMBOL_CLASS_7(IfStatement, if_statement, if_keyword0, expression, then_keyword0, statement_sequence, elseif_clause_list, else_clause_opt, end_keyword0)
DECLARE_SYMBOL_CLASS_0_2(ElseifClauseList, elseif_clause_list, Empty, Cons, empty, cons, elseif_clause_list, elseif_clause)
DECLARE_SYMBOL_CLASS_4(ElseifClause, elseif_clause, elseif_keyword0, expression, then_keyword0, statement_sequence)
DECLARE_SYMBOL_CLASS_0_1(ElseClauseOpt, else_clause_opt, Empty, Value, empty, value, else_clause)
DECLARE_SYMBOL_CLASS_2(ElseClause, else_clause, else_keyword0, statement_sequence)
DECLARE_SYMBOL_CLASS_5(WhileStatement, while_statement, while_keyword0, expression, do_keyword0, statement_sequence, end_keyword0)
DECLARE_SYMBOL_CLASS_4(RepeatStatement, repeat_statement, repeat_keyword0, statement_sequence, until_keyword0, expression)
DECLARE_SYMBOL_CLASS_9(ForStatement, for_statement, for_keyword0, identifier, colonequals_operator0, expression0, to_or_downto, expression1, do_keyword0, statement_sequence, end_keyword0)
DECLARE_SYMBOL_CLASS_1_1(ToOrDownto, to_or_downto, To, Downto, to, downto, to_keyword0, downto_keyword0)
DECLARE_SYMBOL_CLASS_1(StopStatement, stop_statement, stop_keyword0)
DECLARE_SYMBOL_CLASS_2(ReturnStatement, return_statement, return_keyword0, expression_opt)
DECLARE_SYMBOL_CLASS_0_1(ExpressionOpt, expression_opt, Empty, Value, empty, value, expression)
DECLARE_SYMBOL_CLASS_4(ReadStatement, read_statement, read_keyword0, leftparenthesis_operator0, lvalue_sequence, rightparenthesis_operator0)
DECLARE_SYMBOL_CLASS_2(LvalueSequence, lvalue_sequence, lvalue, lvalue_prefixed_list)
DECLARE_SYMBOL_CLASS_0_3(LvaluePrefixedList, lvalue_prefixed_list, Empty, Cons, empty, cons, lvalue_prefixed_list, comma_operator0, lvalue)
DECLARE_SYMBOL_CLASS_4(WriteStatement, write_statement, write_keyword0, leftparenthesis_operator0, expression_sequence, rightparenthesis_operator0)
DECLARE_SYMBOL_CLASS_2(ExpressionSequence, expression_sequence, expression, expression_prefixed_list)
DECLARE_SYMBOL_CLASS_0_3(ExpressionPrefixedList, expression_prefixed_list, Empty, Cons, empty, cons, expression_prefixed_list, comma_operator0, expression)
DECLARE_SYMBOL_CLASS_4(ProcedureCall, procedure_call, identifier, leftparenthesis_operator0, expression_sequence_opt, rightparenthesis_operator0)
DECLARE_SYMBOL_CLASS_0_1(ExpressionSequenceOpt, expression_sequence_opt, Empty, Value, empty, value, expression_sequence)
DECLARE_SYMBOL_CLASS_0(NullStatement, null_statement)
DECLARE_SYMBOL_CLASS_3_3_3_3_3_3_3_3_3_3_3_3_3_2_2_3_4_4_4_4_4_1_1_1_1(
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
DECLARE_SYMBOL_CLASS_2(Lvalue, lvalue, identifier, lvalue_accessor_clause_list)
DECLARE_SYMBOL_CLASS_0_2(LvalueAccessorClauseList, lvalue_accessor_clause_list, Empty, Cons, empty, cons, lvalue_accessor_clause_list, lvalue_accessor_clause)
DECLARE_SYMBOL_CLASS_2_3(
	LvalueAccessorClause, lvalue_accessor_clause, Index, Array, index, array,
	dot_operator0, identifier,
	leftbracket_operator0, expression, rightbracket_operator0
)

/*
 * Grammar class.
 */

#define DECLARE_SYMBOL_FIELDS_0( \
	S, s \
) \
std::vector<S> STRCAT_2(s, _storage); \
uint64_t STRCAT_2(new_, s)();

#define DECLARE_SYMBOL_FIELDS_1( \
	S, s, c0 \
) \
std::vector<S> STRCAT_2(s, _storage); \
uint64_t STRCAT_2(new_, s)(uint64_t c0);

#define DECLARE_SYMBOL_FIELDS_2( \
	S, s, c0, c1 \
) \
std::vector<S> STRCAT_2(s, _storage); \
uint64_t STRCAT_2(new_, s)(uint64_t c0, uint64_t c1);

#define DECLARE_SYMBOL_FIELDS_3( \
	S, s, c0, c1, c2 \
) \
std::vector<S> STRCAT_2(s, _storage); \
uint64_t STRCAT_2(new_, s)(uint64_t c0, uint64_t c1, uint64_t c2);

#define DECLARE_SYMBOL_FIELDS_4( \
	S, s, c0, c1, c2, c3 \
) \
std::vector<S> STRCAT_2(s, _storage); \
uint64_t STRCAT_2(new_, s)(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3);

#define DECLARE_SYMBOL_FIELDS_5( \
	S, s, c0, c1, c2, c3, c4 \
) \
std::vector<S> STRCAT_2(s, _storage); \
uint64_t STRCAT_2(new_, s)(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3, uint64_t c4);

#define DECLARE_SYMBOL_FIELDS_6( \
	S, s, c0, c1, c2, c3, c4, c5 \
) \
std::vector<S> STRCAT_2(s, _storage); \
uint64_t STRCAT_2(new_, s)(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3, uint64_t c4, uint64_t c5);

#define DECLARE_SYMBOL_FIELDS_7( \
	S, s, c0, c1, c2, c3, c4, c5, c6 \
) \
std::vector<S> STRCAT_2(s, _storage); \
uint64_t STRCAT_2(new_, s)(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3, uint64_t c4, uint64_t c5, uint64_t c6);

#define DECLARE_SYMBOL_FIELDS_8( \
	S, s, c0, c1, c2, c3, c4, c5, c6, c7 \
) \
std::vector<S> STRCAT_2(s, _storage); \
uint64_t STRCAT_2(new_, s)(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3, uint64_t c4, uint64_t c5, uint64_t c6, uint64_t c7);

#define DECLARE_SYMBOL_FIELDS_9( \
	S, s, c0, c1, c2, c3, c4, c5, c6, c7, c8 \
) \
std::vector<S> STRCAT_2(s, _storage); \
uint64_t STRCAT_2(new_, s)(uint64_t c0, uint64_t c1, uint64_t c2, uint64_t c3, uint64_t c4, uint64_t c5, uint64_t c6, uint64_t c7, uint64_t c8);

#define DECLARE_SYMBOL_FIELDS_0_1( \
	S, s, B0, B1, b0, b1, b1c0 \
) \
std::vector<S> STRCAT_2(s, _storage); \
std::vector<S::B0> STRCAT_4(s, _, b0, _storage); \
std::vector<S::B1> STRCAT_4(s, _, b1, _storage); \
uint64_t STRCAT_4(new_, s, _, b0)(); \
uint64_t STRCAT_4(new_, s, _, b1)(uint64_t b1c0);

#define DECLARE_SYMBOL_FIELDS_0_2( \
	S, s, B0, B1, b0, b1, b1c0, b1c1 \
) \
std::vector<S> STRCAT_2(s, _storage); \
std::vector<S::B0> STRCAT_4(s, _, b0, _storage); \
std::vector<S::B1> STRCAT_4(s, _, b1, _storage); \
uint64_t STRCAT_4(new_, s, _, b0)(); \
uint64_t STRCAT_4(new_, s, _, b1)(uint64_t b1c0, uint64_t b1c1);

#define DECLARE_SYMBOL_FIELDS_0_3( \
	S, s, B0, B1, b0, b1, b1c0, b1c1, b1c2 \
) \
std::vector<S> STRCAT_2(s, _storage); \
std::vector<S::B0> STRCAT_4(s, _, b0, _storage); \
std::vector<S::B1> STRCAT_4(s, _, b1, _storage); \
uint64_t STRCAT_4(new_, s, _, b0)(); \
uint64_t STRCAT_4(new_, s, _, b1)(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2);

#define DECLARE_SYMBOL_FIELDS_1_1( \
	S, s, B0, B1, b0, b1, b0c0, b1c0 \
) \
std::vector<S> STRCAT_2(s, _storage); \
std::vector<S::B0> STRCAT_4(s, _, b0, _storage); \
std::vector<S::B1> STRCAT_4(s, _, b1, _storage); \
uint64_t STRCAT_4(new_, s, _, b0)(uint64_t b0c0); \
uint64_t STRCAT_4(new_, s, _, b1)(uint64_t b1c0);

#define DECLARE_SYMBOL_FIELDS_2_3( \
	S, s, B0, B1, b0, b1, b0c0, b0c1, b1c0, b1c1, b1c2 \
) \
std::vector<S> STRCAT_2(s, _storage); \
std::vector<S::B0> STRCAT_4(s, _, b0, _storage); \
std::vector<S::B1> STRCAT_4(s, _, b1, _storage); \
uint64_t STRCAT_4(new_, s, _, b0)(uint64_t b0c0, uint64_t b0c1); \
uint64_t STRCAT_4(new_, s, _, b1)(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2);

#define DECLARE_SYMBOL_FIELDS_8_8( \
	S, s, B0, B1, b0, b1, b0c0, b0c1, b0c2, b0c3, b0c4, b0c5, b0c6, b0c7, b1c0, b1c1, b1c2, b1c3, b1c4, b1c5, b1c6, b1c7 \
) \
std::vector<S> STRCAT_2(s, _storage); \
std::vector<S::B0> STRCAT_4(s, _, b0, _storage); \
std::vector<S::B1> STRCAT_4(s, _, b1, _storage); \
uint64_t STRCAT_4(new_, s, _, b0)(uint64_t b0c0, uint64_t b0c1, uint64_t b0c2, uint64_t b0c3, uint64_t b0c4, uint64_t b0c5, uint64_t b0c6, uint64_t b0c7); \
uint64_t STRCAT_4(new_, s, _, b1)(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2, uint64_t b1c3, uint64_t b1c4, uint64_t b1c5, uint64_t b1c6, uint64_t b1c7);

#define DECLARE_SYMBOL_FIELDS_10_10( \
	S, s, B0, B1, b0, b1, b0c0, b0c1, b0c2, b0c3, b0c4, b0c5, b0c6, b0c7, b0c8, b0c9, b1c0, b1c1, b1c2, b1c3, b1c4, b1c5, b1c6, b1c7, b1c8, b1c9 \
) \
std::vector<S> STRCAT_2(s, _storage); \
std::vector<S::B0> STRCAT_4(s, _, b0, _storage); \
std::vector<S::B1> STRCAT_4(s, _, b1, _storage); \
uint64_t STRCAT_4(new_, s, _, b0)(uint64_t b0c0, uint64_t b0c1, uint64_t b0c2, uint64_t b0c3, uint64_t b0c4, uint64_t b0c5, uint64_t b0c6, uint64_t b0c7, uint64_t b0c8, uint64_t b0c9); \
uint64_t STRCAT_4(new_, s, _, b1)(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2, uint64_t b1c3, uint64_t b1c4, uint64_t b1c5, uint64_t b1c6, uint64_t b1c7, uint64_t b1c8, uint64_t b1c9);

#define DECLARE_SYMBOL_FIELDS_1_1_0( \
	S, s, B0, B1, B2, b0, b1, b2, b0c0, b1c0 \
) \
std::vector<S> STRCAT_2(s, _storage); \
std::vector<S::B0> STRCAT_4(s, _, b0, _storage); \
std::vector<S::B1> STRCAT_4(s, _, b1, _storage); \
std::vector<S::B2> STRCAT_4(s, _, b2, _storage); \
uint64_t STRCAT_4(new_, s, _, b0)(uint64_t b0c0); \
uint64_t STRCAT_4(new_, s, _, b1)(uint64_t b1c0); \
uint64_t STRCAT_4(new_, s, _, b2)();

#define DECLARE_SYMBOL_FIELDS_1_1_1( \
	S, s, B0, B1, B2, b0, b1, b2, b0c0, b1c0, b2c0 \
) \
std::vector<S> STRCAT_2(s, _storage); \
std::vector<S::B0> STRCAT_4(s, _, b0, _storage); \
std::vector<S::B1> STRCAT_4(s, _, b1, _storage); \
std::vector<S::B2> STRCAT_4(s, _, b2, _storage); \
uint64_t STRCAT_4(new_, s, _, b0)(uint64_t b0c0); \
uint64_t STRCAT_4(new_, s, _, b1)(uint64_t b1c0); \
uint64_t STRCAT_4(new_, s, _, b2)(uint64_t b2c0);

#define DECLARE_SYMBOL_FIELDS_1_1_1_1_1_1_1_1_1_1_1( \
	S, s, B0, B1, B2, B3, B4, B5, B6, B7, B8, B9, B10, b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b0c0, b1c0, b2c0, b3c0, b4c0, b5c0, b6c0, b7c0, b8c0, b9c0, b10c0 \
) \
std::vector<S> STRCAT_2(s, _storage); \
std::vector<S::B0> STRCAT_4(s, _, b0, _storage); \
std::vector<S::B1> STRCAT_4(s, _, b1, _storage); \
std::vector<S::B2> STRCAT_4(s, _, b2, _storage); \
std::vector<S::B3> STRCAT_4(s, _, b3, _storage); \
std::vector<S::B4> STRCAT_4(s, _, b4, _storage); \
std::vector<S::B5> STRCAT_4(s, _, b5, _storage); \
std::vector<S::B6> STRCAT_4(s, _, b6, _storage); \
std::vector<S::B7> STRCAT_4(s, _, b7, _storage); \
std::vector<S::B8> STRCAT_4(s, _, b8, _storage); \
std::vector<S::B9> STRCAT_4(s, _, b9, _storage); \
std::vector<S::B10> STRCAT_4(s, _, b10, _storage); \
uint64_t STRCAT_4(new_, s, _, b0)(uint64_t b0c0); \
uint64_t STRCAT_4(new_, s, _, b1)(uint64_t b1c0); \
uint64_t STRCAT_4(new_, s, _, b2)(uint64_t b2c0); \
uint64_t STRCAT_4(new_, s, _, b3)(uint64_t b3c0); \
uint64_t STRCAT_4(new_, s, _, b4)(uint64_t b4c0); \
uint64_t STRCAT_4(new_, s, _, b5)(uint64_t b5c0); \
uint64_t STRCAT_4(new_, s, _, b6)(uint64_t b6c0); \
uint64_t STRCAT_4(new_, s, _, b7)(uint64_t b7c0); \
uint64_t STRCAT_4(new_, s, _, b8)(uint64_t b8c0); \
uint64_t STRCAT_4(new_, s, _, b9)(uint64_t b9c0); \
uint64_t STRCAT_4(new_, s, _, b10)(uint64_t b10c0);

#define DECLARE_SYMBOL_FIELDS_3_3_3_3_3_3_3_3_3_3_3_3_3_2_2_3_4_4_4_4_4_1_1_1_1( \
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
std::vector<S> STRCAT_2(s, _storage); \
std::vector<S::B0> STRCAT_4(s, _, b0, _storage); \
std::vector<S::B1> STRCAT_4(s, _, b1, _storage); \
std::vector<S::B2> STRCAT_4(s, _, b2, _storage); \
std::vector<S::B3> STRCAT_4(s, _, b3, _storage); \
std::vector<S::B4> STRCAT_4(s, _, b4, _storage); \
std::vector<S::B5> STRCAT_4(s, _, b5, _storage); \
std::vector<S::B6> STRCAT_4(s, _, b6, _storage); \
std::vector<S::B7> STRCAT_4(s, _, b7, _storage); \
std::vector<S::B8> STRCAT_4(s, _, b8, _storage); \
std::vector<S::B9> STRCAT_4(s, _, b9, _storage); \
std::vector<S::B10> STRCAT_4(s, _, b10, _storage); \
std::vector<S::B11> STRCAT_4(s, _, b11, _storage); \
std::vector<S::B12> STRCAT_4(s, _, b12, _storage); \
std::vector<S::B13> STRCAT_4(s, _, b13, _storage); \
std::vector<S::B14> STRCAT_4(s, _, b14, _storage); \
std::vector<S::B15> STRCAT_4(s, _, b15, _storage); \
std::vector<S::B16> STRCAT_4(s, _, b16, _storage); \
std::vector<S::B17> STRCAT_4(s, _, b17, _storage); \
std::vector<S::B18> STRCAT_4(s, _, b18, _storage); \
std::vector<S::B19> STRCAT_4(s, _, b19, _storage); \
std::vector<S::B20> STRCAT_4(s, _, b20, _storage); \
std::vector<S::B21> STRCAT_4(s, _, b21, _storage); \
std::vector<S::B22> STRCAT_4(s, _, b22, _storage); \
std::vector<S::B23> STRCAT_4(s, _, b23, _storage); \
std::vector<S::B24> STRCAT_4(s, _, b24, _storage); \
uint64_t STRCAT_4(new_, s, _, b0)(uint64_t b0c0, uint64_t b0c1, uint64_t b0c2); \
uint64_t STRCAT_4(new_, s, _, b1)(uint64_t b1c0, uint64_t b1c1, uint64_t b1c2); \
uint64_t STRCAT_4(new_, s, _, b2)(uint64_t b2c0, uint64_t b2c1, uint64_t b2c2); \
uint64_t STRCAT_4(new_, s, _, b3)(uint64_t b3c0, uint64_t b3c1, uint64_t b3c2); \
uint64_t STRCAT_4(new_, s, _, b4)(uint64_t b4c0, uint64_t b4c1, uint64_t b4c2); \
uint64_t STRCAT_4(new_, s, _, b5)(uint64_t b5c0, uint64_t b5c1, uint64_t b5c2); \
uint64_t STRCAT_4(new_, s, _, b6)(uint64_t b6c0, uint64_t b6c1, uint64_t b6c2); \
uint64_t STRCAT_4(new_, s, _, b7)(uint64_t b7c0, uint64_t b7c1, uint64_t b7c2); \
uint64_t STRCAT_4(new_, s, _, b8)(uint64_t b8c0, uint64_t b8c1, uint64_t b8c2); \
uint64_t STRCAT_4(new_, s, _, b9)(uint64_t b9c0, uint64_t b9c1, uint64_t b9c2); \
uint64_t STRCAT_4(new_, s, _, b10)(uint64_t b10c0, uint64_t b10c1, uint64_t b10c2); \
uint64_t STRCAT_4(new_, s, _, b11)(uint64_t b11c0, uint64_t b11c1, uint64_t b11c2); \
uint64_t STRCAT_4(new_, s, _, b12)(uint64_t b12c0, uint64_t b12c1, uint64_t b12c2); \
uint64_t STRCAT_4(new_, s, _, b13)(uint64_t b13c0, uint64_t b13c1); \
uint64_t STRCAT_4(new_, s, _, b14)(uint64_t b14c0, uint64_t b14c1); \
uint64_t STRCAT_4(new_, s, _, b15)(uint64_t b15c0, uint64_t b15c1, uint64_t b15c2); \
uint64_t STRCAT_4(new_, s, _, b16)(uint64_t b16c0, uint64_t b16c1, uint64_t b16c2, uint64_t b16c3); \
uint64_t STRCAT_4(new_, s, _, b17)(uint64_t b17c0, uint64_t b17c1, uint64_t b17c2, uint64_t b17c3); \
uint64_t STRCAT_4(new_, s, _, b18)(uint64_t b18c0, uint64_t b18c1, uint64_t b18c2, uint64_t b18c3); \
uint64_t STRCAT_4(new_, s, _, b19)(uint64_t b19c0, uint64_t b19c1, uint64_t b19c2, uint64_t b19c3); \
uint64_t STRCAT_4(new_, s, _, b20)(uint64_t b20c0, uint64_t b20c1, uint64_t b20c2, uint64_t b20c3); \
uint64_t STRCAT_4(new_, s, _, b21)(uint64_t b21c0); \
uint64_t STRCAT_4(new_, s, _, b22)(uint64_t b22c0); \
uint64_t STRCAT_4(new_, s, _, b23)(uint64_t b23c0); \
uint64_t STRCAT_4(new_, s, _, b24)(uint64_t b24c0);

// | A complete parse tree of a vector of lexemes.
//
// This class also represents a storage without partial deletion, but nothing
// will need to be deleted until the entire storage is freed.
class Grammar {
public:
	Grammar();
	Grammar(const std::vector<Lexeme> &lexemes);
	Grammar(std::vector<Lexeme> &&lexemes);

	std::vector<Lexeme> lexemes;

	DECLARE_SYMBOL_FIELDS_1(Keyword, keyword, lexeme)
	DECLARE_SYMBOL_FIELDS_1(Operator, operator, lexeme)
	DECLARE_SYMBOL_FIELDS_1(Start, start, program)
	DECLARE_SYMBOL_FIELDS_6(Program, program, constant_decl_opt, type_decl_opt, var_decl_opt, procedure_decl_or_function_decl_list, block, dot_operator0)
	DECLARE_SYMBOL_FIELDS_0_1(ConstantDeclOpt, constant_decl_opt, Empty, Value, empty, value, constant_decl)
	DECLARE_SYMBOL_FIELDS_0_1(TypeDeclOpt, type_decl_opt, Empty, Value, empty, value, type_decl)
	DECLARE_SYMBOL_FIELDS_0_1(VarDeclOpt, var_decl_opt, Empty, Value, empty, value, var_decl)
	DECLARE_SYMBOL_FIELDS_0_2(ProcedureDeclOrFunctionDeclList, procedure_decl_or_function_decl_list, Empty, Cons, empty, cons, procedure_decl_or_function_decl_list, procedure_decl_or_function_decl)
	DECLARE_SYMBOL_FIELDS_1_1(ProcedureDeclOrFunctionDecl, procedure_decl_or_function_decl, Procedure, Function, procedure, function, procedure_decl, function_decl)
	DECLARE_SYMBOL_FIELDS_3(ConstantDecl, constant_decl, const_keyword0, constant_assignment, constant_assignment_list)
	DECLARE_SYMBOL_FIELDS_0_2(ConstantAssignmentList, constant_assignment_list, Empty, Cons, empty, cons, constant_assignment_list, constant_assignment)
	DECLARE_SYMBOL_FIELDS_4(ConstantAssignment, constant_assignment, identifier, equals_operator0, expression, semicolon_operator0)
	DECLARE_SYMBOL_FIELDS_8_8(
		ProcedureDecl, procedure_decl, Forward, Definition, forward, definition,
		procedure_keyword0, identifier, leftparenthesis_operator0, formal_parameters, rightparenthesis_operator0, semicolon_operator0, forward_keyword0, semicolon_operator1,
		procedure_keyword0, identifier, leftparenthesis_operator0, formal_parameters, rightparenthesis_operator0, semicolon_operator0, body, semicolon_operator1
	)
	DECLARE_SYMBOL_FIELDS_10_10(
		FunctionDecl, function_decl, Forward, Definition, forward, definition,
		function_keyword0, identifier, leftparenthesis_operator0, formal_parameters, rightparenthesis_operator0, colon_operator0, type, semicolon_operator0, forward_keyword0, semicolon_operator1,
		function_keyword0, identifier, leftparenthesis_operator0, formal_parameters, rightparenthesis_operator0, colon_operator0, type, semicolon_operator0, body, semicolon_operator1
	)
	DECLARE_SYMBOL_FIELDS_0_2(FormalParameters, formal_parameters, Empty, First, empty, first, formal_parameter, formal_parameter_prefixed_list)
	DECLARE_SYMBOL_FIELDS_0_3(FormalParameterPrefixedList, formal_parameter_prefixed_list, Empty, Cons, empty, cons, formal_parameter_prefixed_list, semicolon_operator0, formal_parameter)
	DECLARE_SYMBOL_FIELDS_4(FormalParameter, formal_parameter, var_or_ref, ident_list, colon_operator0, type)
	DECLARE_SYMBOL_FIELDS_1_1_0(VarOrRef, var_or_ref, Var, Ref, Empty, var, ref, empty, var_keyword0, ref_keyword0)
	DECLARE_SYMBOL_FIELDS_4(Body, body, constant_decl_opt, type_decl_opt, var_decl_opt, block)
	DECLARE_SYMBOL_FIELDS_3(Block, block, begin_keyword0, statement_sequence, end_keyword0)
	DECLARE_SYMBOL_FIELDS_3(TypeDecl, type_decl, type_keyword0, type_assignment, type_assignment_list)
	DECLARE_SYMBOL_FIELDS_0_2(TypeAssignmentList, type_assignment_list, Empty, Cons, empty, cons, type_assignment_list, type_assignment)
	DECLARE_SYMBOL_FIELDS_4(TypeAssignment, type_assignment, identifier, equals_operator0, type, semicolon_operator0)
	DECLARE_SYMBOL_FIELDS_1_1_1(Type, type, Simple, Record, Array, simple, record, array, simple_type, record_type, array_type)
	DECLARE_SYMBOL_FIELDS_1(SimpleType, simple_type, identifier)
	DECLARE_SYMBOL_FIELDS_3(RecordType, record_type, record_keyword0, typed_identifier_sequence_list, end_keyword0)
	DECLARE_SYMBOL_FIELDS_0_2(TypedIdentifierSequenceList, typed_identifier_sequence_list, Empty, Cons, empty, cons, typed_identifier_sequence_list, typed_identifier_sequence)
	DECLARE_SYMBOL_FIELDS_4(TypedIdentifierSequence, typed_identifier_sequence, ident_list, colon_operator0, type, semicolon_operator0)
	DECLARE_SYMBOL_FIELDS_8(ArrayType, array_type, array_keyword0, leftbracket_operator0, expression0, colon_operator0, expression1, rightbracket_operator0, of_keyword0, type)
	DECLARE_SYMBOL_FIELDS_2(IdentList, ident_list, identifier, identifier_prefixed_list)
	DECLARE_SYMBOL_FIELDS_0_3(IdentifierPrefixedList, identifier_prefixed_list, Empty, Cons, empty, cons, identifier_prefixed_list, comma_operator0, identifier)
	DECLARE_SYMBOL_FIELDS_3(VarDecl, var_decl, var_keyword0, typed_identifier_sequence, typed_identifier_sequence_list)
	DECLARE_SYMBOL_FIELDS_2(StatementSequence, statement_sequence, statement, statement_prefixed_list)
	DECLARE_SYMBOL_FIELDS_0_3(StatementPrefixedList, statement_prefixed_list, Empty, Cons, empty, cons, statement_prefixed_list, semicolon_operator0, statement)
	DECLARE_SYMBOL_FIELDS_1_1_1_1_1_1_1_1_1_1_1(
		Statement, statement,
		Assignment, If, While, Repeat, For, Stop, Return, Read, Write, Call, Null_,
		assignment, if, while, repeat, for, stop, return, read, write, call, null_,
		assignment, if_statement, while_statement, repeat_statement, for_statement, stop_statement, return_statement, read_statement, write_statement, procedure_call, null_statement
	)
	DECLARE_SYMBOL_FIELDS_3(Assignment, assignment, lvalue, colonequals_operator0, expression)
	DECLARE_SYMBOL_FIELDS_7(IfStatement, if_statement, if_keyword0, expression, then_keyword0, statement_sequence, elseif_clause_list, else_clause_opt, end_keyword0)
	DECLARE_SYMBOL_FIELDS_0_2(ElseifClauseList, elseif_clause_list, Empty, Cons, empty, cons, elseif_clause_list, elseif_clause)
	DECLARE_SYMBOL_FIELDS_4(ElseifClause, elseif_clause, elseif_keyword0, expression, then_keyword0, statement_sequence)
	DECLARE_SYMBOL_FIELDS_0_1(ElseClauseOpt, else_clause_opt, Empty, Value, empty, value, else_clause)
	DECLARE_SYMBOL_FIELDS_2(ElseClause, else_clause, else_keyword0, statement_sequence)
	DECLARE_SYMBOL_FIELDS_5(WhileStatement, while_statement, while_keyword0, expression, do_keyword0, statement_sequence, end_keyword0)
	DECLARE_SYMBOL_FIELDS_4(RepeatStatement, repeat_statement, repeat_keyword0, statement_sequence, until_keyword0, expression)
	DECLARE_SYMBOL_FIELDS_9(ForStatement, for_statement, for_keyword0, identifier, colonequals_operator0, expression0, to_or_downto, expression1, do_keyword0, statement_sequence, end_keyword0)
	DECLARE_SYMBOL_FIELDS_1_1(ToOrDownto, to_or_downto, To, Downto, to, downto, to_keyword0, downto_keyword0)
	DECLARE_SYMBOL_FIELDS_1(StopStatement, stop_statement, stop_keyword0)
	DECLARE_SYMBOL_FIELDS_2(ReturnStatement, return_statement, return_keyword0, expression_opt)
	DECLARE_SYMBOL_FIELDS_0_1(ExpressionOpt, expression_opt, Empty, Value, empty, value, expression)
	DECLARE_SYMBOL_FIELDS_4(ReadStatement, read_statement, read_keyword0, leftparenthesis_operator0, lvalue_sequence, rightparenthesis_operator0)
	DECLARE_SYMBOL_FIELDS_2(LvalueSequence, lvalue_sequence, lvalue, lvalue_prefixed_list)
	DECLARE_SYMBOL_FIELDS_0_3(LvaluePrefixedList, lvalue_prefixed_list, Empty, Cons, empty, cons, lvalue_prefixed_list, comma_operator0, lvalue)
	DECLARE_SYMBOL_FIELDS_4(WriteStatement, write_statement, write_keyword0, leftparenthesis_operator0, expression_sequence, rightparenthesis_operator0)
	DECLARE_SYMBOL_FIELDS_2(ExpressionSequence, expression_sequence, expression, expression_prefixed_list)
	DECLARE_SYMBOL_FIELDS_0_3(ExpressionPrefixedList, expression_prefixed_list, Empty, Cons, empty, cons, expression_prefixed_list, comma_operator0, expression)
	DECLARE_SYMBOL_FIELDS_4(ProcedureCall, procedure_call, identifier, leftparenthesis_operator0, expression_sequence_opt, rightparenthesis_operator0)
	DECLARE_SYMBOL_FIELDS_0_1(ExpressionSequenceOpt, expression_sequence_opt, Empty, Value, empty, value, expression_sequence)
	DECLARE_SYMBOL_FIELDS_0(NullStatement, null_statement)
	DECLARE_SYMBOL_FIELDS_3_3_3_3_3_3_3_3_3_3_3_3_3_2_2_3_4_4_4_4_4_1_1_1_1(
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
	DECLARE_SYMBOL_FIELDS_2(Lvalue, lvalue, identifier, lvalue_accessor_clause_list)
	DECLARE_SYMBOL_FIELDS_0_2(LvalueAccessorClauseList, lvalue_accessor_clause_list, Empty, Cons, empty, cons, lvalue_accessor_clause_list, lvalue_accessor_clause)
	DECLARE_SYMBOL_FIELDS_2_3(
		LvalueAccessorClause, lvalue_accessor_clause, Index, Array, index, array,
		dot_operator0, identifier,
		leftbracket_operator0, expression, rightbracket_operator0
	)

	// | [begin, end)
	std::string lexemes_text(uint64_t begin, uint64_t end) const;
};

#endif /* #ifndef CPSL_CC_GRAMMAR_HH */
