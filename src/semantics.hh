#ifndef CPSL_CC_SEMANTICS_HH
#define CPSL_CC_SEMANTICS_HH

#include <cstdint>    // int32_t, uint32_t, uint64_t
#include <map>        // std::map
#include <optional>   // std::optional
#include <set>        // std::set
#include <string>     // std::string
#include <utility>    // std::pair
#include <vector>     // std::vector
#include <variant>    // std::monostate, std::variant

#include "grammar.hh"

extern "C" {
#include "util.h"     // A_BILLION
}

// MIPS overview (the project uses MARS rather than SPIM): https://minnie.tuhs.org/CompArch/Resources/mips_quick_tutorial.html
// MIPS green sheet: https://inst.eecs.berkeley.edu/~cs61c/resources/MIPS_Green_Sheet.pdf
// MARS syscalls: https://courses.missouristate.edu/KenVollmar/MARS/Help/SyscallHelp.html

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

#define CPSL_CC_SEMANTICS_COMBINE_IDENTIFIER_NAMESPACES false
#define CPSL_CC_SEMANTICS_MAX_UNIQUE_TRY_ITERATIONS     A_BILLION  // fun
#define CPSL_CC_SEMANTICS_MAX_STRING_REQUESTED_LABEL_SUFFIX_LENGTH 32
#define CPSL_CC_SEMANTICS_ALL_ARRAY_RECORDS_ARE_REFS    false

class Semantics {
public:
	static const bool combine_identifier_namespaces;

	// | In the assembled output, locations marked as symbols will be replaced
	// with a unique and consistent substring.
	//
	// The Symbol class represents what the substring will begin with and a
	// requested suffix that will be used if it does not cause collisions with
	// other symbols or other entities depending on the context, e.g. identifiers.
	//
	// These are used to generate labels in the assembly output.
	class Symbol {
	public:
		Symbol();
		Symbol(const std::string &prefix, const std::string &requested_suffix, uint64_t unique_identifier = 0);
		std::string prefix;
		std::string requested_suffix;
		uint64_t unique_identifier = 0;

		static const uint64_t max_unique_try_iterations;

		static std::map<Symbol, std::string> generate_symbol_values(const std::set<Symbol> &symbols, const std::set<std::string> additional_names);
	};

	// | Output strings, possibly with unexpanded symbols.
	class Output {
	public:
		enum section_e {
			null_section        = 0,
			strings_section     = 1,
			global_vars_section = 2,
			text_section        = 3,
			num_sections        = 3,
		};
		typedef enum section_e section_t;

		class SymbolLocation {
		public:
			SymbolLocation();
			SymbolLocation(section_t section, std::vector<std::string>::size_type line, std::string::size_type start_pos = 0, std::string::size_type length = 0);
			section_t                           section;
			std::vector<std::string>::size_type line;
			std::string::size_type              start_pos;
			std::string::size_type              length;

			// | Returns true if b is less than a, so that when used with
			// std::stable_sort from <algorithm> as the 3rd argument, a vector
			// can be reverse sorted.
			static bool reverse_cmp(const SymbolLocation &a, const SymbolLocation &b);
		};

		// | For convenience, a wrapper class for a line of output that can contain symbols.
		class Line {
		public:
			Line();
			// | No symbols on this line.
			Line(const std::string &line);
			Line(std::string &&line);
			Line(const char *c_str);
			// | Line with only a symbol.
			Line(const Symbol &symbol);
			// | Add a symbol to a line in a new copy.
			Line(const Line &line, const Symbol &symbol, std::string::size_type start_pos = 0, std::string::size_type length = 0);
			// Commented out: these declarations are redundant and would cause some calls to be ambiguous.
#if 0
			// | Line with a single symbol.
			Line(const std::string &line, const Symbol &symbol, std::string::size_type start_pos = 0, std::string::size_type length = 0);
			Line(std::string &&line, const Symbol &symbol, std::string::size_type start_pos = 0, std::string::size_type length = 0);
#endif /* #if 0 */
			// | Line with zero or more symbols.
			Line(const std::string &line, const std::vector<std::pair<Symbol, std::pair<std::string::size_type, std::string::size_type>>> &symbols);
			Line(const std::string &line, std::vector<std::pair<Symbol, std::pair<std::string::size_type, std::string::size_type>>> &&symbols);
			Line(std::string &&line, const std::vector<std::pair<Symbol, std::pair<std::string::size_type, std::string::size_type>>> &symbols);
			Line(std::string &&line, std::vector<std::pair<Symbol, std::pair<std::string::size_type, std::string::size_type>>> &&symbols);
			// | Add zero or more symbols to a line in a new copy.
			Line(const Line &line, const std::vector<std::pair<Symbol, std::pair<std::string::size_type, std::string::size_type>>> &symbols);
			// | Fields.
			std::string line;
			std::vector<std::pair<Symbol, std::pair<std::string::size_type, std::string::size_type>>> symbols;

			// Note: operator+ is defined for Line and Line/Symbol using these methods.
			Line plus(const Line &b) const;
			Line plus(const Symbol &b) const;
			Line flip_plus(const Line &a) const;
			Line flip_plus(const Symbol &a) const;
		};

		Output();

		// | The lines of output, possibly with unexpanded symbol locations.
		std::vector<std::vector<std::string>> sections;

		// | The symbol table.
		std::map<Symbol, std::vector<SymbolLocation>> unexpanded_symbols;
		std::map<std::pair<section_t, std::vector<std::string>::size_type>, std::vector<Symbol>> reverse_unexpanded_symbols;

		// | If present, all lines joined together with symbols expanded.
		std::vector<std::string> normalized_lines;

		// | Add a symbol location.
		void add_symbol_location(const Symbol &symbol, const SymbolLocation &symbol_location);

		// | Does this output contain no unexpanded symbols?
		bool is_normalized() const;

		// | Return a new output, expanding unexpanded symbols, so that they
		// are unique and different from any element of "additional_names".
		//
		// If this output is already normalized, return a copy of this output.
		Output normalize(const std::set<std::string> &additional_names = std::set<std::string>()) const;

		// | Normalize this output if it isn't normalized to a new value, and
		// discard the new output container after returning a copy of its
		// lines.
		//
		// If this output is already normalized, then additional_names is ignored.
		std::vector<std::string> get_normalized_lines_copy(const std::set<std::string> &additional_names = std::set<std::string>()) const;

		// | Add a line to an output section.
		// Commented out: this declaration is redundant and would cause some calls to be ambiguous.
#if 0
		void add_line(section_t section, const std::string &line);
#endif /* #if 0 */
		// | Add a line to an output section with a symbol.
		void add_line(section_t section, const std::string &line, const Symbol &symbol, std::string::size_type start_pos = 0, std::string::size_type length = 0);
		// | Add a line to an output, optionally with symbols.
		void add_line(section_t section, const Line &line);
		// | Add lines to output, optionally with symbols.
		void add_lines(section_t section, const std::vector<Line> &lines);
		// | Add a symbol to the last line.
		void add_symbol_location_current_last_line(section_t section, const Symbol &symbol, std::string::size_type start_pos = 0, std::string::size_type length = 0);

		// | Is the section empty?
		bool is_section_empty(section_t section) const;
	};

	class IdentifierScope;  // Forward declare class IdentifierScope.
	// | A representation of a type.  Another tagged union.
	//
	// Note: this class uses raw Type pointers.  The lifetime of them should
	// not exceed the lifetime of the IdentifierScope in which the Type is
	// stored.
	class Type {
	public:
		// | Anonymous types have .identifier == "".
		class Base {
		public:
			Base();
			Base(const std::string  &identifier, bool fixed_width = true, uint32_t size = 0);
			Base(std::string       &&identifier, bool fixed_width = true, uint32_t size = 0);
			std::string identifier;  // In the scope in which this type is visible, which identifier refers to this type?
			bool fixed_width;        // Are values of this type constrained to a fixed size?
			uint32_t size;           // How many bytes do values of this type occupy in memory?  Ignored if !fixed_width.

