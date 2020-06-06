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
#include "graph.hh"

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

#define CPSL_CC_SEMANTICS_COMBINE_IDENTIFIER_NAMESPACES true
#define CPSL_CC_SEMANTICS_MAX_UNIQUE_TRY_ITERATIONS     A_BILLION  // fun
#define CPSL_CC_SEMANTICS_MAX_STRING_REQUESTED_LABEL_SUFFIX_LENGTH 32

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
		uint64_t unique_identifier;

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
			//
			// This is not checked for cycles!
			// TODO: check for cycles and null.
			const Type &resolve_type() const;
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
		};

		class Array : public Base {
		public:
			Array();
			Array(const std::string &identifier, const Type *base_type, int32_t min_index, int32_t max_index, IdentifierScope &anonymous_storage);

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
		ConstantValue(tag_t tag, const data_t &data, uint64_t lexeme_begin, uint64_t lexeme_end);
		ConstantValue(tag_t tag, data_t &&data, uint64_t lexeme_begin, uint64_t lexeme_end);
		// | Copy the constant value but use new lexeme identifiers.
		ConstantValue(const ConstantValue &constant_value, uint64_t lexeme_begin, uint64_t lexeme_end);
		ConstantValue(ConstantValue &&constant_value, uint64_t lexeme_begin, uint64_t lexeme_end);
		tag_t  tag;
		data_t data;
		uint64_t lexeme_begin;
		uint64_t lexeme_end;

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

			// | Constant expression / static value.
			class Static {
			public:
				Static();
				Static(const ConstantValue &constant_value);
				Static(ConstantValue &&constant_value);

				// Keep a copy of the constant value.
				ConstantValue constant_value;
			};
			// TODO: I don't think you need Dynamic here.
			class Dynamic {
			public:
				// TODO
			};
			using Type = ::Semantics::Type;
			// | Variable.
			class Var {
			public:
				Var();
				Var(bool ref, const Type &type, bool global, Symbol symbol, bool register_, uint8_t arg_register_id, uint32_t offset);
				// | Is this a pointer / reference (Ref)?
				bool ref;
				// | What is the base type of this variable, ignoring the pointer?
				Type type;
				// | Is the variable a global variable or a local variable?
				bool global;
				// | If this variable is global, what is the label that refers to its address?  Ignored if it isn't global.
				Symbol symbol;
				// | If this variable is local, is this variable stored in a register?
				bool register_;
				// | If this variable is stored in a register, which of $a0 - $a3 is it?
				uint8_t arg_register_id;
				// | If this variable is local and not stored in a register, what is its offset relative to the stack pointer?
				uint32_t offset;
			};
			// TODO: Ref is now merged into Var.  Remove.
			// | Pointer variable.
			class Ref {
			public:
				Ref();
				Ref(const Type &type, bool register_, uint8_t arg_register_id, uint32_t offset);
				Type type;
				// | Is this variable stored in a register?
				bool register_;
				// | If this variable is stored in a register, which of $a0 - $a3 is it?
				uint8_t arg_register_id;
				// | If this variable is not stored in a register, what is its offset relative to the stack pointer?
				uint32_t offset;
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
	//
	// TODO: this will work if the grammar parse tree is valid, but check for cycles in case there's a mistake somewhere.
	ConstantValue is_expression_constant(
		// | Reference to the expression in the grammar tree.
		uint64_t expression,
		// | A collection of identifiers of constants available to the scope of the expression.
		// Note: We don't record the identifier scope here.  There is only one
		// identifier scope for each expression.
		const IdentifierScope &expression_constant_scope
	) const;
	ConstantValue is_expression_constant(const Expression &expression, const IdentifierScope &expression_constant_scope) const;

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

	// | A representation of a storage unit: a register, offset on the stack, global address, etc.
	//
	// $t8 and $t9 are reserved in case any working storage units require 2
	// registers to access.  The maximum number of registers required to access
	// a working storage unit is 2.
	//
	// There are 4 storage types:
	// 	1: global_address + x
	// 	2: x(global_address)
	// 	3: $reg + x
	// 	4: x($reg)
	class Storage {
	public:
		Storage();
		Storage(uint32_t max_size, bool is_global, Symbol global_address, const std::string &register_, bool dereference, int32_t offset);

		// | What is the maximum size that this storage can handle?
		uint32_t    max_size;
		// | Is this a global address or a register?
		bool        is_global;
		// | If global, which symbol refers to the address?
		Symbol      global_address;
		// | If this is a register, which string identifies it?
		std::string register_;
		// | Dereference this global address or register, or use it directly?
		bool        dereference;
		// | If global and/or dereferencing, add this value to the loaded result.  If dereferencing, add before dereferencing.  (Does nothing when neither global or dereferencing.)
		int32_t     offset;

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
	};

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
			null_tag           = 0,
			load_immediate_tag = 1,
			load_from_tag      = 2,
			add_from_tag       = 3,
			num_tags           = 3,
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
			LoadFrom(const Base &base, bool is_word, int32_t addition);
			// | Are we loading a byte or a word?
			bool is_word;
			// | destination <- source + addition
			// (Addition applies to values, not addresses.)
			int32_t addition;

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

		using data_t = std::variant<
			std::monostate,
			LoadImmediate,
			LoadFrom,
			AddFrom
		>;

		Instruction();
		Instruction(tag_t tag, const data_t &data);
		Instruction(tag_t tag, data_t &&data);
		tag_t tag;
		data_t data;

		const Base &get_base() const;
		Base &&get_base();

		explicit Instruction(const LoadImmediate &load_immediate);
		explicit Instruction(const LoadFrom      &load_from);
		explicit Instruction(const AddFrom       &add_from);

		bool is_load_immediate() const;
		bool is_load_from()      const;
		bool is_add_from()       const;

		// | The tags must be correct, or else an exception will be thrown, including for set_*.
		const LoadImmediate &get_load_immediate() const;
		const LoadFrom      &get_load_from()      const;
		const AddFrom       &get_add_from()       const;

		LoadImmediate &&get_load_immediate();
		LoadFrom      &&get_load_from();
		AddFrom       &&get_add_from();

		// | Return "load_immediate", "load_from", or "add_from".
		static std::string get_tag_repr(tag_t tag);
		std::string get_tag_repr() const;

		std::vector<uint32_t> get_input_sizes() const;
		std::vector<uint32_t> get_working_sizes() const;
		std::vector<uint32_t> get_output_sizes() const;
		std::vector<uint32_t> get_all_sizes() const;

		std::vector<Output::Line> emit(const std::vector<Storage> &storages) const;
	};

	// TODO: fix required_working_sizes: it's connections *plus* max, not max of connections and max.  But for overlay it's fine as is.
	class MIPSIO {
	public:
		class Node;
		class Connection;
		class Parallel;
		using Graph = FullGraph<Node, Connection, Parallel>;

		// | Connecting Nodes with Empty should have equivalent behavior as the Nodes without Empty.
		class Node {
		public:
			Node(Instruction       &&instruction);
			Node(const Instruction  &instruction);
			Instruction instruction;
		};

		// | Connect every output of "left" with every input of "right".
		class Connection {
		public:
			Connection(const Graph &left, const Graph &right);

			// | Has there been at least one non-empty connection somewhere?
			bool empty;

			std::vector<uint32_t> input_sizes;
			// | For each size, max count of this set of input/output connection, left's, and right's.
			std::map<uint32_t, std::vector<uint32_t>::size_type> required_working_sizes;
			std::vector<uint32_t> output_sizes;
		};

		// | Collect two independent instruction sequences in parallel.
		class Parallel {
		public:
			Parallel(const Graph &left, const Graph &right);

			// | Has there been at least one non-empty branch somewhere?
			bool empty;

			// | The input sizes of all branches / pipelines, which must be equal.
			std::vector<uint32_t> input_sizes;
			// | For each size in any branch, the maximum count in a branch.
			std::map<uint32_t, std::vector<uint32_t>::size_type> required_working_sizes;
			std::vector<uint32_t> output_sizes;
		};

		static std::map<uint32_t, std::vector<uint32_t>::size_type> count_working_sizes(const std::vector<uint32_t> &expanded_working_sizes);
		// | The order might be different, but get a vector of working sizes from largest to smallest.
		static std::vector<uint32_t> expand_working_sizes(const std::map<uint32_t, std::vector<uint32_t>::size_type> &counted_working_sizes);
		// | Set each size to the maximum count/value in which it occurs.
		static std::map<uint32_t, std::vector<uint32_t>::size_type> merge_counted_working_sizes(const std::map<uint32_t, std::vector<uint32_t>::size_type> &a, const std::map<uint32_t, std::vector<uint32_t>::size_type> &b);
		// | merge_counted_working_sizes for any number of counted working sizes vectors.
		static std::map<uint32_t, std::vector<uint32_t>::size_type> merge_counted_working_sizes(const std::vector<std::map<uint32_t, std::vector<uint32_t>::size_type>> &counted_working_sizes_vector);

		MIPSIO();
		MIPSIO(Graph       &&nodes);
		MIPSIO(const Graph  &nodes);
		Graph nodes;

		// | Propagate the storages (e.g. registers) through to the
		// instructions in the instruction graph, and emit instructions using
		// those registers.
		//
		// After the graph is constructed, the calculated needed input,
		// working, and storage units can be used to generate a collection of
		// registers (and, if needed, stack or other space) to pass as
		// "storages".
		//
		// The graph is used calculate requirements for and connect used
		// registers and other storage units.
		std::vector<Output::Line> emit(const std::vector<Storage> &storages) const;

		// | Convenience constructors.
		static MIPSIO copy(const MIPSIO &mips_io);
		// TODO: connect, parallelize (same input and output sizes), etc.
	};

	// The non-const part is the ability to store strings.
	MIPSIO analyze_expression(uint64_t expression, const IdentifierScope &constant_scope, const IdentifierScope &type_scope, const IdentifierScope &var_scope, const IdentifierScope &combined_scope);
	MIPSIO analyze_expression(const Expression &expression, const IdentifierScope &constant_scope, const IdentifierScope &type_scope, const IdentifierScope &var_scope, const IdentifierScope &combined_scope);
	// TODO
	//MIPSIO analyze_statement();

	// | Analyze a sequence of statements.
	//
	// Note: this does not need to necessarily correspond to a ::Block in the
	// grammar tree but can be a sequence of statements without a BEGIN and END
	// keyword.
	// TODO
	//Block analyze_block();

	// TODO
	//Routine analyze_routine();

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
	// | Union of top_level_*_scope.  If !combine_identifier_namespaces, the last identifier overrides.
	IdentifierScope top_level_scope;
	// | The lifetime of anonymous_storage should not exceed that of *_scope, since they may contain raw pointers into this storage.
	IdentifierScope anonymous_storage;

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
