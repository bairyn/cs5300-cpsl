#ifndef CPSL_CC_SEMANTICS_HH
#define CPSL_CC_SEMANTICS_HH

#include <cstdint>    // int32_t, uint64_t
#include <map>        // std::map
#include <optional>   // std::optional
#include <set>        // std::set
#include <string>     // std::string
#include <utility>    // std::pair
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
	// | The result of a constant expression.
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
			boolean_tag = 4,
			string_tag  = 5,
			num_tags    = 5,
		};
		typedef enum tag_e tag_t;

		class Dynamic {};

		using data_t = std::variant<
			std::monostate,
			Dynamic,
			int32_t,
			char,
			bool,
			std::string
		>;

		ConstantValue();
		ConstantValue(tag_t tag, const data_t &data);
		ConstantValue(tag_t tag, data_t &&data);
		tag_t  tag;
		data_t data;

		static const ConstantValue dynamic;
		explicit ConstantValue(int32_t integer);
		explicit ConstantValue(char char_);
		explicit ConstantValue(bool integer);
		ConstantValue(const std::string &string);
		ConstantValue(std::string &&string);

		// | Is the non-null constant value not tagged with "dynamic_tag"?
		bool is_static() const;
		bool is_dynamic() const;
		bool is_integer() const;
		bool is_char() const;
		bool is_boolean() const;
		bool is_string() const;

		// | The tags must be correct, or else an exception will be thrown, including for set_*.
		int32_t get_integer() const;
		char get_char() const;
		bool get_boolean() const;
		std::string get_string_copy() const;
		const std::string &get_string() const;
		std::string &&get_string();
		void set_integer(int32_t integer);
		void set_char(char char_);
		void set_boolean(bool boolean);
		void set_string(const std::string &string);
		void set_string(std::string &&string);

		// | Return "dynamic", "integer", "char", "boolean", or "string".
		static std::string get_tag_repr(tag_t tag);
		std::string get_tag_repr() const;
	};

	// | A representation of a type.  Another tagged union.
	class IdentifierScope;  // Forward declare class IdentifierScope.
	class Type {
	public:
		class Base {
		public:
			Base();
			Base(const std::string  &identifier, bool fixed_width = true, uint32_t size = 0);
			Base(std::string       &&identifier, bool fixed_width = true, uint32_t size = 0);
			std::string identifier;  // In the scope in which this type is visible, which identifier refers to this type?
			bool fixed_width;        // Are values of this type constrained to a fixed size?
			uint32_t size;           // How many bytes do values of this type occupy in memory?  Ignored if !fixed_width.
		};

		class Primitive : public Base {
		public:
			enum tag_e {
				null_tag     = 0,
				integer_tag  = 1,
				char_tag     = 2,
				boolean_tag  = 3,
				string_tag   = 4,
				num_tags     = 4,
			};
			typedef enum tag_e tag_t;

			Primitive();
			Primitive(const Base &base, tag_t tag);
			Primitive(Base &&base, tag_t tag);
			Primitive(tag_t tag);
			tag_t tag;

			bool is_integer() const;
			bool is_char() const;
			bool is_boolean() const;
			bool is_string() const;

			// | Return "integer", "char", "boolean", or "string".
			static std::string get_tag_repr(tag_t tag);
			std::string get_tag_repr() const;
		};

		class Simple : public Base {
		public:
			Simple();
#if 0
			Simple(const Base &base, const Type *referent);
			Simple(Base &&base, const Type *referent);
#endif /* #if 0 */
			// | referent must be non-null; this is not checked!  (TODO: check)
			Simple(const std::string &identifier, const Type *referent);
			// | referent must be non-null (not checked!), its identifier must be in scope, and the identifier must refer to a type.
			Simple(const std::string &identifier, const Type *referent, const IdentifierScope &identifier_scope);
			const Type *referent;

			// | Resolve a chain of aliases.
			//
			// This is not checked for cycles!
			// TODO: check for cycles and null.
			const Type &resolve_type() const;
		};

		class Record : public Base {
		public:
			Record();
			Record(const std::string &identifier, const std::vector<std::pair<std::string, const Type *>> &fields);
			Record(const std::string &identifier, std::vector<std::pair<std::string, const Type *>> &&fields);
			// | Ordered list of identifier, type pairs.
			std::vector<std::pair<std::string, const Type *>> fields;
		};

		class Array : public Base {
		public:
			Array();
			Array(const std::string &identifier, const Type *base_type, uint32_t min_index, uint32_t max_index);

			const Type *base_type;  // ^ This is an array of values of what type?
			uint32_t min_index;
			uint32_t max_index;

			uint32_t get_min_index() const;
			uint32_t get_max_index() const;
			uint32_t get_begin_index() const;
			uint32_t get_end_index() const;
			uint32_t get_index_range() const;
			uint32_t get_offset_of_index(uint32_t index) const;
			uint32_t get_index_of_offset(uint32_t offset) const;
		};

		enum tag_e {
			null_tag      = 0,
			primitive_tag = 1,  // ^ integer, char, boolean, or string.
			simple_tag    = 2,  // ^ An alias of another type.
			record_tag    = 3,  // ^ A record type.
			array_tag     = 4,  // ^ An array type.
			num_tags      = 4,
		};
		typedef enum tag_e tag_t;

		using data_t = std::variant<
			std::monostate,
			Primitive,
			Simple,
			Record,
			Array
		>;

		Type();
		Type(tag_t tag, const data_t &data);
		Type(tag_t tag, data_t &&data);
		tag_t tag;
		data_t data;

		const Base &get_base() const;
		Base &&get_base();

		std::string get_identifier_copy() const;
		bool        get_fixed_width() const;
		uint32_t    get_size() const;

		explicit Type(const Primitive &primitive);
		explicit Type(const Simple    &simple);
		explicit Type(const Record    &record);
		explicit Type(const Array     &array);

		explicit Type(Primitive &&primitive);
		explicit Type(Simple    &&simple);
		explicit Type(Record    &&record);
		explicit Type(Array     &&array);

		bool is_primitive() const;
		bool is_simple() const;
		bool is_record() const;
		bool is_array() const;

		// | The tags must be correct, or else an exception will be thrown, including for set_*.
		const Primitive &get_primitive() const;
		const Simple    &get_simple()    const;
		const Record    &get_record()    const;
		const Array     &get_array()     const;

		Primitive &&get_primitive();
		Simple    &&get_simple();
		Record    &&get_record();
		Array     &&get_array();

		// | Return "primitive", "simple", "record", or "array".
		static std::string get_tag_repr(tag_t tag);
		std::string get_tag_repr() const;
	};

	// | Objects represent a collection of identifiers in scope and what they refer to.
	class IdentifierScope {
	public:
		class IdentifierBinding {
		public:
			enum tag_e {
				null_tag    = 0,
				static_tag  = 1,  // ^ A constant expression.
				dynamic_tag = 2,  // ^ A dynamic expression.
				type_tag    = 3,  // ^ The identifier refers to a type.
				var_tag     = 4,  // ^ The identifier refers to a variable.
				ref_tag     = 5,  // ^ The identifier refers to a reference (pointer) to a variable.
				num_tags    = 5,
			};
			typedef enum tag_e tag_t;

			class Static {
			public:
				Static();
				Static(const ConstantValue &constant_value);
				Static(ConstantValue &&constant_value);

				// Keep a copy of the constant value.
				ConstantValue constant_value;
			};
			class Dynamic {
			public:
				// TODO
			};
			using Type = ::Semantics::Type;
			class Var {
			public:
				// TODO
			};
			class Ref {
			public:
				// TODO
			};

			using data_t = std::variant<
				std::monostate,
				Static,
				Dynamic,
				Type,
				Var,
				Ref
			>;

			IdentifierBinding();
			IdentifierBinding(tag_t tag, const data_t &data);
			IdentifierBinding(tag_t tag, data_t &&data);
			tag_t  tag;
			data_t data;

			explicit IdentifierBinding(const Static &static_);
			explicit IdentifierBinding(const Dynamic &dynamic);
			explicit IdentifierBinding(const Type &type);
			explicit IdentifierBinding(const Var &var);
			explicit IdentifierBinding(const Ref &ref);

			explicit IdentifierBinding(Static &&static_);
			explicit IdentifierBinding(Dynamic &&dynamic);
			explicit IdentifierBinding(Type &&type);
			explicit IdentifierBinding(Var &&var);
			explicit IdentifierBinding(Ref &&ref);

			bool is_static() const;
			bool is_dynamic() const;
			bool is_type() const;
			bool is_var() const;
			bool is_ref() const;

			// | The tags must be correct, or else an exception will be thrown, including for set_*.
			const Static  &get_static()  const;
			const Dynamic &get_dynamic() const;
			const Type    &get_type()    const;
			const Var     &get_var()     const;
			const Ref     &get_ref()     const;

			Static  &get_static();
			Dynamic &get_dynamic();
			Type    &get_type();
			Var     &get_var();
			Ref     &get_ref();

			// | Return "static", "dynamic", "type", "var", or "ref".
			static std::string get_tag_repr(tag_t tag);
			std::string get_tag_repr() const;
		};

		IdentifierScope();
		IdentifierScope(const std::map<std::string, IdentifierBinding> &scope);
		IdentifierScope(std::map<std::string, IdentifierBinding> &&scope);

		std::map<std::string, IdentifierBinding> scope;

		bool has(std::string identifier) const;

		const IdentifierBinding  &get(std::string identifier) const;
		IdentifierBinding       &&get(std::string identifier);

		const IdentifierBinding  &operator[](std::string identifier) const;
		IdentifierBinding       &&operator[](std::string identifier);

		std::optional<IdentifierBinding> lookup_copy(std::string identifier) const;
	};

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
		// Note: We don't record the identifier scope here.  There is only one
		// identifier scope for each expression.
		const IdentifierScope &expression_scope
	);

	static bool would_addition_overflow(int32_t a, int32_t b);
	static bool would_multiplication_overflow(int32_t a, int32_t b);
	static bool would_division_overflow(int32_t a, int32_t b);
	static int32_t euclidian_div(int32_t a, int32_t b);
	static int32_t euclidian_mod(int32_t a, int32_t b);

	// | Clear memoization caches and calculated output values.
	void clear_output();

	// | Force a re-analysis of the semantics data.
	void analyze();

	// TODO: visibility
	IdentifierScope top_level_scope;
	std::set<std::string> string_constants;

protected:
	// | The grammar tree used for the semantics data.
	Grammar grammar;
	// | Whether to automatically construct the semantics analysis after loading the grammar.
	bool auto_analyze;

	std::map<uint64_t, ConstantValue> is_expression_constant_calculations;
};

#endif /* #ifndef CPSL_CC_SEMANTICS_HH */