			const std::string &get_identifier()  const;
			bool               get_fixed_width() const;
			uint32_t           get_size()        const;
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

			static const Primitive integer_type;
			static const Primitive char_type;
			static const Primitive boolean_type;
			static const Primitive string_type;

			bool is_integer() const;
			bool is_char() const;
			bool is_boolean() const;
			bool is_string() const;

			// | Return "integer", "char", "boolean", or "string".
			static std::string get_tag_repr(tag_t tag);
			std::string get_tag_repr() const;

			// | Determine whether a 4-byte MIPS word is needed to store this value rather than only 1 byte.
			// All constant value types can fit into either 4 bytes or 1 bytes; they are fixed size.
			bool is_word(bool permit_in_between_size = true) const;

			inline bool operator< (const Primitive &other) const;
			inline bool operator> (const Primitive &other) const;
			inline bool operator<=(const Primitive &other) const;
			inline bool operator>=(const Primitive &other) const;

			inline bool operator==(const Primitive &other) const;
			inline bool operator!=(const Primitive &other) const;
		};

		class Simple : public Base {
		public:
			Simple();
#if 0
			Simple(const Base &base, const Type *referent);
			Simple(Base &&base, const Type *referent);
#endif /* #if 0 */
			// | referent must be non-null; this is not checked!  (TODO: check)
			Simple(const std::string &identifier, const Type &referent);
			// | referent must be non-null (not checked!), its identifier must be in scope, and the identifier must refer to a type.
			Simple(const std::string &identifier, const Type &referent, const IdentifierScope &identifier_type_scope);
			const Type *referent;

			// | Resolve a chain of aliases.
			const Type &resolve_type(bool check_cycles = true) const;

			inline bool operator< (const Simple &other) const;
			inline bool operator> (const Simple &other) const;
			inline bool operator<=(const Simple &other) const;
			inline bool operator>=(const Simple &other) const;

			inline bool operator==(const Simple &other) const;
			inline bool operator!=(const Simple &other) const;
		};

		class Record : public Base {
		public:
			Record();
			Record(const std::string &identifier, const std::vector<std::pair<std::string, const Type *>> &fields, IdentifierScope &anonymous_storage);
			Record(const std::string &identifier, std::vector<std::pair<std::string, const Type *>> &&fields, IdentifierScope &anonymous_storage);
			// | Ordered list of identifier, type pairs.
			std::vector<std::pair<std::string, const Type *>> fields;
			// | Storage of anonymous types used by this record.
			IdentifierScope *anonymous_storage;

			// | Used for comparison and equality checking.
			std::vector<std::pair<std::string, Type>> get_dereferenced_fields() const;

			inline bool operator< (const Record &other) const;
			inline bool operator> (const Record &other) const;
			inline bool operator<=(const Record &other) const;
			inline bool operator>=(const Record &other) const;

			inline bool operator==(const Record &other) const;
			inline bool operator!=(const Record &other) const;
		};

		class Array : public Base {
		public:
			Array();
			Array(const std::string &identifier, const Type &base_type, int32_t min_index, int32_t max_index, IdentifierScope &anonymous_storage);

			const Type *base_type;  // ^ This is an array of values of what type?
			int32_t min_index;
			int32_t max_index;
			// | Storage of anonymous types used by this array.
			IdentifierScope *anonymous_storage;

			int32_t get_min_index() const;
			int32_t get_max_index() const;
			int32_t get_begin_index() const;
			int32_t get_end_index() const;
			uint32_t get_index_range() const;
			uint32_t get_offset_of_index(int32_t index) const;
			int32_t get_index_of_offset(uint32_t offset) const;

			inline bool operator< (const Array &other) const;
			inline bool operator> (const Array &other) const;
			inline bool operator<=(const Array &other) const;
			inline bool operator>=(const Array &other) const;

			inline bool operator==(const Array &other) const;
			inline bool operator!=(const Array &other) const;
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

		static const Type integer_type;
		static const Type char_type;
		static const Type boolean_type;
		static const Type string_type;

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

		// | If this is a type alias, resolve the type to get the base type;
		// otherwise, just return this type.
		const Type &resolve_type(bool check_cycles = true) const;

		inline bool operator< (const Type &other) const;
		inline bool operator> (const Type &other) const;
		inline bool operator<=(const Type &other) const;
		inline bool operator>=(const Type &other) const;

		inline bool operator==(const Type &other) const;
		inline bool operator!=(const Type &other) const;
	};

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

		class Dynamic {
		public:
			static const Dynamic dynamic;
		};

		using data_t = std::variant<
			std::monostate,
			Dynamic,
			int32_t,
			char,
			bool,
			std::string
		>;

		ConstantValue();
		ConstantValue(tag_t tag, const data_t &data, uint64_t lexeme_begin = 0, uint64_t lexeme_end = 0);
		ConstantValue(tag_t tag, data_t &&data, uint64_t lexeme_begin = 0, uint64_t lexeme_end = 0);
		// | Copy the constant value but use new lexeme identifiers.
		ConstantValue(const ConstantValue &constant_value, uint64_t lexeme_begin, uint64_t lexeme_end);
		ConstantValue(ConstantValue &&constant_value, uint64_t lexeme_begin, uint64_t lexeme_end);
		tag_t  tag;
		data_t data;
		uint64_t lexeme_begin = 0;
		uint64_t lexeme_end   = 0;

		static const ConstantValue true_constant;
		static const ConstantValue false_constant;

		ConstantValue(const Dynamic &dynamic, uint64_t lexeme_begin, uint64_t lexeme_end);
		ConstantValue(Dynamic &&dynamic, uint64_t lexeme_begin, uint64_t lexeme_end);
		explicit ConstantValue(int32_t integer, uint64_t lexeme_begin, uint64_t lexeme_end);
		explicit ConstantValue(char char_, uint64_t lexeme_begin, uint64_t lexeme_end);
		explicit ConstantValue(bool integer, uint64_t lexeme_begin, uint64_t lexeme_end);
		ConstantValue(const std::string &string, uint64_t lexeme_begin, uint64_t lexeme_end);
		ConstantValue(std::string &&string, uint64_t lexeme_begin, uint64_t lexeme_end);

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

		// | Get the primitive type of the constant value, which must be static.
		//
		// Raise an error if it's not static.
		Type::Primitive get_static_primitive_type() const;
		Type            get_static_type() const;

		// | Get a string representation of the static value.
		std::string get_static_repr() const;

		static const std::map<char, std::string> char_escapes;
		static const std::map<std::string, char> reverse_char_escapes;

		static std::string escape_char(char char_);
		static std::string quote_char(char char_);
		static std::string quote_string(const std::string &string);
	};

