#ifndef CPSL_CC_SEMANTICS_HH
#define CPSL_CC_SEMANTICS_HH

#include <cstdint>    // uint32_t, uint64_t
#include <map>        // std::map
#include <string>     // std::string
#include <vector>     // std::vector
#include <variant>    // std::monostate, std::variant

#include "grammar.hh"

/*
 * Exceptions types.
 */

class SemanticsError : public std::runtime_error {
public:
	SemanticsError();
	SemanticsError(const std::string &message);
};

/*
 * Grammar types.
 */

class Semantics {
public:
	class ConstantValue {
	public:
		// | Is this value considered to be a constant calculable at compile-time?
		//
		// If so, which type of constant is it?
		enum tag_e {
			null_tag    = 0,
			dynamic_tag = 1,
			integer_tag = 2,
			char_tag    = 3,
			string_tag  = 4,
			num_tags    = 4,
		};
		typedef enum tag_e tag_t;

		class Dynamic {};

		using data_t = std::variant<
			std::monostate,
			Dynamic,
			uint32_t,
			char,
			std::string
		>;

		ConstantValue();
		ConstantValue(tag_t tag, const data_t &data);
		ConstantValue(tag_t tag, data_t &&data);
		tag_t  tag;
		data_t data;

		static const ConstantValue dynamic;
		explicit ConstantValue(uint32_t integer);
		explicit ConstantValue(char char_);
		ConstantValue(const std::string &string);
		ConstantValue(std::string &&string);

		// | Is the non-null constant value not tagged with "dynamic_tag"?
		bool is_static() const;
		bool is_dynamic() const;
		bool is_integer() const;
		bool is_char() const;
		bool is_string() const;

		// | The tags must be correct, or else an exception will be thrown, including for set_*.
		uint32_t get_integer() const;
		char get_char() const;
		std::string get_string() const;
		void set_integer(uint32_t integer);
		void set_char(char char_);
		void set_string(const std::string &string);
		void set_string(std::string &&string);
	};

	/*
	class TypeDecl {
	public:
		TypeDecl();
		uint64_t type_decl;
		uint32_t size;
	};
	*/

	Semantics();
	Semantics(bool auto_analyze);
	Semantics(const Grammar &grammar, bool auto_analyze = true);
	Semantics(Grammar &&grammar, bool auto_analyze = true);

	const Grammar get_grammar() const;
	void set_grammar(const Grammar &grammar);
	void set_grammar(Grammar &&grammar);

	// TODO: constants.
	//std::

	// TODO: std::vector<std::string> string_literals;

	// | Determine whether the expression in the grammar tree is a constant expression.
	// The result will be memoized in is_expression_constant_calculations.
	ConstantValue is_expression_constant(
		// | Reference to the expression in the grammar tree.
		uint64_t expression,
		// | A collection of identifiers of constants available to the scope of the expression.
		const std::map<std::string, ConstantValue> &const_identifier_scope
	);

	// | Force a re-analysis of the semantics data.
	void analyze();

protected:
	// | The grammar tree used for the semantics data.
	Grammar grammar;
	// | Whether to automatically construct the semantics analysis after loading the grammar.
	bool auto_analyze;

	std::map<uint64_t, ConstantValue> is_expression_constant_calculations;
};

#endif /* #ifndef CPSL_CC_SEMANTICS_HH */