	// | A representation of a storage unit: a register, offset on the stack, global address, etc.
	//
	// $t8 and $t9 are reserved in case any working storage units require 2
	// registers to access.  The maximum number of registers required to access
	// a working storage unit is 2.
	//
	// There are 4 storage types:
	// 	1: global_address + x
	// 	2: x(global_address)
	// 	3: $reg
	// 	4: x($reg)
	//
	// Pointers are 4-byte MIPS words, so if dereferencing, max_size refers to
	// the pointed-to data, not the 4-byte MIPS word pointer.
	class Storage {
	public:
		Storage();
		Storage(uint32_t max_size, bool is_global, Symbol global_address, const std::string &register_, bool dereference, int32_t offset, bool no_sp_adjust = false, bool is_caller_preserved = false);
		// Specialized storage constructors.
		// | Storage type #1: (&global) + x, and
		// | Storage type #2: ((uint8_t *) &global)[x];  -- if global is already a byte pointer/array, global[x].
		Storage(Symbol global_address, bool dereference, uint32_t max_size, int32_t offset = 0);
		// | Storage type #3: 4-byte direct register.  (No dereference.)  To
		// use a 4-byte direct register as a 1-byte storage, you'll need to use
		// the normal constructor.
		Storage(const std::string &register_, bool no_sp_adjust = false);
		// | Storage type #4: dereferenced register.
		Storage(const std::string &register_, uint32_t max_size, int32_t offset = 0, bool no_sp_adjust = false);

		// | What is the maximum size that this storage can handle?
		uint32_t    max_size = 0;
		// | Is this a global address or a register?
		bool        is_global = false;
		// | If global, which symbol refers to the address?
		Symbol      global_address;
		// | If this is a register, which string identifies it?
		std::string register_;
		// | Dereference this global address or register, or use it directly?
		bool        dereference = false;
		// | If global and/or dereferencing, add this value to the loaded result.  If dereferencing, add before dereferencing.  (Does nothing when neither global or dereferencing.)
		int32_t     offset = 0;

		// | If this is a dereferenced SP, adjust offset according to AddSp instructions?
		bool no_sp_adjust = false;

		// | If this is a register, does the caller preserve it?
		bool is_caller_preserved = false;

		// | Is this size ideal with for this storage?  (Does it equal max_size?)
		bool ideal_size(uint32_t size) const;
		// | Is this size compatible with this storage?  (Is it <= max_size?)
		bool compatible_size(uint32_t size) const;

		static std::vector<uint32_t> get_sizes(const std::vector<Storage> &storage);

		// Type of the register.
		enum type_e {
			null_type = 0,
			global_address_type       = 1,
			global_dereference_type   = 2,
			register_direct_type      = 3,
			register_dereference_type = 4,
			num_types                 = 4,
		};
		typedef type_e type_t;
		type_t get_type() const;

		// | Note: is_global_address() == true means it's global and *not* a dereference.
		bool is_global_address()       const;
		bool is_global_dereference()   const;
		bool is_register_direct()      const;
		bool is_register_dereference() const;

		using Index = std::vector<Storage>::size_type;
	};

	// | Objects represent a collection of identifiers in scope and what they refer to.
	class IdentifierScope {
	public:
		class IdentifierBinding {
		public:
			enum tag_e {
				null_tag                = 0,
				static_tag              = 1,  // ^ A constant expression.
				type_tag                = 2,  // ^ The identifier refers to a type.
				var_tag                 = 3,  // ^ The identifier refers to a variable.
				routine_declaration_tag = 4,  // ^ The identifier refers to a function or procedure.
				num_tags                = 4,
			};
			typedef enum tag_e tag_t;

			// | Constant expression / static value.
			class Static {
			public:
				Static();
				Static(const ConstantValue &constant_value);
				Static(ConstantValue &&constant_value);

				// Keep a copy of the constant value.
				ConstantValue constant_value;
			};
			using Type = ::Semantics::Type;
			// | Typed fixed storage.
			class Var {
			public:
				Var();
				Var(const Type &type, const Storage &storage);
				const Type *type;
				Storage storage;
			};

			// | Inputs and output.
			class RoutineDeclaration {
			public:
				RoutineDeclaration();
				RoutineDeclaration(const Symbol &location, const std::vector<std::pair<bool, const Type *>> &parameters, std::optional<const Type *> output);
				Symbol location;
				// | <is_ref, type>
				std::vector<std::pair<bool, const Type *>> parameters;
				std::optional<const Type *>                output;

				std::vector<std::pair<bool, Type>> get_dereferenced_parameters() const;
				std::optional<Type>                get_dereferenced_output()     const;

				inline bool operator< (const RoutineDeclaration &other) const;
				inline bool operator> (const RoutineDeclaration &other) const;
				inline bool operator<=(const RoutineDeclaration &other) const;
				inline bool operator>=(const RoutineDeclaration &other) const;

				inline bool operator==(const RoutineDeclaration &other) const;
				inline bool operator!=(const RoutineDeclaration &other) const;
			};

			using data_t = std::variant<
				std::monostate,
				Static,
				Type,
				Var,
				RoutineDeclaration
			>;

			IdentifierBinding();
			IdentifierBinding(tag_t tag, const data_t &data);
			IdentifierBinding(tag_t tag, data_t &&data);
			tag_t  tag;
			data_t data;

			explicit IdentifierBinding(const Static &static_);
			explicit IdentifierBinding(const Type &type);
			explicit IdentifierBinding(const Var &var);
			explicit IdentifierBinding(const RoutineDeclaration &ref);

			explicit IdentifierBinding(Static &&static_);
			explicit IdentifierBinding(Type &&type);
			explicit IdentifierBinding(Var &&var);
			explicit IdentifierBinding(RoutineDeclaration &&ref);

			bool is_static() const;
			bool is_type() const;
			bool is_var() const;
			bool is_routine_declaration() const;

			// | The tags must be correct, or else an exception will be thrown, including for set_*.
			const Static             &get_static()              const;
			const Type               &get_type()                const;
			const Var                &get_var()                 const;
			const RoutineDeclaration &get_routine_declaration() const;

			Static             &get_static();
			Type               &get_type();
			Var                &get_var();
			RoutineDeclaration &get_routine_declaration();

			// | Return "static", "type", "var", or "routine_declaration".
			static std::string get_tag_repr(tag_t tag);
			std::string get_tag_repr() const;
		};

		IdentifierScope();
		IdentifierScope(const std::map<std::string, IdentifierBinding> &scope);
		IdentifierScope(std::map<std::string, IdentifierBinding> &&scope);

		// | Identifier bindings mapped by identifier strings.
		std::map<std::string, IdentifierBinding> scope;
		// | Anonymous identifier bindings mapped by index.
		std::vector<IdentifierBinding> anonymous_bindings;

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

	// | Get a copy of the normalized output lines.
	std::vector<std::string> get_normalized_output_lines_copy() const;

	const Grammar get_grammar() const;
	void set_grammar(const Grammar &grammar);
	void set_grammar(Grammar &&grammar);

	// | Determine whether the expression in the grammar tree is a constant expression.
	ConstantValue is_expression_constant(
		// | Reference to the expression in the grammar tree.
		uint64_t expression,
		// | A collection of identifiers of constants available to the scope of the expression.
		// Note: We don't record the identifier scope here.  There is only one
		// identifier scope for each expression.
		const IdentifierScope &expression_constant_scope
	) const;
	ConstantValue is_expression_constant(const ::Expression &expression, const IdentifierScope &expression_constant_scope) const;

	// | From the parse tree Type, construct a Semantics::Type that represents the type.
	//
	// If the type contains anonymous subtypes, they will be stored in
	// "storage".  In this case, the lifetime of the type should not exceed the
	// lifetime of the storage, since the type will contain raw pointers to it.
	//
	// The lifetimes of types should not exceed the lifetime of their
	// referents, which are normally stored inside of the type_type_scope
	// IdentifierScope passed to this method.
	Type analyze_type(const std::string &identifier, const ::Type &type, const IdentifierScope &type_constant_scope, const IdentifierScope &type_type_scope, IdentifierScope &anonymous_storage);

	// | An intermediate unit representation of MIPS instructions.
	//
	// "Registers" may be registers or other types of storage, e.g. offsets on the stack.  Which type they are can be analyzed afterward.
	//
	// In an instruction, "registers" have an index into the virtually
	// concatenated vector of workings + inputs + outputs.  e.g. if these
	// vectors have lengths 2, 8, 8, an index of 3 refers to inputs[1].
	class Instruction {
	public:
		enum tag_e {
			null_tag               = 0,
			ignore_tag             = 1,
			custom_tag             = 2,
			syscall_tag            = 3,
			add_sp_tag             = 4,
			load_immediate_tag     = 5,
			load_from_tag          = 6,
			less_than_from_tag     = 7,
			nor_from_tag           = 8,
			and_from_tag           = 9,
			or_from_tag            = 10,
			add_from_tag           = 11,
			sub_from_tag           = 12,
			mult_from_tag          = 13,
			div_from_tag           = 14,
			jump_to_tag            = 15,
			jump_tag               = 16,
			call_tag               = 17,
			return_tag             = 18,
			branch_zero_tag        = 19,
			branch_nonnegative_tag = 20,
			num_tags               = 20,
		};
		typedef enum tag_e tag_t;

		class Base {
		public:
			Base();
			Base(bool has_symbol, Symbol symbol);
			// | Does this instruction have a label at the beginning of this instruction?
			bool has_symbol;
			Symbol symbol;

			std::vector<uint32_t> get_input_sizes() const;
			std::vector<uint32_t> get_working_sizes() const;
			std::vector<uint32_t> get_output_sizes() const;
			std::vector<uint32_t> get_all_sizes() const;

			std::vector<Output::Line> emit(const std::vector<Storage> &storages) const;
		};

		// | Pseudoinstruction: consume but ignore the output value, so that
		// when instruction graphs are emitted, the output will end up in a
		// temporary storage unit that can be re-used.
		//
		// Emits no output except for a symbol, if one is provided.
		//
		// This can also be used to add a pseudoinstruction nop that only emits a label as a symbol, by setting "has_input" to false.
		class Ignore : public Base {
		public:
			Ignore();
			Ignore(const Base &base, bool has_input = true, bool is_word = true);
			bool has_input;
			// | Are we loading a byte or a word?
			bool is_word;

			std::vector<uint32_t> get_input_sizes() const;
			std::vector<uint32_t> get_working_sizes() const;
			std::vector<uint32_t> get_output_sizes() const;
			std::vector<uint32_t> get_all_sizes() const;

			std::vector<Output::Line> emit(const std::vector<Storage> &storages) const;
		};

		// | Custom instruction that has no dynamic inputs or outputs.
		class Custom : public Base {
		public:
			Custom();
			Custom(const Base &base, const std::vector<Output::Line> &lines);
			std::vector<Output::Line> lines;

			std::vector<uint32_t> get_input_sizes() const;
			std::vector<uint32_t> get_working_sizes() const;
			std::vector<uint32_t> get_output_sizes() const;
			std::vector<uint32_t> get_all_sizes() const;

			std::vector<Output::Line> emit(const std::vector<Storage> &storages) const;
		};

		// | Syscall.
		class Syscall : public Base {
		public:
			Syscall();
			Syscall(const Base &base);

			std::vector<uint32_t> get_input_sizes() const;
			std::vector<uint32_t> get_working_sizes() const;
			std::vector<uint32_t> get_output_sizes() const;
			std::vector<uint32_t> get_all_sizes() const;

			std::vector<Output::Line> emit(const std::vector<Storage> &storages) const;
		};

		// | AddSp.
		// Add or subtract the $sp stack pointer, and tell emit() to modify $sp-based Storages sent to instructions.
		// If "offset" is not 8-byte aligned, it will be rounded away from 0 to the nearest 8-byte boundary.
		class AddSp : public Base {
		public:
			AddSp();
			AddSp(const Base &base, int32_t offset);
			int32_t offset;

			std::vector<uint32_t> get_input_sizes() const;
			std::vector<uint32_t> get_working_sizes() const;
			std::vector<uint32_t> get_output_sizes() const;
			std::vector<uint32_t> get_all_sizes() const;

			std::vector<Output::Line> emit(const std::vector<Storage> &storages) const;

			static int32_t round_to_align(int32_t offset, uint32_t alignment = 8);
		};

		// | Load an immediate value into a storage unit, e.g. 3 or the address of "string literal".
		class LoadImmediate : public Base {
		public:
			LoadImmediate();
			LoadImmediate(const Base &base, bool is_word, const ConstantValue &constant_value, Symbol string_symbol = Symbol());
			// | Are we loading a byte or a word?
			bool is_word;
			ConstantValue constant_value;
			// | If the constant value is a string (value unused), what is the symbol to its global address?
			// This symbol doesn't have to refer to a string literal; it can be
			// an array, for example.  Only the address is used here.  It
			// should refer to a label, however.
			Symbol string_symbol;

			std::vector<uint32_t> get_input_sizes() const;
			std::vector<uint32_t> get_working_sizes() const;
			std::vector<uint32_t> get_output_sizes() const;
			std::vector<uint32_t> get_all_sizes() const;

			std::vector<Output::Line> emit(const std::vector<Storage> &storages) const;
		};

		// | Load from one storage unit to another.
		class LoadFrom : public Base {
		public:
			LoadFrom();
			LoadFrom(const Base &base, bool is_word_save, bool is_word_load, int32_t addition, bool is_save_fixed, bool is_load_fixed, const Storage &fixed_save_storage, const Storage &fixed_load_storage, bool dereference_save = false, bool dereference_load = false);
			// Specialized constructors: load from and save to dynamic storage units.
			LoadFrom(const Base &base, bool is_word_save, bool is_word_load, int32_t addition = 0);
			LoadFrom(const Base &base, bool is_word, int32_t addition = 0);
			// | Are we saving a byte or a word?
			bool is_word_save;
			// | Permit resizing.
			bool is_word_load;
			// | destination <- source + addition
			// (Addition applies to values, not addresses.)
			int32_t addition = 0;
			bool is_save_fixed = false;
			bool is_load_fixed = false;
			Storage fixed_save_storage = Storage();
			Storage fixed_load_storage = Storage();
			// | Instead of saving to the storage unit, save to what the storage unit points to?
			bool dereference_save = false;
			// | Instead of loading from the storage unit, load from what the storage unit points to?
			bool dereference_load = false;

			std::vector<uint32_t> get_input_sizes() const;
			std::vector<uint32_t> get_working_sizes() const;
			std::vector<uint32_t> get_output_sizes() const;
			std::vector<uint32_t> get_all_sizes() const;

			std::vector<Output::Line> emit(const std::vector<Storage> &storages) const;
		};

		// | If the first input is less than the second input, write value 1 to
		// the output, else write value 0 to the output.
		class LessThanFrom : public Base {
		public:
			LessThanFrom();
			LessThanFrom(const Base &base, bool is_word, bool is_signed = false);
			// | Are we loading a byte or a word?
			bool is_word;
			// | Is this a signed comparison?
			bool is_signed;

			std::vector<uint32_t> get_input_sizes() const;
			std::vector<uint32_t> get_working_sizes() const;
			std::vector<uint32_t> get_output_sizes() const;
			std::vector<uint32_t> get_all_sizes() const;

			std::vector<Output::Line> emit(const std::vector<Storage> &storages) const;
		};

		// | Nor from two storage units to another.
		class NorFrom : public Base {
		public:
			NorFrom();
			NorFrom(const Base &base, bool is_word);
			// | Are we loading a byte or a word?
			bool is_word;

			std::vector<uint32_t> get_input_sizes() const;
			std::vector<uint32_t> get_working_sizes() const;
			std::vector<uint32_t> get_output_sizes() const;
			std::vector<uint32_t> get_all_sizes() const;

			std::vector<Output::Line> emit(const std::vector<Storage> &storages) const;
		};

		// | And from two storage units to another.
		class AndFrom : public Base {
		public:
			AndFrom();
			AndFrom(const Base &base, bool is_word);
			// | Are we loading a byte or a word?
			bool is_word;

			std::vector<uint32_t> get_input_sizes() const;
			std::vector<uint32_t> get_working_sizes() const;
			std::vector<uint32_t> get_output_sizes() const;
			std::vector<uint32_t> get_all_sizes() const;

			std::vector<Output::Line> emit(const std::vector<Storage> &storages) const;
		};

		// | Or from two storage units to another.
		class OrFrom : public Base {
		public:
			OrFrom();
			OrFrom(const Base &base, bool is_word);
			// | Are we loading a byte or a word?
			bool is_word;

			std::vector<uint32_t> get_input_sizes() const;
			std::vector<uint32_t> get_working_sizes() const;
			std::vector<uint32_t> get_output_sizes() const;
			std::vector<uint32_t> get_all_sizes() const;

			std::vector<Output::Line> emit(const std::vector<Storage> &storages) const;
		};

		// | Add from two storage units to another.
		class AddFrom : public Base {
		public:
			AddFrom();
			AddFrom(const Base &base, bool is_word);
			// | Are we loading a byte or a word?
			bool is_word;

			std::vector<uint32_t> get_input_sizes() const;
			std::vector<uint32_t> get_working_sizes() const;
			std::vector<uint32_t> get_output_sizes() const;
			std::vector<uint32_t> get_all_sizes() const;

			std::vector<Output::Line> emit(const std::vector<Storage> &storages) const;
		};

		// | Subtract two storage units into another.
		class SubFrom : public Base {
		public:
			SubFrom();
			SubFrom(const Base &base, bool is_word);
			// | Are we loading a byte or a word?
			bool is_word;

			std::vector<uint32_t> get_input_sizes() const;
			std::vector<uint32_t> get_working_sizes() const;
			std::vector<uint32_t> get_output_sizes() const;
			std::vector<uint32_t> get_all_sizes() const;

			std::vector<Output::Line> emit(const std::vector<Storage> &storages) const;
		};

		// | Multiply two storage units into two others: Lo, then Hi.
		class MultFrom : public Base {
		public:
			MultFrom();
			MultFrom(const Base &base, bool is_word);
			// | Are we loading a byte or a word?
			bool is_word;

			std::vector<uint32_t> get_input_sizes() const;
			std::vector<uint32_t> get_working_sizes() const;
			std::vector<uint32_t> get_output_sizes() const;
			std::vector<uint32_t> get_all_sizes() const;

			std::vector<Output::Line> emit(const std::vector<Storage> &storages) const;
		};

		// | Divide two storage units into two others: quotient, then remainder.
		class DivFrom : public Base {
		public:
			DivFrom();
			DivFrom(const Base &base, bool is_word);
			// | Are we loading a byte or a word?
			bool is_word;

			std::vector<uint32_t> get_input_sizes() const;
			std::vector<uint32_t> get_working_sizes() const;
			std::vector<uint32_t> get_output_sizes() const;
			std::vector<uint32_t> get_all_sizes() const;

			std::vector<Output::Line> emit(const std::vector<Storage> &storages) const;
		};

		// | Unconditionally jump to a location referred to by the input.
		class JumpTo : public Base {
		public:
			JumpTo();
			JumpTo(const Base &base);

			std::vector<uint32_t> get_input_sizes() const;
			std::vector<uint32_t> get_working_sizes() const;
			std::vector<uint32_t> get_output_sizes() const;
			std::vector<uint32_t> get_all_sizes() const;

			std::vector<Output::Line> emit(const std::vector<Storage> &storages) const;
		};

		// | Unconditionally jump to a label.
		class Jump : public Base {
		public:
			Jump();
			Jump(const Base &base, Symbol jump_destination);
			Symbol jump_destination;

			std::vector<uint32_t> get_input_sizes() const;
			std::vector<uint32_t> get_working_sizes() const;
			std::vector<uint32_t> get_output_sizes() const;
			std::vector<uint32_t> get_all_sizes() const;

			std::vector<Output::Line> emit(const std::vector<Storage> &storages) const;
		};

		// | jal instruction to a label.  Store return address in $ra.
		class Call : public Base {
		public:
			Call();
			Call(const Base &base, const Symbol &jump_destination, bool push_saved_registers = false, bool pop_saved_registers = false);
			Symbol jump_destination;
			// | Instead of performing a call, tell MIPSIO::emit() to push saved registers onto the stack.
			bool push_saved_registers = false;
			// | Instead of performing a call, tell MIPSIO::emit() to pop saved registers from the stack.
			bool pop_saved_registers = false;

			std::vector<uint32_t> get_input_sizes() const;
			std::vector<uint32_t> get_working_sizes() const;
			std::vector<uint32_t> get_output_sizes() const;
			std::vector<uint32_t> get_all_sizes() const;

			std::vector<Output::Line> emit(const std::vector<Storage> &storages) const;
		};

		// | jr instruction.
		class Return : public Base {
		public:
			Return();
			Return(const Base &base, bool fixed_storage = true, const Storage &storage = Storage("$ra"));
			bool fixed_storage;
			Storage storage;

			std::vector<uint32_t> get_input_sizes() const;
			std::vector<uint32_t> get_working_sizes() const;
			std::vector<uint32_t> get_output_sizes() const;
			std::vector<uint32_t> get_all_sizes() const;

			std::vector<Output::Line> emit(const std::vector<Storage> &storages) const;
		};

		// | Branch to a label if the input is equal to zero.
		class BranchZero : public Base {
		public:
			BranchZero();
			BranchZero(const Base &base, bool is_word, Symbol branch_destination);
			bool is_word;
			Symbol branch_destination;

			std::vector<uint32_t> get_input_sizes() const;
			std::vector<uint32_t> get_working_sizes() const;
			std::vector<uint32_t> get_output_sizes() const;
			std::vector<uint32_t> get_all_sizes() const;

			std::vector<Output::Line> emit(const std::vector<Storage> &storages) const;
		};

		// | Branch to a label if the input is greater than or equal to zero.
		class BranchNonnegative : public Base {
		public:
			BranchNonnegative();
			BranchNonnegative(const Base &base, bool is_word, Symbol branch_destination);
			bool is_word;
			Symbol branch_destination;

			std::vector<uint32_t> get_input_sizes() const;
			std::vector<uint32_t> get_working_sizes() const;
			std::vector<uint32_t> get_output_sizes() const;
			std::vector<uint32_t> get_all_sizes() const;

			std::vector<Output::Line> emit(const std::vector<Storage> &storages) const;
		};

		using data_t = std::variant<
			std::monostate,
			Ignore,
			Custom,
			Syscall,
			AddSp,
			LoadImmediate,
			LoadFrom,
			LessThanFrom,
			NorFrom,
			AndFrom,
			OrFrom,
			AddFrom,
			SubFrom,
			MultFrom,
			DivFrom,
			JumpTo,
			Jump,
			Call,
			Return,
			BranchZero,
			BranchNonnegative
		>;

		Instruction();
		Instruction(tag_t tag, const data_t &data);
		Instruction(tag_t tag, data_t &&data);
		tag_t tag;
		data_t data;

		const Base &get_base() const;
		Base &&get_base();
		Base &get_base_mutable();

		Instruction(const Ignore            &ignore);
		Instruction(const Custom            &load_custom);
		Instruction(const Syscall           &load_syscall);
		Instruction(const AddSp             &load_add_sp);
		Instruction(const LoadImmediate     &load_immediate);
		Instruction(const LoadFrom          &load_from);
		Instruction(const LessThanFrom      &less_than_from);
		Instruction(const NorFrom           &nor_from);
		Instruction(const AndFrom           &and_from);
		Instruction(const OrFrom            &or_from);
		Instruction(const AddFrom           &add_from);
		Instruction(const SubFrom           &sub_from);
		Instruction(const MultFrom          &mult_from);
		Instruction(const DivFrom           &div_from);
		Instruction(const JumpTo            &jump_to);
		Instruction(const Jump              &jump);
		Instruction(const Call              &call);
		Instruction(const Return            &return_);
		Instruction(const BranchZero        &branch_zero);
		Instruction(const BranchNonnegative &branch_nonnegative);

		bool is_ignore()             const;
		bool is_custom()             const;
		bool is_syscall()            const;
		bool is_add_sp()             const;
		bool is_load_immediate()     const;
		bool is_load_from()          const;
		bool is_less_than_from()     const;
		bool is_nor_from()           const;
		bool is_and_from()           const;
		bool is_or_from()            const;
		bool is_add_from()           const;
		bool is_sub_from()           const;
		bool is_mult_from()          const;
		bool is_div_from()           const;
		bool is_jump_to()            const;
		bool is_jump()               const;
		bool is_call()               const;
		bool is_return()             const;
		bool is_branch_zero()        const;
		bool is_branch_nonnegative() const;

		// | The tags must be correct, or else an exception will be thrown, including for set_*.
		const Ignore            &get_ignore()             const;
		const Custom            &get_custom()             const;
		const Syscall           &get_syscall()            const;
		const AddSp             &get_add_sp()             const;
		const LoadImmediate     &get_load_immediate()     const;
		const LoadFrom          &get_load_from()          const;
		const LessThanFrom      &get_less_than_from()     const;
		const NorFrom           &get_nor_from()           const;
		const AndFrom           &get_and_from()           const;
		const OrFrom            &get_or_from()            const;
		const AddFrom           &get_add_from()           const;
		const SubFrom           &get_sub_from()           const;
		const MultFrom          &get_mult_from()          const;
		const DivFrom           &get_div_from()           const;
		const JumpTo            &get_jump_to()            const;
		const Jump              &get_jump()               const;
		const Call              &get_call()               const;
		const Return            &get_return()             const;
		const BranchZero        &get_branch_zero()        const;
		const BranchNonnegative &get_branch_nonnegative() const;

		Ignore            &&get_ignore();
		Custom            &&get_custom();
		Syscall           &&get_syscall();
		AddSp             &&get_add_sp();
		LoadImmediate     &&get_load_immediate();
		LoadFrom          &&get_load_from();
		LessThanFrom      &&get_less_than_from();
		NorFrom           &&get_nor_from();
		AndFrom           &&get_and_from();
		OrFrom            &&get_or_from();
		AddFrom           &&get_add_from();
		SubFrom           &&get_sub_from();
		MultFrom          &&get_mult_from();
		DivFrom           &&get_div_from();
		JumpTo            &&get_jump_to();
		Jump              &&get_jump();
		Call              &&get_call();
		Return            &&get_return();
		BranchZero        &&get_branch_zero();
		BranchNonnegative &&get_branch_nonnegative();

		Ignore            &get_ignore_mutable();
		Custom            &get_custom_mutable();
		Syscall           &get_syscall_mutable();
		AddSp             &get_add_sp_mutable();
		LoadImmediate     &get_load_immediate_mutable();
		LoadFrom          &get_load_from_mutable();
		LessThanFrom      &get_less_than_from_mutable();
		NorFrom           &get_nor_from_mutable();
		AndFrom           &get_and_from_mutable();
		OrFrom            &get_or_from_mutable();
		AddFrom           &get_add_from_mutable();
		SubFrom           &get_sub_from_mutable();
		MultFrom          &get_mult_from_mutable();
		DivFrom           &get_div_from_mutable();
		JumpTo            &get_jump_to_mutable();
		Jump              &get_jump_mutable();
		Call              &get_call_mutable();
		Return            &get_return_mutable();
		BranchZero        &get_branch_zero_mutable();
		BranchNonnegative &get_branch_nonnegative_mutable();

		// | Return "ignore", "custom", "syscall", "add_sp", "load_immediate", "less_than_from", "load_from", or "nor_from", etc.
		static std::string get_tag_repr(tag_t tag);
		std::string get_tag_repr() const;

		std::vector<uint32_t> get_input_sizes() const;
		std::vector<uint32_t> get_working_sizes() const;
		std::vector<uint32_t> get_output_sizes() const;
		std::vector<uint32_t> get_all_sizes() const;

		std::vector<Output::Line> emit(const std::vector<Storage> &storages) const;
	};

	class MIPSIO {
	public:
		// | Reference to an instruction contained in this MIPSIO.
		using Index   = std::vector<Instruction>::size_type;
		// | Given an instruction, this is a reference to an input, working, or output storage.
		using IOIndex = std::vector<uint32_t>::size_type;
		// | Reference to an instruction and one of its inputs or outputs, depending on the context.
		using IO = std::pair<Index, Index>;

		// | Vertices.
		//
		// Each instruction has zero or more inputs, zero or more required working storages, and zero or more outputs.
		//
		// Instructions can be added.  Connections between an instruction's
		// input and an instruction's output can also be added.  An
		// instruction's input can have at most one connection.
		//
		// For emission, all the inputs of all instructions must have exactly
		// one producer, either another instruction's output or a storage unit
		// provided by "input_storages".
		std::vector<Instruction> instructions;

		// | All vertices that have inputs.
		std::map<IO, IO>           connections;           // connections[input]   == output that provides the input.
		std::map<IO, std::set<IO>> reversed_connections;  // connections[output]  == {all inputs that this output supplies}.
		std::map<Index, Index>     sequences;             // sequences[this_node] == the node that should be emitted (after its unemitted children (inputs) are emitted) right after this_node is emitted.
		std::map<Index, Index>     reversed_sequences;    // sequences reversed.  We have a bimap now.

		// | In order to emit these MIPSIO instructions that write these outputs, how many working storages are needed?
		// TODO: optimization: count which storages are used most so the caller
		// knows which to prioritize as registers.  Storages that are used more
		// often should appear first, but this is currently unimplemented.
		// (Count number of locks / claims, and then stable sort the output before returning.)
		std::vector<uint32_t> prepare(const std::set<IO> &capture_outputs) const;
		std::vector<uint32_t> prepare(const std::map<IO, Storage> &capture_outputs) const;
		// | Emit the collections of instructions using the provided storages.
		//
		// There must be a path between every node and capture_outputs, or an
		// error is thrown.  This simplifies the algorithm's working storage
		// tracking, to know when they can be re-used.
		//
		// Uncaptured outputs should be consumed with another connection.  If
		// it is unused, use the Ignore instruction to consume it.  To disable
		// this restriction, pass permit_uncaptured_outputs = true.
		std::vector<Output::Line> emit(const std::map<IO, Storage> &input_storages, const std::vector<Storage> &working_storages, const std::map<IO, Storage> &capture_outputs, bool permit_uncaptured_outputs = false) const;

		template<typename A, typename B, typename C>
		static std::map<A, std::map<B, C>> expand_map(const std::map<std::pair<A, B>, C> &map) {
			std::map<A, std::map<B, C>> map_expanded;
			for (const typename std::map<std::pair<A, B>, C>::value_type &map_pair : std::as_const(map)) {
				typename std::map<A, std::map<B, C>>::iterator map_expanded_search = map_expanded.find(map_pair.first.first);
				if (map_expanded_search == map_expanded.end()) {
					map_expanded.insert({map_pair.first.first, {{map_pair.first.second, map_pair.second}}});
				} else {
					map_expanded_search->second.insert({map_pair.first.second, map_pair.second});
				}
			}
			return map_expanded;
		}

		template<typename A, typename B, typename C>
		static std::map<std::pair<A, B>, C> shrink_map(const std::map<A, std::map<B, C>> &map) {
			std::map<std::pair<A, B>, C> map_shrinked;
			for (const typename std::map<A, std::map<B, C>>::value_type &map_pair : std::as_const(map)) {
				for (const typename std::map<B, C>::value_type &submap_pair : std::as_const(map_pair.second)) {
					map_shrinked.insert({map_pair.first, submap_pair.first}, submap_pair.second);
				}
			}
			return map_shrinked;
		}

		// Utility methods.

		// | Straightforwardly add an instruction, optionally connecting its
		// first arguments with the first output of the instructions
		// corresponding to the input indices.
		//
		// There is a variant that can take an index to an instruction that
		// should run before the instruction being added.
		Index add_instruction(const Instruction &instruction, const std::vector<Index> inputs = {});
		Index add_instruction(const Instruction &instruction, const std::vector<Index> inputs, const Index after);
		Index add_instruction(const Instruction &instruction, const std::vector<Index> inputs, const std::optional<Index> after);
		//Index add_instruction(const Instruction &instruction, const Index after);
		// | Same as before, but allow specification of which output in case there are multiple outputs.
		Index add_instruction_indexed(const Instruction &instruction, const std::vector<IO> inputs = {});
		Index add_instruction_indexed(const Instruction &instruction, const std::vector<IO> inputs, const Index after);
		Index add_instruction_indexed(const Instruction &instruction, const std::vector<IO> inputs, const std::optional<Index> after);
		//Index add_instruction_indexed(const Instruction &instruction, const Index after);
		// | Set "output"'s given output as "input"'s given input.
		void add_connection(IO output, IO input);
		// | Right after "before" is emitted, emit "after"'s unemitted children and then "after".
		void add_sequence_connection(Index before, Index after);
		void add_sequence_connection(std::pair<Index, Index> before_after);
		void add_sequence_connections(const std::vector<std::pair<Index, Index>> before_afters);
		void add_sequence_connections(const std::vector<Index> befores, const std::vector<Index> afters);
		// | Add "other"'s instructions to this; any indices returned from
		// "add_instruction" into "other" (but not "this") must be added by the
		// returned value to remain correct.
		Index merge(const MIPSIO &other);
	};

// TODO: inline support.
#if 0
	// | Refers to a Jump instruction, the IOs referring to the argument inputs, and the IOs referring to the argument outputs.
	//
	// Storing calls enables inlining function calls before emitting them.
	// TODO: implement Call and merges with calls.
	class Call {
	public:
		Symbol routine_symbol;
		Index jump_index;
		std::vector<MIPSIO::IO> arguments;
		std::vector<MIPSIO::IO> outputs;

		static merge_mipsio_with_calls(MIPSIO &mips_io, std::vector<Call> &calls, const MIPSIO &other_mips_io, const std::vector<Call> &other_calls);
	};
#endif /* #if 0 */

	static const bool all_arrays_records_are_refs;

	class Expression {
	public:
		MIPSIO            instructions;
// TODO: inline support.
#if 0
		std::vector<Call> calls;
#endif /* #if 0 */
		Type              output_type;
		MIPSIO::Index     output_index;
		uint64_t          lexeme_begin = 0;
		uint64_t          lexeme_end   = 0;

		Expression();  // (No instructions; null type.)
		Expression(const MIPSIO  &instructions, const Type  &output_type, MIPSIO::Index output_index, uint64_t lexeme_begin = 0, uint64_t lexeme_end = 0);
		Expression(const MIPSIO  &instructions,       Type &&output_type, MIPSIO::Index output_index, uint64_t lexeme_begin = 0, uint64_t lexeme_end = 0);
		Expression(      MIPSIO &&instructions, const Type  &output_type, MIPSIO::Index output_index, uint64_t lexeme_begin = 0, uint64_t lexeme_end = 0);
		Expression(      MIPSIO &&instructions,       Type &&output_type, MIPSIO::Index output_index, uint64_t lexeme_begin = 0, uint64_t lexeme_end = 0);

// TODO: inline support.
#if 0
		Expression();  // (No instructions; null type.)
		Expression(const MIPSIO  &instructions, const std::vector<Call> &calls, const Type  &output_type, MIPSIO::Index output_index, uint64_t lexeme_begin = 0, uint64_t lexeme_end = 0);
		Expression(const MIPSIO  &instructions, const std::vector<Call> &calls,       Type &&output_type, MIPSIO::Index output_index, uint64_t lexeme_begin = 0, uint64_t lexeme_end = 0);
		Expression(      MIPSIO &&instructions, const std::vector<Call> &calls, const Type  &output_type, MIPSIO::Index output_index, uint64_t lexeme_begin = 0, uint64_t lexeme_end = 0);
		Expression(      MIPSIO &&instructions, const std::vector<Call> &calls,       Type &&output_type, MIPSIO::Index output_index, uint64_t lexeme_begin = 0, uint64_t lexeme_end = 0);

		// | Merge / compose constructor.
		Expression(const MIPSIO &a, const MIPSIO &b);

		// | Merge both instructions and calls, and update lexeme_begin and
		// lexeme_end to the minimum and maximum, respectively.
		MIPSIO::Index merge(const Expression &other);
#endif /* #if 0 */
	};

	// The non-const part is the ability to store strings.
	Expression analyze_expression(uint64_t expression, const IdentifierScope &constant_scope, const IdentifierScope &type_scope, const IdentifierScope &routine_scope, const IdentifierScope &var_scope, const IdentifierScope &combined_scope, bool no_dereference_record_array = false);
	Expression analyze_expression(const ::Expression &expression, const IdentifierScope &constant_scope, const IdentifierScope &type_scope, const IdentifierScope &routine_scope, const IdentifierScope &var_scope, const IdentifierScope &combined_scope, bool no_dereference_record_array = false);

	// | A computation graph from needed input storage units to needed output storage units.
	// MIPSIO with input types and locations and output types and locations.
	//
	// Since global variables are accessible to all blocks, their types are are
	// included in all block type signatures.
	//
	// A routine that adds a var adds an input.  A routine that adds a ref adds
	// an input and output.  A CPSL function as opposed to procedure adds an
	// output.
	class Block {
	public:
		MIPSIO instructions;
		// | "front" and "back" are used for sequence connections.
		//
		// | back: When this node is sequenced (through a sequence
		// connection), the dependencies propagate to the nodes in the entire
		// block.
		//
		// "back" may already have a sequence connection for an instruction
		// that occurs *before* it, but it shouldn't have a sequence connection
		// for an instruction that occurs *after* it before it is merged.  One
		// way to append block "other" into "this" is to set "this"'s "back" to
		// "other" after merging.  (Another way to append block "other" into
		// "this" is to merge "other" into this, to then add a nop
		// pseudoinstruction, i.e. Ignore, that occurs after "other.back", and
		// then sequence that Ignore after "other.back".) Either way, be sure
		// to also link "other.front" after the original "this.back".
		//
		// "front" should have no sequence connections *before* it (but might
		// have one after it).  Since when merging a block "other", other is
		// independent of "this", it should be safe to sequence "other.front"
		// after the original "this.back" (and then setting the final
		// "this.back" to "other.final") without causing sequence connection
		// conflicts / duplicates.
		MIPSIO::Index    front = 0;  // Ignored if instructions is empty.
		MIPSIO::Index    back  = 0;  // Ignored if instructions is empty.
		std::map<std::string, const Type *> local_variables;
		uint64_t         lexeme_begin = 0;
		uint64_t         lexeme_end   = 0;

		Block();
		Block(const MIPSIO  &instructions, MIPSIO::Index front, MIPSIO::Index back, const std::map<std::string, const Type *> &local_variables, uint64_t lexeme_begin = 0, uint64_t lexeme_end = 0);
		Block(      MIPSIO &&instructions, MIPSIO::Index front, MIPSIO::Index back, const std::map<std::string, const Type *> &local_variables, uint64_t lexeme_begin = 0, uint64_t lexeme_end = 0);
	};

	// | Analyze a sequence of statements.
	//
	// Note: this does not need to necessarily correspond to a ::Block in the
	// grammar tree but can be a sequence of statements without a BEGIN and END
	// keyword.
	Block analyze_statements(const std::vector<uint64_t> &statements, const IdentifierScope &constant_scope, const IdentifierScope &type_scope, const IdentifierScope &routine_scope, const IdentifierScope &var_scope, const IdentifierScope &combined_scope);
	Block analyze_statements(const StatementSequence &statement_sequence, const IdentifierScope &constant_scope, const IdentifierScope &type_scope, const IdentifierScope &routine_scope, const IdentifierScope &var_scope, const IdentifierScope &combined_scope);

	// | Analyze a BEGIN [statement]... END block.
	std::vector<Output::Line> analyze_block(const IdentifierScope::IdentifierBinding::RoutineDeclaration &routine_declaration, const ::Block &block, const IdentifierScope &constant_scope, const IdentifierScope &type_scope, const IdentifierScope &routine_scope, const IdentifierScope &var_scope, const IdentifierScope &combined_scope, const std::map<std::string, const Type *> &local_variables = {});

	// | Analyze a routine definition.
	//
	// "analyze_block" but look for additional types, constants, and variables.
	std::vector<Output::Line> analyze_routine(const IdentifierScope::IdentifierBinding::RoutineDeclaration &routine_declaration, const Body &body, const IdentifierScope &constant_scope, const IdentifierScope &type_scope, const IdentifierScope &routine_scope, const IdentifierScope &var_scope, const IdentifierScope &combined_scope);

	// | Get the symbol to a string literal, tracking it if this is the first time encountering it.
	Symbol string_literal(const std::string &string);
	static const uint64_t max_string_requested_label_suffix_length;
	static std::string labelify(const std::string &string);

	static bool would_addition_overflow(int32_t a, int32_t b);
	static bool would_multiplication_overflow(int32_t a, int32_t b);
	static bool would_division_overflow(int32_t a, int32_t b);
	static int32_t euclidian_div(int32_t a, int32_t b);
	static int32_t euclidian_mod(int32_t a, int32_t b);

	// | Clear memoization caches and calculated output values and reset them to default values.
	//
	// Set up the identifier scopes with the 12 built-in identifiers:
	// - integer : integer type
	// - char    : char type
	// - boolean : boolean type
	// - string  : string type
	// - true    : true boolean constant
	// - false   : false boolean constant
	// - INTEGER : integer type
	// - CHAR    : char type
	// - BOOLEAN : boolean type
	// - STRING  : string type
	// - TRUE    : true boolean constant
	// - FALSE   : false boolean constant
	void reset_output();

	// | Force a re-analysis of the semantics data.
	void analyze();

protected:
	// | The grammar tree used for the semantics data.
	Grammar grammar;
	// | Whether to automatically construct the semantics analysis after loading the grammar.
	bool auto_analyze;

	// | Collection of string constants we collect as we analyze the parse tree.
	//
	// Assembled output can contain a section of labeled string constants that
	// other parts can refer to.
	std::map<std::string, Symbol> string_constants;

	IdentifierScope top_level_constant_scope;
	IdentifierScope top_level_type_scope;
	IdentifierScope top_level_var_scope;
	IdentifierScope top_level_routine_scope;
	// | Union of top_level_*_scope.  If !combine_identifier_namespaces, the last identifier overrides.
	IdentifierScope top_level_scope;
	// | The lifetime of anonymous_storage should not exceed that of *_scope, since they may contain raw pointers into this storage.
	IdentifierScope anonymous_storage;
	std::set<std::string> routine_definitions;

	// | Ordered copy of the Var identifier bindings in top_level_var_scope.
	std::vector<IdentifierScope::IdentifierBinding::Var> top_level_vars;

	// The analyzed assembly output.
	Output output;
};

// Symbol.
inline bool operator< (const Semantics::Symbol &a, const Semantics::Symbol &b);
inline bool operator> (const Semantics::Symbol &a, const Semantics::Symbol &b);
inline bool operator<=(const Semantics::Symbol &a, const Semantics::Symbol &b);
inline bool operator>=(const Semantics::Symbol &a, const Semantics::Symbol &b);

inline bool operator==(const Semantics::Symbol &a, const Semantics::Symbol &b);
inline bool operator!=(const Semantics::Symbol &a, const Semantics::Symbol &b);

// SymbolLocation.
inline bool operator< (const Semantics::Output::SymbolLocation &a, const Semantics::Output::SymbolLocation &b);
inline bool operator> (const Semantics::Output::SymbolLocation &a, const Semantics::Output::SymbolLocation &b);
inline bool operator<=(const Semantics::Output::SymbolLocation &a, const Semantics::Output::SymbolLocation &b);
inline bool operator>=(const Semantics::Output::SymbolLocation &a, const Semantics::Output::SymbolLocation &b);

inline bool operator==(const Semantics::Output::SymbolLocation &a, const Semantics::Output::SymbolLocation &b);
inline bool operator!=(const Semantics::Output::SymbolLocation &a, const Semantics::Output::SymbolLocation &b);

// Line.
inline bool operator< (const Semantics::Output::Line &a, const Semantics::Output::Line &b);
inline bool operator> (const Semantics::Output::Line &a, const Semantics::Output::Line &b);
inline bool operator<=(const Semantics::Output::Line &a, const Semantics::Output::Line &b);
inline bool operator>=(const Semantics::Output::Line &a, const Semantics::Output::Line &b);

inline bool operator==(const Semantics::Output::Line &a, const Semantics::Output::Line &b);
inline bool operator!=(const Semantics::Output::Line &a, const Semantics::Output::Line &b);

// Line plus.
inline Semantics::Output::Line operator+(const Semantics::Output::Line &a, const Semantics::Output::Line &b);
inline Semantics::Output::Line operator+(const Semantics::Output::Line &a, const Semantics::Symbol       &b);
inline Semantics::Output::Line operator+(const Semantics::Symbol       &a, const Semantics::Output::Line &b);
inline Semantics::Output::Line operator+(const Semantics::Symbol       &a, const Semantics::Symbol       &b);

// Storage.
inline bool operator< (const Semantics::Storage &a, const Semantics::Storage &b);
inline bool operator> (const Semantics::Storage &a, const Semantics::Storage &b);
inline bool operator<=(const Semantics::Storage &a, const Semantics::Storage &b);
inline bool operator>=(const Semantics::Storage &a, const Semantics::Storage &b);

inline bool operator==(const Semantics::Storage &a, const Semantics::Storage &b);
inline bool operator!=(const Semantics::Storage &a, const Semantics::Storage &b);

class UnitTests {
public:
	static UnitTests unit_tests;

	UnitTests();

	void run();

	void test_mips_io();
	void test_mips_io2();
};

// Commented out: these declarations are redundant and would cause some calls to be ambiguous.
#if 0
inline Semantics::Output::Line operator+(const std::string             &a, const Semantics::Output::Line &b);
inline Semantics::Output::Line operator+(const Semantics::Output::Line &a, const std::string             &b);
#endif /* #if 0 */

#endif /* #ifndef CPSL_CC_SEMANTICS_HH */
