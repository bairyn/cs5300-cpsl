#include <algorithm>     // std::max, std::reverse, std::stable_sort, std::swap
#include <cassert>       // assert
#include <cctype>        // isalnum, isprint, tolower
#include <cstddef>       // std::size_t
#include <iomanip>       // std::fill, std::left, std::right, std::setw
#include <ios>           // std::hex
#include <limits>        // std::numeric_limits
#include <optional>      // std::optional
#include <set>           // std::set
#include <sstream>       // std::ostringstream
#include <string>        // std::string, std::to_string
#include <type_traits>   // std::make_unsigned
#include <utility>       // std::as_const, std::move, std::pair
#include <vector>        // std::vector
#include <variant>       // std::get, std::monostate

#include "grammar.hh"

#include "semantics.hh"

/*
 * Exception types.
 */

SemanticsError::SemanticsError()
	: runtime_error("A semantics error occurred.")
	{}

SemanticsError::SemanticsError(const std::string &message)
	: runtime_error(message)
	{}

/*
 * Semantics types.
 */

const bool Semantics::combine_identifier_namespaces = CPSL_CC_SEMANTICS_COMBINE_IDENTIFIER_NAMESPACES;

Semantics::Symbol::Symbol()
	{}

Semantics::Symbol::Symbol(const std::string &prefix, const std::string &requested_suffix, uint64_t unique_identifier)
	: prefix(prefix)
	, requested_suffix(requested_suffix)
	, unique_identifier(unique_identifier)
	{}

const uint64_t Semantics::Symbol::max_unique_try_iterations = CPSL_CC_SEMANTICS_MAX_UNIQUE_TRY_ITERATIONS;

std::map<Semantics::Symbol, std::string> Semantics::Symbol::generate_symbol_values(const std::set<Symbol> &symbols, const std::set<std::string> additional_names) {
	std::set<std::string> used_names(additional_names);

	std::map<Symbol, std::string> symbol_values;

	for (const Symbol &symbol : std::as_const(symbols)) {
		std::string start(symbol.prefix + symbol.requested_suffix);

		// Is the name available with no modifications?
		if (used_names.find(start) == used_names.cend()) {
			// Use it.
			used_names.insert(start);
			symbol_values.insert({symbol, start});
			continue;
		}

		// Find a different name; just apply numbers, beginning with 2
		bool found = false;
		static const uint64_t start_number = 2;
		for (uint64_t iterations = 0; iterations < max_unique_try_iterations; ++iterations) {
			std::ostringstream stry;
			stry << start << iterations + start_number;
			std::string try_ = stry.str();

			// Does this one work?
			if (used_names.find(try_) == used_names.cend()) {
				// Use it.
				symbol_values.insert({symbol, try_});
				found = true;
				break;
			}
		}
		if (!found) {
			std::ostringstream sstr;
			sstr
				<< "Semantics::Symbol::generate_symbol_values: internal error: failed to find a unique symbol value in " << max_unique_try_iterations << " iterations." << std::endl
				<< "  symbol.prefix            : " << symbol.prefix             << std::endl
				<< "  symbol.requested_suffix  : " << symbol.requested_suffix   << std::endl
				<< "  symbol.unique_identifier : " << symbol.unique_identifier
				;
			throw SemanticsError(sstr.str());
		}
	}

	return symbol_values;
}

Semantics::Output::SymbolLocation::SymbolLocation()
	{}

Semantics::Output::SymbolLocation::SymbolLocation(section_t section, std::vector<std::string>::size_type line, std::string::size_type start_pos, std::string::size_type length)
	: section(section)
	, line(line)
	, start_pos(start_pos)
	, length(length)
	{}

// | Returns true if b is less than a, so that when used with
// std::stable_sort from <algorithm> as the 3rd argument, a vector
// can be reverse sorted.
bool Semantics::Output::SymbolLocation::reverse_cmp(const SymbolLocation &a, const SymbolLocation &b) {
	if        (b.section   < a.section) {
		return true;
	} else if (b.line      < a.line) {
		return true;
	} else if (b.start_pos < a.start_pos) {
		return true;
	} else if (b.length    < a.length) {
		return true;
	} else {
		return false;
	}
}

Semantics::Output::Line::Line()
	{}

// | No symbols on this line.
Semantics::Output::Line::Line(const std::string &line)
	: line(line)
	{}

Semantics::Output::Line::Line(std::string &&line)
	: line(std::move(line))
	{}

Semantics::Output::Line::Line(const char *c_str)
	: line(std::string(c_str))
	{}

// | Line with only a symbol.
Semantics::Output::Line::Line(const Symbol &symbol)
	: Line(std::as_const(""), symbol)
	{}

// | Add a symbol to a line in a new copy.
Semantics::Output::Line::Line(const Line &line, const Symbol &symbol, std::string::size_type start_pos, std::string::size_type length)
	: Line(line)
{
	symbols.push_back({symbol, {start_pos, length}});
}

#if 0
// | Line with a single symbol.
Semantics::Output::Line::Line(const std::string &line, const Symbol &symbol, std::string::size_type start_pos, std::string::size_type length)
	: line(line)
	, symbols(1, {symbol, {start_pos, length}})
	{}

Semantics::Output::Line::Line(std::string &&line, const Symbol &symbol, std::string::size_type start_pos, std::string::size_type length)
	: line(std::move(line))
	, symbols(1, {symbol, {start_pos, length}})
	{}
#endif /* #if 0 */

// | Line with zero or more symbols.
Semantics::Output::Line::Line(const std::string &line, const std::vector<std::pair<Symbol, std::pair<std::string::size_type, std::string::size_type>>> &symbols)
	: line(line)
	, symbols(symbols)
	{}

Semantics::Output::Line::Line(const std::string &line, std::vector<std::pair<Symbol, std::pair<std::string::size_type, std::string::size_type>>> &&symbols)
	: line(line)
	, symbols(std::move(symbols))
	{}

Semantics::Output::Line::Line(std::string &&line, const std::vector<std::pair<Symbol, std::pair<std::string::size_type, std::string::size_type>>> &symbols)
	: line(std::move(line))
	, symbols(symbols)
	{}

Semantics::Output::Line::Line(std::string &&line, std::vector<std::pair<Symbol, std::pair<std::string::size_type, std::string::size_type>>> &&symbols)
	: line(std::move(line))
	, symbols(std::move(symbols))
	{}

// | Add zero or more symbols to a line in a new copy.
Semantics::Output::Line::Line(const Line &line, const std::vector<std::pair<Symbol, std::pair<std::string::size_type, std::string::size_type>>> &a_symbols)
	: Line(line)
{
	this->symbols.insert(this->symbols.end(), a_symbols.cbegin(), a_symbols.cend());
}

// Note: operator+ is defined for Line and Line/Symbol using these methods.
Semantics::Output::Line Semantics::Output::Line::plus(const Line &b) const {
	Line concatenated_line(std::as_const(*this));
	std::string::size_type a_length = this->line.size();
	concatenated_line.line += b.line;
	for (const std::pair<Symbol, std::pair<std::string::size_type, std::string::size_type>> &b_symbol : std::as_const(b.symbols)) {
		concatenated_line.symbols.push_back({b_symbol.first, {b_symbol.second.first + a_length, b_symbol.second.second}});
	}
	return concatenated_line;
}

Semantics::Output::Line Semantics::Output::Line::plus(const Symbol &b) const {
	return this->plus(Line({"", b}));
}

Semantics::Output::Line Semantics::Output::Line::flip_plus(const Line &a) const {
	return a.plus(*this);
}

Semantics::Output::Line Semantics::Output::Line::flip_plus(const Symbol &a) const {
	return Line(a).plus(*this);
}

Semantics::Output::Output()
	{}

// | Add a symbol location.
void Semantics::Output::add_symbol_location(const Symbol &symbol, const SymbolLocation &symbol_location) {
	// Is this a recognized symbol?
	std::map<Symbol, std::vector<SymbolLocation>>::iterator unexpanded_symbols_search = unexpanded_symbols.find(symbol);
	if (unexpanded_symbols_search == unexpanded_symbols.end()) {
		// Nope.
		unexpanded_symbols.insert({symbol, std::vector<SymbolLocation>(1, symbol_location)});
	} else {
		// Yep.
		unexpanded_symbols_search->second.push_back(symbol_location);
	}

	// Is this a recognized line?
	std::pair<section_t, std::vector<std::string>::size_type> line {symbol_location.section, symbol_location.line};
	std::map<std::pair<section_t, std::vector<std::string>::size_type>, std::vector<Symbol>>::iterator reverse_unexpanded_symbols_search = reverse_unexpanded_symbols.find(std::as_const(line));
	if (reverse_unexpanded_symbols_search == reverse_unexpanded_symbols.end()) {
		// Nope.
		reverse_unexpanded_symbols.insert({std::as_const(line), std::vector<Symbol>(1, symbol)});
	} else {
		// Yep.
		reverse_unexpanded_symbols_search->second.push_back(symbol);
	}
}

bool Semantics::Output::is_normalized() const {
	if (unexpanded_symbols.size() > 0) {
		// There are unexpanded symbols.
		return false;
	}

	// It's normalized if both normalized_output and sections are available,
	// or the entire output is empty.
	bool empty_normalized = normalized_lines.size() <= 0;
	bool empty_sections = true;
	if (sections.size() > 0) {
		for (const std::vector<std::string> &section : std::as_const(sections)) {
			if (section.size() > 0) {
				empty_sections = false;
				break;
			}
		}
	}

	return empty_normalized == empty_sections;
}

// | Return a new output, expanding unexpanded symbols, so that they
// are unique and different from any element of "additional_names".
//
// If this output is already normalized, return a copy of this output.
Semantics::Output Semantics::Output::normalize(const std::set<std::string> &additional_names) const {
	if (is_normalized()) {
		return Output(std::as_const(*this));
	} else {
		Output normalized_output;

		// If sections is empty, treat it as empty.
		if (sections.size() <= 0) {
			// No sections; just ensure normalized_output's normalized_lines is clear.
			normalized_output.normalized_lines.clear();
		} else {
			// Make sure we have the correct number of sections.
			if (sections.size() != num_sections + 1) {
				std::ostringstream sstr;
				sstr << "Semantics::Output::normalize: invalid number of sections: " << sections.size() << " != " << num_sections + 1;
				throw SemanticsError(sstr.str());
			}

			// Make sure the null section is empty.
			if (sections[0].size() > 0) {
				std::ostringstream sstr;
				sstr << "Semantics::Output::normalize: error: the null section is not empty.";
				throw SemanticsError(sstr.str());
			}

			// Get unique names for each symbol.
			std::set<Symbol> symbols;
			for (const std::pair<Symbol, std::vector<SymbolLocation>> &pair : std::as_const(unexpanded_symbols)) {
				const Symbol &symbol = pair.first;
				symbols.insert(symbol);
			}
			std::map<Symbol, std::string> symbol_values = Symbol::generate_symbol_values(symbols, additional_names);

			// Normalize the sections.

			// Iterate over each line in the unexpanded output.
			for (const std::vector<std::string> &section : std::as_const(sections)) {
				section_t section_index = static_cast<section_t>(&section - &sections[0]);

				normalized_output.sections.push_back(std::vector<std::string>());
				std::vector<std::string> &normalized_section = normalized_output.sections[normalized_output.sections.size() - 1];

				for (const std::string &line : std::as_const(section)) {
					std::vector<std::string>::size_type line_index = static_cast<std::vector<std::string>::size_type>(&line - &section[0]);

					std::string normalized_line;

					// Are there symbols on this line?
					std::map<std::pair<section_t, std::vector<std::string>::size_type>, std::vector<Symbol>>::const_iterator reverse_unexpanded_symbols_search = reverse_unexpanded_symbols.find({section_index, line_index});
					if (reverse_unexpanded_symbols_search == reverse_unexpanded_symbols.cend()) {
						// Nope.
						normalized_line = line;
					} else {
						// Yep.
						const std::vector<Symbol> &line_symbols = reverse_unexpanded_symbols_search->second;

						// Just copy the line and then do new substitutions.
						normalized_line = line;

						// Substitute each symbol.
						for (const Symbol &symbol : std::as_const(line_symbols)) {
							// Get the locations and the unique name for this symbol.
							std::map<Symbol, std::string>::const_iterator                 symbol_values_search      = symbol_values.find(symbol);
							std::map<Symbol, std::vector<SymbolLocation>>::const_iterator unexpanded_symbols_search = unexpanded_symbols.find(symbol);

							// These both should exist, or there is an internal error.
							if (symbol_values_search == symbol_values.cend()) {
								std::ostringstream sstr;
								sstr
									<< "Semantics::Output::normalize: internal error: failed to find the generated unique name for the unexpanded symbol ``"
									<< symbol.prefix << "\":``" << symbol.requested_suffix
									<< "\".  Are ``symbol_values\" (local) and ``reverse_unexpanded_symbols\" consistent?"
									;
								throw SemanticsError(sstr.str());
							}
							if (unexpanded_symbols_search == unexpanded_symbols.cend()) {
								std::ostringstream sstr;
								sstr
									<< "Semantics::Output::normalize: internal error: failed to find locations for the unexpanded symbol ``"
									<< symbol.prefix << "\":``" << symbol.requested_suffix
									<< "\".  Are ``unexpanded_symbols\" and ``reverse_unexpanded_symbols\" consistent?"
									;
								throw SemanticsError(sstr.str());
							}

							// Assign the locations and the unique name for this symbol.
							const std::string                 &symbol_value     = symbol_values_search->second;
							const std::vector<SymbolLocation> &symbol_locations = unexpanded_symbols_search->second;

							// Sort the symbol locations right-to-left.
							std::vector<SymbolLocation> sorted_symbol_locations(symbol_locations);
							std::stable_sort(sorted_symbol_locations.begin(), sorted_symbol_locations.end(), SymbolLocation::reverse_cmp);

							// Perform a substitution at each location.
							for (const SymbolLocation &symbol_location : std::as_const(sorted_symbol_locations)) {
								// Make sure the location is within bounds.
								if (symbol_location.start_pos >= line.size() || symbol_location.start_pos + symbol_location.length >= line.size()) {
									std::ostringstream sstr;
									sstr
										<< "Semantics::Output::normalize: error: a symbol location in the output refers to an out-of-bounds location." << std::endl
										<< "  symbol.prefix             : " << symbol.prefix             << std::endl
										<< "  symbol.requested_suffix   : " << symbol.requested_suffix   << std::endl
										<< "  symbol.unique_identifier  : " << symbol.unique_identifier  << std::endl
										<< "  symbol_value              : " << symbol_value              << std::endl
										<< "  symbol_location.line      : " << symbol_location.line      << std::endl
										<< "  symbol_location.start_pos : " << symbol_location.start_pos << std::endl
										<< "  symbol_location.length    : " << symbol_location.length
										;
									throw SemanticsError(sstr.str());
								}

								// Perform the substitution.
								normalized_line = normalized_line.substr(0, symbol_location.start_pos) + symbol_value + normalized_line.substr(symbol_location.start_pos + symbol_location.length);
							}
						}
					}

					// Add this normalized line to the normalized output.
					normalized_section.push_back(normalized_line);
					// Also add this normalized line to the normalized_output vector.
					normalized_output.normalized_lines.push_back(normalized_line);
				}
			}

#if 0
			// Now that the sections are normalized, join the output.
			// Ensure normalized_output's normalized_lines is clear before we add to it.
			normalized_output.normalized_lines.clear();
			for (const std::vector<std::string> &section : sections) {
				for (const std::string &line : section) {
					normalized_output.normalized_lines.push_back(line);
				}
			}
#endif /* #if 0 */
		}

		// Make sure the new output is actually normalized, or else get_normalized_lines_copy() may loop infinitely.
		if (!normalized_output.is_normalized()) {
			std::ostringstream sstr;
			sstr << "Semantics::Output::normalize: internal error: normalize() produced output that was not detected as normalized.";
			throw SemanticsError(sstr.str());
		}

		// Return the new normalized output.
		return normalized_output;
	}
}

// | Normalize this output if it isn't normalized to a new value, and
// discard the new output container after returning a copy of its
// lines.
std::vector<std::string> Semantics::Output::get_normalized_lines_copy(const std::set<std::string> &additional_names) const {
	if (is_normalized()) {
		return std::vector<std::string>(std::as_const(normalized_lines));
	} else {
		return normalize(additional_names).get_normalized_lines_copy(additional_names);
	}
}

// Commented out: this definition is redundant and would cause some calls to be ambiguous.
#if 0
// | Add a line to an output section.
void Semantics::Output::add_line(section_t section, const std::string &line) {
	// If the section vector is empty, initialize it.
	if (sections.size() <= 0) {
		for (Output::section_t section = Output::null_section; section <= Output::num_sections; section = static_cast<Output::section_t>(static_cast<int>(section) + 1)) {
			sections.push_back(std::vector<std::string>());
		}
	}

	// Make sure we have the correct number of sections.
	if (sections.size() != num_sections + 1) {
		std::ostringstream sstr;
		sstr << "Semantics::Output::add_line: invalid number of sections: " << sections.size() << " != " << num_sections + 1;
		throw SemanticsError(sstr.str());
	}

	// Make sure the section exists.
	if (section >= sections.size()) {
		std::ostringstream sstr;
		sstr
			<< "Semantics::Output::add_line: error: attempted to add a line to a non-existent output section." << std::endl
			<< "  section : " << section << std::endl
			<< "  line    :"  << line
			;
		throw SemanticsError(sstr.str());
	}

	// Add the line.
	sections[section].push_back(line);
}
#endif /* #if 0 */

// | Add a line to an output section with a symbol.
void Semantics::Output::add_line(section_t section, const std::string &line, const Symbol &symbol, std::string::size_type start_pos, std::string::size_type length) {
	add_line(section, line);
	add_symbol_location_current_last_line(section, symbol, start_pos, length);
}

#if 0
// | Add a line to an output, optionally with symbols.
void Semantics::Output::add_line(section_t section, const Line &line) {
	add_line(section, line);
	for (const std::pair<Symbol, std::pair<std::string::size_type, std::string::size_type>> &symbol : std::as_const(line.symbols)) {
		add_symbol_location_current_last_line(section, symbol.first, symbol.second.first, symbol.second.second);
	}
}
#endif /* #if 0 */

// | Add a line to an output, optionally with symbols.
void Semantics::Output::add_line(section_t section, const Line &line) {
	// If the section vector is empty, initialize it.
	if (sections.size() <= 0) {
		for (Output::section_t section = Output::null_section; section <= Output::num_sections; section = static_cast<Output::section_t>(static_cast<int>(section) + 1)) {
			sections.push_back(std::vector<std::string>());
		}
	}

	// Make sure we have the correct number of sections.
	if (sections.size() != num_sections + 1) {
		std::ostringstream sstr;
		sstr << "Semantics::Output::add_line: invalid number of sections: " << sections.size() << " != " << num_sections + 1;
		throw SemanticsError(sstr.str());
	}

	// Make sure the section exists.
	if (section >= sections.size()) {
		std::ostringstream sstr;
		sstr
			<< "Semantics::Output::add_line: error: attempted to add a line to a non-existent output section." << std::endl
			<< "  section : " << section << std::endl
			<< "  line    :"  << line.line
			;
		throw SemanticsError(sstr.str());
	}

	// Add the line.
	sections[section].push_back(line.line);

	// Add the symbols.
	for (const std::pair<Symbol, std::pair<std::string::size_type, std::string::size_type>> &symbol : std::as_const(line.symbols)) {
		add_symbol_location_current_last_line(section, symbol.first, symbol.second.first, symbol.second.second);
	}
}

// | Add lines to output, optionally with symbols.
void Semantics::Output::add_lines(section_t section, const std::vector<Line> &lines) {
	for (const Line &line : std::as_const(lines)) {
		add_line(section, line);
	}
}

// | Add a symbol to the last line.
void Semantics::Output::add_symbol_location_current_last_line(section_t section, const Symbol &symbol, std::string::size_type start_pos, std::string::size_type length) {
	// If the section vector is empty, initialize it.
	if (sections.size() <= 0) {
		for (Output::section_t section = Output::null_section; section <= Output::num_sections; section = static_cast<Output::section_t>(static_cast<int>(section) + 1)) {
			sections.push_back(std::vector<std::string>());
		}
	}

	// Make sure we have the correct number of sections.
	if (sections.size() != num_sections + 1) {
		std::ostringstream sstr;
		sstr << "Semantics::Output::add_symbol_location_current_last_line: invalid number of sections: " << sections.size() << " != " << num_sections + 1;
		throw SemanticsError(sstr.str());
	}

	// Make sure the section exists.
	if (section >= sections.size()) {
		std::ostringstream sstr;
		sstr
			<< "Semantics::Output::add_symbol_location_current_last_line: error: attempted to add a symbol location to a non-existent output section." << std::endl
			<< "  section : " << section << std::endl
			<< "  symbol.prefix            : " << symbol.prefix             << std::endl
			<< "  symbol.requested_suffix  : " << symbol.requested_suffix   << std::endl
			<< "  symbol.unique_identifier : " << symbol.unique_identifier  << std::endl
			<< "  start_pos                : " << start_pos                 << std::endl
			<< "  length                   : " << length
			;
		throw SemanticsError(sstr.str());
	}

	// Get the section.
	const std::vector<std::string> &output_section = sections[section];

	// Make sure the section isn't empty.
	if (output_section.size() <= 0) {
		std::ostringstream sstr;
		sstr
			<< "Semantics::Output::add_symbol_location_current_last_line: error: attempted to add a symbol location to the current last line when there are currently no lines." << std::endl
			<< "  section : " << section << std::endl
			<< "  symbol.prefix            : " << symbol.prefix             << std::endl
			<< "  symbol.requested_suffix  : " << symbol.requested_suffix   << std::endl
			<< "  symbol.unique_identifier : " << symbol.unique_identifier  << std::endl
			<< "  start_pos                : " << start_pos                 << std::endl
			<< "  length                   : " << length
			;
		throw SemanticsError(sstr.str());
	}

	// Add the symbol location.
	add_symbol_location(symbol, SymbolLocation(section, output_section.size() - 1, start_pos, length));
}

bool Semantics::Output::is_section_empty(section_t section) const {
	// Make sure the section index is valid.
	if (section < null_section || section > num_sections) {
		std::ostringstream sstr;
		sstr
			<< "Semantics::Output::is_section_empty: error: attempted to access a section with an invalid index." << std::endl
			<< "  section                   : " << section      << std::endl
			<< "  num_sections (recognized) : " << num_sections
			;
		throw SemanticsError(sstr.str());
	}

	// If the sections vector is empty, just treat this section as empty.
	if (sections.size() <= 0) {
		return true;
	}

	// Make sure we have the correct number of sections.
	if (sections.size() != num_sections + 1) {
		std::ostringstream sstr;
		sstr << "Semantics::Output::is_section_empty: invalid number of sections: " << sections.size() << " != " << num_sections + 1;
		throw SemanticsError(sstr.str());
	}

	// Make sure the section exists.
	if (section >= sections.size()) {
		std::ostringstream sstr;
		sstr
			<< "Semantics::Output::is_section_empty: error: attempted to access a non-existent section when checking whether it is empty." << std::endl
			<< "  section : " << section
			;
		throw SemanticsError(sstr.str());
	}

	// Return whether it's empty.
	return sections[section].size() <= 0;
}

Semantics::Type::Base::Base()
	{}

Semantics::Type::Base::Base(const std::string &identifier, bool fixed_width, uint32_t size)
	: identifier(identifier)
	, fixed_width(fixed_width)
	, size(size)
	{}

Semantics::Type::Base::Base(std::string &&identifier, bool fixed_width, uint32_t size)
	: identifier(std::move(identifier))
	, fixed_width(fixed_width)
	, size(size)
	{}

const std::string &Semantics::Type::Base::get_identifier()  const { return identifier; }
bool               Semantics::Type::Base::get_fixed_width() const { return fixed_width; }
uint32_t           Semantics::Type::Base::get_size()        const { return size; }

Semantics::Type::Primitive::Primitive()
	{}

Semantics::Type::Primitive::Primitive(const Base &base, tag_t tag)
	: Base(base)
	, tag(tag)
	{}

Semantics::Type::Primitive::Primitive(Base &&base, tag_t tag)
	: Base(std::move(base))
	, tag(tag)
	{}

Semantics::Type::Primitive::Primitive(tag_t tag)
	: tag(tag)
	{}

const Semantics::Type::Primitive Semantics::Type::Primitive::integer_type {
	{"integer", true, 4},
	Semantics::Type::Primitive::integer_tag,
};

const Semantics::Type::Primitive Semantics::Type::Primitive::char_type {
	{"char", true, 1},
	Semantics::Type::Primitive::char_tag,
};

const Semantics::Type::Primitive Semantics::Type::Primitive::boolean_type {
	{"boolean", true, 1},
	Semantics::Type::Primitive::boolean_tag,
};

const Semantics::Type::Primitive Semantics::Type::Primitive::string_type {
	//{"string", false, 0},  // dynamic array
	{"string", true, 4},  // char *
	Semantics::Type::Primitive::string_tag,
};

bool Semantics::Type::Primitive::is_integer() const {
	switch(tag) {
		case integer_tag:
			return true;
		case char_tag:
		case boolean_tag:
		case string_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Type::Primitive::is_integer: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::Type::Primitive::is_char() const {
	switch(tag) {
		case integer_tag:
			return false;
		case char_tag:
			return true;
		case boolean_tag:
		case string_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Type::Primitive::is_char: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::Type::Primitive::is_boolean() const {
	switch(tag) {
		case integer_tag:
		case char_tag:
			return false;
		case boolean_tag:
			return true;
		case string_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Type::Primitive::is_boolean: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::Type::Primitive::is_string() const {
	switch(tag) {
		case integer_tag:
		case char_tag:
		case boolean_tag:
			return false;
		case string_tag:
			return true;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Type::Primitive::is_string: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

// | Return "integer", "char", "boolean", or "string".
std::string Semantics::Type::Primitive::get_tag_repr(tag_t tag) {
	switch(tag) {
		case integer_tag:
			return "integer";
		case char_tag:
			return "char";
		case boolean_tag:
			return "boolean";
		case string_tag:
			return "string";

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Type::Primitive::get_tag_repr: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

std::string Semantics::Type::Primitive::get_tag_repr() const {
	return get_tag_repr(tag);
}

bool Semantics::Type::Primitive::is_word(bool permit_in_between_size) const {
	if (!fixed_width) {
		std::ostringstream sstr;
		sstr << "Semantics::Type::Primitive::is_word: error: primitive type ``" << get_tag_repr() << "\" is not fixed width when calculating fixed primitive type size.";
		throw SemanticsError(sstr.str());
	}

	if (size > 4) {
		std::ostringstream sstr;
		sstr << "Semantics::Type::Primitive::is_word: error: primitive type ``" << get_tag_repr() << "\" has size (" << size << ") greater than 4 when calculating fixed primitive type size.";
		throw SemanticsError(sstr.str());
	}

	if (!permit_in_between_size && size != 4 && size != 1) {
		std::ostringstream sstr;
		sstr
			<< "Semantics::Type::Primitive::is_word: error: primitive type ``"
			<< get_tag_repr() << "\" with size " << size
			<< " is not of size either 4 or 1 when calculating fixed primitive type size, and the ``permit_in_between_size\" argument is false."
			;
		throw SemanticsError(sstr.str());
	}

	if (!permit_in_between_size) {
		if (size == 4) {
			return true;
		} else {
			return false;
		}
	} else {
		if (size > 1) {
			// size <= 4.
			return false;
		} else {
			return true;
		}
	}
}

Semantics::Type::Simple::Simple()
	{}

#if 0
Semantics::Type::Simple::Simple(const Base &base, const Type &referent)
	: Base(base)
	, referent(&referent)
	{}

Semantics::Type::Simple::Simple(Base &&base, const Type &referent)
	: Base(std::move(base))
	, referent(&referent)
	{}
#endif /* #if 0 */

Semantics::Type::Simple::Simple(const std::string &identifier, const Type &referent)
	: Base(identifier, referent.get_fixed_width(), referent.get_size())
	, referent(&referent)
	{}

Semantics::Type::Simple::Simple(const std::string &identifier, const Type &referent, const IdentifierScope &identifier_type_scope)
	: Base(identifier, identifier_type_scope.get(referent.get_identifier_copy()).get_type().get_fixed_width(), identifier_type_scope.get(referent.get_identifier_copy()).get_type().get_size())
	, referent(&referent)
	{}

// | Resolve a chain of aliases.
// TODO: check for cycles and null.
const Semantics::Type &Semantics::Type::Simple::resolve_type() const {
	const Type *type = referent;
	while (type->is_simple()) {
		type = type->get_simple().referent;
	}
	return *type;
}

Semantics::Type::Record::Record()
	{}

Semantics::Type::Record::Record(const std::string &identifier, const std::vector<std::pair<std::string, const Type *>> &fields, IdentifierScope &anonymous_storage)
	: Record(identifier, std::move(std::vector<std::pair<std::string, const Type *>>(fields)), anonymous_storage)
	{}

Semantics::Type::Record::Record(const std::string &identifier, std::vector<std::pair<std::string, const Type *>> &&fields, IdentifierScope &anonymous_storage)
	: fields(fields)
	, anonymous_storage(&anonymous_storage)
{
	this->identifier = identifier;
	fixed_width = true;
	size = 0;
	for (const std::pair<std::string, const Type *> &field : this->fields) {
		// TODO: check for overflow.
		const std::string &field_identifier = field.first;
		const Type        &field_type       = *field.second;

		if (!field_type.get_fixed_width()) {
			fixed_width = false;
		}

		size += field_type.get_size();
	}
}

Semantics::Type::Array::Array()
	{}

Semantics::Type::Array::Array(const std::string &identifier, const Type *base_type, int32_t min_index, int32_t max_index, IdentifierScope &anonymous_storage)
	: base_type(base_type)
	, min_index(min_index)
	, max_index(max_index)
	, anonymous_storage(&anonymous_storage)
{
	this->identifier = identifier;
	fixed_width = base_type->get_fixed_width();
	if (min_index > max_index) {
		std::ostringstream sstr;
		if (identifier.size() <= 0) {
			sstr << "Semantics::Type::Array::Array: attempt to construct an anonymous array type with a minimum index greater than the maximum: " << min_index << " > " << max_index << ".";
		} else {
			sstr << "Semantics::Type::Array::Array: attempt to construct an array type (``" << identifier << "\") with a minimum index greater than the maximum: " << min_index << " > " << max_index << ".";
		}
		throw SemanticsError(sstr.str());
	}
	if (would_addition_overflow(get_index_range(), 1) || would_multiplication_overflow(get_index_range() + 1, base_type->get_size())) {
		std::ostringstream sstr;
		if (identifier.size() <= 0) {
			sstr << "Semantics::Type::Array::Array: attempt to construct an anonymous array type with a size that is too large: some indices would be too big to fit into a 32-bit signed integer.";
		} else {
			sstr << "Semantics::Type::Array::Array: attempt to construct an array type (``" << identifier << "\") with a size that is too large: some indices would be too big to fit into a 32-bit signed integer.";
		}
		sstr << "  Index range: " << get_index_range() << "; base type size: " << base_type->get_size();
		throw SemanticsError(sstr.str());
	}
	size = get_index_range() * base_type->get_size();
}

int32_t Semantics::Type::Array::get_min_index() const {
	return min_index;
}

int32_t Semantics::Type::Array::get_max_index() const {
	return max_index;
}

int32_t Semantics::Type::Array::get_begin_index() const {
	return min_index;
}

int32_t Semantics::Type::Array::get_end_index() const {
	return max_index + 1;
}

uint32_t Semantics::Type::Array::get_index_range() const {
	return get_end_index() - get_begin_index();
}

uint32_t Semantics::Type::Array::get_offset_of_index(int32_t index) const {
	if (index < get_begin_index()) {
		std::ostringstream sstr;
		sstr << "Semantics::Type::Array::get_offset_of_index: attempt to obtain offset of out-of-bounds array index: " << index << " < " << get_begin_index() << ".";
		throw SemanticsError(sstr.str());
	}

	if (index >= get_end_index()) {
		std::ostringstream sstr;
		sstr << "Semantics::Type::Array::get_offset_of_index: attempt to obtain offset of out-of-bounds array index: " << index << " >= " << get_end_index() << ".";
		throw SemanticsError(sstr.str());
	}

	return index - get_begin_index();
}

int32_t Semantics::Type::Array::get_index_of_offset(uint32_t offset) const {
	if (get_index_range() <= 0) {
		std::ostringstream sstr;
		sstr << "Semantics::Type::Array::get_index_of_offset: attempt to obtain array index of an empty array.";
		throw SemanticsError(sstr.str());
	}

	if (offset >= get_index_range()) {
		std::ostringstream sstr;
		sstr << "Semantics::Type::Array::get_index_of_offset: attempt to obtain array index of out-of-bounds offset: " << offset << " >= " << get_index_range() << ".";
		throw SemanticsError(sstr.str());
	}

	return get_begin_index() + offset;
}

Semantics::Type::Type()
	{}

Semantics::Type::Type(tag_t tag, const data_t &data)
	: tag(tag)
	, data(data)
	{}

Semantics::Type::Type(tag_t tag, data_t &&data)
	: tag(tag)
	, data(std::move(data))
	{}

const Semantics::Type Semantics::Type::integer_type(std::as_const(Primitive::integer_type));
const Semantics::Type Semantics::Type::char_type(std::as_const(Primitive::char_type));
const Semantics::Type Semantics::Type::boolean_type(std::as_const(Primitive::boolean_type));
const Semantics::Type Semantics::Type::string_type(std::as_const(Primitive::string_type));

const Semantics::Type::Base &Semantics::Type::get_base() const {
	switch(tag) {
		case primitive_tag:
			return get_primitive();
		case simple_tag:
			return get_simple();
		case record_tag:
			return get_record();
		case array_tag:
			return get_array();

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Type::get_base: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

std::string Semantics::Type::get_identifier_copy() const {
	return std::string(std::as_const(get_base().identifier));
}

bool Semantics::Type::get_fixed_width() const {
	return get_base().fixed_width;
}

uint32_t Semantics::Type::get_size() const {
	return get_base().size;
}

Semantics::Type::Base &&Semantics::Type::get_base() {
	switch(tag) {
		case primitive_tag:
			return get_primitive();
		case simple_tag:
			return get_simple();
		case record_tag:
			return get_record();
		case array_tag:
			return get_array();

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Type::get_base: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

Semantics::Type::Type(const Primitive &primitive)
	: Type(primitive_tag, primitive)
	{}

Semantics::Type::Type(const Simple &simple)
	: Type(simple_tag, simple)
	{}

Semantics::Type::Type(const Record &record)
	: Type(record_tag, record)
	{}

Semantics::Type::Type(const Array &array)
	: Type(array_tag, array)
	{}

Semantics::Type::Type(Primitive &&primitive)
	: Type(primitive_tag, std::move(primitive))
	{}

Semantics::Type::Type(Simple &&simple)
	: Type(simple_tag, std::move(simple))
	{}

Semantics::Type::Type(Record &&record)
	: Type(record_tag, std::move(record))
	{}

Semantics::Type::Type(Array &&array)
	: Type(array_tag, std::move(array))
	{}

bool Semantics::Type::is_primitive() const {
	switch(tag) {
		case primitive_tag:
			return true;
		case simple_tag:
		case record_tag:
		case array_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Type::is_primitive: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::Type::is_simple() const {
	switch(tag) {
		case primitive_tag:
			return false;
		case simple_tag:
			return true;
		case record_tag:
		case array_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Type::is_simple: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::Type::is_record() const {
	switch(tag) {
		case primitive_tag:
		case simple_tag:
			return false;
		case record_tag:
			return true;
		case array_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Type::is_record: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::Type::is_array() const {
	switch(tag) {
		case primitive_tag:
		case simple_tag:
		case record_tag:
			return false;
		case array_tag:
			return true;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Type::is_array: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

// | The tags must be correct, or else an exception will be thrown, including for set_*.
const Semantics::Type::Primitive &Semantics::Type::get_primitive() const {
	switch(tag) {
		case primitive_tag:
		case simple_tag:
		case record_tag:
		case array_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Type::get_primitive: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_primitive()) {
		std::ostringstream sstr;
		sstr << "Semantics::Type::get_primitive: binding has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<Primitive>(data);
}

const Semantics::Type::Simple &Semantics::Type::get_simple() const {
	switch(tag) {
		case primitive_tag:
		case simple_tag:
		case record_tag:
		case array_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Type::get_simple: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_simple()) {
		std::ostringstream sstr;
		sstr << "Semantics::Type::get_simple: binding has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<Simple>(data);
}

const Semantics::Type::Record &Semantics::Type::get_record() const {
	switch(tag) {
		case primitive_tag:
		case simple_tag:
		case record_tag:
		case array_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Type::get_record: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_record()) {
		std::ostringstream sstr;
		sstr << "Semantics::Type::get_record: binding has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<Record>(data);
}

const Semantics::Type::Array &Semantics::Type::get_array() const {
	switch(tag) {
		case primitive_tag:
		case simple_tag:
		case record_tag:
		case array_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Type::get_array: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_array()) {
		std::ostringstream sstr;
		sstr << "Semantics::Type::get_array: binding has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<Array>(data);
}

// | The tags must be correct, or else an exception will be thrown, including for set_*.
Semantics::Type::Primitive &&Semantics::Type::get_primitive() {
	switch(tag) {
		case primitive_tag:
		case simple_tag:
		case record_tag:
		case array_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Type::get_primitive: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_primitive()) {
		std::ostringstream sstr;
		sstr << "Semantics::Type::get_primitive: binding has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<Primitive>(std::move(data));
}

Semantics::Type::Simple &&Semantics::Type::get_simple() {
	switch(tag) {
		case primitive_tag:
		case simple_tag:
		case record_tag:
		case array_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Type::get_simple: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_simple()) {
		std::ostringstream sstr;
		sstr << "Semantics::Type::get_simple: binding has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<Simple>(std::move(data));
}

Semantics::Type::Record &&Semantics::Type::get_record() {
	switch(tag) {
		case primitive_tag:
		case simple_tag:
		case record_tag:
		case array_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Type::get_record: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_record()) {
		std::ostringstream sstr;
		sstr << "Semantics::Type::get_record: binding has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<Record>(std::move(data));
}

Semantics::Type::Array &&Semantics::Type::get_array() {
	switch(tag) {
		case primitive_tag:
		case simple_tag:
		case record_tag:
		case array_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Type::get_array: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_array()) {
		std::ostringstream sstr;
		sstr << "Semantics::Type::get_array: binding has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<Array>(std::move(data));
}

// | Return "primitive", "simple", "record", or "array".
std::string Semantics::Type::get_tag_repr(tag_t tag) {
	switch(tag) {
		case primitive_tag:
			return "primitive";
		case simple_tag:
			return "simple";
		case record_tag:
			return "record";
		case array_tag:
			return "array";

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::get_tag_repr: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

std::string Semantics::Type::get_tag_repr() const {
	return get_tag_repr(tag);
}

// | If this is a type alias, resolve the type to get the base type;
// otherwise, just return this type.
const Semantics::Type &Semantics::Type::resolve_type() const {
	if (is_simple()) {
		return get_simple().resolve_type();
	} else {
		return *this;
	}
}

const Semantics::ConstantValue::Dynamic Semantics::ConstantValue::Dynamic::dynamic {};

Semantics::ConstantValue::ConstantValue()
	: tag(null_tag)
	, data(std::monostate())
	{}

Semantics::ConstantValue::ConstantValue(tag_t tag, const data_t &data, uint64_t lexeme_begin, uint64_t lexeme_end)
	: tag(tag)
	, data(data)
	, lexeme_begin(lexeme_begin)
	, lexeme_end(lexeme_end)
	{}

Semantics::ConstantValue::ConstantValue(tag_t tag, data_t &&data, uint64_t lexeme_begin, uint64_t lexeme_end)
	: tag(tag)
	, data(std::move(data))
	, lexeme_begin(lexeme_begin)
	, lexeme_end(lexeme_end)
	{}

// | Copy the constant value but use new lexeme identifiers.
Semantics::ConstantValue::ConstantValue(const ConstantValue &constant_value, uint64_t lexeme_begin, uint64_t lexeme_end)
	: tag(constant_value.tag)
	, data(constant_value.data)
	, lexeme_begin(lexeme_begin)
	, lexeme_end(lexeme_end)
	{}

Semantics::ConstantValue::ConstantValue(ConstantValue &&constant_value, uint64_t lexeme_begin, uint64_t lexeme_end)
	: tag(constant_value.tag)
	, data(std::move(constant_value.data))
	, lexeme_begin(lexeme_begin)
	, lexeme_end(lexeme_end)
	{}

const Semantics::ConstantValue Semantics::ConstantValue::true_constant  {static_cast<bool>(true), 0, 0};
const Semantics::ConstantValue Semantics::ConstantValue::false_constant {static_cast<bool>(false), 0, 0};

Semantics::ConstantValue::ConstantValue(const Dynamic &dynamic, uint64_t lexeme_begin, uint64_t lexeme_end)
	: tag(dynamic_tag)
	, data(dynamic)
	, lexeme_begin(lexeme_begin)
	, lexeme_end(lexeme_end)
	{}

Semantics::ConstantValue::ConstantValue(Dynamic &&dynamic, uint64_t lexeme_begin, uint64_t lexeme_end)
	: tag(dynamic_tag)
	, data(std::move(dynamic))
	, lexeme_begin(lexeme_begin)
	, lexeme_end(lexeme_end)
	{}

Semantics::ConstantValue::ConstantValue(int32_t integer, uint64_t lexeme_begin, uint64_t lexeme_end)
	: tag(integer_tag)
	, data(integer)
	, lexeme_begin(lexeme_begin)
	, lexeme_end(lexeme_end)
	{}

Semantics::ConstantValue::ConstantValue(char char_, uint64_t lexeme_begin, uint64_t lexeme_end)
	: tag(char_tag)
	, data(char_)
	, lexeme_begin(lexeme_begin)
	, lexeme_end(lexeme_end)
	{}

Semantics::ConstantValue::ConstantValue(bool boolean, uint64_t lexeme_begin, uint64_t lexeme_end)
	: tag(integer_tag)
	, data(boolean)
	, lexeme_begin(lexeme_begin)
	, lexeme_end(lexeme_end)
	{}

Semantics::ConstantValue::ConstantValue(const std::string &string, uint64_t lexeme_begin, uint64_t lexeme_end)
	: tag(string_tag)
	, data(string)
	, lexeme_begin(lexeme_begin)
	, lexeme_end(lexeme_end)
	{}

Semantics::ConstantValue::ConstantValue(std::string &&string, uint64_t lexeme_begin, uint64_t lexeme_end)
	: tag(string_tag)
	, data(std::move(string))
	, lexeme_begin(lexeme_begin)
	, lexeme_end(lexeme_end)
	{}

bool Semantics::ConstantValue::is_static() const {
	switch(tag) {
		case dynamic_tag:
			return false;
		case integer_tag:
		case char_tag:
		case boolean_tag:
		case string_tag:
			return true;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::is_static: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::ConstantValue::is_dynamic() const {
	switch(tag) {
		case dynamic_tag:
			return true;
		case integer_tag:
		case char_tag:
		case boolean_tag:
		case string_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::is_dynamic: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::ConstantValue::is_integer() const {
	switch(tag) {
		case dynamic_tag:
			return false;
		case integer_tag:
			return true;
		case char_tag:
		case boolean_tag:
		case string_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::is_integer: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::ConstantValue::is_char() const {
	switch(tag) {
		case dynamic_tag:
		case integer_tag:
			return false;
		case char_tag:
			return true;
		case boolean_tag:
		case string_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::is_char: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::ConstantValue::is_boolean() const {
	switch(tag) {
		case dynamic_tag:
		case integer_tag:
		case char_tag:
			return false;
		case boolean_tag:
			return true;
		case string_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::is_boolean: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::ConstantValue::is_string() const {
	switch(tag) {
		case dynamic_tag:
		case integer_tag:
		case char_tag:
		case boolean_tag:
			return false;
		case string_tag:
			return true;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::is_string: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

int32_t Semantics::ConstantValue::get_integer() const {
	switch(tag) {
		case dynamic_tag:
		case integer_tag:
		case char_tag:
		case boolean_tag:
		case string_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::get_integer: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_integer()) {
		std::ostringstream sstr;
		sstr << "Semantics::ConstantValue::get_integer: constant value has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<int32_t>(data);
}

char Semantics::ConstantValue::get_char() const {
	switch(tag) {
		case dynamic_tag:
		case integer_tag:
		case char_tag:
		case boolean_tag:
		case string_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::get_char: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_char()) {
		std::ostringstream sstr;
		sstr << "Semantics::ConstantValue::get_char: constant value has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<char>(data);
}

bool Semantics::ConstantValue::get_boolean() const {
	switch(tag) {
		case dynamic_tag:
		case integer_tag:
		case char_tag:
		case boolean_tag:
		case string_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::get_boolean: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_boolean()) {
		std::ostringstream sstr;
		sstr << "Semantics::ConstantValue::get_boolean: constant value has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<bool>(data);
}

std::string Semantics::ConstantValue::get_string_copy() const {
	switch(tag) {
		case dynamic_tag:
		case integer_tag:
		case char_tag:
		case boolean_tag:
		case string_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::get_string_copy: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_string()) {
		std::ostringstream sstr;
		sstr << "Semantics::ConstantValue::get_string_copy: constant value has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::string(std::as_const(std::get<std::string>(data)));
}

const std::string &Semantics::ConstantValue::get_string() const {
	switch(tag) {
		case dynamic_tag:
		case integer_tag:
		case char_tag:
		case boolean_tag:
		case string_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::get_string: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_string()) {
		std::ostringstream sstr;
		sstr << "Semantics::ConstantValue::get_string: constant value has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<std::string>(data);
}

std::string &&Semantics::ConstantValue::get_string() {
	switch(tag) {
		case dynamic_tag:
		case integer_tag:
		case char_tag:
		case boolean_tag:
		case string_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::get_string: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_string()) {
		std::ostringstream sstr;
		sstr << "Semantics::ConstantValue::get_string: constant value has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::move(std::get<std::string>(data));
}

void Semantics::ConstantValue::set_integer(int32_t integer) {
	switch(tag) {
		case dynamic_tag:
		case integer_tag:
		case char_tag:
		case boolean_tag:
		case string_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::set_integer: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_integer()) {
		std::ostringstream sstr;
		sstr << "Semantics::ConstantValue::set_integer: constant value has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	data = integer;
}

void Semantics::ConstantValue::set_char(char char_) {
	switch(tag) {
		case dynamic_tag:
		case integer_tag:
		case char_tag:
		case boolean_tag:
		case string_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::set_char: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_char()) {
		std::ostringstream sstr;
		sstr << "Semantics::ConstantValue::set_char: constant value has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	data = char_;
}

void Semantics::ConstantValue::set_boolean(bool boolean) {
	switch(tag) {
		case dynamic_tag:
		case integer_tag:
		case char_tag:
		case boolean_tag:
		case string_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::set_boolean: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_boolean()) {
		std::ostringstream sstr;
		sstr << "Semantics::ConstantValue::set_boolean: constant value has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	data = boolean;
}

void Semantics::ConstantValue::set_string(const std::string &string) {
	switch(tag) {
		case dynamic_tag:
		case integer_tag:
		case char_tag:
		case boolean_tag:
		case string_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::set_string: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_string()) {
		std::ostringstream sstr;
		sstr << "Semantics::ConstantValue::set_string: constant value has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	data = string;
}

void Semantics::ConstantValue::set_string(std::string &&string) {
	switch(tag) {
		case dynamic_tag:
		case integer_tag:
		case char_tag:
		case boolean_tag:
		case string_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::set_string: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_string()) {
		std::ostringstream sstr;
		sstr << "Semantics::ConstantValue::set_string: constant value has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	data = std::string(string);
}

std::string Semantics::ConstantValue::get_tag_repr(tag_t tag) {
	switch(tag) {
		case dynamic_tag:
			return "dynamic";
		case integer_tag:
			return "integer";
		case char_tag:
			return "char";
		case boolean_tag:
			return "boolean";
		case string_tag:
			return "string";

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::get_tag_repr: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

std::string Semantics::ConstantValue::get_tag_repr() const {
	return get_tag_repr(tag);
}

Semantics::Type::Primitive Semantics::ConstantValue::get_static_primitive_type() const {
	switch(tag) {
		case dynamic_tag: {
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::get_static_primitive_type: internal error: cannot obtain the static primitive type of a dynamic value.";
			throw SemanticsError(sstr.str());
		} case integer_tag: {
			return Type::Primitive::integer_type;
		} case char_tag: {
			return Type::Primitive::char_type;
		} case boolean_tag: {
			return Type::Primitive::boolean_type;
		} case string_tag: {
			return Type::Primitive::string_type;
		}

		case null_tag:
		default: {
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::get_static_primitive_type: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
		}
	}
}

Semantics::Type Semantics::ConstantValue::get_static_type() const {
	switch(tag) {
		case dynamic_tag: {
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::get_static_type: internal error: cannot obtain the static type of a dynamic value.";
			throw SemanticsError(sstr.str());
		} case integer_tag: {
			return Type::integer_type;
		} case char_tag: {
			return Type::char_type;
		} case boolean_tag: {
			return Type::boolean_type;
		} case string_tag: {
			return Type::string_type;
		}

		case null_tag:
		default: {
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::get_static_type: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
		}
	}
}

// | Get a string representation of the static value.
std::string Semantics::ConstantValue::get_static_repr() const {
	switch(tag) {
		case dynamic_tag: {
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::get_static_repr: internal error: cannot obtain the static representation of a dynamic value.";
			throw SemanticsError(sstr.str());
		} case integer_tag: {
			std::ostringstream sstr;
			sstr << get_integer();
			return sstr.str();
		} case char_tag: {
			return quote_char(get_char());
		} case boolean_tag: {
			if (get_boolean()) {
				return "1";
			} else {
				return "0";
			}
		} case string_tag: {
			return quote_string(get_string());
		}

		case null_tag:
		default: {
			std::ostringstream sstr;
			sstr << "Semantics::ConstantValue::get_static_repr: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
		}
	}
}

const std::map<char, std::string> Semantics::ConstantValue::char_escapes {
	{'\n', "\\n"},
	{'\r', "\\r"},
	{'\b', "\\b"},
	{'\t', "\\t"},
	{'\f', "\\f"},
};

const std::map<std::string, char> Semantics::ConstantValue::reverse_char_escapes {
	{"\\n", '\n'},
	{"\\r", '\r'},
	{"\\b", '\b'},
	{"\\t", '\t'},
	{"\\f", '\f'},
};

std::string Semantics::ConstantValue::escape_char(char char_) {
	// Is this in char_escapes?
	std::map<char, std::string>::const_iterator char_escapes_search = char_escapes.find(char_);
	if (char_escapes_search != char_escapes.cend()) {
		// It's an escape.
		return std::string(char_escapes_search->second);
	} else if (isprint(char_)) {
		// It's not, but it's printable.
		return std::string(1, char_);
	} else {
		// Print the hex escape of the non-printable character.
		std::ostringstream sstr;
		sstr
			<< "\\x"
			<< std::right << std::setfill('0') << std::setw(2) << std::hex << static_cast<std::size_t>(static_cast<std::make_unsigned<std::string::value_type>::type>(char_))
			;
		return sstr.str();
	}
}

std::string Semantics::ConstantValue::quote_char(char char_) {
	return "\'" + std::move(escape_char(char_)) + "\'";
}

std::string Semantics::ConstantValue::quote_string(const std::string &string) {
	std::string quoted;
	quoted.push_back('"');
	for (const char &unescaped : std::as_const(string)) {
		for (const char &escaped : escape_char(unescaped)) {
			quoted.push_back(escaped);
		}
	}
	quoted.push_back('"');
	return std::move(quoted);
}

Semantics::IdentifierScope::IdentifierBinding::Static::Static()
	{}

Semantics::IdentifierScope::IdentifierBinding::Static::Static(const ConstantValue &constant_value)
	: constant_value(constant_value)
	{}

Semantics::IdentifierScope::IdentifierBinding::Static::Static(ConstantValue &&constant_value)
	: constant_value(std::move(constant_value))
	{}

Semantics::IdentifierScope::IdentifierBinding::Var::Var()
	{}

Semantics::IdentifierScope::IdentifierBinding::Var::Var(bool ref, const Type &type, bool global, Symbol symbol, bool register_, uint8_t arg_register_id, uint32_t offset)
	: ref(ref)
	, type(type)
	, global(global)
	, symbol(symbol)
	, register_(register_)
	, arg_register_id(arg_register_id)
	, offset(offset)
	{}

Semantics::IdentifierScope::IdentifierBinding::Ref::Ref()
	{}

Semantics::IdentifierScope::IdentifierBinding::Ref::Ref(const Type &type, bool register_, uint8_t arg_register_id, uint32_t offset)
	: type(type)
	, register_(register_)
	, arg_register_id(arg_register_id)
	, offset(offset)
	{}

Semantics::IdentifierScope::IdentifierBinding::IdentifierBinding()
	{}

Semantics::IdentifierScope::IdentifierBinding::IdentifierBinding(tag_t tag, const data_t &data)
	: tag(tag)
	, data(data)
	{}

Semantics::IdentifierScope::IdentifierBinding::IdentifierBinding(tag_t tag, data_t &&data)
	: tag(tag)
	, data(std::move(data))
	{}

Semantics::IdentifierScope::IdentifierBinding::IdentifierBinding(const Static &static_)
	: IdentifierBinding(static_tag, static_)
	{}

Semantics::IdentifierScope::IdentifierBinding::IdentifierBinding(const Dynamic &dynamic)
	: IdentifierBinding(dynamic_tag, dynamic)
	{}

Semantics::IdentifierScope::IdentifierBinding::IdentifierBinding(const Type &type)
	: IdentifierBinding(type_tag, type)
	{}

Semantics::IdentifierScope::IdentifierBinding::IdentifierBinding(const Var &var)
	: IdentifierBinding(var_tag, var)
	{}

Semantics::IdentifierScope::IdentifierBinding::IdentifierBinding(const Ref &ref)
	: IdentifierBinding(ref_tag, ref)
	{}

Semantics::IdentifierScope::IdentifierBinding::IdentifierBinding(Static &&static_)
	: IdentifierBinding(static_tag, std::move(static_))
	{}

Semantics::IdentifierScope::IdentifierBinding::IdentifierBinding(Dynamic &&dynamic)
	: IdentifierBinding(dynamic_tag, std::move(dynamic))
	{}

Semantics::IdentifierScope::IdentifierBinding::IdentifierBinding(Type &&type)
	: IdentifierBinding(type_tag, std::move(type))
	{}

Semantics::IdentifierScope::IdentifierBinding::IdentifierBinding(Var &&var)
	: IdentifierBinding(var_tag, std::move(var))
	{}

Semantics::IdentifierScope::IdentifierBinding::IdentifierBinding(Ref &&ref)
	: IdentifierBinding(ref_tag, std::move(ref))
	{}

bool Semantics::IdentifierScope::IdentifierBinding::is_static() const {
	switch(tag) {
		case static_tag:
			return true;
		case dynamic_tag:
		case type_tag:
		case var_tag:
		case ref_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::is_static: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::IdentifierScope::IdentifierBinding::is_dynamic() const {
	switch(tag) {
		case static_tag:
			return false;
		case dynamic_tag:
			return true;
		case type_tag:
		case var_tag:
		case ref_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::is_dynamic: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::IdentifierScope::IdentifierBinding::is_type() const {
	switch(tag) {
		case static_tag:
		case dynamic_tag:
			return false;
		case type_tag:
			return true;
		case var_tag:
		case ref_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::is_type: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::IdentifierScope::IdentifierBinding::is_var() const {
	switch(tag) {
		case static_tag:
		case dynamic_tag:
		case type_tag:
			return false;
		case var_tag:
			return true;
		case ref_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::is_var: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::IdentifierScope::IdentifierBinding::is_ref() const {
	switch(tag) {
		case static_tag:
		case dynamic_tag:
		case type_tag:
		case var_tag:
			return false;
		case ref_tag:
			return true;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::is_ref: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

// | The tags must be correct, or else an exception will be thrown, including for set_*.
const Semantics::IdentifierScope::IdentifierBinding::Static &Semantics::IdentifierScope::IdentifierBinding::get_static() const {
	switch(tag) {
		case static_tag:
		case dynamic_tag:
		case type_tag:
		case var_tag:
		case ref_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::get_static: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_static()) {
		std::ostringstream sstr;
		sstr << "Semantics::IdentifierScope::IdentifierBinding::get_static: binding has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<Static>(data);
}

const Semantics::IdentifierScope::IdentifierBinding::Dynamic &Semantics::IdentifierScope::IdentifierBinding::get_dynamic() const {
	switch(tag) {
		case static_tag:
		case dynamic_tag:
		case type_tag:
		case var_tag:
		case ref_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::get_dynamic: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_dynamic()) {
		std::ostringstream sstr;
		sstr << "Semantics::IdentifierScope::IdentifierBinding::get_dynamic: binding has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<Dynamic>(data);
}

const Semantics::IdentifierScope::IdentifierBinding::Type &Semantics::IdentifierScope::IdentifierBinding::get_type() const {
	switch(tag) {
		case static_tag:
		case dynamic_tag:
		case type_tag:
		case var_tag:
		case ref_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::get_type: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_type()) {
		std::ostringstream sstr;
		sstr << "Semantics::IdentifierScope::IdentifierBinding::get_type: binding has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<Type>(data);
}

const Semantics::IdentifierScope::IdentifierBinding::Var &Semantics::IdentifierScope::IdentifierBinding::get_var() const {
	switch(tag) {
		case static_tag:
		case dynamic_tag:
		case type_tag:
		case var_tag:
		case ref_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::get_var: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_var()) {
		std::ostringstream sstr;
		sstr << "Semantics::IdentifierScope::IdentifierBinding::get_var: binding has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<Var>(data);
}

const Semantics::IdentifierScope::IdentifierBinding::Ref &Semantics::IdentifierScope::IdentifierBinding::get_ref() const {
	switch(tag) {
		case static_tag:
		case dynamic_tag:
		case type_tag:
		case var_tag:
		case ref_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::get_ref: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_ref()) {
		std::ostringstream sstr;
		sstr << "Semantics::IdentifierScope::IdentifierBinding::get_ref: binding has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<Ref>(data);
}

Semantics::IdentifierScope::IdentifierBinding::Static &Semantics::IdentifierScope::IdentifierBinding::get_static() {
	switch(tag) {
		case static_tag:
		case dynamic_tag:
		case type_tag:
		case var_tag:
		case ref_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::get_static: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_static()) {
		std::ostringstream sstr;
		sstr << "Semantics::IdentifierScope::IdentifierBinding::get_static: binding has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<Static>(data);
}

Semantics::IdentifierScope::IdentifierBinding::Dynamic &Semantics::IdentifierScope::IdentifierBinding::get_dynamic() {
	switch(tag) {
		case static_tag:
		case dynamic_tag:
		case type_tag:
		case var_tag:
		case ref_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::get_dynamic: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_dynamic()) {
		std::ostringstream sstr;
		sstr << "Semantics::IdentifierScope::IdentifierBinding::get_dynamic: binding has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<Dynamic>(data);
}

Semantics::IdentifierScope::IdentifierBinding::Type &Semantics::IdentifierScope::IdentifierBinding::get_type() {
	switch(tag) {
		case static_tag:
		case dynamic_tag:
		case type_tag:
		case var_tag:
		case ref_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::get_type: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_type()) {
		std::ostringstream sstr;
		sstr << "Semantics::IdentifierScope::IdentifierBinding::get_type: binding has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<Type>(data);
}

Semantics::IdentifierScope::IdentifierBinding::Var &Semantics::IdentifierScope::IdentifierBinding::get_var() {
	switch(tag) {
		case static_tag:
		case dynamic_tag:
		case type_tag:
		case var_tag:
		case ref_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::get_var: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_var()) {
		std::ostringstream sstr;
		sstr << "Semantics::IdentifierScope::IdentifierBinding::get_var: binding has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<Var>(data);
}

Semantics::IdentifierScope::IdentifierBinding::Ref &Semantics::IdentifierScope::IdentifierBinding::get_ref() {
	switch(tag) {
		case static_tag:
		case dynamic_tag:
		case type_tag:
		case var_tag:
		case ref_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::get_ref: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	if (!is_ref()) {
		std::ostringstream sstr;
		sstr << "Semantics::IdentifierScope::IdentifierBinding::get_ref: binding has a different type tag: " << tag;
		throw SemanticsError(sstr.str());
	}

	return std::get<Ref>(data);
}

// | Return "static", "dynamic", "type", "var", or "ref".
std::string Semantics::IdentifierScope::IdentifierBinding::get_tag_repr(tag_t tag) {
	switch(tag) {
		case static_tag:
			return "static";
		case dynamic_tag:
			return "dynamic";
		case type_tag:
			return "type";
		case var_tag:
			return "var";
		case ref_tag:
			return "ref";

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::IdentifierScope::IdentifierBinding::get_tag_repr: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

std::string Semantics::IdentifierScope::IdentifierBinding::get_tag_repr() const {
	return get_tag_repr(tag);
}

Semantics::IdentifierScope::IdentifierScope()
	{}

Semantics::IdentifierScope::IdentifierScope(const std::map<std::string, IdentifierBinding> &scope)
	: scope(scope)
	{}

Semantics::IdentifierScope::IdentifierScope(std::map<std::string, IdentifierBinding> &&scope)
	: scope(std::move(scope))
	{}

bool Semantics::IdentifierScope::has(std::string identifier) const {
	std::map<std::string, IdentifierBinding>::const_iterator identifier_binding_search = scope.find(identifier);
	if (identifier_binding_search == scope.cend()) {
		return false;
	} else {
		return true;
	}
}

const Semantics::IdentifierScope::IdentifierBinding &Semantics::IdentifierScope::get(std::string identifier) const {
	std::map<std::string, IdentifierBinding>::const_iterator identifier_binding_search = scope.find(identifier);
	if (identifier_binding_search == scope.cend()) {
		std::ostringstream sstr;
		sstr << "Semantics::IdentifierScope::get: the identifier is missing from scope: " << identifier;
		throw SemanticsError(sstr.str());
	} else {
		return identifier_binding_search->second;
	}
}

Semantics::IdentifierScope::IdentifierBinding &&Semantics::IdentifierScope::get(std::string identifier) {
	std::map<std::string, IdentifierBinding>::iterator identifier_binding_search = scope.find(identifier);
	if (identifier_binding_search == scope.end()) {
		std::ostringstream sstr;
		sstr << "Semantics::IdentifierScope::get: the identifier is missing from scope: " << identifier;
		throw SemanticsError(sstr.str());
	} else {
		return std::move(identifier_binding_search->second);
	}
}

const Semantics::IdentifierScope::IdentifierBinding &Semantics::IdentifierScope::operator[](std::string identifier) const {
	return get(identifier);
}

Semantics::IdentifierScope::IdentifierBinding &&Semantics::IdentifierScope::operator[](std::string identifier) {
	return std::move(get(identifier));
}

std::optional<Semantics::IdentifierScope::IdentifierBinding> Semantics::IdentifierScope::lookup_copy(std::string identifier) const {
	std::map<std::string, IdentifierBinding>::const_iterator identifier_binding_search = scope.find(identifier);
	if (identifier_binding_search == scope.cend()) {
		return std::optional<IdentifierBinding>();
	} else {
		return std::optional<IdentifierBinding>(std::move(IdentifierBinding(std::as_const(identifier_binding_search->second))));
	}
}

Semantics::Semantics()
	: auto_analyze(true)
{
	if (auto_analyze) {
		analyze();
	}
}

Semantics::Semantics(bool auto_analyze)
	: auto_analyze(auto_analyze)
{
	if (auto_analyze) {
		analyze();
	}
}

Semantics::Semantics(const Grammar &grammar, bool auto_analyze)
	: grammar(grammar)
	, auto_analyze(auto_analyze)
{
	if (auto_analyze) {
		analyze();
	}
}

Semantics::Semantics(Grammar &&grammar, bool auto_analyze)
	: grammar(std::move(grammar))
	, auto_analyze(auto_analyze)
{
	if (auto_analyze) {
		analyze();
	}
}

std::vector<std::string> Semantics::get_normalized_output_lines_copy() const {
	return std::move(output.get_normalized_lines_copy());
}

const Grammar Semantics::get_grammar() const {
	return grammar;
}

void Semantics::set_grammar(const Grammar &grammar) {
	this->grammar = grammar;

	if (auto_analyze) {
		analyze();
	}
}

void Semantics::set_grammar(Grammar &&grammar) {
	grammar = std::move(grammar);

	if (auto_analyze) {
		analyze();
	}
}

// | Determine whether the expression in the grammar tree is a constant expression.
//
// TODO: this will work if the grammar parse tree is valid, but check for cycles in case there's a mistake somewhere.
Semantics::ConstantValue Semantics::is_expression_constant(
	// | Reference to the expression in the grammar tree.
	uint64_t expression,
	// | A collection of identifiers of constants available to the scope of the expression.
	const IdentifierScope &expression_constant_scope
) const {
	// TODO: assert() or assert(this->verify()) and configure macros to enable
	// assertions only when debugging is enabled (DEBUG=1 is defined).
	if (expression > grammar.expression_storage.size()) {
		std::ostringstream sstr;
		sstr << "Semantics::is_expression_constant: out of bounds expression reference: " << expression << " >= " << grammar.expression_storage.size() << ".";
		throw SemanticsError(sstr.str());
	}

	// Lookup the expression from the grammar tree.
	const ::Expression &expression_symbol = grammar.expression_storage[expression];

	// Prepare the calculated constant value.
	ConstantValue expression_constant_value;

	// Branch according to the expression type.
	switch (expression_symbol.branch) {
		// These 16 branches are static iff all subexpressions are static.
		case ::Expression::pipe_branch: {
			const ::Expression::Pipe &pipe           = grammar.expression_pipe_storage.at(expression_symbol.data);
			const ::Expression       &expression0    = grammar.expression_storage.at(pipe.expression0); (void) expression0;
			const LexemeOperator     &pipe_operator0 = grammar.lexemes.at(pipe.pipe_operator0).get_operator();
			const ::Expression       &expression1    = grammar.expression_storage.at(pipe.expression1); (void) expression1;

			// Is either subexpression dynamic?  If so, this expression is also dynamic.
			// (Normally we'd operate on the left side first, but since order
			// of evaluation is referentially transparent and the parser tree
			// is left-recursive, check the expression on the right first,
			// which is more efficient.)
			ConstantValue right = is_expression_constant(pipe.expression1, expression_constant_scope);
			if (right.is_dynamic()) {
				expression_constant_value = right;
				break;
			}
			ConstantValue left  = is_expression_constant(pipe.expression0, expression_constant_scope);
			if (left.is_dynamic()) {
				expression_constant_value = left;
				break;
			}

			// Are the expressions of the same type?
			if (left.tag != right.tag) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< pipe_operator0.line << " col " << pipe_operator0.column
					<< "): refusing to OR values of different types, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Are we attempting to operate on a string?
			if (left.is_string() || right.is_string()) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< pipe_operator0.line << " col " << pipe_operator0.column
					<< "): cannot apply bitwise OR on a string expression, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Apply bitwise OR depending on the integer type.
			if        (left.is_integer()) {
				expression_constant_value = ConstantValue(static_cast<int32_t>(static_cast<int32_t>(left.get_integer()) | static_cast<int32_t>(right.get_integer())), left.lexeme_begin, right.lexeme_end);
				break;
			} else if (left.is_char()) {
				expression_constant_value = ConstantValue(static_cast<char>(static_cast<char>(left.get_char()) | static_cast<char>(right.get_char())), left.lexeme_begin, right.lexeme_end);
				break;
			} else if (left.is_boolean()) {
				expression_constant_value = ConstantValue(static_cast<bool>(static_cast<bool>(left.get_boolean()) | static_cast<bool>(right.get_boolean())), left.lexeme_begin, right.lexeme_end);
				break;
			} else {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: internal error (line "
					<< pipe_operator0.line << " col " << pipe_operator0.column
					<< "): unhandled constant expression type for bitwise OR: "
					<< left.get_tag_repr()
					;
				throw SemanticsError(sstr.str());
			}
		} case ::Expression::ampersand_branch: {
			const ::Expression::Ampersand &ampersand           = grammar.expression_ampersand_storage.at(expression_symbol.data);
			const ::Expression            &expression0         = grammar.expression_storage.at(ampersand.expression0); (void) expression0;
			const LexemeOperator          &ampersand_operator0 = grammar.lexemes.at(ampersand.ampersand_operator0).get_operator();
			const ::Expression            &expression1         = grammar.expression_storage.at(ampersand.expression1); (void) expression1;

			// Is either subexpression dynamic?  If so, this expression is also dynamic.
			ConstantValue right = is_expression_constant(ampersand.expression1, expression_constant_scope);
			if (right.is_dynamic()) {
				expression_constant_value = right;
				break;
			}
			ConstantValue left  = is_expression_constant(ampersand.expression0, expression_constant_scope);
			if (left.is_dynamic()) {
				expression_constant_value = left;
				break;
			}

			// Are the expressions of the same type?
			if (left.tag != right.tag) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< ampersand_operator0.line << " col " << ampersand_operator0.column
					<< "): refusing to AND values of different types, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Are we attempting to operate on a string?
			if (left.is_string() || right.is_string()) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< ampersand_operator0.line << " col " << ampersand_operator0.column
					<< "): cannot apply bitwise AND on a string expression, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Apply bitwise AND depending on the integer type.
			if        (left.is_integer()) {
				expression_constant_value = ConstantValue(static_cast<int32_t>(static_cast<int32_t>(left.get_integer()) & static_cast<int32_t>(right.get_integer())), left.lexeme_begin, right.lexeme_end);
				break;
			} else if (left.is_char()) {
				expression_constant_value = ConstantValue(static_cast<char>(static_cast<char>(left.get_char()) & static_cast<char>(right.get_char())), left.lexeme_begin, right.lexeme_end);
				break;
			} else if (left.is_boolean()) {
				expression_constant_value = ConstantValue(static_cast<bool>(static_cast<bool>(left.get_boolean()) & static_cast<bool>(right.get_boolean())), left.lexeme_begin, right.lexeme_end);
				break;
			} else {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: internal error (line "
					<< ampersand_operator0.line << " col " << ampersand_operator0.column
					<< "): unhandled constant expression type for bitwise AND: "
					<< left.get_tag_repr()
					;
				throw SemanticsError(sstr.str());
			}
		} case ::Expression::equals_branch: {
			const ::Expression::Equals &equals           = grammar.expression_equals_storage.at(expression_symbol.data);
			const ::Expression         &expression0      = grammar.expression_storage.at(equals.expression0); (void) expression0;
			const LexemeOperator       &equals_operator0 = grammar.lexemes.at(equals.equals_operator0).get_operator();
			const ::Expression         &expression1      = grammar.expression_storage.at(equals.expression1); (void) expression1;

			// Is either subexpression dynamic?  If so, this expression is also dynamic.
			ConstantValue right = is_expression_constant(equals.expression1, expression_constant_scope);
			if (right.is_dynamic()) {
				expression_constant_value = right;
				break;
			}
			ConstantValue left  = is_expression_constant(equals.expression0, expression_constant_scope);
			if (left.is_dynamic()) {
				expression_constant_value = left;
				break;
			}

			// Are the expressions of the same type?
			if (left.tag != right.tag) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< equals_operator0.line << " col " << equals_operator0.column
					<< "): refusing to compare values of different types for =, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Apply = comparison depending on the type.
			if        (left.is_integer()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_integer() == right.get_integer()), left.lexeme_begin, right.lexeme_end);
				break;
			} else if (left.is_char()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_char() == right.get_char()), left.lexeme_begin, right.lexeme_end);
				break;
			} else if (left.is_boolean()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_boolean() == right.get_boolean()), left.lexeme_begin, right.lexeme_end);
				break;
			} else if (left.is_string()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_string() == right.get_string()), left.lexeme_begin, right.lexeme_end);
				break;
			} else {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: internal error (line "
					<< equals_operator0.line << " col " << equals_operator0.column
					<< "): unhandled constant expression type for = comparison: "
					<< left.get_tag_repr()
					;
				throw SemanticsError(sstr.str());
			}
		} case ::Expression::lt_or_gt_branch: {
			const ::Expression::LtOrGt &lt_or_gt           = grammar.expression_lt_or_gt_storage.at(expression_symbol.data);
			const ::Expression         &expression0        = grammar.expression_storage.at(lt_or_gt.expression0); (void) expression0;
			const LexemeOperator       &lt_or_gt_operator0 = grammar.lexemes.at(lt_or_gt.lt_or_gt_operator0).get_operator();
			const ::Expression         &expression1        = grammar.expression_storage.at(lt_or_gt.expression1); (void) expression1;

			// Is either subexpression dynamic?  If so, this expression is also dynamic.
			ConstantValue right = is_expression_constant(lt_or_gt.expression1, expression_constant_scope);
			if (right.is_dynamic()) {
				expression_constant_value = right;
				break;
			}
			ConstantValue left  = is_expression_constant(lt_or_gt.expression0, expression_constant_scope);
			if (left.is_dynamic()) {
				expression_constant_value = left;
				break;
			}

			// Are the expressions of the same type?
			if (left.tag != right.tag) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< lt_or_gt_operator0.line << " col " << lt_or_gt_operator0.column
					<< "): refusing to compare values of different types for <>, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Apply <> comparison depending on the type.
			if        (left.is_integer()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_integer() != right.get_integer()), left.lexeme_begin, right.lexeme_end);
				break;
			} else if (left.is_char()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_char() != right.get_char()), left.lexeme_begin, right.lexeme_end);
				break;
			} else if (left.is_boolean()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_boolean() != right.get_boolean()), left.lexeme_begin, right.lexeme_end);
				break;
			} else if (left.is_string()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_string() != right.get_string()), left.lexeme_begin, right.lexeme_end);
				break;
			} else {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: internal error (line "
					<< lt_or_gt_operator0.line << " col " << lt_or_gt_operator0.column
					<< "): unhandled constant expression type for <> comparison: "
					<< left.get_tag_repr()
					;
				throw SemanticsError(sstr.str());
			}
		} case ::Expression::le_branch: {
			const ::Expression::Le &le           = grammar.expression_le_storage.at(expression_symbol.data);
			const ::Expression     &expression0  = grammar.expression_storage.at(le.expression0); (void) expression0;
			const LexemeOperator   &le_operator0 = grammar.lexemes.at(le.le_operator0).get_operator();
			const ::Expression     &expression1  = grammar.expression_storage.at(le.expression1); (void) expression1;

			// Is either subexpression dynamic?  If so, this expression is also dynamic.
			ConstantValue right = is_expression_constant(le.expression1, expression_constant_scope);
			if (right.is_dynamic()) {
				expression_constant_value = right;
				break;
			}
			ConstantValue left  = is_expression_constant(le.expression0, expression_constant_scope);
			if (left.is_dynamic()) {
				expression_constant_value = left;
				break;
			}

			// Are the expressions of the same type?
			if (left.tag != right.tag) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< le_operator0.line << " col " << le_operator0.column
					<< "): refusing to compare values of different types for <=, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Apply <= comparison depending on the type.
			if        (left.is_integer()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_integer() <= right.get_integer()), left.lexeme_begin, right.lexeme_end);
				break;
			} else if (left.is_char()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_char() <= right.get_char()), left.lexeme_begin, right.lexeme_end);
				break;
			} else if (left.is_boolean()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_boolean() <= right.get_boolean()), left.lexeme_begin, right.lexeme_end);
				break;
			} else if (left.is_string()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_string() <= right.get_string()), left.lexeme_begin, right.lexeme_end);
				break;
			} else {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: internal error (line "
					<< le_operator0.line << " col " << le_operator0.column
					<< "): unhandled constant expression type for <= comparison: "
					<< left.get_tag_repr()
					;
				throw SemanticsError(sstr.str());
			}
		} case ::Expression::ge_branch: {
			const ::Expression::Ge &ge           = grammar.expression_ge_storage.at(expression_symbol.data);
			const ::Expression     &expression0  = grammar.expression_storage.at(ge.expression0); (void) expression0;
			const LexemeOperator   &ge_operator0 = grammar.lexemes.at(ge.ge_operator0).get_operator();
			const ::Expression     &expression1  = grammar.expression_storage.at(ge.expression1); (void) expression1;

			// Is either subexpression dynamic?  If so, this expression is also dynamic.
			ConstantValue right = is_expression_constant(ge.expression1, expression_constant_scope);
			if (right.is_dynamic()) {
				expression_constant_value = right;
				break;
			}
			ConstantValue left  = is_expression_constant(ge.expression0, expression_constant_scope);
			if (left.is_dynamic()) {
				expression_constant_value = left;
				break;
			}

			// Are the expressions of the same type?
			if (left.tag != right.tag) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< ge_operator0.line << " col " << ge_operator0.column
					<< "): refusing to compare values of different types for >=, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Apply >= comparison depending on the type.
			if        (left.is_integer()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_integer() >= right.get_integer()), left.lexeme_begin, right.lexeme_end);
				break;
			} else if (left.is_char()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_char() >= right.get_char()), left.lexeme_begin, right.lexeme_end);
				break;
			} else if (left.is_boolean()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_boolean() >= right.get_boolean()), left.lexeme_begin, right.lexeme_end);
				break;
			} else if (left.is_string()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_string() >= right.get_string()), left.lexeme_begin, right.lexeme_end);
				break;
			} else {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: internal error (line "
					<< ge_operator0.line << " col " << ge_operator0.column
					<< "): unhandled constant expression type for >= comparison: "
					<< left.get_tag_repr()
					;
				throw SemanticsError(sstr.str());
			}
		} case ::Expression::lt_branch: {
			const ::Expression::Lt &lt           = grammar.expression_lt_storage.at(expression_symbol.data);
			const ::Expression     &expression0  = grammar.expression_storage.at(lt.expression0); (void) expression0;
			const LexemeOperator   &lt_operator0 = grammar.lexemes.at(lt.lt_operator0).get_operator();
			const ::Expression     &expression1  = grammar.expression_storage.at(lt.expression1); (void) expression1;

			// Is either subexpression dynamic?  If so, this expression is also dynamic.
			ConstantValue right = is_expression_constant(lt.expression1, expression_constant_scope);
			if (right.is_dynamic()) {
				expression_constant_value = right;
				break;
			}
			ConstantValue left  = is_expression_constant(lt.expression0, expression_constant_scope);
			if (left.is_dynamic()) {
				expression_constant_value = left;
				break;
			}

			// Are the expressions of the same type?
			if (left.tag != right.tag) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< lt_operator0.line << " col " << lt_operator0.column
					<< "): refusing to compare values of different types for <, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Apply < comparison depending on the type.
			if        (left.is_integer()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_integer() < right.get_integer()), left.lexeme_begin, right.lexeme_end);
				break;
			} else if (left.is_char()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_char() < right.get_char()), left.lexeme_begin, right.lexeme_end);
				break;
			} else if (left.is_boolean()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_boolean() < right.get_boolean()), left.lexeme_begin, right.lexeme_end);
				break;
			} else if (left.is_string()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_string() < right.get_string()), left.lexeme_begin, right.lexeme_end);
				break;
			} else {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: internal error (line "
					<< lt_operator0.line << " col " << lt_operator0.column
					<< "): unhandled constant expression type for < comparison: "
					<< left.get_tag_repr()
					;
				throw SemanticsError(sstr.str());
			}
		} case ::Expression::gt_branch: {
			const ::Expression::Gt &gt           = grammar.expression_gt_storage.at(expression_symbol.data);
			const ::Expression     &expression0  = grammar.expression_storage.at(gt.expression0); (void) expression0;
			const LexemeOperator   &gt_operator0 = grammar.lexemes.at(gt.gt_operator0).get_operator();
			const ::Expression     &expression1  = grammar.expression_storage.at(gt.expression1); (void) expression1;

			// Is either subexpression dynamic?  If so, this expression is also dynamic.
			ConstantValue right = is_expression_constant(gt.expression1, expression_constant_scope);
			if (right.is_dynamic()) {
				expression_constant_value = right;
				break;
			}
			ConstantValue left  = is_expression_constant(gt.expression0, expression_constant_scope);
			if (left.is_dynamic()) {
				expression_constant_value = left;
				break;
			}

			// Are the expressions of the same type?
			if (left.tag != right.tag) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< gt_operator0.line << " col " << gt_operator0.column
					<< "): refusing to compare values of different types for >, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Apply > comparison depending on the type.
			if        (left.is_integer()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_integer() > right.get_integer()), left.lexeme_begin, right.lexeme_end);
				break;
			} else if (left.is_char()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_char() > right.get_char()), left.lexeme_begin, right.lexeme_end);
				break;
			} else if (left.is_boolean()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_boolean() > right.get_boolean()), left.lexeme_begin, right.lexeme_end);
				break;
			} else if (left.is_string()) {
				expression_constant_value = ConstantValue(static_cast<bool>(left.get_string() > right.get_string()), left.lexeme_begin, right.lexeme_end);
				break;
			} else {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: internal error (line "
					<< gt_operator0.line << " col " << gt_operator0.column
					<< "): unhandled constant expression type for > comparison: "
					<< left.get_tag_repr()
					;
				throw SemanticsError(sstr.str());
			}
		} case ::Expression::plus_branch: {
			const ::Expression::Plus &plus           = grammar.expression_plus_storage.at(expression_symbol.data);
			const ::Expression       &expression0    = grammar.expression_storage.at(plus.expression0); (void) expression0;
			const LexemeOperator     &plus_operator0 = grammar.lexemes.at(plus.plus_operator0).get_operator();
			const ::Expression       &expression1    = grammar.expression_storage.at(plus.expression1); (void) expression1;

			// Is either subexpression dynamic?  If so, this expression is also dynamic.
			ConstantValue right = is_expression_constant(plus.expression1, expression_constant_scope);
			if (right.is_dynamic()) {
				expression_constant_value = right;
				break;
			}
			ConstantValue left  = is_expression_constant(plus.expression0, expression_constant_scope);
			if (left.is_dynamic()) {
				expression_constant_value = left;
				break;
			}

			// Are the expressions of the same type?
			if (left.tag != right.tag) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< plus_operator0.line << " col " << plus_operator0.column
					<< "): refusing to add different types, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Are we attempting to operate on a string?
			if (left.is_string() || right.is_string()) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< plus_operator0.line << " col " << plus_operator0.column
					<< "): cannot apply addition on a string expression, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Are we attempting to operate on a non-integer?
			if (left.is_char() || left.is_boolean() || right.is_char() || right.is_boolean()) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< plus_operator0.line << " col " << plus_operator0.column
					<< "): refusing to apply addition on a non-integer, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Apply addition depending on the integer type.
			if (left.is_integer()) {
				// Detect overflow in constant expression.
				if (would_addition_overflow(left.get_integer(), right.get_integer())) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::is_expression_constant: error (line "
						<< plus_operator0.line << " col " << plus_operator0.column
						<< "): addition would result in an overflow, for "
						<< left.get_integer() << " + " << right.get_integer()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				expression_constant_value = ConstantValue(static_cast<int32_t>(static_cast<int32_t>(left.get_integer()) + static_cast<int32_t>(right.get_integer())), left.lexeme_begin, right.lexeme_end);
				break;
			} else {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: internal error (line "
					<< plus_operator0.line << " col " << plus_operator0.column
					<< "): unhandled constant expression type for addition: "
					<< left.get_tag_repr()
					;
				throw SemanticsError(sstr.str());
			}
		} case ::Expression::minus_branch: {
			const ::Expression::Minus &minus           = grammar.expression_minus_storage.at(expression_symbol.data);
			const ::Expression        &expression0     = grammar.expression_storage.at(minus.expression0); (void) expression0;
			const LexemeOperator      &minus_operator0 = grammar.lexemes.at(minus.minus_operator0).get_operator();
			const ::Expression        &expression1     = grammar.expression_storage.at(minus.expression1); (void) expression1;

			// Is either subexpression dynamic?  If so, this expression is also dynamic.
			ConstantValue right = is_expression_constant(minus.expression1, expression_constant_scope);
			if (right.is_dynamic()) {
				expression_constant_value = right;
				break;
			}
			ConstantValue left  = is_expression_constant(minus.expression0, expression_constant_scope);
			if (left.is_dynamic()) {
				expression_constant_value = left;
				break;
			}

			// Are the expressions of the same type?
			if (left.tag != right.tag) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< minus_operator0.line << " col " << minus_operator0.column
					<< "): refusing to substract values of different types, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Are we attempting to operate on a string?
			if (left.is_string() || right.is_string()) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< minus_operator0.line << " col " << minus_operator0.column
					<< "): cannot apply subtraction on a string expression, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Are we attempting to operate on a non-integer?
			if (left.is_char() || left.is_boolean() || right.is_char() || right.is_boolean()) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< minus_operator0.line << " col " << minus_operator0.column
					<< "): refusing to apply subtraction on a non-integer, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Apply subtraction depending on the integer type.
			if (left.is_integer()) {
				// Detect overflow in constant expression.
				if (would_addition_overflow(left.get_integer(), -right.get_integer())) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::is_expression_constant: error (line "
						<< minus_operator0.line << " col " << minus_operator0.column
						<< "): subtraction would result in an overflow, for "
						<< left.get_integer() << " - " << right.get_integer()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				expression_constant_value = ConstantValue(static_cast<int32_t>(static_cast<int32_t>(left.get_integer()) - static_cast<int32_t>(right.get_integer())), left.lexeme_begin, right.lexeme_end);
				break;
			} else {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: internal error (line "
					<< minus_operator0.line << " col " << minus_operator0.column
					<< "): unhandled constant expression type for subtraction: "
					<< left.get_tag_repr()
					;
				throw SemanticsError(sstr.str());
			}
		} case ::Expression::times_branch: {
			const ::Expression::Times &times           = grammar.expression_times_storage.at(expression_symbol.data);
			const ::Expression        &expression0     = grammar.expression_storage.at(times.expression0); (void) expression0;
			const LexemeOperator      &times_operator0 = grammar.lexemes.at(times.times_operator0).get_operator();
			const ::Expression        &expression1     = grammar.expression_storage.at(times.expression1); (void) expression1;

			// Is either subexpression dynamic?  If so, this expression is also dynamic.
			ConstantValue right = is_expression_constant(times.expression1, expression_constant_scope);
			if (right.is_dynamic()) {
				expression_constant_value = right;
				break;
			}
			ConstantValue left  = is_expression_constant(times.expression0, expression_constant_scope);
			if (left.is_dynamic()) {
				expression_constant_value = left;
				break;
			}

			// Are the expressions of the same type?
			if (left.tag != right.tag) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< times_operator0.line << " col " << times_operator0.column
					<< "): refusing to multiply values of different types, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Are we attempting to operate on a string?
			if (left.is_string() || right.is_string()) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< times_operator0.line << " col " << times_operator0.column
					<< "): cannot apply multiplication on a string expression, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Are we attempting to operate on a non-integer?
			if (left.is_char() || left.is_boolean() || right.is_char() || right.is_boolean()) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< times_operator0.line << " col " << times_operator0.column
					<< "): refusing to apply multiplication on a non-integer, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Apply multiplication depending on the integer type.
			if (left.is_integer()) {
				// Detect overflow in constant expression.
				if (would_multiplication_overflow(left.get_integer(), right.get_integer())) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::is_expression_constant: error (line "
						<< times_operator0.line << " col " << times_operator0.column
						<< "): multiplication would result in an overflow, for "
						<< left.get_integer() << " * " << right.get_integer()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				expression_constant_value = ConstantValue(static_cast<int32_t>(static_cast<int32_t>(left.get_integer()) * static_cast<int32_t>(right.get_integer())), left.lexeme_begin, right.lexeme_end);
				break;
			} else {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: internal error (line "
					<< times_operator0.line << " col " << times_operator0.column
					<< "): unhandled constant expression type for multiplication: "
					<< left.get_tag_repr()
					;
				throw SemanticsError(sstr.str());
			}
		} case ::Expression::slash_branch: {
			const ::Expression::Slash &slash           = grammar.expression_slash_storage.at(expression_symbol.data);
			const ::Expression        &expression0     = grammar.expression_storage.at(slash.expression0); (void) expression0;
			const LexemeOperator      &slash_operator0 = grammar.lexemes.at(slash.slash_operator0).get_operator();
			const ::Expression        &expression1     = grammar.expression_storage.at(slash.expression1); (void) expression1;

			// Is either subexpression dynamic?  If so, this expression is also dynamic.
			ConstantValue right = is_expression_constant(slash.expression1, expression_constant_scope);
			if (right.is_dynamic()) {
				expression_constant_value = right;
				break;
			}
			ConstantValue left  = is_expression_constant(slash.expression0, expression_constant_scope);
			if (left.is_dynamic()) {
				expression_constant_value = left;
				break;
			}

			// Are the expressions of the same type?
			if (left.tag != right.tag) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< slash_operator0.line << " col " << slash_operator0.column
					<< "): refusing to divide values of different types, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Are we attempting to operate on a string?
			if (left.is_string() || right.is_string()) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< slash_operator0.line << " col " << slash_operator0.column
					<< "): cannot apply division on a string expression, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Are we attempting to operate on a non-integer?
			if (left.is_char() || left.is_boolean() || right.is_char() || right.is_boolean()) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< slash_operator0.line << " col " << slash_operator0.column
					<< "): refusing to apply division on a non-integer, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Apply division depending on the integer type.
			if (left.is_integer()) {
				// Detect overflow in constant expression.
				if (would_division_overflow(left.get_integer(), right.get_integer())) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::is_expression_constant: error (line "
						<< slash_operator0.line << " col " << slash_operator0.column
						<< "): division would result in an overflow, for "
						<< left.get_integer() << " / " << right.get_integer()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Detect division by zero in constant expression.
				if (right.get_integer() == 0) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::is_expression_constant: error (line "
						<< slash_operator0.line << " col " << slash_operator0.column
						<< "): division by zero, for "
						<< left.get_integer() << " / " << right.get_integer()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Of three standard division/mod algorithms:
				// - QuotRem: C-style division and mod that truncates toward 0.
				// - DivMod: Division truncates toward negative infinite.
				// - Euclidian division: the remainder is non-negative.
				// I prefer Euclidian division, even though this deviates from
				// traditional division and modding in C.  So I'll use it.
				//expression_constant_value = ConstantValue(static_cast<int32_t>(static_cast<int32_t>(left.get_integer()) / static_cast<int32_t>(right.get_integer())), left.lexeme_begin, right.lexeme_end);
				expression_constant_value = ConstantValue(static_cast<int32_t>(euclidian_div(static_cast<int32_t>(left.get_integer()), static_cast<int32_t>(right.get_integer()))), left.lexeme_begin, right.lexeme_end);
				break;
			} else {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: internal error (line "
					<< slash_operator0.line << " col " << slash_operator0.column
					<< "): unhandled constant expression type for division: "
					<< left.get_tag_repr()
					;
				throw SemanticsError(sstr.str());
			}
		} case ::Expression::percent_branch: {
			const ::Expression::Percent &percent           = grammar.expression_percent_storage.at(expression_symbol.data);
			const ::Expression          &expression0       = grammar.expression_storage.at(percent.expression0); (void) expression0;
			const LexemeOperator        &percent_operator0 = grammar.lexemes.at(percent.percent_operator0).get_operator();
			const ::Expression          &expression1       = grammar.expression_storage.at(percent.expression1); (void) expression1;

			// Is either subexpression dynamic?  If so, this expression is also dynamic.
			ConstantValue right = is_expression_constant(percent.expression1, expression_constant_scope);
			if (right.is_dynamic()) {
				expression_constant_value = right;
				break;
			}
			ConstantValue left  = is_expression_constant(percent.expression0, expression_constant_scope);
			if (left.is_dynamic()) {
				expression_constant_value = left;
				break;
			}

			// Are the expressions of the same type?
			if (left.tag != right.tag) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< percent_operator0.line << " col " << percent_operator0.column
					<< "): refusing to mod values of different types, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Are we attempting to operate on a string?
			if (left.is_string() || right.is_string()) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< percent_operator0.line << " col " << percent_operator0.column
					<< "): cannot apply mod on a string expression, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Are we attempting to operate on a non-integer?
			if (left.is_char() || left.is_boolean() || right.is_char() || right.is_boolean()) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< percent_operator0.line << " col " << percent_operator0.column
					<< "): refusing to apply mod on a non-integer, for "
					<< left.get_tag_repr() << " with " << right.get_tag_repr()
					<< "."
					;
				throw SemanticsError(sstr.str());
			}

			// Apply mod depending on the integer type.
			if (left.is_integer()) {
				// Detect division by zero in constant expression.
				if (right.get_integer() == 0) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::is_expression_constant: error (line "
						<< percent_operator0.line << " col " << percent_operator0.column
						<< "): division by zero, for "
						<< left.get_integer() << " % " << right.get_integer()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				//expression_constant_value = ConstantValue(static_cast<int32_t>(static_cast<int32_t>(left.get_integer()) % static_cast<int32_t>(right.get_integer())), left.lexeme_begin, right.lexeme_end);
				expression_constant_value = ConstantValue(static_cast<int32_t>(euclidian_mod(static_cast<int32_t>(left.get_integer()), static_cast<int32_t>(right.get_integer()))), left.lexeme_begin, right.lexeme_end);
				break;
			} else {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: internal error (line "
					<< percent_operator0.line << " col " << percent_operator0.column
					<< "): unhandled constant expression type for mod: "
					<< left.get_tag_repr()
					;
				throw SemanticsError(sstr.str());
			}
		} case ::Expression::tilde_branch: {
			const ::Expression::Tilde &tilde           = grammar.expression_tilde_storage.at(expression_symbol.data);
			const LexemeOperator      &tilde_operator0 = grammar.lexemes.at(tilde.tilde_operator0).get_operator();
			const ::Expression        &expression0     = grammar.expression_storage.at(tilde.expression); (void) expression0;

			// Is the subexpression dynamic?  If so, this expression is also dynamic.
			ConstantValue value = is_expression_constant(tilde.expression, expression_constant_scope);
			if (value.is_dynamic()) {
				expression_constant_value = value;
				break;
			}

			// Are we attempting to operate on a string?
			if (value.is_string()) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< tilde_operator0.line << " col " << tilde_operator0.column
					<< "): cannot apply bitwise NOT on a string expression."
					;
				throw SemanticsError(sstr.str());
			}

			// Apply bitwise NOT depending on the integer type.
			if        (value.is_integer()) {
				expression_constant_value = ConstantValue(static_cast<int32_t>(~static_cast<int32_t>(value.get_integer())), tilde.tilde_operator0, value.lexeme_end);
				break;
			} else if (value.is_char()) {
				expression_constant_value = ConstantValue(static_cast<char>(~static_cast<char>(value.get_integer())), tilde.tilde_operator0, value.lexeme_end);
				break;
			} else if (value.is_boolean()) {
				expression_constant_value = ConstantValue(static_cast<bool>(~static_cast<bool>(value.get_integer())), tilde.tilde_operator0, value.lexeme_end);
				break;
			} else {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: internal error (line "
					<< tilde_operator0.line << " col " << tilde_operator0.column
					<< "): unhandled constant expression type for bitwise NOT: "
					<< value.get_tag_repr()
					;
				throw SemanticsError(sstr.str());
			}
		} case ::Expression::unary_minus_branch: {
			const ::Expression::UnaryMinus &unary_minus     = grammar.expression_unary_minus_storage.at(expression_symbol.data);
			const LexemeOperator           &minus_operator0 = grammar.lexemes.at(unary_minus.minus_operator0).get_operator();
			const ::Expression             &expression0     = grammar.expression_storage.at(unary_minus.expression); (void) expression0;

			// Is the subexpression dynamic?  If so, this expression is also dynamic.
			ConstantValue value = is_expression_constant(unary_minus.expression, expression_constant_scope);
			if (value.is_dynamic()) {
				expression_constant_value = value;
				break;
			}

			// Are we attempting to operate on a string?
			if (value.is_string()) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< minus_operator0.line << " col " << minus_operator0.column
					<< "): cannot apply unary minus on a string expression."
					;
				throw SemanticsError(sstr.str());
			}

			// Are we attempting to operate on a non-integer?
			if (value.is_char() || value.is_boolean()) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: error (line "
					<< minus_operator0.line << " col " << minus_operator0.column
					<< "): refusing to apply unary minus on a non-integer, for "
					<< value.get_tag_repr() << "."
					;
				throw SemanticsError(sstr.str());
			}

			// Apply unary minus depending on the integer type.
			if (value.is_integer()) {
				// Detect overflow in constant expression.
				if (would_multiplication_overflow(-1, value.get_integer())) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::is_expression_constant: error (line "
						<< minus_operator0.line << " col " << minus_operator0.column
						<< "): unary minus would result in an overflow, for "
						<< value.get_integer() << "."
						;
					throw SemanticsError(sstr.str());
				}

				expression_constant_value = ConstantValue(static_cast<int32_t>(-static_cast<int32_t>(value.get_integer())), unary_minus.minus_operator0, value.lexeme_end);
				break;
			} else {
				std::ostringstream sstr;
				sstr
					<< "Semantics::is_expression_constant: internal error (line "
					<< minus_operator0.line << " col " << minus_operator0.column
					<< "): unhandled constant expression type for unary minus: "
					<< value.get_tag_repr()
					;
				throw SemanticsError(sstr.str());
			}
		} case ::Expression::parentheses_branch: {
			const ::Expression::Parentheses &parentheses                = grammar.expression_parentheses_storage.at(expression_symbol.data);
			const LexemeOperator            &leftparenthesis_operator0  = grammar.lexemes.at(parentheses.leftparenthesis_operator0).get_operator(); (void) leftparenthesis_operator0;
			const ::Expression              &expression0                = grammar.expression_storage.at(parentheses.expression); (void) expression0;
			const LexemeOperator            &rightparenthesis_operator0 = grammar.lexemes.at(parentheses.rightparenthesis_operator0).get_operator(); (void) rightparenthesis_operator0;

			// Is the subexpression dynamic?  If so, this expression is also dynamic.
			ConstantValue value = is_expression_constant(parentheses.expression, expression_constant_scope);
			if (value.is_dynamic()) {
				expression_constant_value = value;
				break;
			}

			// The constant value of this expression is equivalent to the
			// constant value of the subexpression.
			expression_constant_value = ConstantValue(value, parentheses.leftparenthesis_operator0, parentheses.rightparenthesis_operator0 + 1);
			break;
		}

		// These 5 branches are dynamic.
		case ::Expression::call_branch: {
			const ::Expression::Call      &call                        = grammar.expression_call_storage.at(expression_symbol.data);
			const LexemeIdentifier        &call_identifier             = grammar.lexemes.at(call.identifier).get_identifier(); (void) call_identifier;
			const LexemeOperator          &leftparenthesis_operator0   = grammar.lexemes.at(call.leftparenthesis_operator0).get_operator(); (void) leftparenthesis_operator0;
			const ExpressionSequenceOpt   &expression_sequence_opt     = grammar.expression_sequence_opt_storage.at(call.expression_sequence_opt); (void) expression_sequence_opt;
			const LexemeOperator          &rightparenthesis_operator0  = grammar.lexemes.at(call.rightparenthesis_operator0).get_operator(); (void) rightparenthesis_operator0;

			expression_constant_value = ConstantValue(ConstantValue::Dynamic::dynamic, call.identifier, call.rightparenthesis_operator0 + 1);

			break;
		} case ::Expression::chr_branch: {
			const ::Expression::Chr &chr                        = grammar.expression_chr_storage.at(expression_symbol.data);
			const LexemeKeyword     &chr_keyword0               = grammar.lexemes.at(chr.chr_keyword0).get_keyword(); (void) chr_keyword0;
			const LexemeOperator    &leftparenthesis_operator0  = grammar.lexemes.at(chr.leftparenthesis_operator0).get_operator(); (void) leftparenthesis_operator0;
			const ::Expression      &expression0                = grammar.expression_storage.at(chr.expression); (void) expression0;
			const LexemeOperator    &rightparenthesis_operator0 = grammar.lexemes.at(chr.rightparenthesis_operator0).get_operator(); (void) rightparenthesis_operator0;

			expression_constant_value = ConstantValue(ConstantValue::Dynamic::dynamic, chr.chr_keyword0, chr.rightparenthesis_operator0 + 1);

			break;
		} case ::Expression::ord_branch: {
			const ::Expression::Ord &ord                        = grammar.expression_ord_storage.at(expression_symbol.data);
			const LexemeKeyword     &ord_keyword0               = grammar.lexemes.at(ord.ord_keyword0).get_keyword(); (void) ord_keyword0;
			const LexemeOperator    &leftparenthesis_operator0  = grammar.lexemes.at(ord.leftparenthesis_operator0).get_operator(); (void) leftparenthesis_operator0;
			const ::Expression      &expression0                = grammar.expression_storage.at(ord.expression); (void) expression0;
			const LexemeOperator    &rightparenthesis_operator0 = grammar.lexemes.at(ord.rightparenthesis_operator0).get_operator(); (void) rightparenthesis_operator0;

			expression_constant_value = ConstantValue(ConstantValue::Dynamic::dynamic, ord.ord_keyword0, ord.rightparenthesis_operator0 + 1);

			break;
		} case ::Expression::pred_branch: {
			const ::Expression::Pred &pred                       = grammar.expression_pred_storage.at(expression_symbol.data);
			const LexemeKeyword      &pred_keyword0              = grammar.lexemes.at(pred.pred_keyword0).get_keyword(); (void) pred_keyword0;
			const LexemeOperator     &leftparenthesis_operator0  = grammar.lexemes.at(pred.leftparenthesis_operator0).get_operator(); (void) leftparenthesis_operator0;
			const ::Expression       &expression0                = grammar.expression_storage.at(pred.expression); (void) expression0;
			const LexemeOperator     &rightparenthesis_operator0 = grammar.lexemes.at(pred.rightparenthesis_operator0).get_operator(); (void) rightparenthesis_operator0;

			expression_constant_value = ConstantValue(ConstantValue::Dynamic::dynamic, pred.pred_keyword0, pred.rightparenthesis_operator0 + 1);

			break;
		} case ::Expression::succ_branch: {
			const ::Expression::Succ &succ                       = grammar.expression_succ_storage.at(expression_symbol.data);
			const LexemeKeyword      &succ_keyword0              = grammar.lexemes.at(succ.succ_keyword0).get_keyword(); (void) succ_keyword0;
			const LexemeOperator     &leftparenthesis_operator0  = grammar.lexemes.at(succ.leftparenthesis_operator0).get_operator(); (void) leftparenthesis_operator0;
			const ::Expression       &expression0                = grammar.expression_storage.at(succ.expression); (void) expression0;
			const LexemeOperator     &rightparenthesis_operator0 = grammar.lexemes.at(succ.rightparenthesis_operator0).get_operator(); (void) rightparenthesis_operator0;

			expression_constant_value = ConstantValue(ConstantValue::Dynamic::dynamic, succ.succ_keyword0, succ.rightparenthesis_operator0 + 1);

			break;
		}

		// lvalue_branch may or may not be dynamic, depending on whether it is
		// a simple identifier (without .foo or [bar]) that refers to a known
		// constant value in the scope of the expression.
		case ::Expression::lvalue_branch: {
			const ::Expression::Lvalue       &lvalue                      = grammar.expression_lvalue_storage.at(expression_symbol.data);
			const Lvalue                     &lvalue_symbol               = grammar.lvalue_storage.at(lvalue.lvalue);
			const LexemeIdentifier           &lexeme_identifier           = grammar.lexemes.at(lvalue_symbol.identifier).get_identifier();
			const LvalueAccessorClauseList   &lvalue_accessor_clause_list = grammar.lvalue_accessor_clause_list_storage.at(lvalue_symbol.lvalue_accessor_clause_list);

			// According to the documentation, only lvalues without
			// accessors can be constant (static) expressions.  So check the
			// lvalue type, to see if it's just an identifier.
			if (lvalue_accessor_clause_list.branch != LvalueAccessorClauseList::empty_branch) {
				// It's not just an identifier.  This lvalue expression is not
				// a constant expression.

				// Unpack the lvalue_accessor_clause_list just enough to get the last lexeme.
				uint64_t lexeme_end;
				switch (lvalue_accessor_clause_list.branch) {
					// We already checked that the list is not empty.
					//case LvalueAccessorClauseList::empty_branch: {
					//	// ...
					//	break;
					//}

					case LvalueAccessorClauseList::cons_branch: {
						// Unpack the list.
						const LvalueAccessorClauseList::Cons &last_lvalue_accessor_clause_list_cons = grammar.lvalue_accessor_clause_list_cons_storage.at(lvalue_accessor_clause_list.data);
						const LvalueAccessorClauseList       &last_lvalue_accessor_clause_list      = grammar.lvalue_accessor_clause_list_storage.at(last_lvalue_accessor_clause_list_cons.lvalue_accessor_clause_list);
						const LvalueAccessorClause           &last_lvalue_accessor_clause           = grammar.lvalue_accessor_clause_storage.at(last_lvalue_accessor_clause_list_cons.lvalue_accessor_clause); (void) last_lvalue_accessor_clause;

						// Unpack just the last LvalueAccessorClause.
						switch (last_lvalue_accessor_clause.branch) {
							case LvalueAccessorClause::index_branch: {
								const LvalueAccessorClause::Index &index            = grammar.lvalue_accessor_clause_index_storage.at(last_lvalue_accessor_clause.data);
								const LexemeOperator              &dot_operator0    = grammar.lexemes.at(index.dot_operator0).get_operator(); (void) dot_operator0;
								const LexemeIdentifier            &index_identifier = grammar.lexemes.at(index.identifier).get_identifier(); (void) index_identifier;

								lexeme_end = index.identifier + 1;

								break;
							}

							case LvalueAccessorClause::array_branch: {
								const LvalueAccessorClause::Array &array                  = grammar.lvalue_accessor_clause_array_storage.at(last_lvalue_accessor_clause.data);
								const LexemeOperator              &leftbracket_operator0  = grammar.lexemes.at(array.leftbracket_operator0).get_operator(); (void) leftbracket_operator0;
								const ::Expression                &expression0            = grammar.expression_storage.at(array.expression); (void) expression0;
								const LexemeOperator              &rightbracket_operator0 = grammar.lexemes.at(array.rightbracket_operator0).get_operator(); (void) rightbracket_operator0;

								lexeme_end = array.rightbracket_operator0 + 1;

								break;
							}

							// Unrecognized branch.
							default: {
								std::ostringstream sstr;
								sstr << "Semantics::is_expression_constant: internal error: invalid lvalue_accessor_clause branch at index " << last_lvalue_accessor_clause_list_cons.lvalue_accessor_clause << ": " << last_lvalue_accessor_clause.branch;
								throw SemanticsError(sstr.str());
							}
						}

						break;
					}

					// Unrecognized branch.
					default: {
						std::ostringstream sstr;
						sstr << "Semantics::is_expression_constant: internal error: invalid lvalue_accessor_clause_list branch at index " << lvalue_symbol.lvalue_accessor_clause_list << ": " << lvalue_accessor_clause_list.branch;
						throw SemanticsError(sstr.str());
					}
				};

				expression_constant_value = ConstantValue(ConstantValue::Dynamic::dynamic, lvalue_symbol.identifier, lexeme_end);
				break;
			}

			// Lookup the identifier binding.
			std::optional<IdentifierScope::IdentifierBinding> identifier_binding_search = expression_constant_scope.lookup_copy(lexeme_identifier.text);
			if (!identifier_binding_search) {
				std::ostringstream sstr;
				sstr << "Semantics::is_expression_constant: error (line " << lexeme_identifier.line << " col " << lexeme_identifier.column << "): identifier out of scope when checking for constant lvalue: " << lexeme_identifier.text;
				throw SemanticsError(sstr.str());
			}

			if (!identifier_binding_search->is_static()) {
				// The identifier does not refer to a constant expression.
				expression_constant_value = ConstantValue(ConstantValue::Dynamic::dynamic, lvalue_symbol.identifier, lvalue_symbol.identifier + 1);
				break;
			} else {  // identifier_binding_search->is_static()
				const IdentifierScope::IdentifierBinding::Static &static_ = identifier_binding_search->get_static();
				// Copy the constant value.
				expression_constant_value = ConstantValue(static_.constant_value, lvalue_symbol.identifier, lvalue_symbol.identifier + 1);
				break;
			}
		}

		// These 3 branches are static.
		case ::Expression::integer_branch: {
			const ::Expression::Integer &integer        = grammar.expression_integer_storage.at(expression_symbol.data);
			const LexemeInteger         &lexeme_integer = grammar.lexemes.at(integer.integer).get_integer();
			if (lexeme_integer.first_digits > std::numeric_limits<int32_t>::max() || lexeme_integer.remaining_digits.size() > 0) {
				std::ostringstream sstr;
				sstr << "Semantics::is_expression_constant: error (line " << lexeme_integer.line << " col " << lexeme_integer.column << "): integer is too large to encode in 32 bits: " << lexeme_integer.text;
				throw SemanticsError(sstr.str());
			}
			expression_constant_value = ConstantValue(static_cast<int32_t>(lexeme_integer.first_digits), integer.integer, integer.integer + 1);
			break;
		} case ::Expression::char__branch: {
			const ::Expression::Char_ &char_       = grammar.expression_char__storage.at(expression_symbol.data);
			const LexemeChar          &lexeme_char = grammar.lexemes.at(char_.char_).get_char();
			expression_constant_value = ConstantValue(static_cast<char>(lexeme_char.char_), char_.char_, char_.char_ + 1);
			break;
		} case ::Expression::string_branch: {
			const ::Expression::String &string        = grammar.expression_string_storage.at(expression_symbol.data);
			const LexemeString         &lexeme_string = grammar.lexemes.at(string.string).get_string();
			expression_constant_value = ConstantValue(std::move(std::string(lexeme_string.expanded)), string.string, string.string + 1);
			break;
		}

		// Unrecognized branch.
		default: {
			std::ostringstream sstr;
			sstr << "Semantics::is_expression_constant: internal error: invalid expression branch at index " << expression << ": " << expression_symbol.branch;
			throw SemanticsError(sstr.str());
		}
	}

	// Return the calculated constant value.
	return expression_constant_value;
}

Semantics::ConstantValue Semantics::is_expression_constant(const ::Expression &expression, const IdentifierScope &expression_constant_scope) const {
	return is_expression_constant(&expression - &grammar.expression_storage[0], expression_constant_scope);
}

// | From the parse tree Type, construct a Semantics::Type that represents the type.
Semantics::Type Semantics::analyze_type(const std::string &identifier, const ::Type &type, const IdentifierScope &type_constant_scope, const IdentifierScope &type_type_scope, IdentifierScope &anonymous_storage) {
	switch (type.branch) {
		case ::Type::simple_branch: {
			// Unpack the simple_type.
			const ::Type::Simple   &simple            = grammar.type_simple_storage.at(type.data);
			const SimpleType       &simple_type       = grammar.simple_type_storage.at(simple.simple_type);
			const LexemeIdentifier &simple_identifier = grammar.lexemes.at(simple_type.identifier).get_identifier();

			// Check for redefinitions.  (Redundant; this has already been checked, with better location information.)
			if (type_type_scope.has(identifier)) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::analyze_type: error (near line "
					<< simple_identifier.line << " col " << simple_identifier.column
					<< "): redefinition of type ``" << identifier << "\"."
					<< "  (Internal error: this should already have been detected by now.)"
					;
				throw SemanticsError(sstr.str());
			}

			// Lookup the referent.
			if (!type_type_scope.has(simple_identifier.text)) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::analyze_type: error (line "
					<< simple_identifier.line << " col " << simple_identifier.column
					<< "): couldn't find type ``" << simple_identifier.text
					<< "\" when defining type alias ``" << identifier
					<< "\"."
					;
				throw SemanticsError(sstr.str());
			}
			// type_type_scope should only have Type identifier bindings.
			const IdentifierScope::IdentifierBinding::Type &referent_binding = type_type_scope.get(simple_identifier.text).get_type();
			// The pointer's lifetime should not exceed the lifetime of the referent, normally inside the identifier scope.
			const Type &referent = referent_binding;

			// Construct the Simple type.
			Type::Simple semantics_simple(identifier, referent);

			// Return the constructed simple type.
			return Type(semantics_simple);
		}

		case ::Type::record_branch: {
			// Unpack the record_type.
			const ::Type::Record              &record                         = grammar.type_record_storage.at(type.data);
			const RecordType                  &record_type                    = grammar.record_type_storage.at(record.record_type);
			const LexemeKeyword               &record_keyword0                = grammar.lexemes.at(record_type.record_keyword0).get_keyword(); (void) record_keyword0;
			const TypedIdentifierSequenceList &typed_identifier_sequence_list = grammar.typed_identifier_sequence_list_storage.at(record_type.typed_identifier_sequence_list);
			const LexemeKeyword               &end_keyword0                   = grammar.lexemes.at(record_type.end_keyword0).get_keyword(); (void) end_keyword0;

			// Check for redefinitions.  (Redundant; this has already been checked.)
			if (type_type_scope.has(identifier)) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::analyze_type: error (line "
					<< record_keyword0.line << " col " << record_keyword0.column
					<< "): redefinition of type ``" << identifier << "\"."
					<< "  (Internal error: this should already have been detected by now.)"
					;
				throw SemanticsError(sstr.str());
			}

			// Prepare the fields vector.
			std::set<std::string>                             field_identifiers;
			std::vector<std::pair<std::string, const Type *>> fields;

			// Collect the typed identifier sequences in the list.
			std::vector<const TypedIdentifierSequence *> typed_identifier_sequences;
			bool reached_end = false;
			for (const TypedIdentifierSequenceList *last_list = &typed_identifier_sequence_list; !reached_end; ) {
				// Unpack the last list encountered.
				switch(last_list->branch) {
					case TypedIdentifierSequenceList::empty_branch: {
						// We're done.
						// (No need to unpack the empty branch.)
						reached_end = true;
						break;
					}

					case TypedIdentifierSequenceList::cons_branch: {
						// Unpack the list.
						const TypedIdentifierSequenceList::Cons &last_typed_identifier_sequence_list_cons = grammar.typed_identifier_sequence_list_cons_storage.at(last_list->data);
						const TypedIdentifierSequenceList       &last_typed_identifier_sequence_list      = grammar.typed_identifier_sequence_list_storage.at(last_typed_identifier_sequence_list_cons.typed_identifier_sequence_list);
						const TypedIdentifierSequence           &last_typed_identifier_sequence           = grammar.typed_identifier_sequence_storage.at(last_typed_identifier_sequence_list_cons.typed_identifier_sequence);

						// Add the constant assignment.
						typed_identifier_sequences.push_back(&last_typed_identifier_sequence);
						last_list = &last_typed_identifier_sequence_list;

						// Loop.
						break;
					}

					// Unrecognized branch.
					default: {
						std::ostringstream sstr;
						sstr << "Semantics::analyze_type: internal error: invalid typed_identifier_sequence_list branch at index " << last_list - &grammar.typed_identifier_sequence_list_storage[0] << ": " << last_list->branch;
						throw SemanticsError(sstr.str());
					}
				}
			}

			// Correct the order of the list.
			std::reverse(typed_identifier_sequences.begin(), typed_identifier_sequences.end());

			// Handle the typed identifier sequences.
			for (const TypedIdentifierSequence *next_typed_identifier_sequence : typed_identifier_sequences) {
				const IdentList      &ident_list          = grammar.ident_list_storage.at(next_typed_identifier_sequence->ident_list);
				const LexemeOperator &colon_operator0     = grammar.lexemes.at(next_typed_identifier_sequence->colon_operator0).get_operator(); (void) colon_operator0;
				const ::Type         &next_type           = grammar.type_storage.at(next_typed_identifier_sequence->type);
				const LexemeOperator &semicolon_operator0 = grammar.lexemes.at(next_typed_identifier_sequence->semicolon_operator0).get_operator(); (void) semicolon_operator0;

				// Get a copy of the subtype or construct a new anonymous subtype using "anonymous_storage".
				const Type *next_semantics_type;
				// Branch on next_type.  If it's in the "simple" type alias
				// format, it should refer to an existing type, although it's
				// not a type alias.  Otherwise, create an anonymous type.
				if (next_type.branch == ::Type::simple_branch) {
					const ::Type::Simple   &simple            = grammar.type_simple_storage.at(next_type.data);
					const SimpleType       &simple_type       = grammar.simple_type_storage.at(simple.simple_type);
					const LexemeIdentifier &simple_identifier = grammar.lexemes.at(simple_type.identifier).get_identifier();

					// Make sure the reference typed is in scope.
					if (!type_type_scope.has(simple_identifier.text)) {
						std::ostringstream sstr;
						sstr
							<< "Semantics::analyze_type: error (line "
							<< simple_identifier.line << " col " << simple_identifier.column
							<< "): undefined type ``" << simple_identifier.text << "\": not in scope."
							;
						throw SemanticsError(sstr.str());
					}

					// Set next_semantics_type.
					next_semantics_type = &type_type_scope.get(simple_identifier.text).get_type();
				} else {
					// Create an anonymous type.
					Type anonymous_type = analyze_type("", next_type, type_constant_scope, type_type_scope, anonymous_storage);
					anonymous_storage.anonymous_bindings.push_back(IdentifierScope::IdentifierBinding(anonymous_type));
					next_semantics_type = &anonymous_storage.anonymous_bindings[anonymous_storage.anonymous_bindings.size() - 1].get_type();
				}

				// Unpack the ident_list.
				const LexemeIdentifier       &first_identifier         = grammar.lexemes.at(ident_list.identifier).get_identifier();
				const IdentifierPrefixedList &identifier_prefixed_list = grammar.identifier_prefixed_list_storage.at(ident_list.identifier_prefixed_list);

				// Collect the identifiers in the list.
				std::vector<const LexemeIdentifier *> identifiers;
				identifiers.push_back(&first_identifier);
				bool reached_end = false;
				for (const IdentifierPrefixedList *last_list = &identifier_prefixed_list; !reached_end; ) {
					// Unpack the last list encountered.
					switch(last_list->branch) {
						case IdentifierPrefixedList::empty_branch: {
							// We're done.
							// (No need to unpack the empty branch.)
							reached_end = true;
							break;
						}

						case IdentifierPrefixedList::cons_branch: {
							// Unpack the list.
							const IdentifierPrefixedList::Cons &last_identifier_prefixed_list_cons = grammar.identifier_prefixed_list_cons_storage.at(last_list->data);
							const IdentifierPrefixedList       &last_identifier_prefixed_list      = grammar.identifier_prefixed_list_storage.at(last_identifier_prefixed_list_cons.identifier_prefixed_list);
							const LexemeOperator               &last_colon_operator0               = grammar.lexemes.at(last_identifier_prefixed_list_cons.comma_operator0).get_operator(); (void) last_colon_operator0;
							const LexemeIdentifier             &last_identifier                    = grammar.lexemes.at(last_identifier_prefixed_list_cons.identifier).get_identifier();

							// Add the identifier.
							identifiers.push_back(&last_identifier);
							last_list = &last_identifier_prefixed_list;

							// Loop.
							break;
						}

						// Unrecognized branch.
						default: {
							std::ostringstream sstr;
							sstr << "Semantics::analyze_type: internal error: invalid identifier_prefixed_list branch at index " << last_list - &grammar.identifier_prefixed_list_storage[0] << ": " << last_list->branch;
							throw SemanticsError(sstr.str());
						}
					}
				}

				// Correct the order of the list.
				std::reverse(identifiers.begin() + 1, identifiers.end());

				// Handle the identifiers.
				for (const LexemeIdentifier *next_identifier : identifiers) {
					// Duplicate identifier in fields?
					if (field_identifiers.find(next_identifier->text) != field_identifiers.cend()) {
						std::ostringstream sstr;
						sstr
							<< "Semantics::analyze_type: error (line "
							<< next_identifier->line << " col " << next_identifier->column
							<< "): duplicate field name ``" << next_identifier->text << "\" in record type."
							;
						throw SemanticsError(sstr.str());
					}

					// Add a field.
					field_identifiers.insert({next_identifier->text});
					fields.push_back({next_identifier->text, next_semantics_type});
				}
			}

			// Construct the Record type.
			Type::Record semantics_record(identifier, fields, anonymous_storage);

			// Return the constructed record type.
			return Type(semantics_record);
		}

		case ::Type::array_branch: {
			// Unpack the array_type.
			const ::Type::Array    &array                  = grammar.type_array_storage.at(type.data);
			const ArrayType        &array_type             = grammar.array_type_storage.at(array.array_type);
			const LexemeKeyword    &array_keyword0         = grammar.lexemes.at(array_type.array_keyword0).get_keyword(); (void) array_keyword0;
			const LexemeOperator   &leftbracket_operator0  = grammar.lexemes.at(array_type.leftbracket_operator0).get_operator();
			const ::Expression     &expression0            = grammar.expression_storage.at(array_type.expression0);
			const LexemeOperator   &colon_operator0        = grammar.lexemes.at(array_type.colon_operator0).get_operator();
			const ::Expression     &expression1            = grammar.expression_storage.at(array_type.expression1);
			const LexemeOperator   &rightbracket_operator0 = grammar.lexemes.at(array_type.rightbracket_operator0).get_operator(); (void) rightbracket_operator0;
			const LexemeKeyword    &of_keyword0            = grammar.lexemes.at(array_type.of_keyword0).get_keyword(); (void) of_keyword0;
			const ::Type           &base_type              = grammar.type_storage.at(array_type.type);

			// Check for redefinitions.  (Redundant; this has already been checked.)
			if (type_type_scope.has(identifier)) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::analyze_type: error (line "
					<< array_keyword0.line << " col " << array_keyword0.column
					<< "): redefinition of type ``" << identifier << "\"."
					<< "  (Internal error: this should already have been detected by now.)"
					;
				throw SemanticsError(sstr.str());
			}

			// Get minimum and maximum indices.
			ConstantValue min_index_value = is_expression_constant(expression0, type_constant_scope);
			ConstantValue max_index_value = is_expression_constant(expression1, type_constant_scope);

			// Make sure they're static (constant).
			if (min_index_value.is_dynamic()) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::analyze_type: error (line "
					<< grammar.lexemes.at(min_index_value.lexeme_begin).get_line() << " col " << grammar.lexemes.at(min_index_value.lexeme_begin).get_column()
					<< "): the minimum index of an array is not a constant value."
					;
				throw SemanticsError(sstr.str());
			}
			if (max_index_value.is_dynamic()) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::analyze_type: error (line "
					<< grammar.lexemes.at(max_index_value.lexeme_begin).get_line() << " col " << grammar.lexemes.at(max_index_value.lexeme_begin).get_column()
					<< "): the maximum index of an array is not a constant value."
					;
				throw SemanticsError(sstr.str());
			}

			// Make sure they're integers.
			if (!min_index_value.is_integer()) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::analyze_type: error (line "
					<< grammar.lexemes.at(min_index_value.lexeme_begin).get_line() << " col " << grammar.lexemes.at(min_index_value.lexeme_begin).get_column()
					<< "): the minimum index of an array is not an integer value."
					;
				throw SemanticsError(sstr.str());
			}
			if (!max_index_value.is_integer()) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::analyze_type: error (line "
					<< grammar.lexemes.at(max_index_value.lexeme_begin).get_line() << " col " << grammar.lexemes.at(max_index_value.lexeme_begin).get_column()
					<< "): the maximum index of an array is not an integer value."
					;
				throw SemanticsError(sstr.str());
			}

			// Get the minimum and maximum indices.
			int32_t min_index = min_index_value.get_integer();
			int32_t max_index = max_index_value.get_integer();

			// Make sure the minimum index is not > the maximum index.
			if (!min_index_value.is_integer()) {
				std::ostringstream sstr;
				sstr
					<< "Semantics::analyze_type: error (line "
					<< leftbracket_operator0.line << " col " << leftbracket_operator0.column
					<< "): the minimum index of an array is greater than the maximum index: "
					<< min_index << " > " << max_index
					;
				throw SemanticsError(sstr.str());
			}

			// Get a copy of the subtype or construct a new anonymous subtype using "anonymous_storage".
			const Type *base_semantics_type;
			// Branch on base_type.  If it's in the "simple" type alias
			// format, it should refer to an existing type, although it's
			// not a type alias.  Otherwise, create an anonymous type.
			if (base_type.branch == ::Type::simple_branch) {
				const ::Type::Simple   &simple            = grammar.type_simple_storage.at(base_type.data);
				const SimpleType       &simple_type       = grammar.simple_type_storage.at(simple.simple_type);
				const LexemeIdentifier &simple_identifier = grammar.lexemes.at(simple_type.identifier).get_identifier();

				// Make sure the reference typed is in scope.
				if (!type_type_scope.has(simple_identifier.text)) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_type: error (line "
						<< simple_identifier.line << " col " << simple_identifier.column
						<< "): undefined type ``" << simple_identifier.text << "\": not in scope."
						;
					throw SemanticsError(sstr.str());
				}

				// Set base_semantics_type.
				base_semantics_type = &type_type_scope.get(simple_identifier.text).get_type();
			} else {
				// Create an anonymous type.
				Type anonymous_type = analyze_type("", base_type, type_constant_scope, type_type_scope, anonymous_storage);
				anonymous_storage.anonymous_bindings.push_back(IdentifierScope::IdentifierBinding(anonymous_type));
				base_semantics_type = &anonymous_storage.anonymous_bindings[anonymous_storage.anonymous_bindings.size() - 1].get_type();
			}

			// Construct the Array type.
			Type::Array semantics_array(identifier, base_semantics_type, min_index, max_index, anonymous_storage);

			// Return the constructed array type.
			return Type(semantics_array);
		}

		// Unrecognized branch.
		default: {
			std::ostringstream sstr;
			sstr << "Semantics::analyze: internal error: invalid type branch at index " << &type - &grammar.type_storage[0] << ": " << type.branch;
			throw SemanticsError(sstr.str());
		}
	}
}

Semantics::Storage::Storage()
	{}

Semantics::Storage::Storage(uint32_t max_size, bool is_global, Symbol global_address, const std::string &register_, bool dereference, int32_t offset)
	: max_size(max_size)
	, is_global(is_global)
	, global_address(global_address)
	, register_(register_)
	, dereference(dereference)
	, offset(offset)
	{}

// Specialized storage constructors.
// | Storage type #1: (&global) + x, and
// | Storage type #2: ((uint8_t *) &global)[x];  -- if global is already a byte pointer/array, global[x].
Semantics::Storage::Storage(Symbol global_address, bool dereference, uint32_t max_size, int32_t offset)
	: max_size(max_size)
	, is_global(true)
	, global_address(global_address)
	, register_("")
	, dereference(dereference)
	, offset(offset)
	{}

// | Storage type #3: 4-byte direct register.  (No dereference.)
Semantics::Storage::Storage(const std::string &register_)
	: max_size(4)
	, is_global(false)
	, global_address(Symbol())
	, register_(register_)
	, dereference(false)
	, offset(0)
	{}

// | Storage type #4: dereferenced register.
Semantics::Storage::Storage(const std::string &register_, uint32_t max_size, int32_t offset)
	: max_size(max_size)
	, is_global(false)
	, global_address(Symbol())
	, register_(register_)
	, dereference(true)
	, offset(offset)
	{}

// | Is this size ideal with for this storage?  (Does it equal max_size?)
bool Semantics::Storage::ideal_size(uint32_t size) const {
	return size == max_size;
}

// | Is this size compatible with this storage?  (Is it <= max_size?)
bool Semantics::Storage::compatible_size(uint32_t size) const {
	return size <= max_size;
}

std::vector<uint32_t> Semantics::Storage::get_sizes(const std::vector<Storage> &storage) {
	std::vector<uint32_t> sizes;
	for (const Storage &storage_unit : std::as_const(storage)) {
		sizes.push_back(storage_unit.max_size);
	}
	return sizes;
}

// Type of the register.
Semantics::Storage::type_t Semantics::Storage::get_type() const {
	if           ( is_global && !dereference) {
		return global_address_type;
	} else if    ( is_global &&  dereference) {
		return global_dereference_type;
	} else if    (!is_global && !dereference) {
		return register_direct_type;
	} else {  // (!is_global &&  dereference) {
		return register_dereference_type;
	}
}

bool Semantics::Storage::is_global_address()       const { return get_type() == global_address_type; }
bool Semantics::Storage::is_global_dereference()   const { return get_type() == global_dereference_type; }
bool Semantics::Storage::is_register_direct()      const { return get_type() == register_direct_type; }
bool Semantics::Storage::is_register_dereference() const { return get_type() == register_dereference_type; }

Semantics::Instruction::Base::Base()
	: has_symbol(false)
	{}

Semantics::Instruction::Base::Base(bool has_symbol, Symbol symbol)
	: has_symbol(has_symbol)
	, symbol(symbol)
	{}

std::vector<uint32_t> Semantics::Instruction::Base::get_input_sizes() const {
	std::ostringstream sstr;
	sstr << "Semantics::Instruction::Base::get_input_sizes: error: Base class method called; only the subclass method should be called.";
	throw SemanticsError(sstr.str());
}

std::vector<uint32_t> Semantics::Instruction::Base::get_working_sizes() const {
	std::ostringstream sstr;
	sstr << "Semantics::Instruction::Base::get_working_sizes: error: Base class method called; only the subclass method should be called.";
	throw SemanticsError(sstr.str());
}

std::vector<uint32_t> Semantics::Instruction::Base::get_output_sizes() const {
	std::ostringstream sstr;
	sstr << "Semantics::Instruction::Base::get_output_sizes: error: Base class method called; only the subclass method should be called.";
	throw SemanticsError(sstr.str());
}

std::vector<uint32_t> Semantics::Instruction::Base::get_all_sizes() const {
	std::ostringstream sstr;
	sstr << "Semantics::Instruction::Base::get_all_sizes: error: Base class method called; only the subclass method should be called.";
	throw SemanticsError(sstr.str());
}

std::vector<Semantics::Output::Line> Semantics::Instruction::Base::emit(const std::vector<Storage> &storages) const {
	std::ostringstream sstr;
	sstr << "Semantics::Instruction::Base::emit: error: Base class method called; only the subclass method should be called.";
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::Ignore::Ignore()
	{}

Semantics::Instruction::Ignore::Ignore(const Base &base, bool has_input, bool is_word)
	: Base(base)
	, has_input(has_input)
	, is_word(is_word)
	{}

std::vector<uint32_t> Semantics::Instruction::Ignore::get_input_sizes() const { if (!has_input) { return {}; } else { return {static_cast<uint32_t>(is_word ? 4 : 1)}; } }
std::vector<uint32_t> Semantics::Instruction::Ignore::get_working_sizes() const { return {}; }
std::vector<uint32_t> Semantics::Instruction::Ignore::get_output_sizes() const { return {}; }
std::vector<uint32_t> Semantics::Instruction::Ignore::get_all_sizes() const { std::vector<uint32_t> v, i(std::move(get_input_sizes())), w(std::move(get_working_sizes())), o(std::move(get_output_sizes())); v.insert(v.end(), i.cbegin(), i.cend()); v.insert(v.end(), w.cbegin(), w.cend()); v.insert(v.end(), o.cbegin(), o.cend()); return v; }

std::vector<Semantics::Output::Line> Semantics::Instruction::Ignore::emit(const std::vector<Storage> &storages) const {
	// Check sizes.
	if (Storage::get_sizes(storages) != get_all_sizes()) {
		std::ostringstream sstr;
		sstr << "Semantics::Instruction::Ignore::emit: the number or sizes of storage units provided does not match what was expected.";
		throw SemanticsError(sstr.str());
	}

	// Prepare output vector.
	std::vector<Output::Line> lines;

	// Emit a symbol for this instruction if there is one.
	if (has_symbol) {
		lines.push_back({":", symbol});
	}

	// Emit no code.

	// Return the output.
	return lines;
}

Semantics::Instruction::LoadImmediate::LoadImmediate()
	{}

Semantics::Instruction::LoadImmediate::LoadImmediate(const Base &base, bool is_word, const ConstantValue &constant_value, Symbol string_symbol)
	: Base(base)
	, is_word(is_word)
	, constant_value(constant_value)
	, string_symbol(string_symbol)
	{}

std::vector<uint32_t> Semantics::Instruction::LoadImmediate::get_input_sizes() const { return {}; }
std::vector<uint32_t> Semantics::Instruction::LoadImmediate::get_working_sizes() const { return {}; }
std::vector<uint32_t> Semantics::Instruction::LoadImmediate::get_output_sizes() const { return {static_cast<uint32_t>(is_word ? 4 : 1)}; }
std::vector<uint32_t> Semantics::Instruction::LoadImmediate::get_all_sizes() const { std::vector<uint32_t> v, i(std::move(get_input_sizes())), w(std::move(get_working_sizes())), o(std::move(get_output_sizes())); v.insert(v.end(), i.cbegin(), i.cend()); v.insert(v.end(), w.cbegin(), w.cend()); v.insert(v.end(), o.cbegin(), o.cend()); return v; }

std::vector<Semantics::Output::Line> Semantics::Instruction::LoadImmediate::emit(const std::vector<Storage> &storages) const {
	// Check sizes.
	if (Storage::get_sizes(storages) != get_all_sizes()) {
		std::ostringstream sstr;
		sstr << "Semantics::Instruction::LoadImmediate::emit: the number or sizes of storage units provided does not match what was expected.";
		throw SemanticsError(sstr.str());
	}
	const Storage &destination_storage = storages[0];

	// Prepare output vector.
	std::vector<Output::Line> lines;

	// Emit a symbol for this instruction if there is one.
	if (has_symbol) {
		lines.push_back({":", symbol});
	}

	// If !this->is_word, replace sw with sb.
	// If the constant value is a string, replace "li" with "la", and use the symbol rather than the constant value.
	//
	// 4 storage types for destination:
	// 	1: global_address + x  (Store into this address.)
	// 	2: x(global_address)   (Store into this dereferenced address.)
	// 	3: $reg                (Store into this register.)
	// 	4: x($reg)             (Store into this dereferenced register.)
	//
	// 2 source types:
	// constant_non_string, constant_string
	//
	// X <- constant_non_string:
	// 	1: la   $t9, destination_storage.global_address
	// 	   la   $t9, destination_storage.offset($t9)     if != 0
	// 	   li   $t8, constant_non_string
	// 	   sw   $t8, ($t9)
	// 	2: la   $t9, destination_storage.global_address
	// 	   lw   $t9, destination_storage.offset($t9)     (the only difference from #1: la -> lw, and line is present if offset == 0)
	// 	   li   $t8, constant_non_string
	// 	   sw   $t8, ($t9)
	// 	3: li   REG, constant_non_string
	// 	4: li   $t9, constant_non_string
	// 	   sw   $t9, destination_storage.offset(REG)

	// Get the final store operation.
	Output::Line store_op;
	if (is_word) {
		store_op = "\tsw   ";
	} else {
		store_op = "\tsb   ";
	}

	// Get the li operation.  Are we loading a label or a number?
	Output::Line constant_load_op;
	Output::Line value;
	if (!constant_value.is_string()) {
		constant_load_op = "\tli   ";
		value = constant_value.get_static_repr();
	} else {
		constant_load_op = "\tla   ";
		value = destination_storage.global_address;
	}

	// Emit the output depending on the destination storage type.
	if           ( destination_storage.is_global && !destination_storage.dereference) {
		lines.push_back("\tla   $t9, " + destination_storage.global_address);
		if (destination_storage.offset != 0) {
			lines.push_back("\tla   $t9, " + std::to_string(destination_storage.offset) + "($t9)");
		}
		lines.push_back(constant_load_op + "$t8, " + value);
		lines.push_back("\tsw   $t8, ($t9)");
	} else if    ( destination_storage.is_global &&  destination_storage.dereference) {
		lines.push_back("\tla   $t9, " + destination_storage.global_address);
		lines.push_back("\tlw   $t9, " + std::to_string(destination_storage.offset) + "($t9)");
		lines.push_back(constant_load_op + "$t8, " + value);
		lines.push_back("\tsw   $t8, ($t9)");
	} else if    (!destination_storage.is_global && !destination_storage.dereference) {
		lines.push_back(constant_load_op + destination_storage.register_ + ", " + value);
	} else {  // (!destination_storage.is_global &&  destination_storage.dereference) {
		lines.push_back(constant_load_op + "$t9, " + value);
		lines.push_back("\tsw   $t9, " + std::to_string(destination_storage.offset) + "(" + destination_storage.register_ + ")");
	}

	// Return the output.
	return lines;
}

Semantics::Instruction::LoadFrom::LoadFrom()
	{}

Semantics::Instruction::LoadFrom::LoadFrom(const Base &base, bool is_word_save, bool is_word_load, int32_t addition)
	: Base(base)
	, is_word_save(is_word_save)
	, is_word_load(is_word_load)
	, addition(addition)
	{}

Semantics::Instruction::LoadFrom::LoadFrom(const Base &base, bool is_word, int32_t addition)
	: Base(base)
	, is_word_load(is_word)
	, is_word_save(is_word)
	, addition(addition)
	{}

std::vector<uint32_t> Semantics::Instruction::LoadFrom::get_input_sizes() const { return {static_cast<uint32_t>(is_word_load ? 4 : 1)}; }
std::vector<uint32_t> Semantics::Instruction::LoadFrom::get_working_sizes() const { return {}; }
std::vector<uint32_t> Semantics::Instruction::LoadFrom::get_output_sizes() const { return {static_cast<uint32_t>(is_word_save ? 4 : 1)}; }
std::vector<uint32_t> Semantics::Instruction::LoadFrom::get_all_sizes() const { std::vector<uint32_t> v, i(std::move(get_input_sizes())), w(std::move(get_working_sizes())), o(std::move(get_output_sizes())); v.insert(v.end(), i.cbegin(), i.cend()); v.insert(v.end(), w.cbegin(), w.cend()); v.insert(v.end(), o.cbegin(), o.cend()); return v; }

std::vector<Semantics::Output::Line> Semantics::Instruction::LoadFrom::emit(const std::vector<Storage> &storages) const {
	// Check sizes.
	if (Storage::get_sizes(storages) != get_all_sizes()) {
		std::ostringstream sstr;
		sstr << "Semantics::Instruction::LoadFrom::emit: the number or sizes of storage units provided does not match what was expected.";
		throw SemanticsError(sstr.str());
	}
	const Storage &source_storage      = storages[0];
	const Storage &destination_storage = storages[1];

	// Prepare output vector.
	std::vector<Output::Line> lines;

	// Emit a symbol for this instruction if there is one.
	if (has_symbol) {
		lines.push_back({":", symbol});
	}

	// If !this->is_word, replace sw with sb and lw with lb.
	//
	// 4 storage types for destination:
	// 	1: global_address + x  (Store into this address.)
	// 	2: x(global_address)   (Store into this dereferenced address.)
	// 	3: $reg                (Store into this register.)
	// 	4: x($reg)             (Store into this dereferenced register.)
	//
	// 4 storage types for source:
	// 	1: global_address + x  (Read from this address.)
	// 	2: x(global_address)   (Read from this dereferenced address.)
	// 	3: $reg                (Read from this register.)
	// 	4: x($reg)             (Read from this dereferenced register.)
	//
	// 16 cases:
	//
	// 1:  global_address + x <- global_address + x
	// 2:  global_address + x <- x(global_address)
	// 3:  global_address + x <- $reg
	// 4:  global_address + x <- x($reg)
	//
	// 5:  x(global_address) <- global_address + x
	// 6:  x(global_address) <- x(global_address)
	// 7:  x(global_address) <- $reg
	// 8:  x(global_address) <- x($reg)
	//
	// 9:  $reg <- global_address + x
	// 10: $reg <- x(global_address)
	// 11: $reg <- $reg
	// 12: $reg <- x($reg)
	//
	// 13: x($reg) <- global_address + x
	// 14: x($reg) <- x(global_address)
	// 15: x($reg) <- $reg
	// 16: x($reg) <- x($reg)
	//
	// Equivalent:
	//
	// 1: global_address + x <- global_address + x:
	// 	# Part 1: get destination address.
	// 	la   $t9, destination_storage.global_address
	// 	la   $t9, destination_storage.offset($t9)     if != 0
	//
	// 	# Part 2: get source address.
	// 	la   $t8, source_storage.global_address
	// 	la   $t8, source_storage.offset($t8)          if != 0
	//
	// 	# Part 3: load source.
	// 	lw   $t8, ($t8)
	// 	la   $t8, addition($t8)                       if != 0
	//
	// 	# Part 4: write destination.
	// 	sw   $t8, ($t9)
	// 2: global_address + x <- x(global_address):
	// 	...
	//
	// 5-8:
	// 	Copy #1-4.  3{s/la/lw/; s/if.*$//}
	//
	// ...

	Output::Line sized_load;
	Output::Line sized_save;

	if (is_word_load) {
		sized_load = "\tlw   ";
	} else {
		sized_load = "\tlb   ";
	}

	if (is_word_save) {
		sized_save = "\tsw   ";
	} else {
		sized_save = "\tsb   ";
	}

	// Part 1: get destination address.
	if           ( destination_storage.is_global && !destination_storage.dereference) {
		lines.push_back("\tla   $t9, " + destination_storage.global_address);
		if (destination_storage.offset != 0) {
			lines.push_back("\tla   $t9, " + std::to_string(destination_storage.offset) + "($t9)");
		}
	} else if    ( destination_storage.is_global &&  destination_storage.dereference) {
		lines.push_back("\tla   $t9, " + destination_storage.global_address);
		lines.push_back("\tlw   $t9, " + std::to_string(destination_storage.offset) + "($t9)");
	} else if    (!destination_storage.is_global && !destination_storage.dereference) {
	} else {  // (!destination_storage.is_global &&  destination_storage.dereference) {
		lines.push_back("\tla   $t9, " + std::to_string(destination_storage.offset) + "(" + destination_storage.register_ + ")");
	}

	// Part 2: get source address.
	if           ( source_storage.is_global && !source_storage.dereference) {
		lines.push_back("\tla   $t8, " + source_storage.global_address);
		if (source_storage.offset != 0) {
			lines.push_back("\tla   $t8, " + std::to_string(source_storage.offset) + "($t8)");
		}
	} else if    ( source_storage.is_global &&  source_storage.dereference) {
		lines.push_back("\tla   $t8, " + source_storage.global_address);
		lines.push_back("\tlw   $t8, " + std::to_string(source_storage.offset) + "($t8)");
	} else if    (!source_storage.is_global && !source_storage.dereference) {
	} else {  // (!source_storage.is_global &&  source_storage.dereference) {
		lines.push_back("\tla   $t8, " + std::to_string(source_storage.offset) + "(" + source_storage.register_ + ")");
	}

	// Part 3: load source.  (Don't apply addition yet.)
	if           ( source_storage.is_global && !source_storage.dereference) {
		lines.push_back(sized_load + "$t8, ($t8)");
	} else if    ( source_storage.is_global &&  source_storage.dereference) {
		lines.push_back(sized_load + "$t8, ($t8)");
	} else if    (!source_storage.is_global && !source_storage.dereference) {
	} else {  // (!source_storage.is_global &&  source_storage.dereference) {
		lines.push_back(sized_load + "$t8, ($t8)");
	}

	// Part 4: write destination.
	if           ( destination_storage.is_global && !destination_storage.dereference) {
		if (!source_storage.is_global && !source_storage.dereference) {
			if (addition != 0) {
				lines.push_back("\tla   $t8, " + std::to_string(addition) + "(" + source_storage.register_ + ")");
				lines.push_back(sized_save + "$t8, ($t9)");
			} else {
				lines.push_back(sized_save + source_storage.register_ + ", ($t9)");
			}
		} else {
			lines.push_back("\tla   $t8, " + std::to_string(addition) + "($t8)");
			lines.push_back(sized_save + "$t8, ($t9)");
		}
	} else if    ( destination_storage.is_global &&  destination_storage.dereference) {
		if (!source_storage.is_global && !source_storage.dereference) {
			if (addition != 0) {
				lines.push_back("\tla   $t8, " + std::to_string(addition) + "(" + source_storage.register_ + ")");
				lines.push_back(sized_save + "$t8, ($t9)");
			} else {
				lines.push_back(sized_save + source_storage.register_ + ", ($t9)");
			}
		} else {
			lines.push_back("\tla   $t8, " + std::to_string(addition) + "($t8)");
			lines.push_back(sized_save + "$t8, ($t9)");
		}
	} else if    (!destination_storage.is_global && !destination_storage.dereference) {
		if (!source_storage.is_global && !source_storage.dereference) {
			if (addition != 0) {
				lines.push_back("\tla   " + destination_storage.register_ + ", " + std::to_string(addition) + "(" + source_storage.register_ + ")");
			} else {
				lines.push_back("\tla   " + destination_storage.register_ + ", " + std::to_string(addition) + "(" + source_storage.register_ + ")");
			}
		} else {
			lines.push_back("\tla   " + destination_storage.register_ + ", " + std::to_string(addition) + "($t8)");
		}
	} else {  // (!destination_storage.is_global &&  destination_storage.dereference) {
		if (!source_storage.is_global && !source_storage.dereference) {
			if (addition != 0) {
				lines.push_back("\tla   $t8, " + std::to_string(addition) + "(" + source_storage.register_ + ")");
				lines.push_back(sized_save + "$t8, ($t9)");
			} else {
				lines.push_back(sized_save + source_storage.register_ + ", ($t9)");
			}
		} else {
			lines.push_back("\tla   $t8, " + std::to_string(addition) + "($t8)");
			lines.push_back(sized_save + "$t8, ($t9)");
		}
	}

	// Return the output.
	return lines;
}

Semantics::Instruction::LessThanFrom::LessThanFrom()
	{}

Semantics::Instruction::LessThanFrom::LessThanFrom(const Base &base, bool is_word, bool is_signed)
	: Base(base)
	, is_word(is_word)
	, is_signed(is_signed)
	{}

std::vector<uint32_t> Semantics::Instruction::LessThanFrom::get_input_sizes() const { return {static_cast<uint32_t>(is_word ? 4 : 1), static_cast<uint32_t>(is_word ? 4 : 1)}; }
std::vector<uint32_t> Semantics::Instruction::LessThanFrom::get_working_sizes() const { return {}; }
std::vector<uint32_t> Semantics::Instruction::LessThanFrom::get_output_sizes() const { return {static_cast<uint32_t>(1)}; }
std::vector<uint32_t> Semantics::Instruction::LessThanFrom::get_all_sizes() const { std::vector<uint32_t> v, i(std::move(get_input_sizes())), w(std::move(get_working_sizes())), o(std::move(get_output_sizes())); v.insert(v.end(), i.cbegin(), i.cend()); v.insert(v.end(), w.cbegin(), w.cend()); v.insert(v.end(), o.cbegin(), o.cend()); return v; }

std::vector<Semantics::Output::Line> Semantics::Instruction::LessThanFrom::emit(const std::vector<Storage> &storages) const {
	// Check sizes.
	if (Storage::get_sizes(storages) != get_all_sizes()) {
		std::ostringstream sstr;
		sstr << "Semantics::Instruction::LessThanFrom::emit: the number or sizes of storage units provided does not match what was expected.";
		throw SemanticsError(sstr.str());
	}
	const Storage &left_source_storage  = storages[0];
	const Storage &right_source_storage = storages[1];
	const Storage &destination_storage  = storages[2];

	// Prepare output vector.
	std::vector<Output::Line> lines;

	// Emit a symbol for this instruction if there is one.
	if (has_symbol) {
		lines.push_back({":", symbol});
	}

	// If !this->is_word, replace sw with sb and lw with lb.
	//
	// 4 storage types for destination:
	// 	1: global_address + x  (Store into this address.)
	// 	2: x(global_address)   (Store into this dereferenced address.)
	// 	3: $reg                (Store into this register.)
	// 	4: x($reg)             (Store into this dereferenced register.)
	//
	// 4 storage types for a source:
	// 	1: global_address + x  (Read from this address.)
	// 	2: x(global_address)   (Read from this dereferenced address.)
	// 	3: $reg                (Read from this register.)
	// 	4: x($reg)             (Read from this dereferenced register.)

	Output::Line sized_load;
	Output::Line sized_save;

	if (is_word) {
		sized_load = "\tlw   ";
	} else {
		sized_load = "\tlb   ";
	}
	sized_save = "\tsb   ";

	Output::Line slt_operator;

	if (!is_signed) {
		slt_operator = "\tsltu ";
	} else {
		slt_operator = "\tslt  ";
	}

	// Part 1: get left source address.
	if           (left_source_storage.is_global_address()) {
		lines.push_back("\tla   $t8, " + left_source_storage.global_address);
		if (left_source_storage.offset != 0) {
			lines.push_back("\tla   $t8, " + std::to_string(left_source_storage.offset) + "($t8)");
		}
	} else if    (left_source_storage.is_global_dereference()) {
		lines.push_back("\tla   $t8, " + left_source_storage.global_address);
		lines.push_back("\tlw   $t8, " + std::to_string(left_source_storage.offset) + "($t8)");
	} else if    (left_source_storage.is_register_direct()) {
	} else {  // (left_source_storage.is_register_dereference()) {
		lines.push_back("\tla   $t8, " + std::to_string(left_source_storage.offset) + "(" + left_source_storage.register_ + ")");
	}

	// Part 2: load left source.
	if (!left_source_storage.is_register_direct()) {
		lines.push_back(sized_load + "$t8, ($t8)");
	}

	// Part 3: get right source address.
	if           (right_source_storage.is_global_address()) {
		lines.push_back("\tla   $t9, " + right_source_storage.global_address);
		if (right_source_storage.offset != 0) {
			lines.push_back("\tla   $t9, " + std::to_string(right_source_storage.offset) + "($t9)");
		}
	} else if    (right_source_storage.is_global_dereference()) {
		lines.push_back("\tla   $t9, " + right_source_storage.global_address);
		lines.push_back("\tlw   $t9, " + std::to_string(right_source_storage.offset) + "($t9)");
	} else if    (right_source_storage.is_register_direct()) {
	} else {  // (right_source_storage.is_register_dereference()) {
		lines.push_back("\tla   $t9, " + std::to_string(right_source_storage.offset) + "(" + right_source_storage.register_ + ")");
	}

	// Part 4: load right source.
	if (!right_source_storage.is_register_direct()) {
		lines.push_back(sized_load + "$t9, ($t9)");
	}

	// Part 5: if both $t8 and $t9 are used, $t8 += $t9, to free $t9 for destination.
	// If $t8 is free but $t9 is used, mark $t8 as being free.
	std::string destination_address_register;
	std::string sum_register;
	if (left_source_storage.is_register_direct()) {
		// $t8 is free.
		destination_address_register = "$t8";
		sum_register                 = "$t9";
	} else {
		// Make sure $t9 is free.
		destination_address_register = "$t9";
		sum_register                 = "$t8";
		if (!(right_source_storage.is_register_direct())) {
			lines.push_back(slt_operator + "$t8, $t8, $t9");
		}
	}

	// Part 6: get destination address.
	if           (destination_storage.is_global_address()) {
		lines.push_back("\tla   " + destination_address_register + ", " + destination_storage.global_address);
		if (destination_storage.offset != 0) {
			lines.push_back("\tla   " + destination_address_register + ", " + std::to_string(destination_storage.offset) + "(" + destination_address_register + ")");
		}
	} else if    (destination_storage.is_global_dereference()) {
		lines.push_back("\tla   " + destination_address_register + ", " + destination_storage.global_address);
		lines.push_back("\tlw   " + destination_address_register + ", " + std::to_string(destination_storage.offset) + "(" + destination_address_register + ")");
	} else if    (destination_storage.is_register_direct()) {
	} else {  // (destination_storage.is_register_dereference()) {
		lines.push_back("\tla   " + destination_address_register + ", " + std::to_string(destination_storage.offset) + "(" + destination_storage.register_ + ")");
	}

	// Part 7: write destination.
	if (destination_storage.is_register_direct()) {
		if           ( left_source_storage.is_register_direct() &&  right_source_storage.is_register_direct()) {
			lines.push_back(slt_operator + destination_storage.register_ + ", " + left_source_storage.register_ + ", " + right_source_storage.register_);
		} else if    ( left_source_storage.is_register_direct() && !right_source_storage.is_register_direct()) {
			lines.push_back(slt_operator + destination_storage.register_ + ", " + left_source_storage.register_ + ", $t9");
		} else if    (!left_source_storage.is_register_direct() &&  right_source_storage.is_register_direct()) {
			lines.push_back(slt_operator + destination_storage.register_ + ", $t8, " + right_source_storage.register_);
		} else {  // (!left_source_storage.is_register_direct() && !right_source_storage.is_register_direct()) {
			lines.push_back(slt_operator + destination_storage.register_ + ", $t8, $t9");
		}
	} else {
		if           ( left_source_storage.is_register_direct() &&  right_source_storage.is_register_direct()) {
			lines.push_back(slt_operator + sum_register + ", " + left_source_storage.register_ + ", " + right_source_storage.register_);
			lines.push_back(sized_save + sum_register + ", (" + destination_address_register + ")");
		} else if    ( left_source_storage.is_register_direct() && !right_source_storage.is_register_direct()) {
			lines.push_back(slt_operator + sum_register + ", " + left_source_storage.register_ + ", $t9");
			lines.push_back(sized_save + sum_register + ", (" + destination_address_register + ")");
		} else if    (!left_source_storage.is_register_direct() &&  right_source_storage.is_register_direct()) {
			lines.push_back(slt_operator + sum_register + ", $t8, " + right_source_storage.register_);
			lines.push_back(sized_save + sum_register + ", (" + destination_address_register + ")");
		} else {  // (!left_source_storage.is_register_direct() && !right_source_storage.is_register_direct()) {
			// (Addition already performed before loading the address.)
			lines.push_back(sized_save + sum_register + ", (" + destination_address_register + ")");
		}
	}

	// Return the output.
	return lines;
}

Semantics::Instruction::NorFrom::NorFrom()
	{}

Semantics::Instruction::NorFrom::NorFrom(const Base &base, bool is_word)
	: Base(base)
	, is_word(is_word)
	{}

std::vector<uint32_t> Semantics::Instruction::NorFrom::get_input_sizes() const { return {static_cast<uint32_t>(is_word ? 4 : 1), static_cast<uint32_t>(is_word ? 4 : 1)}; }
std::vector<uint32_t> Semantics::Instruction::NorFrom::get_working_sizes() const { return {}; }
std::vector<uint32_t> Semantics::Instruction::NorFrom::get_output_sizes() const { return {static_cast<uint32_t>(is_word ? 4 : 1)}; }
std::vector<uint32_t> Semantics::Instruction::NorFrom::get_all_sizes() const { std::vector<uint32_t> v, i(std::move(get_input_sizes())), w(std::move(get_working_sizes())), o(std::move(get_output_sizes())); v.insert(v.end(), i.cbegin(), i.cend()); v.insert(v.end(), w.cbegin(), w.cend()); v.insert(v.end(), o.cbegin(), o.cend()); return v; }

std::vector<Semantics::Output::Line> Semantics::Instruction::NorFrom::emit(const std::vector<Storage> &storages) const {
	// Check sizes.
	if (Storage::get_sizes(storages) != get_all_sizes()) {
		std::ostringstream sstr;
		sstr << "Semantics::Instruction::NorFrom::emit: the number or sizes of storage units provided does not match what was expected.";
		throw SemanticsError(sstr.str());
	}
	const Storage &left_source_storage  = storages[0];
	const Storage &right_source_storage = storages[1];
	const Storage &destination_storage  = storages[2];

	// Prepare output vector.
	std::vector<Output::Line> lines;

	// Emit a symbol for this instruction if there is one.
	if (has_symbol) {
		lines.push_back({":", symbol});
	}

	// If !this->is_word, replace sw with sb and lw with lb.
	//
	// 4 storage types for destination:
	// 	1: global_address + x  (Store into this address.)
	// 	2: x(global_address)   (Store into this dereferenced address.)
	// 	3: $reg                (Store into this register.)
	// 	4: x($reg)             (Store into this dereferenced register.)
	//
	// 4 storage types for a source:
	// 	1: global_address + x  (Read from this address.)
	// 	2: x(global_address)   (Read from this dereferenced address.)
	// 	3: $reg                (Read from this register.)
	// 	4: x($reg)             (Read from this dereferenced register.)

	Output::Line sized_load;
	Output::Line sized_save;

	if (is_word) {
		sized_load = "\tlw   ";
		sized_save = "\tsw   ";
	} else {
		sized_load = "\tlb   ";
		sized_save = "\tsb   ";
	}

	// Part 1: get left source address.
	if           (left_source_storage.is_global_address()) {
		lines.push_back("\tla   $t8, " + left_source_storage.global_address);
		if (left_source_storage.offset != 0) {
			lines.push_back("\tla   $t8, " + std::to_string(left_source_storage.offset) + "($t8)");
		}
	} else if    (left_source_storage.is_global_dereference()) {
		lines.push_back("\tla   $t8, " + left_source_storage.global_address);
		lines.push_back("\tlw   $t8, " + std::to_string(left_source_storage.offset) + "($t8)");
	} else if    (left_source_storage.is_register_direct()) {
	} else {  // (left_source_storage.is_register_dereference()) {
		lines.push_back("\tla   $t8, " + std::to_string(left_source_storage.offset) + "(" + left_source_storage.register_ + ")");
	}

	// Part 2: load left source.
	if (!left_source_storage.is_register_direct()) {
		lines.push_back(sized_load + "$t8, ($t8)");
	}

	// Part 3: get right source address.
	if           (right_source_storage.is_global_address()) {
		lines.push_back("\tla   $t9, " + right_source_storage.global_address);
		if (right_source_storage.offset != 0) {
			lines.push_back("\tla   $t9, " + std::to_string(right_source_storage.offset) + "($t9)");
		}
	} else if    (right_source_storage.is_global_dereference()) {
		lines.push_back("\tla   $t9, " + right_source_storage.global_address);
		lines.push_back("\tlw   $t9, " + std::to_string(right_source_storage.offset) + "($t9)");
	} else if    (right_source_storage.is_register_direct()) {
	} else {  // (right_source_storage.is_register_dereference()) {
		lines.push_back("\tla   $t9, " + std::to_string(right_source_storage.offset) + "(" + right_source_storage.register_ + ")");
	}

	// Part 4: load right source.
	if (!right_source_storage.is_register_direct()) {
		lines.push_back(sized_load + "$t9, ($t9)");
	}

	// Part 5: if both $t8 and $t9 are used, $t8 += $t9, to free $t9 for destination.
	// If $t8 is free but $t9 is used, mark $t8 as being free.
	std::string destination_address_register;
	std::string sum_register;
	if (left_source_storage.is_register_direct()) {
		// $t8 is free.
		destination_address_register = "$t8";
		sum_register                 = "$t9";
	} else {
		// Make sure $t9 is free.
		destination_address_register = "$t9";
		sum_register                 = "$t8";
		if (!(right_source_storage.is_register_direct())) {
			lines.push_back("\tnor  $t8, $t8, $t9");
		}
	}

	// Part 6: get destination address.
	if           (destination_storage.is_global_address()) {
		lines.push_back("\tla   " + destination_address_register + ", " + destination_storage.global_address);
		if (destination_storage.offset != 0) {
			lines.push_back("\tla   " + destination_address_register + ", " + std::to_string(destination_storage.offset) + "(" + destination_address_register + ")");
		}
	} else if    (destination_storage.is_global_dereference()) {
		lines.push_back("\tla   " + destination_address_register + ", " + destination_storage.global_address);
		lines.push_back("\tlw   " + destination_address_register + ", " + std::to_string(destination_storage.offset) + "(" + destination_address_register + ")");
	} else if    (destination_storage.is_register_direct()) {
	} else {  // (destination_storage.is_register_dereference()) {
		lines.push_back("\tla   " + destination_address_register + ", " + std::to_string(destination_storage.offset) + "(" + destination_storage.register_ + ")");
	}

	// Part 7: write destination.
	if (destination_storage.is_register_direct()) {
		if           ( left_source_storage.is_register_direct() &&  right_source_storage.is_register_direct()) {
			lines.push_back("\tnor  " + destination_storage.register_ + ", " + left_source_storage.register_ + ", " + right_source_storage.register_);
		} else if    ( left_source_storage.is_register_direct() && !right_source_storage.is_register_direct()) {
			lines.push_back("\tnor  " + destination_storage.register_ + ", " + left_source_storage.register_ + ", $t9");
		} else if    (!left_source_storage.is_register_direct() &&  right_source_storage.is_register_direct()) {
			lines.push_back("\tnor  " + destination_storage.register_ + ", $t8, " + right_source_storage.register_);
		} else {  // (!left_source_storage.is_register_direct() && !right_source_storage.is_register_direct()) {
			lines.push_back("\tnor  " + destination_storage.register_ + ", $t8, $t9");
		}
	} else {
		if           ( left_source_storage.is_register_direct() &&  right_source_storage.is_register_direct()) {
			lines.push_back("\tnor  " + sum_register + ", " + left_source_storage.register_ + ", " + right_source_storage.register_);
			lines.push_back(sized_save + sum_register + ", (" + destination_address_register + ")");
		} else if    ( left_source_storage.is_register_direct() && !right_source_storage.is_register_direct()) {
			lines.push_back("\tnor  " + sum_register + ", " + left_source_storage.register_ + ", $t9");
			lines.push_back(sized_save + sum_register + ", (" + destination_address_register + ")");
		} else if    (!left_source_storage.is_register_direct() &&  right_source_storage.is_register_direct()) {
			lines.push_back("\tnor  " + sum_register + ", $t8, " + right_source_storage.register_);
			lines.push_back(sized_save + sum_register + ", (" + destination_address_register + ")");
		} else {  // (!left_source_storage.is_register_direct() && !right_source_storage.is_register_direct()) {
			// (NOR already performed before loading the address.)
			lines.push_back(sized_save + sum_register + ", (" + destination_address_register + ")");
		}
	}

	// Return the output.
	return lines;
}

Semantics::Instruction::AndFrom::AndFrom()
	{}

Semantics::Instruction::AndFrom::AndFrom(const Base &base, bool is_word)
	: Base(base)
	, is_word(is_word)
	{}

std::vector<uint32_t> Semantics::Instruction::AndFrom::get_input_sizes() const { return {static_cast<uint32_t>(is_word ? 4 : 1), static_cast<uint32_t>(is_word ? 4 : 1)}; }
std::vector<uint32_t> Semantics::Instruction::AndFrom::get_working_sizes() const { return {}; }
std::vector<uint32_t> Semantics::Instruction::AndFrom::get_output_sizes() const { return {static_cast<uint32_t>(is_word ? 4 : 1)}; }
std::vector<uint32_t> Semantics::Instruction::AndFrom::get_all_sizes() const { std::vector<uint32_t> v, i(std::move(get_input_sizes())), w(std::move(get_working_sizes())), o(std::move(get_output_sizes())); v.insert(v.end(), i.cbegin(), i.cend()); v.insert(v.end(), w.cbegin(), w.cend()); v.insert(v.end(), o.cbegin(), o.cend()); return v; }

std::vector<Semantics::Output::Line> Semantics::Instruction::AndFrom::emit(const std::vector<Storage> &storages) const {
	// Check sizes.
	if (Storage::get_sizes(storages) != get_all_sizes()) {
		std::ostringstream sstr;
		sstr << "Semantics::Instruction::AndFrom::emit: the number or sizes of storage units provided does not match what was expected.";
		throw SemanticsError(sstr.str());
	}
	const Storage &left_source_storage  = storages[0];
	const Storage &right_source_storage = storages[1];
	const Storage &destination_storage  = storages[2];

	// Prepare output vector.
	std::vector<Output::Line> lines;

	// Emit a symbol for this instruction if there is one.
	if (has_symbol) {
		lines.push_back({":", symbol});
	}

	// If !this->is_word, replace sw with sb and lw with lb.
	//
	// 4 storage types for destination:
	// 	1: global_address + x  (Store into this address.)
	// 	2: x(global_address)   (Store into this dereferenced address.)
	// 	3: $reg                (Store into this register.)
	// 	4: x($reg)             (Store into this dereferenced register.)
	//
	// 4 storage types for a source:
	// 	1: global_address + x  (Read from this address.)
	// 	2: x(global_address)   (Read from this dereferenced address.)
	// 	3: $reg                (Read from this register.)
	// 	4: x($reg)             (Read from this dereferenced register.)

	Output::Line sized_load;
	Output::Line sized_save;

	if (is_word) {
		sized_load = "\tlw   ";
		sized_save = "\tsw   ";
	} else {
		sized_load = "\tlb   ";
		sized_save = "\tsb   ";
	}

	// Part 1: get left source address.
	if           (left_source_storage.is_global_address()) {
		lines.push_back("\tla   $t8, " + left_source_storage.global_address);
		if (left_source_storage.offset != 0) {
			lines.push_back("\tla   $t8, " + std::to_string(left_source_storage.offset) + "($t8)");
		}
	} else if    (left_source_storage.is_global_dereference()) {
		lines.push_back("\tla   $t8, " + left_source_storage.global_address);
		lines.push_back("\tlw   $t8, " + std::to_string(left_source_storage.offset) + "($t8)");
	} else if    (left_source_storage.is_register_direct()) {
	} else {  // (left_source_storage.is_register_dereference()) {
		lines.push_back("\tla   $t8, " + std::to_string(left_source_storage.offset) + "(" + left_source_storage.register_ + ")");
	}

	// Part 2: load left source.
	if (!left_source_storage.is_register_direct()) {
		lines.push_back(sized_load + "$t8, ($t8)");
	}

	// Part 3: get right source address.
	if           (right_source_storage.is_global_address()) {
		lines.push_back("\tla   $t9, " + right_source_storage.global_address);
		if (right_source_storage.offset != 0) {
			lines.push_back("\tla   $t9, " + std::to_string(right_source_storage.offset) + "($t9)");
		}
	} else if    (right_source_storage.is_global_dereference()) {
		lines.push_back("\tla   $t9, " + right_source_storage.global_address);
		lines.push_back("\tlw   $t9, " + std::to_string(right_source_storage.offset) + "($t9)");
	} else if    (right_source_storage.is_register_direct()) {
	} else {  // (right_source_storage.is_register_dereference()) {
		lines.push_back("\tla   $t9, " + std::to_string(right_source_storage.offset) + "(" + right_source_storage.register_ + ")");
	}

	// Part 4: load right source.
	if (!right_source_storage.is_register_direct()) {
		lines.push_back(sized_load + "$t9, ($t9)");
	}

	// Part 5: if both $t8 and $t9 are used, $t8 += $t9, to free $t9 for destination.
	// If $t8 is free but $t9 is used, mark $t8 as being free.
	std::string destination_address_register;
	std::string sum_register;
	if (left_source_storage.is_register_direct()) {
		// $t8 is free.
		destination_address_register = "$t8";
		sum_register                 = "$t9";
	} else {
		// Make sure $t9 is free.
		destination_address_register = "$t9";
		sum_register                 = "$t8";
		if (!(right_source_storage.is_register_direct())) {
			lines.push_back("\tand  $t8, $t8, $t9");
		}
	}

	// Part 6: get destination address.
	if           (destination_storage.is_global_address()) {
		lines.push_back("\tla   " + destination_address_register + ", " + destination_storage.global_address);
		if (destination_storage.offset != 0) {
			lines.push_back("\tla   " + destination_address_register + ", " + std::to_string(destination_storage.offset) + "(" + destination_address_register + ")");
		}
	} else if    (destination_storage.is_global_dereference()) {
		lines.push_back("\tla   " + destination_address_register + ", " + destination_storage.global_address);
		lines.push_back("\tlw   " + destination_address_register + ", " + std::to_string(destination_storage.offset) + "(" + destination_address_register + ")");
	} else if    (destination_storage.is_register_direct()) {
	} else {  // (destination_storage.is_register_dereference()) {
		lines.push_back("\tla   " + destination_address_register + ", " + std::to_string(destination_storage.offset) + "(" + destination_storage.register_ + ")");
	}

	// Part 7: write destination.
	if (destination_storage.is_register_direct()) {
		if           ( left_source_storage.is_register_direct() &&  right_source_storage.is_register_direct()) {
			lines.push_back("\tand  " + destination_storage.register_ + ", " + left_source_storage.register_ + ", " + right_source_storage.register_);
		} else if    ( left_source_storage.is_register_direct() && !right_source_storage.is_register_direct()) {
			lines.push_back("\tand  " + destination_storage.register_ + ", " + left_source_storage.register_ + ", $t9");
		} else if    (!left_source_storage.is_register_direct() &&  right_source_storage.is_register_direct()) {
			lines.push_back("\tand  " + destination_storage.register_ + ", $t8, " + right_source_storage.register_);
		} else {  // (!left_source_storage.is_register_direct() && !right_source_storage.is_register_direct()) {
			lines.push_back("\tand  " + destination_storage.register_ + ", $t8, $t9");
		}
	} else {
		if           ( left_source_storage.is_register_direct() &&  right_source_storage.is_register_direct()) {
			lines.push_back("\tand  " + sum_register + ", " + left_source_storage.register_ + ", " + right_source_storage.register_);
			lines.push_back(sized_save + sum_register + ", (" + destination_address_register + ")");
		} else if    ( left_source_storage.is_register_direct() && !right_source_storage.is_register_direct()) {
			lines.push_back("\tand  " + sum_register + ", " + left_source_storage.register_ + ", $t9");
			lines.push_back(sized_save + sum_register + ", (" + destination_address_register + ")");
		} else if    (!left_source_storage.is_register_direct() &&  right_source_storage.is_register_direct()) {
			lines.push_back("\tand  " + sum_register + ", $t8, " + right_source_storage.register_);
			lines.push_back(sized_save + sum_register + ", (" + destination_address_register + ")");
		} else {  // (!left_source_storage.is_register_direct() && !right_source_storage.is_register_direct()) {
			// (AND already performed before loading the address.)
			lines.push_back(sized_save + sum_register + ", (" + destination_address_register + ")");
		}
	}

	// Return the output.
	return lines;
}

Semantics::Instruction::OrFrom::OrFrom()
	{}

Semantics::Instruction::OrFrom::OrFrom(const Base &base, bool is_word)
	: Base(base)
	, is_word(is_word)
	{}

std::vector<uint32_t> Semantics::Instruction::OrFrom::get_input_sizes() const { return {static_cast<uint32_t>(is_word ? 4 : 1), static_cast<uint32_t>(is_word ? 4 : 1)}; }
std::vector<uint32_t> Semantics::Instruction::OrFrom::get_working_sizes() const { return {}; }
std::vector<uint32_t> Semantics::Instruction::OrFrom::get_output_sizes() const { return {static_cast<uint32_t>(is_word ? 4 : 1)}; }
std::vector<uint32_t> Semantics::Instruction::OrFrom::get_all_sizes() const { std::vector<uint32_t> v, i(std::move(get_input_sizes())), w(std::move(get_working_sizes())), o(std::move(get_output_sizes())); v.insert(v.end(), i.cbegin(), i.cend()); v.insert(v.end(), w.cbegin(), w.cend()); v.insert(v.end(), o.cbegin(), o.cend()); return v; }

std::vector<Semantics::Output::Line> Semantics::Instruction::OrFrom::emit(const std::vector<Storage> &storages) const {
	// Check sizes.
	if (Storage::get_sizes(storages) != get_all_sizes()) {
		std::ostringstream sstr;
		sstr << "Semantics::Instruction::OrFrom::emit: the number or sizes of storage units provided does not match what was expected.";
		throw SemanticsError(sstr.str());
	}
	const Storage &left_source_storage  = storages[0];
	const Storage &right_source_storage = storages[1];
	const Storage &destination_storage  = storages[2];

	// Prepare output vector.
	std::vector<Output::Line> lines;

	// Emit a symbol for this instruction if there is one.
	if (has_symbol) {
		lines.push_back({":", symbol});
	}

	// If !this->is_word, replace sw with sb and lw with lb.
	//
	// 4 storage types for destination:
	// 	1: global_address + x  (Store into this address.)
	// 	2: x(global_address)   (Store into this dereferenced address.)
	// 	3: $reg                (Store into this register.)
	// 	4: x($reg)             (Store into this dereferenced register.)
	//
	// 4 storage types for a source:
	// 	1: global_address + x  (Read from this address.)
	// 	2: x(global_address)   (Read from this dereferenced address.)
	// 	3: $reg                (Read from this register.)
	// 	4: x($reg)             (Read from this dereferenced register.)

	Output::Line sized_load;
	Output::Line sized_save;

	if (is_word) {
		sized_load = "\tlw   ";
		sized_save = "\tsw   ";
	} else {
		sized_load = "\tlb   ";
		sized_save = "\tsb   ";
	}

	// Part 1: get left source address.
	if           (left_source_storage.is_global_address()) {
		lines.push_back("\tla   $t8, " + left_source_storage.global_address);
		if (left_source_storage.offset != 0) {
			lines.push_back("\tla   $t8, " + std::to_string(left_source_storage.offset) + "($t8)");
		}
	} else if    (left_source_storage.is_global_dereference()) {
		lines.push_back("\tla   $t8, " + left_source_storage.global_address);
		lines.push_back("\tlw   $t8, " + std::to_string(left_source_storage.offset) + "($t8)");
	} else if    (left_source_storage.is_register_direct()) {
	} else {  // (left_source_storage.is_register_dereference()) {
		lines.push_back("\tla   $t8, " + std::to_string(left_source_storage.offset) + "(" + left_source_storage.register_ + ")");
	}

	// Part 2: load left source.
	if (!left_source_storage.is_register_direct()) {
		lines.push_back(sized_load + "$t8, ($t8)");
	}

	// Part 3: get right source address.
	if           (right_source_storage.is_global_address()) {
		lines.push_back("\tla   $t9, " + right_source_storage.global_address);
		if (right_source_storage.offset != 0) {
			lines.push_back("\tla   $t9, " + std::to_string(right_source_storage.offset) + "($t9)");
		}
	} else if    (right_source_storage.is_global_dereference()) {
		lines.push_back("\tla   $t9, " + right_source_storage.global_address);
		lines.push_back("\tlw   $t9, " + std::to_string(right_source_storage.offset) + "($t9)");
	} else if    (right_source_storage.is_register_direct()) {
	} else {  // (right_source_storage.is_register_dereference()) {
		lines.push_back("\tla   $t9, " + std::to_string(right_source_storage.offset) + "(" + right_source_storage.register_ + ")");
	}

	// Part 4: load right source.
	if (!right_source_storage.is_register_direct()) {
		lines.push_back(sized_load + "$t9, ($t9)");
	}

	// Part 5: if both $t8 and $t9 are used, $t8 += $t9, to free $t9 for destination.
	// If $t8 is free but $t9 is used, mark $t8 as being free.
	std::string destination_address_register;
	std::string sum_register;
	if (left_source_storage.is_register_direct()) {
		// $t8 is free.
		destination_address_register = "$t8";
		sum_register                 = "$t9";
	} else {
		// Make sure $t9 is free.
		destination_address_register = "$t9";
		sum_register                 = "$t8";
		if (!(right_source_storage.is_register_direct())) {
			lines.push_back("\tor   $t8, $t8, $t9");
		}
	}

	// Part 6: get destination address.
	if           (destination_storage.is_global_address()) {
		lines.push_back("\tla   " + destination_address_register + ", " + destination_storage.global_address);
		if (destination_storage.offset != 0) {
			lines.push_back("\tla   " + destination_address_register + ", " + std::to_string(destination_storage.offset) + "(" + destination_address_register + ")");
		}
	} else if    (destination_storage.is_global_dereference()) {
		lines.push_back("\tla   " + destination_address_register + ", " + destination_storage.global_address);
		lines.push_back("\tlw   " + destination_address_register + ", " + std::to_string(destination_storage.offset) + "(" + destination_address_register + ")");
	} else if    (destination_storage.is_register_direct()) {
	} else {  // (destination_storage.is_register_dereference()) {
		lines.push_back("\tla   " + destination_address_register + ", " + std::to_string(destination_storage.offset) + "(" + destination_storage.register_ + ")");
	}

	// Part 7: write destination.
	if (destination_storage.is_register_direct()) {
		if           ( left_source_storage.is_register_direct() &&  right_source_storage.is_register_direct()) {
			lines.push_back("\tor   " + destination_storage.register_ + ", " + left_source_storage.register_ + ", " + right_source_storage.register_);
		} else if    ( left_source_storage.is_register_direct() && !right_source_storage.is_register_direct()) {
			lines.push_back("\tor   " + destination_storage.register_ + ", " + left_source_storage.register_ + ", $t9");
		} else if    (!left_source_storage.is_register_direct() &&  right_source_storage.is_register_direct()) {
			lines.push_back("\tor   " + destination_storage.register_ + ", $t8, " + right_source_storage.register_);
		} else {  // (!left_source_storage.is_register_direct() && !right_source_storage.is_register_direct()) {
			lines.push_back("\tor   " + destination_storage.register_ + ", $t8, $t9");
		}
	} else {
		if           ( left_source_storage.is_register_direct() &&  right_source_storage.is_register_direct()) {
			lines.push_back("\tor   " + sum_register + ", " + left_source_storage.register_ + ", " + right_source_storage.register_);
			lines.push_back(sized_save + sum_register + ", (" + destination_address_register + ")");
		} else if    ( left_source_storage.is_register_direct() && !right_source_storage.is_register_direct()) {
			lines.push_back("\tor   " + sum_register + ", " + left_source_storage.register_ + ", $t9");
			lines.push_back(sized_save + sum_register + ", (" + destination_address_register + ")");
		} else if    (!left_source_storage.is_register_direct() &&  right_source_storage.is_register_direct()) {
			lines.push_back("\tor   " + sum_register + ", $t8, " + right_source_storage.register_);
			lines.push_back(sized_save + sum_register + ", (" + destination_address_register + ")");
		} else {  // (!left_source_storage.is_register_direct() && !right_source_storage.is_register_direct()) {
			// (NOR already performed before loading the address.)
			lines.push_back(sized_save + sum_register + ", (" + destination_address_register + ")");
		}
	}

	// Return the output.
	return lines;
}

Semantics::Instruction::AddFrom::AddFrom()
	{}

Semantics::Instruction::AddFrom::AddFrom(const Base &base, bool is_word)
	: Base(base)
	, is_word(is_word)
	{}

std::vector<uint32_t> Semantics::Instruction::AddFrom::get_input_sizes() const { return {static_cast<uint32_t>(is_word ? 4 : 1), static_cast<uint32_t>(is_word ? 4 : 1)}; }
std::vector<uint32_t> Semantics::Instruction::AddFrom::get_working_sizes() const { return {}; }
std::vector<uint32_t> Semantics::Instruction::AddFrom::get_output_sizes() const { return {static_cast<uint32_t>(is_word ? 4 : 1)}; }
std::vector<uint32_t> Semantics::Instruction::AddFrom::get_all_sizes() const { std::vector<uint32_t> v, i(std::move(get_input_sizes())), w(std::move(get_working_sizes())), o(std::move(get_output_sizes())); v.insert(v.end(), i.cbegin(), i.cend()); v.insert(v.end(), w.cbegin(), w.cend()); v.insert(v.end(), o.cbegin(), o.cend()); return v; }

std::vector<Semantics::Output::Line> Semantics::Instruction::AddFrom::emit(const std::vector<Storage> &storages) const {
	// Check sizes.
	if (Storage::get_sizes(storages) != get_all_sizes()) {
		std::ostringstream sstr;
		sstr << "Semantics::Instruction::AddFrom::emit: the number or sizes of storage units provided does not match what was expected.";
		throw SemanticsError(sstr.str());
	}
	const Storage &left_source_storage  = storages[0];
	const Storage &right_source_storage = storages[1];
	const Storage &destination_storage  = storages[2];

	// Prepare output vector.
	std::vector<Output::Line> lines;

	// Emit a symbol for this instruction if there is one.
	if (has_symbol) {
		lines.push_back({":", symbol});
	}

	// If !this->is_word, replace sw with sb and lw with lb.
	//
	// 4 storage types for destination:
	// 	1: global_address + x  (Store into this address.)
	// 	2: x(global_address)   (Store into this dereferenced address.)
	// 	3: $reg                (Store into this register.)
	// 	4: x($reg)             (Store into this dereferenced register.)
	//
	// 4 storage types for a source:
	// 	1: global_address + x  (Read from this address.)
	// 	2: x(global_address)   (Read from this dereferenced address.)
	// 	3: $reg                (Read from this register.)
	// 	4: x($reg)             (Read from this dereferenced register.)

	Output::Line sized_load;
	Output::Line sized_save;

	if (is_word) {
		sized_load = "\tlw   ";
		sized_save = "\tsw   ";
	} else {
		sized_load = "\tlb   ";
		sized_save = "\tsb   ";
	}

	// Part 1: get left source address.
	if           (left_source_storage.is_global_address()) {
		lines.push_back("\tla   $t8, " + left_source_storage.global_address);
		if (left_source_storage.offset != 0) {
			lines.push_back("\tla   $t8, " + std::to_string(left_source_storage.offset) + "($t8)");
		}
	} else if    (left_source_storage.is_global_dereference()) {
		lines.push_back("\tla   $t8, " + left_source_storage.global_address);
		lines.push_back("\tlw   $t8, " + std::to_string(left_source_storage.offset) + "($t8)");
	} else if    (left_source_storage.is_register_direct()) {
	} else {  // (left_source_storage.is_register_dereference()) {
		lines.push_back("\tla   $t8, " + std::to_string(left_source_storage.offset) + "(" + left_source_storage.register_ + ")");
	}

	// Part 2: load left source.
	if (!left_source_storage.is_register_direct()) {
		lines.push_back(sized_load + "$t8, ($t8)");
	}

	// Part 3: get right source address.
	if           (right_source_storage.is_global_address()) {
		lines.push_back("\tla   $t9, " + right_source_storage.global_address);
		if (right_source_storage.offset != 0) {
			lines.push_back("\tla   $t9, " + std::to_string(right_source_storage.offset) + "($t9)");
		}
	} else if    (right_source_storage.is_global_dereference()) {
		lines.push_back("\tla   $t9, " + right_source_storage.global_address);
		lines.push_back("\tlw   $t9, " + std::to_string(right_source_storage.offset) + "($t9)");
	} else if    (right_source_storage.is_register_direct()) {
	} else {  // (right_source_storage.is_register_dereference()) {
		lines.push_back("\tla   $t9, " + std::to_string(right_source_storage.offset) + "(" + right_source_storage.register_ + ")");
	}

	// Part 4: load right source.
	if (!right_source_storage.is_register_direct()) {
		lines.push_back(sized_load + "$t9, ($t9)");
	}

	// Part 5: if both $t8 and $t9 are used, $t8 += $t9, to free $t9 for destination.
	// If $t8 is free but $t9 is used, mark $t8 as being free.
	std::string destination_address_register;
	std::string sum_register;
	if (left_source_storage.is_register_direct()) {
		// $t8 is free.
		destination_address_register = "$t8";
		sum_register                 = "$t9";
	} else {
		// Make sure $t9 is free.
		destination_address_register = "$t9";
		sum_register                 = "$t8";
		if (!(right_source_storage.is_register_direct())) {
			lines.push_back("\taddu $t8, $t8, $t9");
		}
	}

	// Part 6: get destination address.
	if           (destination_storage.is_global_address()) {
		lines.push_back("\tla   " + destination_address_register + ", " + destination_storage.global_address);
		if (destination_storage.offset != 0) {
			lines.push_back("\tla   " + destination_address_register + ", " + std::to_string(destination_storage.offset) + "(" + destination_address_register + ")");
		}
	} else if    (destination_storage.is_global_dereference()) {
		lines.push_back("\tla   " + destination_address_register + ", " + destination_storage.global_address);
		lines.push_back("\tlw   " + destination_address_register + ", " + std::to_string(destination_storage.offset) + "(" + destination_address_register + ")");
	} else if    (destination_storage.is_register_direct()) {
	} else {  // (destination_storage.is_register_dereference()) {
		lines.push_back("\tla   " + destination_address_register + ", " + std::to_string(destination_storage.offset) + "(" + destination_storage.register_ + ")");
	}

	// Part 7: write destination.
	if (destination_storage.is_register_direct()) {
		if           ( left_source_storage.is_register_direct() &&  right_source_storage.is_register_direct()) {
			lines.push_back("\taddu " + destination_storage.register_ + ", " + left_source_storage.register_ + ", " + right_source_storage.register_);
		} else if    ( left_source_storage.is_register_direct() && !right_source_storage.is_register_direct()) {
			lines.push_back("\taddu " + destination_storage.register_ + ", " + left_source_storage.register_ + ", $t9");
		} else if    (!left_source_storage.is_register_direct() &&  right_source_storage.is_register_direct()) {
			lines.push_back("\taddu " + destination_storage.register_ + ", $t8, " + right_source_storage.register_);
		} else {  // (!left_source_storage.is_register_direct() && !right_source_storage.is_register_direct()) {
			lines.push_back("\taddu " + destination_storage.register_ + ", $t8, $t9");
		}
	} else {
		if           ( left_source_storage.is_register_direct() &&  right_source_storage.is_register_direct()) {
			lines.push_back("\taddu " + sum_register + ", " + left_source_storage.register_ + ", " + right_source_storage.register_);
			lines.push_back(sized_save + sum_register + ", (" + destination_address_register + ")");
		} else if    ( left_source_storage.is_register_direct() && !right_source_storage.is_register_direct()) {
			lines.push_back("\taddu " + sum_register + ", " + left_source_storage.register_ + ", $t9");
			lines.push_back(sized_save + sum_register + ", (" + destination_address_register + ")");
		} else if    (!left_source_storage.is_register_direct() &&  right_source_storage.is_register_direct()) {
			lines.push_back("\taddu " + sum_register + ", $t8, " + right_source_storage.register_);
			lines.push_back(sized_save + sum_register + ", (" + destination_address_register + ")");
		} else {  // (!left_source_storage.is_register_direct() && !right_source_storage.is_register_direct()) {
			// (Addition already performed before loading the address.)
			lines.push_back(sized_save + sum_register + ", (" + destination_address_register + ")");
		}
	}

	// Return the output.
	return lines;
}

Semantics::Instruction::SubFrom::SubFrom()
	{}

Semantics::Instruction::SubFrom::SubFrom(const Base &base, bool is_word)
	: Base(base)
	, is_word(is_word)
	{}

std::vector<uint32_t> Semantics::Instruction::SubFrom::get_input_sizes() const { return {static_cast<uint32_t>(is_word ? 4 : 1), static_cast<uint32_t>(is_word ? 4 : 1)}; }
std::vector<uint32_t> Semantics::Instruction::SubFrom::get_working_sizes() const { return {}; }
std::vector<uint32_t> Semantics::Instruction::SubFrom::get_output_sizes() const { return {static_cast<uint32_t>(is_word ? 4 : 1)}; }
std::vector<uint32_t> Semantics::Instruction::SubFrom::get_all_sizes() const { std::vector<uint32_t> v, i(std::move(get_input_sizes())), w(std::move(get_working_sizes())), o(std::move(get_output_sizes())); v.insert(v.end(), i.cbegin(), i.cend()); v.insert(v.end(), w.cbegin(), w.cend()); v.insert(v.end(), o.cbegin(), o.cend()); return v; }

std::vector<Semantics::Output::Line> Semantics::Instruction::SubFrom::emit(const std::vector<Storage> &storages) const {
	// Check sizes.
	if (Storage::get_sizes(storages) != get_all_sizes()) {
		std::ostringstream sstr;
		sstr << "Semantics::Instruction::SubFrom::emit: the number or sizes of storage units provided does not match what was expected.";
		throw SemanticsError(sstr.str());
	}
	const Storage &left_source_storage  = storages[0];
	const Storage &right_source_storage = storages[1];
	const Storage &destination_storage  = storages[2];

	// Prepare output vector.
	std::vector<Output::Line> lines;

	// Emit a symbol for this instruction if there is one.
	if (has_symbol) {
		lines.push_back({":", symbol});
	}

	// If !this->is_word, replace sw with sb and lw with lb.
	//
	// 4 storage types for destination:
	// 	1: global_address + x  (Store into this address.)
	// 	2: x(global_address)   (Store into this dereferenced address.)
	// 	3: $reg                (Store into this register.)
	// 	4: x($reg)             (Store into this dereferenced register.)
	//
	// 4 storage types for a source:
	// 	1: global_address + x  (Read from this address.)
	// 	2: x(global_address)   (Read from this dereferenced address.)
	// 	3: $reg                (Read from this register.)
	// 	4: x($reg)             (Read from this dereferenced register.)

	Output::Line sized_load;
	Output::Line sized_save;

	if (is_word) {
		sized_load = "\tlw   ";
		sized_save = "\tsw   ";
	} else {
		sized_load = "\tlb   ";
		sized_save = "\tsb   ";
	}

	// Part 1: get left source address.
	if           (left_source_storage.is_global_address()) {
		lines.push_back("\tla   $t8, " + left_source_storage.global_address);
		if (left_source_storage.offset != 0) {
			lines.push_back("\tla   $t8, " + std::to_string(left_source_storage.offset) + "($t8)");
		}
	} else if    (left_source_storage.is_global_dereference()) {
		lines.push_back("\tla   $t8, " + left_source_storage.global_address);
		lines.push_back("\tlw   $t8, " + std::to_string(left_source_storage.offset) + "($t8)");
	} else if    (left_source_storage.is_register_direct()) {
	} else {  // (left_source_storage.is_register_dereference()) {
		lines.push_back("\tla   $t8, " + std::to_string(left_source_storage.offset) + "(" + left_source_storage.register_ + ")");
	}

	// Part 2: load left source.
	if (!left_source_storage.is_register_direct()) {
		lines.push_back(sized_load + "$t8, ($t8)");
	}

	// Part 3: get right source address.
	if           (right_source_storage.is_global_address()) {
		lines.push_back("\tla   $t9, " + right_source_storage.global_address);
		if (right_source_storage.offset != 0) {
			lines.push_back("\tla   $t9, " + std::to_string(right_source_storage.offset) + "($t9)");
		}
	} else if    (right_source_storage.is_global_dereference()) {
		lines.push_back("\tla   $t9, " + right_source_storage.global_address);
		lines.push_back("\tlw   $t9, " + std::to_string(right_source_storage.offset) + "($t9)");
	} else if    (right_source_storage.is_register_direct()) {
	} else {  // (right_source_storage.is_register_dereference()) {
		lines.push_back("\tla   $t9, " + std::to_string(right_source_storage.offset) + "(" + right_source_storage.register_ + ")");
	}

	// Part 4: load right source.
	if (!right_source_storage.is_register_direct()) {
		lines.push_back(sized_load + "$t9, ($t9)");
	}

	// Part 5: if both $t8 and $t9 are used, $t8 += $t9, to free $t9 for destination.
	// If $t8 is free but $t9 is used, mark $t8 as being free.
	std::string destination_address_register;
	std::string sum_register;
	if (left_source_storage.is_register_direct()) {
		// $t8 is free.
		destination_address_register = "$t8";
		sum_register                 = "$t9";
	} else {
		// Make sure $t9 is free.
		destination_address_register = "$t9";
		sum_register                 = "$t8";
		if (!(right_source_storage.is_register_direct())) {
			lines.push_back("\tsubu $t8, $t8, $t9");
		}
	}

	// Part 6: get destination address.
	if           (destination_storage.is_global_address()) {
		lines.push_back("\tla   " + destination_address_register + ", " + destination_storage.global_address);
		if (destination_storage.offset != 0) {
			lines.push_back("\tla   " + destination_address_register + ", " + std::to_string(destination_storage.offset) + "(" + destination_address_register + ")");
		}
	} else if    (destination_storage.is_global_dereference()) {
		lines.push_back("\tla   " + destination_address_register + ", " + destination_storage.global_address);
		lines.push_back("\tlw   " + destination_address_register + ", " + std::to_string(destination_storage.offset) + "(" + destination_address_register + ")");
	} else if    (destination_storage.is_register_direct()) {
	} else {  // (destination_storage.is_register_dereference()) {
		lines.push_back("\tla   " + destination_address_register + ", " + std::to_string(destination_storage.offset) + "(" + destination_storage.register_ + ")");
	}

	// Part 7: write destination.
	if (destination_storage.is_register_direct()) {
		if           ( left_source_storage.is_register_direct() &&  right_source_storage.is_register_direct()) {
			lines.push_back("\tsubu " + destination_storage.register_ + ", " + left_source_storage.register_ + ", " + right_source_storage.register_);
		} else if    ( left_source_storage.is_register_direct() && !right_source_storage.is_register_direct()) {
			lines.push_back("\tsubu " + destination_storage.register_ + ", " + left_source_storage.register_ + ", $t9");
		} else if    (!left_source_storage.is_register_direct() &&  right_source_storage.is_register_direct()) {
			lines.push_back("\tsubu " + destination_storage.register_ + ", $t8, " + right_source_storage.register_);
		} else {  // (!left_source_storage.is_register_direct() && !right_source_storage.is_register_direct()) {
			lines.push_back("\tsubu " + destination_storage.register_ + ", $t8, $t9");
		}
	} else {
		if           ( left_source_storage.is_register_direct() &&  right_source_storage.is_register_direct()) {
			lines.push_back("\tsubu " + sum_register + ", " + left_source_storage.register_ + ", " + right_source_storage.register_);
			lines.push_back(sized_save + sum_register + ", (" + destination_address_register + ")");
		} else if    ( left_source_storage.is_register_direct() && !right_source_storage.is_register_direct()) {
			lines.push_back("\tsubu " + sum_register + ", " + left_source_storage.register_ + ", $t9");
			lines.push_back(sized_save + sum_register + ", (" + destination_address_register + ")");
		} else if    (!left_source_storage.is_register_direct() &&  right_source_storage.is_register_direct()) {
			lines.push_back("\tsubu " + sum_register + ", $t8, " + right_source_storage.register_);
			lines.push_back(sized_save + sum_register + ", (" + destination_address_register + ")");
		} else {  // (!left_source_storage.is_register_direct() && !right_source_storage.is_register_direct()) {
			// (Subtraction already performed before loading the address.)
			lines.push_back(sized_save + sum_register + ", (" + destination_address_register + ")");
		}
	}

	// Return the output.
	return lines;
}

Semantics::Instruction::MultFrom::MultFrom()
	{}

Semantics::Instruction::MultFrom::MultFrom(const Base &base, bool is_word)
	: Base(base)
	, is_word(is_word)
	{}

std::vector<uint32_t> Semantics::Instruction::MultFrom::get_input_sizes() const { return {static_cast<uint32_t>(is_word ? 4 : 1), static_cast<uint32_t>(is_word ? 4 : 1)}; }
std::vector<uint32_t> Semantics::Instruction::MultFrom::get_working_sizes() const { return {}; }
std::vector<uint32_t> Semantics::Instruction::MultFrom::get_output_sizes() const { return {static_cast<uint32_t>(is_word ? 4 : 1), static_cast<uint32_t>(is_word ? 4 : 1)}; }
std::vector<uint32_t> Semantics::Instruction::MultFrom::get_all_sizes() const { std::vector<uint32_t> v, i(std::move(get_input_sizes())), w(std::move(get_working_sizes())), o(std::move(get_output_sizes())); v.insert(v.end(), i.cbegin(), i.cend()); v.insert(v.end(), w.cbegin(), w.cend()); v.insert(v.end(), o.cbegin(), o.cend()); return v; }

std::vector<Semantics::Output::Line> Semantics::Instruction::MultFrom::emit(const std::vector<Storage> &storages) const {
	// Check sizes.
	if (Storage::get_sizes(storages) != get_all_sizes()) {
		std::ostringstream sstr;
		sstr << "Semantics::Instruction::MultFrom::emit: the number or sizes of storage units provided does not match what was expected.";
		throw SemanticsError(sstr.str());
	}
	const Storage &left_source_storage       = storages[0];
	const Storage &right_source_storage      = storages[1];
	const Storage &left_destination_storage  = storages[2];
	const Storage &right_destination_storage = storages[2];

	// Prepare output vector.
	std::vector<Output::Line> lines;

	// Emit a symbol for this instruction if there is one.
	if (has_symbol) {
		lines.push_back({":", symbol});
	}

	// If !this->is_word, replace sw with sb and lw with lb.
	//
	// 4 storage types for destination:
	// 	1: global_address + x  (Store into this address.)
	// 	2: x(global_address)   (Store into this dereferenced address.)
	// 	3: $reg                (Store into this register.)
	// 	4: x($reg)             (Store into this dereferenced register.)
	//
	// 4 storage types for a source:
	// 	1: global_address + x  (Read from this address.)
	// 	2: x(global_address)   (Read from this dereferenced address.)
	// 	3: $reg                (Read from this register.)
	// 	4: x($reg)             (Read from this dereferenced register.)

	Output::Line sized_load;
	Output::Line sized_save;

	if (is_word) {
		sized_load = "\tlw   ";
		sized_save = "\tsw   ";
	} else {
		sized_load = "\tlb   ";
		sized_save = "\tsb   ";
	}

	// Part 1: get left source address.
	if           (left_source_storage.is_global_address()) {
		lines.push_back("\tla   $t8, " + left_source_storage.global_address);
		if (left_source_storage.offset != 0) {
			lines.push_back("\tla   $t8, " + std::to_string(left_source_storage.offset) + "($t8)");
		}
	} else if    (left_source_storage.is_global_dereference()) {
		lines.push_back("\tla   $t8, " + left_source_storage.global_address);
		lines.push_back("\tlw   $t8, " + std::to_string(left_source_storage.offset) + "($t8)");
	} else if    (left_source_storage.is_register_direct()) {
	} else {  // (left_source_storage.is_register_dereference()) {
		lines.push_back("\tla   $t8, " + std::to_string(left_source_storage.offset) + "(" + left_source_storage.register_ + ")");
	}

	// Part 2: load left source.
	if (!left_source_storage.is_register_direct()) {
		lines.push_back(sized_load + "$t8, ($t8)");
	}

	// Part 3: get right source address.
	if           (right_source_storage.is_global_address()) {
		lines.push_back("\tla   $t9, " + right_source_storage.global_address);
		if (right_source_storage.offset != 0) {
			lines.push_back("\tla   $t9, " + std::to_string(right_source_storage.offset) + "($t9)");
		}
	} else if    (right_source_storage.is_global_dereference()) {
		lines.push_back("\tla   $t9, " + right_source_storage.global_address);
		lines.push_back("\tlw   $t9, " + std::to_string(right_source_storage.offset) + "($t9)");
	} else if    (right_source_storage.is_register_direct()) {
	} else {  // (right_source_storage.is_register_dereference()) {
		lines.push_back("\tla   $t9, " + std::to_string(right_source_storage.offset) + "(" + right_source_storage.register_ + ")");
	}

	// Part 4: load right source.
	if (!right_source_storage.is_register_direct()) {
		lines.push_back(sized_load + "$t9, ($t9)");
	}

	// Part 5: perform multiplication.
	std::string left_source_register;
	std::string right_source_register;
	if (left_source_storage.is_register_direct()) {
		left_source_register = left_source_storage.register_;
	} else {
		left_source_register = "$t8";
	}
	if (right_source_storage.is_register_direct()) {
		right_source_register = right_source_storage.register_;
	} else {
		right_source_register = "$t9";
	}
	lines.push_back("\tmult " + left_source_register + ", " + right_source_register);

	// Part 6: get left destination address.
	if           (left_destination_storage.is_global_address()) {
		lines.push_back("\tla   $t8, " + left_destination_storage.global_address);
		if (left_destination_storage.offset != 0) {
			lines.push_back("\tla   $t8, " + std::to_string(left_destination_storage.offset) + "($t8)");
		}
	} else if    (left_destination_storage.is_global_dereference()) {
		lines.push_back("\tla   $t8, " + left_destination_storage.global_address);
		lines.push_back("\tlw   $t8, " + std::to_string(left_destination_storage.offset) + "($t8)");
	} else if    (left_destination_storage.is_register_direct()) {
	} else {  // (left_destination_storage.is_register_dereference()) {
		lines.push_back("\tla   $t8, " + std::to_string(left_destination_storage.offset) + "(" + left_destination_storage.register_ + ")");
	}

	// Part 7: save left destination.
	if (!left_destination_storage.is_register_direct()) {
		lines.push_back("\tmflo $t9");
		lines.push_back(sized_save + "$t9, ($t8)");
	} else {
		lines.push_back("\tmflo " + left_destination_storage.register_);
	}

	// Part 8: get right destination address.
	if           (right_destination_storage.is_global_address()) {
		lines.push_back("\tla   $t9, " + right_destination_storage.global_address);
		if (right_destination_storage.offset != 0) {
			lines.push_back("\tla   $t9, " + std::to_string(right_destination_storage.offset) + "($t9)");
		}
	} else if    (right_destination_storage.is_global_dereference()) {
		lines.push_back("\tla   $t9, " + right_destination_storage.global_address);
		lines.push_back("\tlw   $t9, " + std::to_string(right_destination_storage.offset) + "($t9)");
	} else if    (right_destination_storage.is_register_direct()) {
	} else {  // (right_destination_storage.is_register_dereference()) {
		lines.push_back("\tla   $t9, " + std::to_string(right_destination_storage.offset) + "(" + right_destination_storage.register_ + ")");
	}

	// Part 9: save right destination.
	if (!right_destination_storage.is_register_direct()) {
		lines.push_back("\tmfhi $t8");
		lines.push_back(sized_save + "$t8, ($t9)");
	} else {
		lines.push_back("\tmfhi " + right_destination_storage.register_);
	}

	// Return the output.
	return lines;
}

Semantics::Instruction::DivFrom::DivFrom()
	{}

Semantics::Instruction::DivFrom::DivFrom(const Base &base, bool is_word)
	: Base(base)
	, is_word(is_word)
	{}

std::vector<uint32_t> Semantics::Instruction::DivFrom::get_input_sizes() const { return {static_cast<uint32_t>(is_word ? 4 : 1), static_cast<uint32_t>(is_word ? 4 : 1)}; }
std::vector<uint32_t> Semantics::Instruction::DivFrom::get_working_sizes() const { return {}; }
std::vector<uint32_t> Semantics::Instruction::DivFrom::get_output_sizes() const { return {static_cast<uint32_t>(is_word ? 4 : 1), static_cast<uint32_t>(is_word ? 4 : 1)}; }
std::vector<uint32_t> Semantics::Instruction::DivFrom::get_all_sizes() const { std::vector<uint32_t> v, i(std::move(get_input_sizes())), w(std::move(get_working_sizes())), o(std::move(get_output_sizes())); v.insert(v.end(), i.cbegin(), i.cend()); v.insert(v.end(), w.cbegin(), w.cend()); v.insert(v.end(), o.cbegin(), o.cend()); return v; }

std::vector<Semantics::Output::Line> Semantics::Instruction::DivFrom::emit(const std::vector<Storage> &storages) const {
	// Check sizes.
	if (Storage::get_sizes(storages) != get_all_sizes()) {
		std::ostringstream sstr;
		sstr << "Semantics::Instruction::DivFrom::emit: the number or sizes of storage units provided does not match what was expected.";
		throw SemanticsError(sstr.str());
	}
	const Storage &left_source_storage       = storages[0];
	const Storage &right_source_storage      = storages[1];
	const Storage &left_destination_storage  = storages[2];
	const Storage &right_destination_storage = storages[2];

	// Prepare output vector.
	std::vector<Output::Line> lines;

	// Emit a symbol for this instruction if there is one.
	if (has_symbol) {
		lines.push_back({":", symbol});
	}

	// If !this->is_word, replace sw with sb and lw with lb.
	//
	// 4 storage types for destination:
	// 	1: global_address + x  (Store into this address.)
	// 	2: x(global_address)   (Store into this dereferenced address.)
	// 	3: $reg                (Store into this register.)
	// 	4: x($reg)             (Store into this dereferenced register.)
	//
	// 4 storage types for a source:
	// 	1: global_address + x  (Read from this address.)
	// 	2: x(global_address)   (Read from this dereferenced address.)
	// 	3: $reg                (Read from this register.)
	// 	4: x($reg)             (Read from this dereferenced register.)

	Output::Line sized_load;
	Output::Line sized_save;

	if (is_word) {
		sized_load = "\tlw   ";
		sized_save = "\tsw   ";
	} else {
		sized_load = "\tlb   ";
		sized_save = "\tsb   ";
	}

	// Part 1: get left source address.
	if           (left_source_storage.is_global_address()) {
		lines.push_back("\tla   $t8, " + left_source_storage.global_address);
		if (left_source_storage.offset != 0) {
			lines.push_back("\tla   $t8, " + std::to_string(left_source_storage.offset) + "($t8)");
		}
	} else if    (left_source_storage.is_global_dereference()) {
		lines.push_back("\tla   $t8, " + left_source_storage.global_address);
		lines.push_back("\tlw   $t8, " + std::to_string(left_source_storage.offset) + "($t8)");
	} else if    (left_source_storage.is_register_direct()) {
	} else {  // (left_source_storage.is_register_dereference()) {
		lines.push_back("\tla   $t8, " + std::to_string(left_source_storage.offset) + "(" + left_source_storage.register_ + ")");
	}

	// Part 2: load left source.
	if (!left_source_storage.is_register_direct()) {
		lines.push_back(sized_load + "$t8, ($t8)");
	}

	// Part 3: get right source address.
	if           (right_source_storage.is_global_address()) {
		lines.push_back("\tla   $t9, " + right_source_storage.global_address);
		if (right_source_storage.offset != 0) {
			lines.push_back("\tla   $t9, " + std::to_string(right_source_storage.offset) + "($t9)");
		}
	} else if    (right_source_storage.is_global_dereference()) {
		lines.push_back("\tla   $t9, " + right_source_storage.global_address);
		lines.push_back("\tlw   $t9, " + std::to_string(right_source_storage.offset) + "($t9)");
	} else if    (right_source_storage.is_register_direct()) {
	} else {  // (right_source_storage.is_register_dereference()) {
		lines.push_back("\tla   $t9, " + std::to_string(right_source_storage.offset) + "(" + right_source_storage.register_ + ")");
	}

	// Part 4: load right source.
	if (!right_source_storage.is_register_direct()) {
		lines.push_back(sized_load + "$t9, ($t9)");
	}

	// Part 5: perform division.
	std::string left_source_register;
	std::string right_source_register;
	if (left_source_storage.is_register_direct()) {
		left_source_register = left_source_storage.register_;
	} else {
		left_source_register = "$t8";
	}
	if (right_source_storage.is_register_direct()) {
		right_source_register = right_source_storage.register_;
	} else {
		right_source_register = "$t9";
	}
	lines.push_back("\tdiv  " + left_source_register + ", " + right_source_register);

	// Part 6: get left destination address.
	if           (left_destination_storage.is_global_address()) {
		lines.push_back("\tla   $t8, " + left_destination_storage.global_address);
		if (left_destination_storage.offset != 0) {
			lines.push_back("\tla   $t8, " + std::to_string(left_destination_storage.offset) + "($t8)");
		}
	} else if    (left_destination_storage.is_global_dereference()) {
		lines.push_back("\tla   $t8, " + left_destination_storage.global_address);
		lines.push_back("\tlw   $t8, " + std::to_string(left_destination_storage.offset) + "($t8)");
	} else if    (left_destination_storage.is_register_direct()) {
	} else {  // (left_destination_storage.is_register_dereference()) {
		lines.push_back("\tla   $t8, " + std::to_string(left_destination_storage.offset) + "(" + left_destination_storage.register_ + ")");
	}

	// Part 7: save left destination.
	if (!left_destination_storage.is_register_direct()) {
		lines.push_back("\tmflo $t9");
		lines.push_back(sized_save + "$t9, ($t8)");
	} else {
		lines.push_back("\tmflo " + left_destination_storage.register_);
	}

	// Part 8: get right destination address.
	if           (right_destination_storage.is_global_address()) {
		lines.push_back("\tla   $t9, " + right_destination_storage.global_address);
		if (right_destination_storage.offset != 0) {
			lines.push_back("\tla   $t9, " + std::to_string(right_destination_storage.offset) + "($t9)");
		}
	} else if    (right_destination_storage.is_global_dereference()) {
		lines.push_back("\tla   $t9, " + right_destination_storage.global_address);
		lines.push_back("\tlw   $t9, " + std::to_string(right_destination_storage.offset) + "($t9)");
	} else if    (right_destination_storage.is_register_direct()) {
	} else {  // (right_destination_storage.is_register_dereference()) {
		lines.push_back("\tla   $t9, " + std::to_string(right_destination_storage.offset) + "(" + right_destination_storage.register_ + ")");
	}

	// Part 9: save right destination.
	if (!right_destination_storage.is_register_direct()) {
		lines.push_back("\tmfhi $t8");
		lines.push_back(sized_save + "$t8, ($t9)");
	} else {
		lines.push_back("\tmfhi " + right_destination_storage.register_);
	}

	// Return the output.
	return lines;
}

Semantics::Instruction::JumpTo::JumpTo()
	{}

Semantics::Instruction::JumpTo::JumpTo(const Base &base)
	: Base(base)
	{}

std::vector<uint32_t> Semantics::Instruction::JumpTo::get_input_sizes() const { return {4}; }
std::vector<uint32_t> Semantics::Instruction::JumpTo::get_working_sizes() const { return {}; }
std::vector<uint32_t> Semantics::Instruction::JumpTo::get_output_sizes() const { return {}; }
std::vector<uint32_t> Semantics::Instruction::JumpTo::get_all_sizes() const { std::vector<uint32_t> v, i(std::move(get_input_sizes())), w(std::move(get_working_sizes())), o(std::move(get_output_sizes())); v.insert(v.end(), i.cbegin(), i.cend()); v.insert(v.end(), w.cbegin(), w.cend()); v.insert(v.end(), o.cbegin(), o.cend()); return v; }

std::vector<Semantics::Output::Line> Semantics::Instruction::JumpTo::emit(const std::vector<Storage> &storages) const {
	// Check sizes.
	if (Storage::get_sizes(storages) != get_all_sizes()) {
		std::ostringstream sstr;
		sstr << "Semantics::Instruction::JumpTo::emit: the number or sizes of storage units provided does not match what was expected.";
		throw SemanticsError(sstr.str());
	}
	const Storage &source_storage = storages[0];

	// Prepare output vector.
	std::vector<Output::Line> lines;

	// Emit a symbol for this instruction if there is one.
	if (has_symbol) {
		lines.push_back({":", symbol});
	}

	// Part 1: get source address.
	if           (source_storage.is_global_address()) {
		lines.push_back("\tla   $t8, " + source_storage.global_address);
		if (source_storage.offset != 0) {
			lines.push_back("\tla   $t8, " + std::to_string(source_storage.offset) + "($t8)");
		}
	} else if    (source_storage.is_global_dereference()) {
		lines.push_back("\tla   $t8, " + source_storage.global_address);
		lines.push_back("\tlw   $t8, " + std::to_string(source_storage.offset) + "($t8)");
	} else if    (source_storage.is_register_direct()) {
	} else {  // (source_storage.is_register_dereference()) {
		lines.push_back("\tla   $t8, " + std::to_string(source_storage.offset) + "(" + source_storage.register_ + ")");
	}

	// Part 2: load source (4-bytes).
	if (!source_storage.is_register_direct()) {
		lines.push_back("\tlw   $t8, ($t8)");
	}

	// Part 3: jump.
	if (!source_storage.is_register_direct()) {
		lines.push_back("\tjr   $t8");
	} else {
		lines.push_back("\tjr   " + source_storage.register_);
	}

	// Return the output.
	return lines;
}

Semantics::Instruction::Jump::Jump()
	{}

Semantics::Instruction::Jump::Jump(const Base &base, Symbol jump_destination)
	: Base(base)
	, jump_destination(jump_destination)
	{}

std::vector<uint32_t> Semantics::Instruction::Jump::get_input_sizes() const { return {}; }
std::vector<uint32_t> Semantics::Instruction::Jump::get_working_sizes() const { return {}; }
std::vector<uint32_t> Semantics::Instruction::Jump::get_output_sizes() const { return {}; }
std::vector<uint32_t> Semantics::Instruction::Jump::get_all_sizes() const { std::vector<uint32_t> v, i(std::move(get_input_sizes())), w(std::move(get_working_sizes())), o(std::move(get_output_sizes())); v.insert(v.end(), i.cbegin(), i.cend()); v.insert(v.end(), w.cbegin(), w.cend()); v.insert(v.end(), o.cbegin(), o.cend()); return v; }

std::vector<Semantics::Output::Line> Semantics::Instruction::Jump::emit(const std::vector<Storage> &storages) const {
	// Check sizes.
	if (Storage::get_sizes(storages) != get_all_sizes()) {
		std::ostringstream sstr;
		sstr << "Semantics::Instruction::Jump::emit: the number or sizes of storage units provided does not match what was expected.";
		throw SemanticsError(sstr.str());
	}

	// Prepare output vector.
	std::vector<Output::Line> lines;

	// Emit a symbol for this instruction if there is one.
	if (has_symbol) {
		lines.push_back({":", symbol});
	}

	// Part 1: jump.
	lines.push_back("\tj    " + jump_destination);

	// Return the output.
	return lines;
}

Semantics::Instruction::Call::Call()
	{}

Semantics::Instruction::Call::Call(const Base &base, Symbol jump_destination)
	: Base(base)
	, jump_destination(jump_destination)
	{}

std::vector<uint32_t> Semantics::Instruction::Call::get_input_sizes() const { return {}; }
std::vector<uint32_t> Semantics::Instruction::Call::get_working_sizes() const { return {}; }
std::vector<uint32_t> Semantics::Instruction::Call::get_output_sizes() const { return {}; }
std::vector<uint32_t> Semantics::Instruction::Call::get_all_sizes() const { std::vector<uint32_t> v, i(std::move(get_input_sizes())), w(std::move(get_working_sizes())), o(std::move(get_output_sizes())); v.insert(v.end(), i.cbegin(), i.cend()); v.insert(v.end(), w.cbegin(), w.cend()); v.insert(v.end(), o.cbegin(), o.cend()); return v; }

std::vector<Semantics::Output::Line> Semantics::Instruction::Call::emit(const std::vector<Storage> &storages) const {
	// Check sizes.
	if (Storage::get_sizes(storages) != get_all_sizes()) {
		std::ostringstream sstr;
		sstr << "Semantics::Instruction::Call::emit: the number or sizes of storage units provided does not match what was expected.";
		throw SemanticsError(sstr.str());
	}

	// Prepare output vector.
	std::vector<Output::Line> lines;

	// Emit a symbol for this instruction if there is one.
	if (has_symbol) {
		lines.push_back({":", symbol});
	}

	// Part 1: jump.
	lines.push_back("\tjal  " + jump_destination);

	// Return the output.
	return lines;
}

Semantics::Instruction::Return::Return()
	{}

Semantics::Instruction::Return::Return(const Base &base, bool fixed_storage, const Storage &storage)
	: Base(base)
	, fixed_storage(fixed_storage)
	, storage(storage)
	{}

std::vector<uint32_t> Semantics::Instruction::Return::get_input_sizes() const { if (fixed_storage) { return {}; } else { return {4}; } }
std::vector<uint32_t> Semantics::Instruction::Return::get_working_sizes() const { return {}; }
std::vector<uint32_t> Semantics::Instruction::Return::get_output_sizes() const { return {}; }
std::vector<uint32_t> Semantics::Instruction::Return::get_all_sizes() const { std::vector<uint32_t> v, i(std::move(get_input_sizes())), w(std::move(get_working_sizes())), o(std::move(get_output_sizes())); v.insert(v.end(), i.cbegin(), i.cend()); v.insert(v.end(), w.cbegin(), w.cend()); v.insert(v.end(), o.cbegin(), o.cend()); return v; }

std::vector<Semantics::Output::Line> Semantics::Instruction::Return::emit(const std::vector<Storage> &storages) const {
	// Check sizes.
	if (Storage::get_sizes(storages) != get_all_sizes()) {
		std::ostringstream sstr;
		sstr << "Semantics::Instruction::Return::emit: the number or sizes of storage units provided does not match what was expected.";
		throw SemanticsError(sstr.str());
	}
	const Storage &source_storage = fixed_storage ? storage : storages[0];

	// Prepare output vector.
	std::vector<Output::Line> lines;

	// Emit a symbol for this instruction if there is one.
	if (has_symbol) {
		lines.push_back({":", symbol});
	}

	// Part 1: get source address.
	if           (source_storage.is_global_address()) {
		lines.push_back("\tla   $t8, " + source_storage.global_address);
		if (source_storage.offset != 0) {
			lines.push_back("\tla   $t8, " + std::to_string(source_storage.offset) + "($t8)");
		}
	} else if    (source_storage.is_global_dereference()) {
		lines.push_back("\tla   $t8, " + source_storage.global_address);
		lines.push_back("\tlw   $t8, " + std::to_string(source_storage.offset) + "($t8)");
	} else if    (source_storage.is_register_direct()) {
	} else {  // (source_storage.is_register_dereference()) {
		lines.push_back("\tla   $t8, " + std::to_string(source_storage.offset) + "(" + source_storage.register_ + ")");
	}

	// Part 2: load source (4-bytes).
	if (!source_storage.is_register_direct()) {
		lines.push_back("\tlw   $t8, ($t8)");
	}

	// Part 3: jump.
	if (!source_storage.is_register_direct()) {
		lines.push_back("\tjr   $t8");
	} else {
		lines.push_back("\tjr   " + source_storage.register_);
	}

	// Return the output.
	return lines;
}

Semantics::Instruction::BranchZero::BranchZero()
	{}

Semantics::Instruction::BranchZero::BranchZero(const Base &base, bool is_word, Symbol branch_destination)
	: Base(base)
	, is_word(is_word)
	, branch_destination(branch_destination)
	{}

std::vector<uint32_t> Semantics::Instruction::BranchZero::get_input_sizes() const { return {4}; }
std::vector<uint32_t> Semantics::Instruction::BranchZero::get_working_sizes() const { return {}; }
std::vector<uint32_t> Semantics::Instruction::BranchZero::get_output_sizes() const { return {}; }
std::vector<uint32_t> Semantics::Instruction::BranchZero::get_all_sizes() const { std::vector<uint32_t> v, i(std::move(get_input_sizes())), w(std::move(get_working_sizes())), o(std::move(get_output_sizes())); v.insert(v.end(), i.cbegin(), i.cend()); v.insert(v.end(), w.cbegin(), w.cend()); v.insert(v.end(), o.cbegin(), o.cend()); return v; }

std::vector<Semantics::Output::Line> Semantics::Instruction::BranchZero::emit(const std::vector<Storage> &storages) const {
	// Check sizes.
	if (Storage::get_sizes(storages) != get_all_sizes()) {
		std::ostringstream sstr;
		sstr << "Semantics::Instruction::BranchZero::emit: the number or sizes of storage units provided does not match what was expected.";
		throw SemanticsError(sstr.str());
	}
	const Storage &source_storage = storages[0];

	// Prepare output vector.
	std::vector<Output::Line> lines;

	// Emit a symbol for this instruction if there is one.
	if (has_symbol) {
		lines.push_back({":", symbol});
	}

	// Part 1: get source address.
	if           (source_storage.is_global_address()) {
		lines.push_back("\tla   $t8, " + source_storage.global_address);
		if (source_storage.offset != 0) {
			lines.push_back("\tla   $t8, " + std::to_string(source_storage.offset) + "($t8)");
		}
	} else if    (source_storage.is_global_dereference()) {
		lines.push_back("\tla   $t8, " + source_storage.global_address);
		lines.push_back("\tlw   $t8, " + std::to_string(source_storage.offset) + "($t8)");
	} else if    (source_storage.is_register_direct()) {
	} else {  // (source_storage.is_register_dereference()) {
		lines.push_back("\tla   $t8, " + std::to_string(source_storage.offset) + "(" + source_storage.register_ + ")");
	}

	// Part 2: load source (4-bytes).
	if (!source_storage.is_register_direct()) {
		lines.push_back("\tlw   $t8, ($t8)");
	}

	// Part 3: jump.
	if (!source_storage.is_register_direct()) {
		lines.push_back("\tbeq  $t8, $zero, " + branch_destination);
	} else {
		lines.push_back("\tbeq  " + source_storage.register_ + ", $zero, " + branch_destination);
	}

	// Return the output.
	return lines;
}

Semantics::Instruction::BranchNonnegative::BranchNonnegative()
	{}

Semantics::Instruction::BranchNonnegative::BranchNonnegative(const Base &base, bool is_word, Symbol branch_destination)
	: Base(base)
	, is_word(is_word)
	, branch_destination(branch_destination)
	{}

std::vector<uint32_t> Semantics::Instruction::BranchNonnegative::get_input_sizes() const { return {4}; }
std::vector<uint32_t> Semantics::Instruction::BranchNonnegative::get_working_sizes() const { return {}; }
std::vector<uint32_t> Semantics::Instruction::BranchNonnegative::get_output_sizes() const { return {}; }
std::vector<uint32_t> Semantics::Instruction::BranchNonnegative::get_all_sizes() const { std::vector<uint32_t> v, i(std::move(get_input_sizes())), w(std::move(get_working_sizes())), o(std::move(get_output_sizes())); v.insert(v.end(), i.cbegin(), i.cend()); v.insert(v.end(), w.cbegin(), w.cend()); v.insert(v.end(), o.cbegin(), o.cend()); return v; }

std::vector<Semantics::Output::Line> Semantics::Instruction::BranchNonnegative::emit(const std::vector<Storage> &storages) const {
	// Check sizes.
	if (Storage::get_sizes(storages) != get_all_sizes()) {
		std::ostringstream sstr;
		sstr << "Semantics::Instruction::BranchNonnegative::emit: the number or sizes of storage units provided does not match what was expected.";
		throw SemanticsError(sstr.str());
	}
	const Storage &source_storage = storages[0];

	// Prepare output vector.
	std::vector<Output::Line> lines;

	// Emit a symbol for this instruction if there is one.
	if (has_symbol) {
		lines.push_back({":", symbol});
	}

	// Part 1: get source address.
	if           (source_storage.is_global_address()) {
		lines.push_back("\tla   $t8, " + source_storage.global_address);
		if (source_storage.offset != 0) {
			lines.push_back("\tla   $t8, " + std::to_string(source_storage.offset) + "($t8)");
		}
	} else if    (source_storage.is_global_dereference()) {
		lines.push_back("\tla   $t8, " + source_storage.global_address);
		lines.push_back("\tlw   $t8, " + std::to_string(source_storage.offset) + "($t8)");
	} else if    (source_storage.is_register_direct()) {
	} else {  // (source_storage.is_register_dereference()) {
		lines.push_back("\tla   $t8, " + std::to_string(source_storage.offset) + "(" + source_storage.register_ + ")");
	}

	// Part 2: load source (4-bytes).
	if (!source_storage.is_register_direct()) {
		lines.push_back("\tlw   $t8, ($t8)");
	}

	// Part 3: jump.
	if (!source_storage.is_register_direct()) {
		lines.push_back("\tbge  $t8, $zero, " + branch_destination);
	} else {
		lines.push_back("\tbge  " + source_storage.register_ + ", $zero, " + branch_destination);
	}

	// Return the output.
	return lines;
}

Semantics::Instruction::Instruction(tag_t tag, const data_t &data)
	: tag(tag)
	, data(data)
	{}

Semantics::Instruction::Instruction(tag_t tag, data_t &&data)
	: tag(tag)
	, data(std::move(data))
	{}

Semantics::Instruction::Instruction(const Ignore &ignore)
	: tag(ignore_tag)
	, data(ignore)
	{}

Semantics::Instruction::Instruction(const LoadImmediate &load_immediate)
	: tag(load_immediate_tag)
	, data(load_immediate)
	{}

Semantics::Instruction::Instruction(const LoadFrom &load_from)
	: tag(load_from_tag)
	, data(load_from)
	{}

Semantics::Instruction::Instruction(const LessThanFrom &less_than_from)
	: tag(less_than_from_tag)
	, data(less_than_from)
	{}

Semantics::Instruction::Instruction(const NorFrom &nor_from)
	: tag(nor_from_tag)
	, data(nor_from)
	{}

Semantics::Instruction::Instruction(const AndFrom &and_from)
	: tag(and_from_tag)
	, data(and_from)
	{}

Semantics::Instruction::Instruction(const OrFrom &or_from)
	: tag(or_from_tag)
	, data(or_from)
	{}

Semantics::Instruction::Instruction(const AddFrom &add_from)
	: tag(add_from_tag)
	, data(add_from)
	{}

Semantics::Instruction::Instruction(const SubFrom &sub_from)
	: tag(sub_from_tag)
	, data(sub_from)
	{}

Semantics::Instruction::Instruction(const MultFrom &mult_from)
	: tag(mult_from_tag)
	, data(mult_from)
	{}

Semantics::Instruction::Instruction(const DivFrom &div_from)
	: tag(div_from_tag)
	, data(div_from)
	{}

Semantics::Instruction::Instruction(const JumpTo &jump_to)
	: tag(jump_to_tag)
	, data(jump_to)
	{}

Semantics::Instruction::Instruction(const Jump &jump)
	: tag(jump_tag)
	, data(jump)
	{}

Semantics::Instruction::Instruction(const Call &call)
	: tag(call_tag)
	, data(call)
	{}

Semantics::Instruction::Instruction(const Return &return_)
	: tag(return_tag)
	, data(return_)
	{}

Semantics::Instruction::Instruction(const BranchZero &branch_zero)
	: tag(branch_zero_tag)
	, data(branch_zero)
	{}

Semantics::Instruction::Instruction(const BranchNonnegative &branch_nonnegative)
	: tag(branch_nonnegative_tag)
	, data(branch_nonnegative)
	{}

const Semantics::Instruction::Base &Semantics::Instruction::get_base() const {
	switch(tag) {
		case ignore_tag:
			return get_ignore();
		case load_immediate_tag:
			return get_load_immediate();
		case load_from_tag:
			return get_load_from();
		case less_than_from_tag:
			return get_less_than_from();
		case nor_from_tag:
			return get_nor_from();
		case and_from_tag:
			return get_and_from();
		case or_from_tag:
			return get_or_from();
		case add_from_tag:
			return get_add_from();
		case sub_from_tag:
			return get_sub_from();
		case mult_from_tag:
			return get_mult_from();
		case div_from_tag:
			return get_div_from();
		case jump_to_tag:
			return get_jump_to();
		case jump_tag:
			return get_jump();
		case call_tag:
			return get_call();
		case return_tag:
			return get_return();
		case branch_zero_tag:
			return get_branch_zero();
		case branch_nonnegative_tag:
			return get_branch_nonnegative();

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_base: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

Semantics::Instruction::Base &&Semantics::Instruction::get_base() {
	switch(tag) {
		case ignore_tag:
			return std::move(get_ignore());
		case load_immediate_tag:
			return std::move(get_load_immediate());
		case load_from_tag:
			return std::move(get_load_from());
		case less_than_from_tag:
			return std::move(get_less_than_from());
		case nor_from_tag:
			return std::move(get_nor_from());
		case and_from_tag:
			return std::move(get_and_from());
		case or_from_tag:
			return std::move(get_or_from());
		case add_from_tag:
			return std::move(get_add_from());
		case sub_from_tag:
			return std::move(get_sub_from());
		case mult_from_tag:
			return std::move(get_mult_from());
		case div_from_tag:
			return std::move(get_div_from());
		case jump_to_tag:
			return std::move(get_jump_to());
		case jump_tag:
			return std::move(get_jump());
		case call_tag:
			return std::move(get_call());
		case return_tag:
			return std::move(get_return());
		case branch_zero_tag:
			return std::move(get_branch_zero());
		case branch_nonnegative_tag:
			return std::move(get_branch_nonnegative());

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_base: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

Semantics::Instruction::Base &Semantics::Instruction::get_base_mutable() {
	switch(tag) {
		case ignore_tag:
			return get_ignore_mutable();
		case load_immediate_tag:
			return get_load_immediate_mutable();
		case load_from_tag:
			return get_load_from_mutable();
		case less_than_from_tag:
			return get_less_than_from_mutable();
		case nor_from_tag:
			return get_nor_from_mutable();
		case and_from_tag:
			return get_and_from_mutable();
		case or_from_tag:
			return get_or_from_mutable();
		case add_from_tag:
			return get_add_from_mutable();
		case sub_from_tag:
			return get_sub_from_mutable();
		case mult_from_tag:
			return get_mult_from_mutable();
		case div_from_tag:
			return get_div_from_mutable();
		case jump_to_tag:
			return get_jump_to_mutable();
		case jump_tag:
			return get_jump_mutable();
		case call_tag:
			return get_call_mutable();
		case return_tag:
			return get_return_mutable();
		case branch_zero_tag:
			return get_branch_zero_mutable();
		case branch_nonnegative_tag:
			return get_branch_nonnegative_mutable();

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_base_mutable: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::Instruction::is_ignore() const {
	switch(tag) {
		case ignore_tag:
			return true;
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::is_ignore: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::Instruction::is_load_immediate() const {
	switch(tag) {
		case ignore_tag:
			return false;
		case load_immediate_tag:
			return true;
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::is_load_immediate: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::Instruction::is_load_from() const {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
			return false;
		case load_from_tag:
			return true;
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::is_load_from: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::Instruction::is_less_than_from() const {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
			return false;
		case less_than_from_tag:
			return true;
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::is_less_than_from: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::Instruction::is_nor_from() const {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
			return false;
		case nor_from_tag:
			return true;
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::is_nor_from: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::Instruction::is_and_from() const {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
			return false;
		case and_from_tag:
			return true;
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::is_and_from: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::Instruction::is_or_from() const {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
			return false;
		case or_from_tag:
			return true;
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::is_or_from: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::Instruction::is_add_from() const {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
			return false;
		case add_from_tag:
			return true;
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::is_add_from: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::Instruction::is_sub_from() const {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
			return false;
		case sub_from_tag:
			return true;
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::is_sub_from: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::Instruction::is_mult_from() const {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
			return false;
		case mult_from_tag:
			return true;
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::is_mult_from: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::Instruction::is_div_from() const {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
			return false;
		case div_from_tag:
			return true;
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::is_div_from: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::Instruction::is_jump_to() const {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
			return false;
		case jump_to_tag:
			return true;
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::is_jump_to: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::Instruction::is_jump() const {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
			return false;
		case jump_tag:
			return true;
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::is_jump: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::Instruction::is_call() const {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
			return false;
		case call_tag:
			return true;
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::is_call: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::Instruction::is_return() const {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
			return false;
		case return_tag:
			return true;
		case branch_zero_tag:
		case branch_nonnegative_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::is_return: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::Instruction::is_branch_zero() const {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
			return false;
		case branch_zero_tag:
			return true;
		case branch_nonnegative_tag:
			return false;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::is_branch_zero: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

bool Semantics::Instruction::is_branch_nonnegative() const {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
			return false;
		case branch_nonnegative_tag:
			return true;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::is_branch_nonnegative: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

// | The tags must be correct, or else an exception will be thrown, including for set_*.
const Semantics::Instruction::Ignore &Semantics::Instruction::get_ignore() const {
	switch(tag) {
		case ignore_tag:
			return std::get<Ignore>(data);
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_ignore: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_ignore: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

const Semantics::Instruction::LoadImmediate &Semantics::Instruction::get_load_immediate() const {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
			return std::get<LoadImmediate>(data);
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_load_immediate: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_load_immediate: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

const Semantics::Instruction::LoadFrom &Semantics::Instruction::get_load_from() const {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
			break;
		case load_from_tag:
			return std::get<LoadFrom>(data);
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_load_from: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_load_from: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

const Semantics::Instruction::LessThanFrom &Semantics::Instruction::get_less_than_from() const {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
			break;
		case less_than_from_tag:
			return std::get<LessThanFrom>(data);
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_less_than_from: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_less_than_from: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

const Semantics::Instruction::NorFrom &Semantics::Instruction::get_nor_from() const {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
			break;
		case nor_from_tag:
			return std::get<NorFrom>(data);
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_nor_from: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_nor_from: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

const Semantics::Instruction::AndFrom &Semantics::Instruction::get_and_from() const {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
			break;
		case and_from_tag:
			return std::get<AndFrom>(data);
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_and_from: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_and_from: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

const Semantics::Instruction::OrFrom &Semantics::Instruction::get_or_from() const {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
			break;
		case or_from_tag:
			return std::get<OrFrom>(data);
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_or_from: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_or_from: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

const Semantics::Instruction::AddFrom &Semantics::Instruction::get_add_from() const {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
			break;
		case add_from_tag:
			return std::get<AddFrom>(data);
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_add_from: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_add_from: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

const Semantics::Instruction::SubFrom &Semantics::Instruction::get_sub_from() const {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
			break;
		case sub_from_tag:
			return std::get<SubFrom>(data);
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_sub_from: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_sub_from: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

const Semantics::Instruction::MultFrom &Semantics::Instruction::get_mult_from() const {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
			break;
		case mult_from_tag:
			return std::get<MultFrom>(data);
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_mult_from: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_mult_from: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

const Semantics::Instruction::DivFrom &Semantics::Instruction::get_div_from() const {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
			break;
		case div_from_tag:
			return std::get<DivFrom>(data);
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_div_from: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_div_from: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

const Semantics::Instruction::JumpTo &Semantics::Instruction::get_jump_to() const {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
			break;
		case jump_to_tag:
			return std::get<JumpTo>(data);
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_jump_to: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_jump_to: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

const Semantics::Instruction::Jump &Semantics::Instruction::get_jump() const {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
			break;
		case jump_tag:
			return std::get<Jump>(data);
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_jump: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_jump: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

const Semantics::Instruction::Call &Semantics::Instruction::get_call() const {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
			break;
		case call_tag:
			return std::get<Call>(data);
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_call: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_call: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

const Semantics::Instruction::Return &Semantics::Instruction::get_return() const {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
			break;
		case return_tag:
			return std::get<Return>(data);
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_return: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_return: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

const Semantics::Instruction::BranchZero &Semantics::Instruction::get_branch_zero() const {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
			break;
		case branch_zero_tag:
			return std::get<BranchZero>(data);
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_branch_zero: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_branch_zero: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

const Semantics::Instruction::BranchNonnegative &Semantics::Instruction::get_branch_nonnegative() const {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
			break;
		case branch_nonnegative_tag:
			return std::get<BranchNonnegative>(data);

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_branch_nonnegative: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_branch_nonnegative: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::Ignore &&Semantics::Instruction::get_ignore() {
	switch(tag) {
		case ignore_tag:
			return std::get<Ignore>(std::move(data));
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_ignore: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_ignore: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::LoadImmediate &&Semantics::Instruction::get_load_immediate() {
	switch(tag) {
		case ignore_tag:
			break;
		case load_immediate_tag:
			return std::get<LoadImmediate>(std::move(data));
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_load_immediate: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_load_immediate: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::LoadFrom &&Semantics::Instruction::get_load_from() {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
			break;
		case load_from_tag:
			return std::get<LoadFrom>(std::move(data));
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_load_from: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_load_from: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::LessThanFrom &&Semantics::Instruction::get_less_than_from() {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
			break;
		case less_than_from_tag:
			return std::get<LessThanFrom>(std::move(data));
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_less_than_from: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_less_than_from: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::NorFrom &&Semantics::Instruction::get_nor_from() {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
			break;
		case nor_from_tag:
			return std::get<NorFrom>(std::move(data));
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_nor_from: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_nor_from: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::AndFrom &&Semantics::Instruction::get_and_from() {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
			break;
		case and_from_tag:
			return std::get<AndFrom>(std::move(data));
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_and_from: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_and_from: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::OrFrom &&Semantics::Instruction::get_or_from() {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
			break;
		case or_from_tag:
			return std::get<OrFrom>(std::move(data));
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_or_from: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_or_from: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::AddFrom &&Semantics::Instruction::get_add_from() {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
			break;
		case add_from_tag:
			return std::get<AddFrom>(std::move(data));
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_add_from: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_add_from: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::SubFrom &&Semantics::Instruction::get_sub_from() {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
			break;
		case sub_from_tag:
			return std::get<SubFrom>(std::move(data));
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_sub_from: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_sub_from: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::MultFrom &&Semantics::Instruction::get_mult_from() {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
			break;
		case mult_from_tag:
			return std::get<MultFrom>(std::move(data));
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_mult_from: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_mult_from: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::DivFrom &&Semantics::Instruction::get_div_from() {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
			break;
		case div_from_tag:
			return std::get<DivFrom>(std::move(data));
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_div_from: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_div_from: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::JumpTo &&Semantics::Instruction::get_jump_to() {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
			break;
		case jump_to_tag:
			return std::get<JumpTo>(std::move(data));
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_jump_to: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_jump_to: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::Jump &&Semantics::Instruction::get_jump() {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
			break;
		case jump_tag:
			return std::get<Jump>(std::move(data));
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_jump: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_jump: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::Call &&Semantics::Instruction::get_call() {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
			break;
		case call_tag:
			return std::get<Call>(std::move(data));
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_call: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_call: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::Return &&Semantics::Instruction::get_return() {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
			break;
		case return_tag:
			return std::get<Return>(std::move(data));
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_return: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_return: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::BranchZero &&Semantics::Instruction::get_branch_zero() {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
			break;
		case branch_zero_tag:
			return std::get<BranchZero>(std::move(data));
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_branch_zero: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_branch_zero: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::BranchNonnegative &&Semantics::Instruction::get_branch_nonnegative() {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
			break;
		case branch_nonnegative_tag:
			return std::get<BranchNonnegative>(std::move(data));

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_branch_nonnegative: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_branch_nonnegative: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

// Non-constant lvalue references.
Semantics::Instruction::Ignore &Semantics::Instruction::get_ignore_mutable() {
	switch(tag) {
		case ignore_tag:
			return std::get<Ignore>(data);
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_ignore_mutable: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_ignore_mutable: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::LoadImmediate &Semantics::Instruction::get_load_immediate_mutable() {
	switch(tag) {
		case ignore_tag:
			break;
		case load_immediate_tag:
			return std::get<LoadImmediate>(data);
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_load_immediate_mutable: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_load_immediate_mutable: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::LoadFrom &Semantics::Instruction::get_load_from_mutable() {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
			break;
		case load_from_tag:
			return std::get<LoadFrom>(data);
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_load_from_mutable: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_load_from_mutable: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::LessThanFrom &Semantics::Instruction::get_less_than_from_mutable() {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
			break;
		case less_than_from_tag:
			return std::get<LessThanFrom>(data);
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_less_than_from_mutable: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_less_than_from_mutable: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::NorFrom &Semantics::Instruction::get_nor_from_mutable() {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
			break;
		case nor_from_tag:
			return std::get<NorFrom>(data);
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_nor_from_mutable: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_nor_from_mutable: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::AndFrom &Semantics::Instruction::get_and_from_mutable() {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
			break;
		case and_from_tag:
			return std::get<AndFrom>(data);
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_and_from_mutable: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_and_from_mutable: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::OrFrom &Semantics::Instruction::get_or_from_mutable() {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
			break;
		case or_from_tag:
			return std::get<OrFrom>(data);
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_or_from_mutable: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_or_from_mutable: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::AddFrom &Semantics::Instruction::get_add_from_mutable() {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
			break;
		case add_from_tag:
			return std::get<AddFrom>(data);
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_add_from_mutable: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_add_from_mutable: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::SubFrom &Semantics::Instruction::get_sub_from_mutable() {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
			break;
		case sub_from_tag:
			return std::get<SubFrom>(data);
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_sub_from_mutable: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_sub_from: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::MultFrom &Semantics::Instruction::get_mult_from_mutable() {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
			break;
		case mult_from_tag:
			return std::get<MultFrom>(data);
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_mult_from_mutable: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_mult_from_mutable: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::DivFrom &Semantics::Instruction::get_div_from_mutable() {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
			break;
		case div_from_tag:
			return std::get<DivFrom>(data);
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_div_from_mutable: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_div_from_mutable: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::JumpTo &Semantics::Instruction::get_jump_to_mutable() {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
			break;
		case jump_to_tag:
			return std::get<JumpTo>(data);
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_jump_to_mutable: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_jump_to_mutable: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::Jump &Semantics::Instruction::get_jump_mutable() {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
			break;
		case jump_tag:
			return std::get<Jump>(data);
		case call_tag:
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_jump_mutable: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_jump_mutable: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::Call &Semantics::Instruction::get_call_mutable() {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
			break;
		case call_tag:
			return std::get<Call>(data);
		case return_tag:
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_call_mutable: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_call_mutable: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::Return &Semantics::Instruction::get_return_mutable() {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
			break;
		case return_tag:
			return std::get<Return>(data);
		case branch_zero_tag:
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_return_mutable: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_return_mutable: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::BranchZero &Semantics::Instruction::get_branch_zero_mutable() {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
			break;
		case branch_zero_tag:
			return std::get<BranchZero>(data);
		case branch_nonnegative_tag:
			break;

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_branch_zero_mutable: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_branch_zero_mutable: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

Semantics::Instruction::BranchNonnegative &Semantics::Instruction::get_branch_nonnegative_mutable() {
	switch(tag) {
		case ignore_tag:
		case load_immediate_tag:
		case load_from_tag:
		case less_than_from_tag:
		case nor_from_tag:
		case and_from_tag:
		case or_from_tag:
		case add_from_tag:
		case sub_from_tag:
		case mult_from_tag:
		case div_from_tag:
		case jump_to_tag:
		case jump_tag:
		case call_tag:
		case return_tag:
		case branch_zero_tag:
			break;
		case branch_nonnegative_tag:
			return std::get<BranchNonnegative>(data);

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_branch_nonnegative_mutable: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}

	std::ostringstream sstr;
	sstr << "Semantics::Instruction::get_branch_nonnegative_mutable: binding has a different type tag: " << tag;
	throw SemanticsError(sstr.str());
}

// | Return "ignore", "load_immediate", "less_than_from", "load_from", or "nor_from", etc.
std::string Semantics::Instruction::get_tag_repr(tag_t tag) {
	switch(tag) {
		case ignore_tag:
			return "ignore";
		case load_immediate_tag:
			return "load_immediate";
		case load_from_tag:
			return "load_from";
		case less_than_from_tag:
			return "less_than_from";
		case nor_from_tag:
			return "nor_from";
		case and_from_tag:
			return "and_from";
		case or_from_tag:
			return "or_from";
		case add_from_tag:
			return "add_from";
		case sub_from_tag:
			return "sub_from";
		case mult_from_tag:
			return "mult_from";
		case div_from_tag:
			return "div_from";
		case jump_to_tag:
			return "jump_to";
		case jump_tag:
			return "jump";
		case call_tag:
			return "call";
		case return_tag:
			return "return";
		case branch_zero_tag:
			return "branch_zero";
		case branch_nonnegative_tag:
			return "branch_nonnegative";

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_tag_repr: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

std::string Semantics::Instruction::get_tag_repr() const {
	return get_tag_repr(tag);
}

std::vector<uint32_t> Semantics::Instruction::get_input_sizes() const {
	switch(tag) {
		case ignore_tag:
			return get_ignore().get_input_sizes();
		case load_immediate_tag:
			return get_load_immediate().get_input_sizes();
		case load_from_tag:
			return get_load_from().get_input_sizes();
		case less_than_from_tag:
			return get_less_than_from().get_input_sizes();
		case nor_from_tag:
			return get_nor_from().get_input_sizes();
		case and_from_tag:
			return get_and_from().get_input_sizes();
		case or_from_tag:
			return get_or_from().get_input_sizes();
		case add_from_tag:
			return get_add_from().get_input_sizes();
		case sub_from_tag:
			return get_sub_from().get_input_sizes();
		case mult_from_tag:
			return get_mult_from().get_input_sizes();
		case div_from_tag:
			return get_div_from().get_input_sizes();
		case jump_to_tag:
			return get_jump_to().get_input_sizes();
		case jump_tag:
			return get_jump().get_input_sizes();
		case call_tag:
			return get_call().get_input_sizes();
		case return_tag:
			return get_return().get_input_sizes();
		case branch_zero_tag:
			return get_branch_zero().get_input_sizes();
		case branch_nonnegative_tag:
			return get_branch_nonnegative().get_input_sizes();

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_input_sizes: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

std::vector<uint32_t> Semantics::Instruction::get_working_sizes() const {
	switch(tag) {
		case ignore_tag:
			return get_ignore().get_working_sizes();
		case load_immediate_tag:
			return get_load_immediate().get_working_sizes();
		case load_from_tag:
			return get_load_from().get_working_sizes();
		case less_than_from_tag:
			return get_less_than_from().get_working_sizes();
		case nor_from_tag:
			return get_nor_from().get_working_sizes();
		case and_from_tag:
			return get_and_from().get_working_sizes();
		case or_from_tag:
			return get_or_from().get_working_sizes();
		case add_from_tag:
			return get_add_from().get_working_sizes();
		case sub_from_tag:
			return get_sub_from().get_working_sizes();
		case mult_from_tag:
			return get_mult_from().get_working_sizes();
		case div_from_tag:
			return get_div_from().get_working_sizes();
		case jump_to_tag:
			return get_jump_to().get_working_sizes();
		case jump_tag:
			return get_jump().get_working_sizes();
		case call_tag:
			return get_call().get_working_sizes();
		case return_tag:
			return get_return().get_working_sizes();
		case branch_zero_tag:
			return get_branch_zero().get_working_sizes();
		case branch_nonnegative_tag:
			return get_branch_nonnegative().get_working_sizes();

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_working_sizes: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

std::vector<uint32_t> Semantics::Instruction::get_output_sizes() const {
	switch(tag) {
		case ignore_tag:
			return get_ignore().get_output_sizes();
		case load_immediate_tag:
			return get_load_immediate().get_output_sizes();
		case load_from_tag:
			return get_load_from().get_output_sizes();
		case less_than_from_tag:
			return get_less_than_from().get_output_sizes();
		case nor_from_tag:
			return get_nor_from().get_output_sizes();
		case and_from_tag:
			return get_and_from().get_output_sizes();
		case or_from_tag:
			return get_or_from().get_output_sizes();
		case add_from_tag:
			return get_add_from().get_output_sizes();
		case sub_from_tag:
			return get_sub_from().get_output_sizes();
		case mult_from_tag:
			return get_mult_from().get_output_sizes();
		case div_from_tag:
			return get_div_from().get_output_sizes();
		case jump_to_tag:
			return get_jump_to().get_output_sizes();
		case jump_tag:
			return get_jump().get_output_sizes();
		case call_tag:
			return get_call().get_output_sizes();
		case return_tag:
			return get_return().get_output_sizes();
		case branch_zero_tag:
			return get_branch_zero().get_output_sizes();
		case branch_nonnegative_tag:
			return get_branch_nonnegative().get_output_sizes();

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_output_sizes: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

std::vector<uint32_t> Semantics::Instruction::get_all_sizes() const {
	switch(tag) {
		case ignore_tag:
			return get_ignore().get_all_sizes();
		case load_immediate_tag:
			return get_load_immediate().get_all_sizes();
		case load_from_tag:
			return get_load_from().get_all_sizes();
		case less_than_from_tag:
			return get_less_than_from().get_all_sizes();
		case nor_from_tag:
			return get_nor_from().get_all_sizes();
		case and_from_tag:
			return get_and_from().get_all_sizes();
		case or_from_tag:
			return get_or_from().get_all_sizes();
		case add_from_tag:
			return get_add_from().get_all_sizes();
		case sub_from_tag:
			return get_sub_from().get_all_sizes();
		case mult_from_tag:
			return get_mult_from().get_all_sizes();
		case div_from_tag:
			return get_div_from().get_all_sizes();
		case jump_to_tag:
			return get_jump_to().get_all_sizes();
		case jump_tag:
			return get_jump().get_all_sizes();
		case call_tag:
			return get_call().get_all_sizes();
		case return_tag:
			return get_return().get_all_sizes();
		case branch_zero_tag:
			return get_branch_zero().get_all_sizes();
		case branch_nonnegative_tag:
			return get_branch_nonnegative().get_all_sizes();

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::get_all_sizes: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

std::vector<Semantics::Output::Line> Semantics::Instruction::emit(const std::vector<Storage> &storages) const {
	switch(tag) {
		case ignore_tag:
			return get_ignore().emit(storages);
		case load_immediate_tag:
			return get_load_immediate().emit(storages);
		case load_from_tag:
			return get_load_from().emit(storages);
		case less_than_from_tag:
			return get_less_than_from().emit(storages);
		case nor_from_tag:
			return get_nor_from().emit(storages);
		case and_from_tag:
			return get_and_from().emit(storages);
		case or_from_tag:
			return get_or_from().emit(storages);
		case add_from_tag:
			return get_add_from().emit(storages);
		case sub_from_tag:
			return get_sub_from().emit(storages);
		case mult_from_tag:
			return get_mult_from().emit(storages);
		case div_from_tag:
			return get_div_from().emit(storages);
		case jump_to_tag:
			return get_jump_to().emit(storages);
		case jump_tag:
			return get_jump().emit(storages);
		case call_tag:
			return get_call().emit(storages);
		case return_tag:
			return get_return().emit(storages);
		case branch_zero_tag:
			return get_branch_zero().emit(storages);
		case branch_nonnegative_tag:
			return get_branch_nonnegative().emit(storages);

		case null_tag:
		default:
			std::ostringstream sstr;
			sstr << "Semantics::Instruction::emit: invalid tag: " << tag;
			throw SemanticsError(sstr.str());
	}
}

std::vector<uint32_t> Semantics::MIPSIO::prepare(const std::map<IO, Storage> &capture_outputs) const {
	std::set<IO> capture_outputs_;
	for (const std::map<IO, Storage>::value_type &capture_output_pair : std::as_const(capture_outputs)) {
		capture_outputs_.insert(capture_output_pair.first);
	}
	return prepare(capture_outputs_);
}

std::vector<uint32_t> Semantics::MIPSIO::prepare(const std::set<IO> &capture_outputs_) const {
	// Emulate emit(), except don't emit, and add working storage unit requirements when more are needed.
	std::vector<Storage> working_storages;

	std::map<IO, Storage> capture_outputs;
	for (const IO &capture_output : std::as_const(capture_outputs_)) {
		capture_outputs.insert({capture_output, Storage()});
	}

	const bool permit_uncaptured_outputs = true;

	// Emulate emit().
	std::map<Index, std::map<IOIndex, Storage>> expanded_capture_outputs = expand_map<Index, IOIndex, Storage>(capture_outputs);

	std::vector<uint32_t> working_storage_sizes = Storage::get_sizes(working_storages);
	std::map<IO, Storage::Index> reverse_claimed_working_storages;  // For a node's output, which working storage units is it using?
	std::map<Storage::Index, IO> claimed_working_storages;          // Which output IO claims a working storage?

	//std::vector<Output::Line> output_lines;

	// DFS from each output vertex.  Don't revisit instructions.  Write outputs
	// to available working storage units.  After all of a given node's output
	// index's connected input nodes are emitted, mark the working storage unit
	// that that node's output index was stored to as available.
	//
	// Don't revisit instructions.  Once the last
	// working storage units that are used as inputs are popped, mark them as
	// re-usable.
	std::set<Index>    visited_instructions;
	std::vector<Index> root_stack;
	std::vector<Index> children_stack;
	std::set<Index>    ancestors;  // Detect cycles: DFS can detect already visited notes (e.g. diamond) but not ancestors.
	for (const std::map<Index, std::map<IOIndex, Storage>>::value_type &output_pair : std::as_const(expanded_capture_outputs)) {
		root_stack.push_back(output_pair.first);
	}
	while (children_stack.size() > 0 || root_stack.size() > 0) {
		if (children_stack.size() <= 0 && root_stack.size() > 0) {
			children_stack.push_back(root_stack.back());
			root_stack.pop_back();
		}

		const Index        this_node   = children_stack.back();
		const Instruction &instruction = instructions.at(this_node);

		// If we've already processed (emitted) this node, skip.
		if (visited_instructions.find(this_node) != visited_instructions.cend()) {
			children_stack.pop_back();
		}

		// If this node has unvisited children, push them onto the stack, and
		// continue to process them before revisiting this node, processing it,
		// and marking it as visited.
		bool has_unvisited_children = false;

		for (IOIndex input_index_ = 0; input_index_ < instruction.get_input_sizes().size(); ++input_index_) {
			const IOIndex input_index = instruction.get_input_sizes().size() - 1 - input_index_;
			std::map<IO, IO>::const_iterator connections_search = connections.find({this_node, input_index});
			if (connections_search != connections.cend()) {
				const Index child_node = connections_search->second.first;

				// Detect cycles.
				if (ancestors.find(child_node) != ancestors.cend()) {
					std::ostringstream sstr;
					sstr << "Semantics::MIPSIO::prepare: error: a cycle was detected in the instruction graph at index " << child_node << " (child of " << this_node << ").";
					throw SemanticsError(sstr.str());
				}

				// Add the child.
				if (visited_instructions.find(child_node) == visited_instructions.cend()) {
					has_unvisited_children = true;
					children_stack.push_back(child_node);
				}
			}
		}

		if (has_unvisited_children) {
			ancestors.insert(this_node);
			continue;
		}

		// We've finished emitting all of the children's nodes.  Mark this one as visited and process (emit) it.
		visited_instructions.insert(this_node);
		ancestors.erase(this_node);
		children_stack.pop_back();  // Save an extra iteration.

		// Emit this node.

		// Construct the instruction's input storage.
		//std::vector<Storage> input_storage;
		for (IOIndex input_index = 0; input_index < instruction.get_input_sizes().size(); ++input_index) {
			IO input_io {this_node, input_index};

			// User-supplied input or connection input from another instruction?
			//const std::map<IO, Storage>::const_iterator input_storages_search = input_storages.find(input_io);
			const std::map<IO, IO>::const_iterator      connections_search    = connections.find(input_io);
			//const bool input_storages_found = input_storages_search != input_storages.cend();
			const bool connections_found    = connections_search    != connections.cend();
			const bool input_storages_found = !connections_found;
			/*
			if        ( input_storages_found &&  connections_found) {
				// Conflict.
				std::ostringstream sstr;
				sstr
					<< "Semantics::MIPSIO::prepare: error: input/connection conflict in instruction graph: connection input was created for a node but the \"input_storages\" argument also contains a reference to the same input." << std::endl
					<< "\tthis_node (index) : " << this_node << std::endl
					<< "\tinput_index       : " << input_index
					;
				throw SemanticsError(sstr.str());
			} else if (!input_storages_found && !connections_found) {
				// No connection, and no input provided!  (Assuming implementation didn't erroneously free the working storage prematurely.)
				std::ostringstream sstr;
				sstr
					<< "Semantics::MIPSIO::prepare: error: input missing without connection in instruction graph: the \"input_storages\" argument contains no reference to an instruction, and no output is provided to by another node through a constructed storage." << std::endl
					<< "\tthis_node (index) : " << this_node << std::endl
					<< "\tinput_index       : " << input_index
					;
				throw SemanticsError(sstr.str());
			}
			*/

			if (input_storages_found) {
				/*
				input_storage.push_back(input_storages_search->second);
				if (instruction.get_input_sizes().at(input_index) != input_storages_search->second.max_size) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::MIPSIO::prepare: error: user-provided input size mismatch in instruction graph: the \"input_storages\" argument contains a reference to an instruction's input, but the size is incorrect." << std::endl
						<< "\tthis_node (index)           : " << this_node << std::endl
						<< "\tinput_index                 : " << input_index << std::endl
						<< "\tprovided input storage size : " << instruction.get_input_sizes().at(input_index) << std::endl
						<< "\tinput storage expected      : " << input_storages_search->second.max_size
						;
					throw SemanticsError(sstr.str());
				}
				*/
			} else {
				const IO output_io = connections_search->second;

				const std::map<IO, Storage::Index>::const_iterator reverse_claimed_working_storages_search = reverse_claimed_working_storages.find(output_io);
				const bool reverse_claimed_working_storages_found = reverse_claimed_working_storages_search != reverse_claimed_working_storages.cend();

				if (!reverse_claimed_working_storages_found) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::MIPSIO::prepare: internal error: there is a bug because an connected input node should already have been emitted by now but its output storage is not claimed." << std::endl
						<< "\tthis_node (index)        : " << this_node << std::endl
						<< "\toutput node              : " << output_io.first << std::endl
						<< "\toutput node output index : " << output_io.second << std::endl
						<< "\tinput_index              : " << input_index << std::endl
						<< "\tinput storage size       : " << instruction.get_input_sizes().at(input_index)
						;
					throw SemanticsError(sstr.str());
				}

				const Storage &working_storage = working_storages[reverse_claimed_working_storages_search->second];
				//input_storage.push_back(working_storage);
				if (working_storage.max_size != instruction.get_input_sizes().at(input_index)) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::MIPSIO::prepare: error: connection input size mismatch in instruction graph: in the instruction graph, there is a connection between an output and an input of different sizes." << std::endl
						<< "\tthis_node (index)        : " << this_node << std::endl
						<< "\toutput node              : " << output_io.first << std::endl
						<< "\toutput node output index : " << output_io.second << std::endl
						<< "\tinput_index              : " << input_index << std::endl
						<< "\toutput storage size      : " << reverse_claimed_working_storages_search->second << std::endl
						<< "\tinput storage size       : " << instruction.get_input_sizes().at(input_index)
						;
					throw SemanticsError(sstr.str());
				}
			}
		}

		// Construct the instruction's output storage.
		//std::vector<Storage> output_storage;
		for (IOIndex output_index = 0; output_index < instruction.get_output_sizes().size(); ++output_index) {
			IO output_io {this_node, output_index};

			// Is there a user-supplied output storage unit, or do we need to claim required working storage?
			const std::map<IO, Storage>::const_iterator      capture_outputs_search      = capture_outputs.find(output_io);
			const std::map<IO, std::set<IO>>::const_iterator reversed_connections_search = reversed_connections.find(output_io);  // (To check for conflicts.)
			const bool capture_outputs_found    = capture_outputs_search      != capture_outputs.cend();
			const bool reversed_connections_any = reversed_connections_search != reversed_connections.cend() && reversed_connections_search->second.size() > 0;
			if        ( capture_outputs_found &&  reversed_connections_any) {
				// Conflict.
				std::ostringstream sstr;
				sstr
					<< "Semantics::MIPSIO::prepare: error: output/connection conflict in instruction graph: connection output was created for a node but the \"capture_outputs\" argument also contains a reference to the same output." << std::endl
					<< "\tthis_node (index) : " << this_node << std::endl
					<< "\toutput_index      : " << output_index
					;
				throw SemanticsError(sstr.str());
			} else if (!capture_outputs_found && !reversed_connections_any) {
				if (!permit_uncaptured_outputs) {
					// Nothing consumes this output.
					std::ostringstream sstr;
					sstr
						<< "Semantics::MIPSIO::prepare: error: output capture missing without connection in instruction graph: the \"capture_outputs\" argument contains no reference to an instruction's output, and the output is is not provided to other node." << std::endl
						<< "\tthis_node (index) : " << this_node << std::endl
						<< "\toutput_index      : " << output_index
						;
					throw SemanticsError(sstr.str());
				}
			}

			if (capture_outputs_found) {
				const Storage &working_storage = capture_outputs_search->second;
				//output_storage.push_back(working_storage);
				/*
				if (working_storage.max_size != instruction.get_output_sizes().at(output_index)) {
					// Output size mismatch.
					std::ostringstream sstr;
					sstr
						<< "Semantics::MIPSIO::prepare: error: user-provided output capture size mismatch in instruction graph: the \"capture_outputs\" argument contains a reference to an instruction's output, but the size is incorrect." << std::endl
						<< "\tthis_node (index)                    : " << this_node << std::endl
						<< "\toutput_index                         : " << output_index << std::endl
						<< "\tprovided output capture storage size : " << working_storage.max_size << std::endl
						<< "\toutput storage expected              : " << instruction.get_output_sizes().at(output_index)
						;
					throw SemanticsError(sstr.str());
				}
				*/
			} else {
				// Claim the next available working storage.
				bool found = false;
				for (Storage::Index working_storage_index = 0; working_storage_index < working_storages.size(); ++working_storage_index) {
					if (claimed_working_storages.find(working_storage_index) == claimed_working_storages.cend()) {
						const Storage &working_storage = working_storages[working_storage_index];
						if (instruction.get_output_sizes().at(output_index) == working_storage.max_size) {
							// Claim the working storage.
							reverse_claimed_working_storages.insert({output_io, working_storage_index});
							claimed_working_storages.insert({working_storage_index, output_io});

							// Add the working storage.
							//output_storage.push_back(working_storage);
							found = true;

							break;
						}
					}
				}
				if (!found) {
					// Not enough available working storage units!

					// Add and claim a new working storage.
					reverse_claimed_working_storages.insert({output_io, working_storages.size()});
					claimed_working_storages.insert({working_storages.size(), output_io});
					working_storages.push_back(Storage(instruction.get_output_sizes().at(output_index), false, Symbol(), "", false, 0));
				}
			}
		}

		// Construct the instruction's working storage.
		//std::vector<Storage> instruction_working_storage;
		// These will be freed right after we emit the instruction, so just
		// create a temporary claim collection.
		std::set<Storage::Index> instruction_claimed_working_storages;
		for (IOIndex working_index = 0; working_index < instruction.get_working_sizes().size(); ++working_index) {
			// Temporarily claim the next available working storage.
			bool found = false;
			for (Storage::Index working_storage_index = 0; working_storage_index < working_storages.size(); ++working_storage_index) {
				if (claimed_working_storages.find(working_storage_index) == claimed_working_storages.cend() && instruction_claimed_working_storages.find(working_storage_index) == instruction_claimed_working_storages.cend()) {
					const Storage &working_storage = working_storages[working_storage_index];
					if (instruction.get_working_sizes().at(working_index) == working_storage.max_size) {
						// Claim the working storage.
						instruction_claimed_working_storages.insert(working_storage_index);

						// Add the working storage.
						//instruction_working_storage.push_back(working_storage);
						found = true;

						break;
					}
				}
			}
			if (!found) {
				// Not enough available working storage units!

				// Add and claim a new working storage.
				instruction_claimed_working_storages.insert(working_storages.size());
				working_storages.push_back(Storage(instruction.get_working_sizes().at(working_index), false, Symbol(), "", false, 0));
			}
		}

		// Concatenate the instruction's storage.
		//std::vector<Storage> instruction_storage(std::move(input_storage));
		//instruction_storage.insert(instruction_storage.end(), instruction_working_storage.cbegin(), instruction_working_storage.cend());
		//instruction_storage.insert(instruction_storage.end(), output_storage.cbegin(), output_storage.cend());

		// Emit the instruction.
		//std::vector<Output::Line> instruction_output;
		//instruction_output = instruction.emit(instruction_storage);
		//output_lines.insert(output_lines.end(), instruction_output.cbegin(), instruction_output.cend());

		// Free working storages: for each input that's in a working storage
		// unit (as opposed to being provided by "input_storages"), check all
		// the other nodes that are using that same output as input.  If there
		// are none, unclaim it.  If all have already been emitted, then this
		// instruction is the last instruction that needs this working storage
		// unit: unclaim it.
		for (IOIndex input_index = 0; input_index < instruction.get_input_sizes().size(); ++input_index) {
			std::map<IO, IO>::const_iterator connections_search = connections.find({this_node, input_index});
			if (connections_search != connections.cend()) {
				// We found a child input node output connection with this
				// node's input.  Just see if for this particular child input
				// node's output, there are any other unemitted nodes that
				// require the same output before we unclaim it.
				const IO child_with_output = connections_search->second;
				bool child_output_has_unemitted_output_nodes = false;

				std::map<IO, std::set<IO>>::const_iterator reversed_connections_search = reversed_connections.find(child_with_output);
				if (reversed_connections_search == reversed_connections.cend()) {
					// How did we get here?
					std::ostringstream sstr;
					sstr
						<< "Semantics::MIPSIO::prepare: internal error: there is a bug in the working storage freeing algorithm, since we're attempting to unlock a working storage that isn't recorded." << std::endl
						<< "\tthis_node (index) : " << this_node << std::endl
						<< "\tinput_index       : " << input_index
						;
					throw SemanticsError(sstr.str());
				} else {
					for (const IO &node_with_input : std::as_const(reversed_connections_search->second)) {
						const Index output_node = node_with_input.first;
						if (visited_instructions.find(output_node) == visited_instructions.cend()) {
							// There is another output node that has not yet been emitted.
							child_output_has_unemitted_output_nodes = true;
							break;
						}
					}
				}

				if (!child_output_has_unemitted_output_nodes) {
					// Unclaim this output.
					const Storage::Index claimed_storage = reverse_claimed_working_storages.at(child_with_output);
					reverse_claimed_working_storages.erase(child_with_output);
					claimed_working_storages.erase(claimed_storage);
				}
			}
		}

		// Finally, if there is another node sequence-connected after this one,
		// push it onto the stack to replace this one, which we've already
		// removed; otherwise, leave this one popped and continue.
		std::map<Index, Index>::const_iterator sequences_search = sequences.find(this_node);
		if (sequences_search != sequences.cend()) {
			Index after_node = sequences_search->second;

			// Detect cycles.
			if (visited_instructions.find(after_node) != visited_instructions.cend()) {
				std::ostringstream sstr;
				sstr << "Semantics::MIPSIO::prepare: error: a cycle, loop, or ordering inconsistency was detected in the instruction graph at index " << after_node << ", which has already been emitted before, but it is sequenced to be after " << this_node << ").";
				throw SemanticsError(sstr.str());
			}

			// Replace this node with the one after it.
			children_stack.push_back(after_node);
		}
	}

	// Make sure there were no unvisited nodes.
	if (visited_instructions.size() < instructions.size()) {
		std::ostringstream sstr;
		sstr
			<< "Semantics::MIPSIO::prepare: error: this algorithm requires all nodes to be reached at least once." << std::endl
			<< "\tvisited : " << visited_instructions.size() << std::endl
			<< "\tnodes   : " << instructions.size()
			;
		throw SemanticsError(sstr.str());
	}

	// Return the emitted output.
	//return output_lines;

	// Return the working storages we found we needed.
	return Storage::get_sizes(working_storages);
}

std::vector<Semantics::Output::Line> Semantics::MIPSIO::emit(const std::map<IO, Storage> &input_storages, const std::vector<Storage> &working_storages, const std::map<IO, Storage> &capture_outputs, bool permit_uncaptured_outputs) const {
	std::map<Index, std::map<IOIndex, Storage>> expanded_capture_outputs = expand_map<Index, IOIndex, Storage>(capture_outputs);

	std::vector<uint32_t> working_storage_sizes = Storage::get_sizes(working_storages);
	std::map<IO, Storage::Index> reverse_claimed_working_storages;  // For a node's output, which working storage units is it using?
	std::map<Storage::Index, IO> claimed_working_storages;          // Which output IO claims a working storage?

	std::vector<Output::Line> output_lines;

	// DFS from each output vertex.  Don't revisit instructions.  Write outputs
	// to available working storage units.  After all of a given node's output
	// index's connected input nodes are emitted, mark the working storage unit
	// that that node's output index was stored to as available.
	//
	// Don't revisit instructions.  Once the last
	// working storage units that are used as inputs are popped, mark them as
	// re-usable.
	std::set<Index>    visited_instructions;
	std::vector<Index> root_stack;
	std::vector<Index> children_stack;
	std::set<Index>    ancestors;  // Detect cycles: DFS can detect already visited notes (e.g. diamond) but not ancestors.
	for (const std::map<Index, std::map<IOIndex, Storage>>::value_type &output_pair : std::as_const(expanded_capture_outputs)) {
		root_stack.push_back(output_pair.first);
	}
	while (children_stack.size() > 0 || root_stack.size() > 0) {
		if (children_stack.size() <= 0 && root_stack.size() > 0) {
			children_stack.push_back(root_stack.back());
			root_stack.pop_back();
		}

		const Index        this_node   = children_stack.back();
		const Instruction &instruction = instructions.at(this_node);

		// If we've already processed (emitted) this node, skip.
		if (visited_instructions.find(this_node) != visited_instructions.cend()) {
			children_stack.pop_back();
		}

		// If this node has unvisited children, push them onto the stack, and
		// continue to process them before revisiting this node, processing it,
		// and marking it as visited.
		bool has_unvisited_children = false;

		for (IOIndex input_index_ = 0; input_index_ < instruction.get_input_sizes().size(); ++input_index_) {
			const IOIndex input_index = instruction.get_input_sizes().size() - 1 - input_index_;

			// Search connections.
			std::map<IO, IO>::const_iterator connections_search = connections.find({this_node, input_index});
			if (connections_search != connections.cend()) {
				const Index child_node = connections_search->second.first;

				// Detect cycles.
				if (ancestors.find(child_node) != ancestors.cend()) {
					std::ostringstream sstr;
					sstr << "Semantics::MIPSIO::emit: error: a cycle was detected in the instruction graph at index " << child_node << " (child of " << this_node << ").";
					throw SemanticsError(sstr.str());
				}

				// Add the child.
				if (visited_instructions.find(child_node) == visited_instructions.cend()) {
					has_unvisited_children = true;
					children_stack.push_back(child_node);
				}
			}

			// Search for a sequence connection for a node that should be emitted before this one.
			std::map<Index, Index>::const_iterator reversed_sequences_search = reversed_sequences.find(this_node);
			if (reversed_sequences_search != reversed_sequences.cend()) {
				Index before_node = reversed_sequences_search->second;

				// Detect cycles.
				if (visited_instructions.find(before_node) != visited_instructions.cend()) {
					std::ostringstream sstr;
					sstr << "Semantics::MIPSIO::emit: error: a cycle was detected in the instruction graph at index " << before_node << " (sequenced after " << this_node << ").";
					throw SemanticsError(sstr.str());
				}

				// Add the child.
				if (visited_instructions.find(before_node) == visited_instructions.cend()) {
					has_unvisited_children = true;
					children_stack.push_back(before_node);
				}
			}
		}

		if (has_unvisited_children) {
			ancestors.insert(this_node);
			continue;
		}

		// We've finished emitting all of the children's nodes.  Mark this one as visited and process (emit) it.
		visited_instructions.insert(this_node);
		ancestors.erase(this_node);
		children_stack.pop_back();  // Save an extra iteration.

		// Emit this node.

		// Construct the instruction's input storage.
		std::vector<Storage> input_storage;
		for (IOIndex input_index = 0; input_index < instruction.get_input_sizes().size(); ++input_index) {
			IO input_io {this_node, input_index};

			// User-supplied input or connection input from another instruction?
			const std::map<IO, Storage>::const_iterator input_storages_search = input_storages.find(input_io);
			const std::map<IO, IO>::const_iterator      connections_search    = connections.find(input_io);
			const bool input_storages_found = input_storages_search != input_storages.cend();
			const bool connections_found    = connections_search    != connections.cend();
			if        ( input_storages_found &&  connections_found) {
				// Conflict.
				std::ostringstream sstr;
				sstr
					<< "Semantics::MIPSIO::emit: error: input/connection conflict in instruction graph: connection input was created for a node but the \"input_storages\" argument also contains a reference to the same input." << std::endl
					<< "\tthis_node (index) : " << this_node << std::endl
					<< "\tinput_index       : " << input_index
					;
				throw SemanticsError(sstr.str());
			} else if (!input_storages_found && !connections_found) {
				// No connection, and no input provided!  (Assuming implementation didn't erroneously free the working storage prematurely.)
				std::ostringstream sstr;
				sstr
					<< "Semantics::MIPSIO::emit: error: input missing without connection in instruction graph: the \"input_storages\" argument contains no reference to an instruction, and no output is provided to by another node through a constructed storage." << std::endl
					<< "\tthis_node (index) : " << this_node << std::endl
					<< "\tinput_index       : " << input_index
					;
				throw SemanticsError(sstr.str());
			}

			if (input_storages_found) {
				input_storage.push_back(input_storages_search->second);
				if (instruction.get_input_sizes().at(input_index) != input_storages_search->second.max_size) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::MIPSIO::emit: error: user-provided input size mismatch in instruction graph: the \"input_storages\" argument contains a reference to an instruction's input, but the size is incorrect." << std::endl
						<< "\tthis_node (index)           : " << this_node << std::endl
						<< "\tinput_index                 : " << input_index << std::endl
						<< "\tprovided input storage size : " << instruction.get_input_sizes().at(input_index) << std::endl
						<< "\tinput storage expected      : " << input_storages_search->second.max_size
						;
					throw SemanticsError(sstr.str());
				}
			} else {
				const IO output_io = connections_search->second;

				const std::map<IO, Storage::Index>::const_iterator reverse_claimed_working_storages_search = reverse_claimed_working_storages.find(output_io);
				const bool reverse_claimed_working_storages_found = reverse_claimed_working_storages_search != reverse_claimed_working_storages.cend();

				if (!reverse_claimed_working_storages_found) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::MIPSIO::emit: internal error: there is a bug because an connected input node should already have been emitted by now but its output storage is not claimed." << std::endl
						<< "\tthis_node (index)        : " << this_node << std::endl
						<< "\toutput node              : " << output_io.first << std::endl
						<< "\toutput node output index : " << output_io.second << std::endl
						<< "\tinput_index              : " << input_index << std::endl
						<< "\tinput storage size       : " << instruction.get_input_sizes().at(input_index)
						;
					throw SemanticsError(sstr.str());
				}

				const Storage &working_storage = working_storages[reverse_claimed_working_storages_search->second];
				input_storage.push_back(working_storage);
				if (working_storage.max_size != instruction.get_input_sizes().at(input_index)) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::MIPSIO::emit: error: connection input size mismatch in instruction graph: in the instruction graph, there is a connection between an output and an input of different sizes." << std::endl
						<< "\tthis_node (index)        : " << this_node << std::endl
						<< "\toutput node              : " << output_io.first << std::endl
						<< "\toutput node output index : " << output_io.second << std::endl
						<< "\tinput_index              : " << input_index << std::endl
						<< "\toutput storage size      : " << reverse_claimed_working_storages_search->second << std::endl
						<< "\tinput storage size       : " << instruction.get_input_sizes().at(input_index)
						;
					throw SemanticsError(sstr.str());
				}
			}
		}

		// Construct the instruction's output storage.
		std::vector<Storage> output_storage;
		for (IOIndex output_index = 0; output_index < instruction.get_output_sizes().size(); ++output_index) {
			IO output_io {this_node, output_index};

			// Is there a user-supplied output storage unit, or do we need to claim required working storage?
			const std::map<IO, Storage>::const_iterator      capture_outputs_search      = capture_outputs.find(output_io);
			const std::map<IO, std::set<IO>>::const_iterator reversed_connections_search = reversed_connections.find(output_io);  // (To check for conflicts.)
			const bool capture_outputs_found    = capture_outputs_search      != capture_outputs.cend();
			const bool reversed_connections_any = reversed_connections_search != reversed_connections.cend() && reversed_connections_search->second.size() > 0;
			if        ( capture_outputs_found &&  reversed_connections_any) {
				// Conflict.
				std::ostringstream sstr;
				sstr
					<< "Semantics::MIPSIO::emit: error: output/connection conflict in instruction graph: connection output was created for a node but the \"capture_outputs\" argument also contains a reference to the same output." << std::endl
					<< "\tthis_node (index) : " << this_node << std::endl
					<< "\toutput_index      : " << output_index
					;
				throw SemanticsError(sstr.str());
			} else if (!capture_outputs_found && !reversed_connections_any) {
				if (!permit_uncaptured_outputs) {
					// Nothing consumes this output.
					std::ostringstream sstr;
					sstr
						<< "Semantics::MIPSIO::emit: error: output capture missing without connection in instruction graph: the \"capture_outputs\" argument contains no reference to an instruction's output, and the output is is not provided to other node." << std::endl
						<< "\tthis_node (index) : " << this_node << std::endl
						<< "\toutput_index      : " << output_index
						;
					throw SemanticsError(sstr.str());
				}
			}

			if (capture_outputs_found) {
				const Storage &working_storage = capture_outputs_search->second;
				output_storage.push_back(working_storage);
				if (working_storage.max_size != instruction.get_output_sizes().at(output_index)) {
					// Output size mismatch.
					std::ostringstream sstr;
					sstr
						<< "Semantics::MIPSIO::emit: error: user-provided output capture size mismatch in instruction graph: the \"capture_outputs\" argument contains a reference to an instruction's output, but the size is incorrect." << std::endl
						<< "\tthis_node (index)                    : " << this_node << std::endl
						<< "\toutput_index                         : " << output_index << std::endl
						<< "\tprovided output capture storage size : " << working_storage.max_size << std::endl
						<< "\toutput storage expected              : " << instruction.get_output_sizes().at(output_index)
						;
					throw SemanticsError(sstr.str());
				}
			} else {
				// Claim the next available working storage.
				bool found = false;
				for (Storage::Index working_storage_index = 0; working_storage_index < working_storages.size(); ++working_storage_index) {
					if (claimed_working_storages.find(working_storage_index) == claimed_working_storages.cend()) {
						const Storage &working_storage = working_storages[working_storage_index];
						if (instruction.get_output_sizes().at(output_index) == working_storage.max_size) {
							// Claim the working storage.
							reverse_claimed_working_storages.insert({output_io, working_storage_index});
							claimed_working_storages.insert({working_storage_index, output_io});

							// Add the working storage.
							output_storage.push_back(working_storage);
							found = true;

							break;
						}
					}
				}
				if (!found) {
					// Not enough available working storage units!
					std::ostringstream sstr;
					sstr
						<< "Semantics::MIPSIO::emit: error: not enough working storage units!: the \"working_storages\" argument does not have any remaining working storage units available for writing an instruction's output." << std::endl
						<< "\tthis_node (index)           : " << this_node << std::endl
						<< "\toutput_index                : " << output_index << std::endl
						<< "\tneeded working storage size : " << instruction.get_output_sizes().at(output_index)
						;
					throw SemanticsError(sstr.str());
				}
			}
		}

		// Construct the instruction's working storage.
		std::vector<Storage> instruction_working_storage;
		// These will be freed right after we emit the instruction, so just
		// create a temporary claim collection.
		std::set<Storage::Index> instruction_claimed_working_storages;
		for (IOIndex working_index = 0; working_index < instruction.get_working_sizes().size(); ++working_index) {
			// Temporarily claim the next available working storage.
			bool found = false;
			for (Storage::Index working_storage_index = 0; working_storage_index < working_storages.size(); ++working_storage_index) {
				if (claimed_working_storages.find(working_storage_index) == claimed_working_storages.cend() && instruction_claimed_working_storages.find(working_storage_index) == instruction_claimed_working_storages.cend()) {
					const Storage &working_storage = working_storages[working_storage_index];
					if (instruction.get_working_sizes().at(working_index) == working_storage.max_size) {
						// Claim the working storage.
						instruction_claimed_working_storages.insert(working_storage_index);

						// Add the working storage.
						instruction_working_storage.push_back(working_storage);
						found = true;

						break;
					}
				}
			}
			if (!found) {
				// Not enough available working storage units!
				std::ostringstream sstr;
				sstr
					<< "Semantics::MIPSIO::emit: error: not enough working storage units!: the \"working_storages\" argument does not have any remaining working storage units available for an instruction." << std::endl
					<< "\tthis_node (index)           : " << this_node << std::endl
					<< "\tworking_index               : " << working_index
					<< "\tneeded working storage size : " << instruction.get_working_sizes().at(working_index)
					;
				throw SemanticsError(sstr.str());
			}
		}

		// Concatenate the instruction's storage.
		std::vector<Storage> instruction_storage(std::move(input_storage));
		instruction_storage.insert(instruction_storage.end(), instruction_working_storage.cbegin(), instruction_working_storage.cend());
		instruction_storage.insert(instruction_storage.end(), output_storage.cbegin(), output_storage.cend());

		// Emit the instruction.
		std::vector<Output::Line> instruction_output;
		instruction_output = instruction.emit(instruction_storage);
		output_lines.insert(output_lines.end(), instruction_output.cbegin(), instruction_output.cend());

		// Free working storages: for each input that's in a working storage
		// unit (as opposed to being provided by "input_storages"), check all
		// the other nodes that are using that same output as input.  If there
		// are none, unclaim it.  If all have already been emitted, then this
		// instruction is the last instruction that needs this working storage
		// unit: unclaim it.
		for (IOIndex input_index = 0; input_index < instruction.get_input_sizes().size(); ++input_index) {
			std::map<IO, IO>::const_iterator connections_search = connections.find({this_node, input_index});
			if (connections_search != connections.cend()) {
				// We found a child input node output connection with this
				// node's input.  Just see if for this particular child input
				// node's output, there are any other unemitted nodes that
				// require the same output before we unclaim it.
				const IO child_with_output = connections_search->second;
				bool child_output_has_unemitted_output_nodes = false;

				std::map<IO, std::set<IO>>::const_iterator reversed_connections_search = reversed_connections.find(child_with_output);
				if (reversed_connections_search == reversed_connections.cend()) {
					// How did we get here?
					std::ostringstream sstr;
					sstr
						<< "Semantics::MIPSIO::emit: internal error: there is a bug in the working storage freeing algorithm, since we're attempting to unlock a working storage that isn't recorded." << std::endl
						<< "\tthis_node (index) : " << this_node << std::endl
						<< "\tinput_index       : " << input_index
						;
					throw SemanticsError(sstr.str());
				} else {
					for (const IO &node_with_input : std::as_const(reversed_connections_search->second)) {
						const Index output_node = node_with_input.first;
						if (visited_instructions.find(output_node) == visited_instructions.cend()) {
							// There is another output node that has not yet been emitted.
							child_output_has_unemitted_output_nodes = true;
							break;
						}
					}
				}

				if (!child_output_has_unemitted_output_nodes) {
					// Unclaim this output.
					const Storage::Index claimed_storage = reverse_claimed_working_storages.at(child_with_output);
					reverse_claimed_working_storages.erase(child_with_output);
					claimed_working_storages.erase(claimed_storage);
				}
			}
		}

		// Finally, if there is another node sequence-connected after this one,
		// push it onto the stack to replace this one, which we've already
		// removed; otherwise, leave this one popped and continue.
		std::map<Index, Index>::const_iterator sequences_search = sequences.find(this_node);
		if (sequences_search != sequences.cend()) {
			Index after_node = sequences_search->second;

			// Detect cycles.
			if (visited_instructions.find(after_node) != visited_instructions.cend()) {
				std::ostringstream sstr;
				sstr << "Semantics::MIPSIO::emit: error: a cycle, loop, or ordering inconsistency was detected in the instruction graph at index " << after_node << ", which has already been emitted before, but it is sequenced to be after " << this_node << ").";
				throw SemanticsError(sstr.str());
			}

			// Replace this node with the one after it.
			children_stack.push_back(after_node);
		}
	}

	// Make sure there were no unvisited nodes.
	if (visited_instructions.size() < instructions.size()) {
		std::ostringstream sstr;
		sstr
			<< "Semantics::MIPSIO::emit: error: this algorithm requires all nodes to be reached at least once." << std::endl
			<< "\tvisited : " << visited_instructions.size() << std::endl
			<< "\tnodes   : " << instructions.size()
			;
		throw SemanticsError(sstr.str());
	}

	// Return the emitted output.
	return output_lines;
}

// | Straightforwardly add an instruction, optionally connecting its
// first arguments with the first output of the instructions
// corresponding to the input indices.
Semantics::MIPSIO::Index Semantics::MIPSIO::add_instruction(const Instruction &instruction, const std::vector<Index> inputs_) {
	std::vector<IO> inputs;
	for (const Index &input : std::as_const(inputs_)) {
		inputs.push_back({input, 0});
	}
	return add_instruction_indexed(instruction, inputs);
}

Semantics::MIPSIO::Index Semantics::MIPSIO::add_instruction(const Instruction &instruction, const std::vector<Index> inputs_, const Index after) {
	std::vector<IO> inputs;
	for (const Index &input : std::as_const(inputs_)) {
		inputs.push_back({input, 0});
	}
	return add_instruction_indexed(instruction, inputs, after);
}

Semantics::MIPSIO::Index Semantics::MIPSIO::add_instruction(const Instruction &instruction, const std::vector<Index> inputs, const std::optional<Index> after) {
	if (after.has_value()) {
		return add_instruction(instruction, inputs, *after);
	} else {
		return add_instruction(instruction, inputs);
	}
}

//Semantics::MIPSIO::Index Semantics::MIPSIO::add_instruction(const Instruction &instruction, const Index after) {
//	return add_instruction_indexed(instruction, after);
//}

// | Same as before, but allow specification of which output in case there are multiple outputs.
Semantics::MIPSIO::Index Semantics::MIPSIO::add_instruction_indexed(const Instruction &instruction, const std::vector<IO> inputs) {
	Index index = instructions.size();

	instructions.push_back(instruction);

	for (const IO &input : std::as_const(inputs)) {
		IOIndex input_index = &input - &inputs[0];
		add_connection(input, {index, input_index});
	}

	return index;
}

Semantics::MIPSIO::Index Semantics::MIPSIO::add_instruction_indexed(const Instruction &instruction, const std::vector<IO> inputs, const Index after) {
	Index index = add_instruction_indexed(instruction, inputs);
	add_sequence_connection(after, index);
	return index;
}

Semantics::MIPSIO::Index Semantics::MIPSIO::add_instruction_indexed(const Instruction &instruction, const std::vector<IO> inputs, const std::optional<Index> after) {
	if (after.has_value()) {
		return add_instruction_indexed(instruction, inputs, *after);
	} else {
		return add_instruction_indexed(instruction, inputs);
	}
}

//Semantics::MIPSIO::Index Semantics::MIPSIO::add_instruction_indexed(const Instruction &instruction, const Index after) {
//	return add_instruction_indexed(instruction, {}, after);
//}

// | Set "output"'s given output as "input"'s given input.
void Semantics::MIPSIO::add_connection(IO output, IO input) {
	if (connections.find(input) != connections.cend()) {
		std::ostringstream sstr;
		sstr
			<< "Semantics::MIPSIO::add_connection: error: attempt to add a connection to an input that already has a connection." << std::endl
			<< "\toutput node                : " << output.first << std::endl
			<< "\toutput node's output index : " << output.first << std::endl
			<< "\tinput node                 : " << input.first << std::endl
			<< "\tinput node's input index   : " << input.first
			;
		throw SemanticsError(sstr.str());
	}

	connections.insert({input, output});

	std::map<IO, std::set<IO>>::iterator reversed_connections_search = reversed_connections.find(output);
	if (reversed_connections_search == reversed_connections.end()) {
		reversed_connections.insert({output, {input}});
	} else {
		reversed_connections_search->second.insert(input);
	}
}

// | Right after "before" is emitted, emit "after"'s unemitted children and then "after".
void Semantics::MIPSIO::add_sequence_connection(Index before, Index after) {
	if (sequences.find(before) != sequences.cend()) {
		std::ostringstream sstr;
		sstr
			<< "Semantics::MIPSIO::add_sequence_connection: error: attempt to add a sequence connection to a \"before\" instruction that already has a connection to an \"after\" instruction that should be emitted (after \"after\"'s unemitted children if any) right after \"before\" is emitted." << std::endl
			<< "\tbefore node : " << before << std::endl
			<< "\tafter node  : " << after
			;
		throw SemanticsError(sstr.str());
	}

	sequences.insert({before, after});

	if (reversed_sequences.find(after) != reversed_sequences.cend()) {
		std::ostringstream sstr;
		sstr
			<< "Semantics::MIPSIO::add_sequence_connection: error: attempt to add a sequence connection to an \"after\" instruction that already has a connection to a \"before\" instruction connected to it." << std::endl
			<< "\tafter node  : " << after << std::endl
			<< "\tbefore node : " << before
			;
		throw SemanticsError(sstr.str());
	}

	reversed_sequences.insert({after, before});
}

void Semantics::MIPSIO::add_sequence_connection(std::pair<Index, Index> before_after) {
	return add_sequence_connection(before_after.first, before_after.second);
}

void Semantics::MIPSIO::add_sequence_connections(const std::vector<std::pair<Index, Index>> before_afters) {
	for (const std::pair<Index, Index> &before_after : std::as_const(before_afters)) {
		add_sequence_connection(before_after);
	}
}

void Semantics::MIPSIO::add_sequence_connections(const std::vector<Index> befores, const std::vector<Index> afters) {
	if (befores.size() != afters.size()) {
		std::ostringstream sstr;
		sstr
			<< "Semantics::MIPSIO::add_sequence_connection: error: the \"befores\" and \"afters\" vectors have different lengths." << std::endl
			<< "\tbefores length : " << befores.size() << std::endl
			<< "\tafters length  : " << afters.size()
			;
		throw SemanticsError(sstr.str());
	}

	for (std::vector<Index>::size_type befores_afters_index = 0; befores_afters_index < befores.size(); ++befores_afters_index) {
		const Index &before = befores[befores_afters_index];
		const Index &after  = afters[befores_afters_index];
		add_sequence_connection(before, after);
	}
}

// | Add "other"'s instructions to this; any indices returned from
// "add_instruction" into "other" (but not "this") must be added by the
// returned value to remain correct.
Semantics::MIPSIO::Index Semantics::MIPSIO::merge(const MIPSIO &other) {
	Index addition = instructions.size();

	instructions.insert(instructions.end(), other.instructions.cbegin(), other.instructions.cend());

	for (const std::map<IO, IO>::value_type &connection : std::as_const(other.connections)) {
		const IO &input  = connection.first;
		const IO &output = connection.second;
		const IO new_input {input.first + addition, input.second};
		const IO new_output {output.first + addition, output.second};
		connections.insert({new_input, new_output});

		std::map<IO, std::set<IO>>::iterator reversed_connections_search = reversed_connections.find(new_output);
		if (reversed_connections_search == reversed_connections.end()) {
			reversed_connections.insert({new_output, {new_input}});
		} else {
			reversed_connections_search->second.insert(new_input);
		}
	}

	for (const std::map<Index, Index>::value_type &sequence : std::as_const(other.sequences)) {
		const Index before     = sequence.first;
		const Index after      = sequence.second;
		const Index new_before = before + addition;
		const Index new_after  = after + addition;
		sequences.insert({new_before, new_after});
		reversed_sequences.insert({new_after, new_before});
	}

	return addition;
}

Semantics::Expression::Expression() {}

Semantics::Expression::Expression(const MIPSIO  &instructions, const Type  &output_type, MIPSIO::Index output_index, uint64_t lexeme_begin, uint64_t lexeme_end) : instructions(          instructions ), output_type(          output_type ), output_index(output_index), lexeme_begin(lexeme_begin), lexeme_end(lexeme_end) {}
Semantics::Expression::Expression(const MIPSIO  &instructions,       Type &&output_type, MIPSIO::Index output_index, uint64_t lexeme_begin, uint64_t lexeme_end) : instructions(          instructions ), output_type(std::move(output_type)), output_index(output_index), lexeme_begin(lexeme_begin), lexeme_end(lexeme_end) {}
Semantics::Expression::Expression(      MIPSIO &&instructions, const Type  &output_type, MIPSIO::Index output_index, uint64_t lexeme_begin, uint64_t lexeme_end) : instructions(std::move(instructions)), output_type(          output_type ), output_index(output_index), lexeme_begin(lexeme_begin), lexeme_end(lexeme_end) {}
Semantics::Expression::Expression(      MIPSIO &&instructions,       Type &&output_type, MIPSIO::Index output_index, uint64_t lexeme_begin, uint64_t lexeme_end) : instructions(std::move(instructions)), output_type(std::move(output_type)), output_index(output_index), lexeme_begin(lexeme_begin), lexeme_end(lexeme_end) {}

Semantics::Expression Semantics::analyze_expression(uint64_t expression, const IdentifierScope &constant_scope, const IdentifierScope &type_scope, const IdentifierScope &var_scope, const IdentifierScope &combined_scope) {
	return analyze_expression(grammar.expression_storage.at(expression), constant_scope, type_scope, var_scope, combined_scope);
}

Semantics::Expression Semantics::analyze_expression(const ::Expression &expression, const IdentifierScope &constant_scope, const IdentifierScope &type_scope, const IdentifierScope &var_scope, const IdentifierScope &combined_scope) {
	// Some type aliases to improve readability.
	using M = Semantics::MIPSIO;
	using I = Semantics::Instruction;
	using B = Semantics::Instruction::Base;
	using Index = M::Index;
	using IO    = M::IO;
	using ConstantValue = Semantics::ConstantValue;
	using Output        = Semantics::Output;
	using Storage       = Semantics::Storage;
	using Symbol        = Semantics::Symbol;

	// TODO
	return Expression();

	// Alias for expression.
	const ::Expression &expression_symbol = expression;

	// Prepare the semantics expression.
	Expression expression_semantics;

	// First, see if this expression is a constant value.
	ConstantValue constant_value = is_expression_constant(expression_symbol, constant_scope);
	if (constant_value.is_static()) {
		expression_semantics.output_type  = constant_value.get_static_type();
		expression_semantics.output_index = expression_semantics.instructions.add_instruction({I::LoadImmediate(B(), constant_value.get_static_primitive_type().is_word(), constant_value)});
	} else {
		// Branch according to the expression type.
		switch (expression_symbol.branch) {
			// These 16 branches are static iff all subexpressions are static, but they aren't.
			case ::Expression::pipe_branch: {
				const ::Expression::Pipe &pipe           = grammar.expression_pipe_storage.at(expression_symbol.data);
				const ::Expression       &expression0    = grammar.expression_storage.at(pipe.expression0);
				const LexemeOperator     &pipe_operator0 = grammar.lexemes.at(pipe.pipe_operator0).get_operator();
				const ::Expression       &expression1    = grammar.expression_storage.at(pipe.expression1);

				// Get left and right subexpressions.
				const Expression left  = analyze_expression(expression0, constant_scope, type_scope, var_scope, combined_scope);
				const Expression right = analyze_expression(expression1, constant_scope, type_scope, var_scope, combined_scope);

				// Make sure left and right are of primitive types.
				if (!left.output_type.resolve_type().is_primitive() || !right.output_type.resolve_type().is_primitive()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< pipe_operator0.line << " col " << pipe_operator0.column
						<< "): cannot apply bitwise OR on a non-primitive-typed expression, for "
						<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}
				const Type::Primitive &left_type  = left.output_type.resolve_type().get_primitive();
				const Type::Primitive &right_type = left.output_type.resolve_type().get_primitive();

				// Are the expressions of the same type?
				if (left.output_type.tag != right.output_type.tag) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< pipe_operator0.line << " col " << pipe_operator0.column
						<< "): refusing to OR values of different types, for "
						<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Are we attempting to operate on a string?
				if (left_type.is_string() || right_type.is_string()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< pipe_operator0.line << " col " << pipe_operator0.column
						<< "): cannot apply bitwise OR on a string expression, for "
						<< left_type.get_tag_repr() << " with " << right_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Apply bitwise OR depending on the integer type.
				expression_semantics.output_type = left.output_type;
				const Index left_index  = expression_semantics.instructions.merge(left.instructions);
				const Index right_index = expression_semantics.instructions.merge(right.instructions);
				const Index or_index    = expression_semantics.instructions.add_instruction({I::OrFrom(B(), left_type.is_word())}, {left.output_index + left_index, right.output_index + right_index});
				expression_semantics.output_index = or_index;
				break;
			} case ::Expression::ampersand_branch: {
				const ::Expression::Ampersand &ampersand           = grammar.expression_ampersand_storage.at(expression_symbol.data);
				const ::Expression            &expression0         = grammar.expression_storage.at(ampersand.expression0);
				const LexemeOperator          &ampersand_operator0 = grammar.lexemes.at(ampersand.ampersand_operator0).get_operator();
				const ::Expression            &expression1         = grammar.expression_storage.at(ampersand.expression1);

				// Get left and right subexpressions.
				const Expression left  = analyze_expression(expression0, constant_scope, type_scope, var_scope, combined_scope);
				const Expression right = analyze_expression(expression1, constant_scope, type_scope, var_scope, combined_scope);
				expression_semantics.lexeme_begin = left.lexeme_begin;
				expression_semantics.lexeme_end   = right.lexeme_end;

				// Make sure left and right are of primitive types.
				if (!left.output_type.resolve_type().is_primitive() || !right.output_type.resolve_type().is_primitive()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< ampersand_operator0.line << " col " << ampersand_operator0.column
						<< "): cannot apply bitwise AND on a non-primitive-typed expression, for "
						<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}
				const Type::Primitive &left_type  = left.output_type.resolve_type().get_primitive();
				const Type::Primitive &right_type = left.output_type.resolve_type().get_primitive();

				// Are the expressions of the same type?
				if (left.output_type.tag != right.output_type.tag) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< ampersand_operator0.line << " col " << ampersand_operator0.column
						<< "): refusing to AND values of different types, for "
						<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Are we attempting to operate on a string?
				if (left_type.is_string() || right_type.is_string()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< ampersand_operator0.line << " col " << ampersand_operator0.column
						<< "): cannot apply bitwise AND on a string expression, for "
						<< left_type.get_tag_repr() << " with " << right_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Apply bitwise AND depending on the integer type.
				expression_semantics.output_type = left.output_type;
				const Index left_index  = expression_semantics.instructions.merge(left.instructions);
				const Index right_index = expression_semantics.instructions.merge(right.instructions);
				const Index and_index   = expression_semantics.instructions.add_instruction({I::AndFrom(B(), left_type.is_word())}, {left.output_index + left_index, right.output_index + right_index});
				expression_semantics.output_index = and_index;
				break;
			} case ::Expression::equals_branch: {
				const ::Expression::Equals &equals           = grammar.expression_equals_storage.at(expression_symbol.data);
				const ::Expression         &expression0      = grammar.expression_storage.at(equals.expression0);
				const LexemeOperator       &equals_operator0 = grammar.lexemes.at(equals.equals_operator0).get_operator();
				const ::Expression         &expression1      = grammar.expression_storage.at(equals.expression1);

				// Get left and right subexpressions.
				const Expression left  = analyze_expression(expression0, constant_scope, type_scope, var_scope, combined_scope);
				const Expression right = analyze_expression(expression1, constant_scope, type_scope, var_scope, combined_scope);
				expression_semantics.lexeme_begin = left.lexeme_begin;
				expression_semantics.lexeme_end   = right.lexeme_end;

				// Are the expressions of the same type?
				if (left.output_type.tag != right.output_type.tag) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< equals_operator0.line << " col " << equals_operator0.column
						<< "): refusing to compare values of different types for =, for "
						<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Apply = comparison depending on the type.
				if        (left.output_type.resolve_type().is_primitive()) {
					const Type::Primitive &left_type = left.output_type.resolve_type().get_primitive();
					if (!left_type.is_string()) {
						expression_semantics.output_type  = Type::boolean_type;
						const Index left_index            = expression_semantics.instructions.merge(left.instructions);
						const Index right_index           = expression_semantics.instructions.merge(right.instructions);
						const Index sub_index             = expression_semantics.instructions.add_instruction({I::SubFrom(B(), left_type.is_word())}, {left.output_index + left_index, right.output_index + right_index});
						const Index load_1_index          = expression_semantics.instructions.add_instruction({I::LoadImmediate(B(), left_type.is_word(), ConstantValue(static_cast<int32_t>(1), 0, 0))});
						const Index eq_index              = expression_semantics.instructions.add_instruction({I::LessThanFrom(B(), left_type.is_word())}, {sub_index, load_1_index});
						expression_semantics.output_index = eq_index;
					} else {
						std::ostringstream sstr;
						sstr
							<< "Semantics::analyze_expression: error (line "
							<< equals_operator0.line << " col " << equals_operator0.column
							<< "): comparison of string types is not yet supported, for "
							<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
							<< "."
							;
						throw SemanticsError(sstr.str());
					}
				} else if (left.output_type.resolve_type().is_record() || left.output_type.resolve_type().is_array()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< equals_operator0.line << " col " << equals_operator0.column
						<< "): comparison of record or array types is not yet supported, for "
						<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				} else {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: internal error (line "
						<< equals_operator0.line << " col " << equals_operator0.column
						<< "): unhandled expression type for = comparison: "
						<< left.output_type.get_tag_repr()
						;
					throw SemanticsError(sstr.str());
				}
				break;
			} case ::Expression::lt_or_gt_branch: {
				const ::Expression::LtOrGt &lt_or_gt           = grammar.expression_lt_or_gt_storage.at(expression_symbol.data);
				const ::Expression         &expression0        = grammar.expression_storage.at(lt_or_gt.expression0);
				const LexemeOperator       &lt_or_gt_operator0 = grammar.lexemes.at(lt_or_gt.lt_or_gt_operator0).get_operator();
				const ::Expression         &expression1        = grammar.expression_storage.at(lt_or_gt.expression1);

				// Get left and right subexpressions.
				const Expression left  = analyze_expression(expression0, constant_scope, type_scope, var_scope, combined_scope);
				const Expression right = analyze_expression(expression1, constant_scope, type_scope, var_scope, combined_scope);
				expression_semantics.lexeme_begin = left.lexeme_begin;
				expression_semantics.lexeme_end   = right.lexeme_end;

				// Are the expressions of the same type?
				if (left.output_type.tag != right.output_type.tag) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< lt_or_gt_operator0.line << " col " << lt_or_gt_operator0.column
						<< "): refusing to compare values of different types for <>, for "
						<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Apply <> comparison depending on the type.
				if        (left.output_type.resolve_type().is_primitive()) {
					const Type::Primitive &left_type = left.output_type.resolve_type().get_primitive();
					if (!left_type.is_string()) {
						expression_semantics.output_type  = Type::boolean_type;
						const Index left_index            = expression_semantics.instructions.merge(left.instructions);
						const Index right_index           = expression_semantics.instructions.merge(right.instructions);
						const Index sub_index             = expression_semantics.instructions.add_instruction({I::SubFrom(B(), left_type.is_word())}, {left.output_index + left_index, right.output_index + right_index});
						const Index load_1_index          = expression_semantics.instructions.add_instruction({I::LoadImmediate(B(), left_type.is_word(), ConstantValue(static_cast<int32_t>(1), 0, 0))});
						const Index eq_index              = expression_semantics.instructions.add_instruction({I::LessThanFrom(B(), left_type.is_word())}, {sub_index, load_1_index});
						const Index neq_index             = expression_semantics.instructions.add_instruction({I::LessThanFrom(B(), left_type.is_word())}, {eq_index, load_1_index});
						expression_semantics.output_index = neq_index;
					} else {
						std::ostringstream sstr;
						sstr
							<< "Semantics::analyze_expression: error (line "
							<< lt_or_gt_operator0.line << " col " << lt_or_gt_operator0.column
							<< "): comparison of string types is not yet supported, for "
							<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
							<< "."
							;
						throw SemanticsError(sstr.str());
					}
				} else if (left.output_type.resolve_type().is_record() || left.output_type.resolve_type().is_array()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< lt_or_gt_operator0.line << " col " << lt_or_gt_operator0.column
						<< "): comparison of record or array types is not yet supported, for "
						<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				} else {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: internal error (line "
						<< lt_or_gt_operator0.line << " col " << lt_or_gt_operator0.column
						<< "): unhandled expression type for <> comparison: "
						<< left.output_type.get_tag_repr()
						;
					throw SemanticsError(sstr.str());
				}
				break;
			} case ::Expression::le_branch: {
				const ::Expression::Le &le           = grammar.expression_le_storage.at(expression_symbol.data);
				const ::Expression     &expression0  = grammar.expression_storage.at(le.expression0);
				const LexemeOperator   &le_operator0 = grammar.lexemes.at(le.le_operator0).get_operator();
				const ::Expression     &expression1  = grammar.expression_storage.at(le.expression1);

				// Get left and right subexpressions.
				const Expression left  = analyze_expression(expression0, constant_scope, type_scope, var_scope, combined_scope);
				const Expression right = analyze_expression(expression1, constant_scope, type_scope, var_scope, combined_scope);
				expression_semantics.lexeme_begin = left.lexeme_begin;
				expression_semantics.lexeme_end   = right.lexeme_end;

				// Are the expressions of the same type?
				if (left.output_type.tag != right.output_type.tag) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< le_operator0.line << " col " << le_operator0.column
						<< "): refusing to compare values of different types for <=, for "
						<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Apply <= comparison depending on the type.
				if        (left.output_type.resolve_type().is_primitive()) {
					const Type::Primitive &left_type = left.output_type.resolve_type().get_primitive();
					if (!left_type.is_string()) {
						expression_semantics.output_type  = Type::boolean_type;
						const Index left_index            = expression_semantics.instructions.merge(left.instructions);
						const Index right_index           = expression_semantics.instructions.merge(right.instructions);
						const Index lt_index              = expression_semantics.instructions.add_instruction({I::LessThanFrom(B(), left_type.is_word(), true)}, {left.output_index + left_index, right.output_index + right_index});
						const Index sub_index             = expression_semantics.instructions.add_instruction({I::SubFrom(B(), left_type.is_word())}, {left.output_index + left_index, right.output_index + right_index});
						const Index load_1_index          = expression_semantics.instructions.add_instruction({I::LoadImmediate(B(), left_type.is_word(), ConstantValue(static_cast<int32_t>(1), 0, 0))});
						const Index eq_index              = expression_semantics.instructions.add_instruction({I::LessThanFrom(B(), left_type.is_word())}, {sub_index, load_1_index});
						const Index le_index              = expression_semantics.instructions.add_instruction({I::OrFrom(B(), false)}, {eq_index, lt_index});
						expression_semantics.output_index = le_index;
					} else {
						std::ostringstream sstr;
						sstr
							<< "Semantics::analyze_expression: error (line "
							<< le_operator0.line << " col " << le_operator0.column
							<< "): comparison of string types is not yet supported, for "
							<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
							<< "."
							;
						throw SemanticsError(sstr.str());
					}
				} else if (left.output_type.resolve_type().is_record() || left.output_type.resolve_type().is_array()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< le_operator0.line << " col " << le_operator0.column
						<< "): comparison of record or array types is not yet supported, for "
						<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				} else {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: internal error (line "
						<< le_operator0.line << " col " << le_operator0.column
						<< "): unhandled expression type for <= comparison: "
						<< left.output_type.get_tag_repr()
						;
					throw SemanticsError(sstr.str());
				}
				break;
			} case ::Expression::ge_branch: {
				const ::Expression::Ge &ge           = grammar.expression_ge_storage.at(expression_symbol.data);
				const ::Expression     &expression0  = grammar.expression_storage.at(ge.expression0);
				const LexemeOperator   &ge_operator0 = grammar.lexemes.at(ge.ge_operator0).get_operator();
				const ::Expression     &expression1  = grammar.expression_storage.at(ge.expression1);

				// Get left and right subexpressions.
				const Expression left  = analyze_expression(expression0, constant_scope, type_scope, var_scope, combined_scope);
				const Expression right = analyze_expression(expression1, constant_scope, type_scope, var_scope, combined_scope);
				expression_semantics.lexeme_begin = left.lexeme_begin;
				expression_semantics.lexeme_end   = right.lexeme_end;

				// Are the expressions of the same type?
				if (left.output_type.tag != right.output_type.tag) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< ge_operator0.line << " col " << ge_operator0.column
						<< "): refusing to compare values of different types for >=, for "
						<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Apply >= comparison depending on the type.
				if        (left.output_type.resolve_type().is_primitive()) {
					const Type::Primitive &left_type = left.output_type.resolve_type().get_primitive();
					if (!left_type.is_string()) {
						expression_semantics.output_type  = Type::boolean_type;
						const Index left_index            = expression_semantics.instructions.merge(left.instructions);
						const Index right_index           = expression_semantics.instructions.merge(right.instructions);
						const Index gt_index              = expression_semantics.instructions.add_instruction({I::LessThanFrom(B(), left_type.is_word(), true)}, {right.output_index + right_index, left.output_index + left_index});
						const Index sub_index             = expression_semantics.instructions.add_instruction({I::SubFrom(B(), left_type.is_word())}, {left.output_index + left_index, right.output_index + right_index});
						const Index load_1_index          = expression_semantics.instructions.add_instruction({I::LoadImmediate(B(), left_type.is_word(), ConstantValue(static_cast<int32_t>(1), 0, 0))});
						const Index eq_index              = expression_semantics.instructions.add_instruction({I::LessThanFrom(B(), left_type.is_word())}, {sub_index, load_1_index});
						const Index ge_index              = expression_semantics.instructions.add_instruction({I::OrFrom(B(), false)}, {eq_index, gt_index});
						expression_semantics.output_index = ge_index;
					} else {
						std::ostringstream sstr;
						sstr
							<< "Semantics::analyze_expression: error (line "
							<< ge_operator0.line << " col " << ge_operator0.column
							<< "): comparison of string types is not yet supported, for "
							<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
							<< "."
							;
						throw SemanticsError(sstr.str());
					}
				} else if (left.output_type.resolve_type().is_record() || left.output_type.resolve_type().is_array()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< ge_operator0.line << " col " << ge_operator0.column
						<< "): comparison of record or array types is not yet supported, for "
						<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				} else {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: internal error (line "
						<< ge_operator0.line << " col " << ge_operator0.column
						<< "): unhandled expression type for >= comparison: "
						<< left.output_type.get_tag_repr()
						;
					throw SemanticsError(sstr.str());
				}
				break;
			} case ::Expression::lt_branch: {
				const ::Expression::Lt &lt           = grammar.expression_lt_storage.at(expression_symbol.data);
				const ::Expression     &expression0  = grammar.expression_storage.at(lt.expression0);
				const LexemeOperator   &lt_operator0 = grammar.lexemes.at(lt.lt_operator0).get_operator();
				const ::Expression     &expression1  = grammar.expression_storage.at(lt.expression1);

				// Get left and right subexpressions.
				const Expression left  = analyze_expression(expression0, constant_scope, type_scope, var_scope, combined_scope);
				const Expression right = analyze_expression(expression1, constant_scope, type_scope, var_scope, combined_scope);
				expression_semantics.lexeme_begin = left.lexeme_begin;
				expression_semantics.lexeme_end   = right.lexeme_end;

				// Are the expressions of the same type?
				if (left.output_type.tag != right.output_type.tag) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< lt_operator0.line << " col " << lt_operator0.column
						<< "): refusing to compare values of different types for <, for "
						<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Apply < comparison depending on the type.
				if        (left.output_type.resolve_type().is_primitive()) {
					const Type::Primitive &left_type = left.output_type.resolve_type().get_primitive();
					if (!left_type.is_string()) {
						expression_semantics.output_type  = Type::boolean_type;
						const Index left_index            = expression_semantics.instructions.merge(left.instructions);
						const Index right_index           = expression_semantics.instructions.merge(right.instructions);
						const Index lt_index              = expression_semantics.instructions.add_instruction({I::LessThanFrom(B(), left_type.is_word(), true)}, {left.output_index + left_index, right.output_index + right_index});
						expression_semantics.output_index = lt_index;
					} else {
						std::ostringstream sstr;
						sstr
							<< "Semantics::analyze_expression: error (line "
							<< lt_operator0.line << " col " << lt_operator0.column
							<< "): comparison of string types is not yet supported, for "
							<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
							<< "."
							;
						throw SemanticsError(sstr.str());
					}
				} else if (left.output_type.resolve_type().is_record() || left.output_type.resolve_type().is_array()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< lt_operator0.line << " col " << lt_operator0.column
						<< "): comparison of record or array types is not yet supported, for "
						<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				} else {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: internal error (line "
						<< lt_operator0.line << " col " << lt_operator0.column
						<< "): unhandled expression type for < comparison: "
						<< left.output_type.get_tag_repr()
						;
					throw SemanticsError(sstr.str());
				}
				break;
			} case ::Expression::gt_branch: {
				const ::Expression::Gt &gt           = grammar.expression_gt_storage.at(expression_symbol.data);
				const ::Expression     &expression0  = grammar.expression_storage.at(gt.expression0);
				const LexemeOperator   &gt_operator0 = grammar.lexemes.at(gt.gt_operator0).get_operator();
				const ::Expression     &expression1  = grammar.expression_storage.at(gt.expression1);

				// Get left and right subexpressions.
				const Expression left  = analyze_expression(expression0, constant_scope, type_scope, var_scope, combined_scope);
				const Expression right = analyze_expression(expression1, constant_scope, type_scope, var_scope, combined_scope);
				expression_semantics.lexeme_begin = left.lexeme_begin;
				expression_semantics.lexeme_end   = right.lexeme_end;

				// Are the expressions of the same type?
				if (left.output_type.tag != right.output_type.tag) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< gt_operator0.line << " col " << gt_operator0.column
						<< "): refusing to compare values of different types for >, for "
						<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Apply > comparison depending on the type.
				if        (left.output_type.resolve_type().is_primitive()) {
					const Type::Primitive &left_type = left.output_type.resolve_type().get_primitive();
					if (!left_type.is_string()) {
						expression_semantics.output_type  = Type::boolean_type;
						const Index left_index            = expression_semantics.instructions.merge(left.instructions);
						const Index right_index           = expression_semantics.instructions.merge(right.instructions);
						const Index gt_index              = expression_semantics.instructions.add_instruction({I::LessThanFrom(B(), left_type.is_word(), true)}, {right.output_index + right_index, left.output_index + left_index});
						expression_semantics.output_index = gt_index;
					} else {
						std::ostringstream sstr;
						sstr
							<< "Semantics::analyze_expression: error (line "
							<< gt_operator0.line << " col " << gt_operator0.column
							<< "): comparison of string types is not yet supported, for "
							<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
							<< "."
							;
						throw SemanticsError(sstr.str());
					}
				} else if (left.output_type.resolve_type().is_record() || left.output_type.resolve_type().is_array()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< gt_operator0.line << " col " << gt_operator0.column
						<< "): comparison of record or array types is not yet supported, for "
						<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				} else {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: internal error (line "
						<< gt_operator0.line << " col " << gt_operator0.column
						<< "): unhandled expression type for > comparison: "
						<< left.output_type.get_tag_repr()
						;
					throw SemanticsError(sstr.str());
				}
				break;
			} case ::Expression::plus_branch: {
				const ::Expression::Plus &plus           = grammar.expression_plus_storage.at(expression_symbol.data);
				const ::Expression       &expression0    = grammar.expression_storage.at(plus.expression0);
				const LexemeOperator     &plus_operator0 = grammar.lexemes.at(plus.plus_operator0).get_operator();
				const ::Expression       &expression1    = grammar.expression_storage.at(plus.expression1);

				// Get left and right subexpressions.
				const Expression left  = analyze_expression(expression0, constant_scope, type_scope, var_scope, combined_scope);
				const Expression right = analyze_expression(expression1, constant_scope, type_scope, var_scope, combined_scope);
				expression_semantics.lexeme_begin = left.lexeme_begin;
				expression_semantics.lexeme_end   = right.lexeme_end;

				// Make sure left and right are of primitive types.
				if (!left.output_type.resolve_type().is_primitive() || !right.output_type.resolve_type().is_primitive()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< plus_operator0.line << " col " << plus_operator0.column
						<< "): cannot apply addition on a non-primitive-typed expression, for "
						<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}
				const Type::Primitive &left_type  = left.output_type.resolve_type().get_primitive();
				const Type::Primitive &right_type = left.output_type.resolve_type().get_primitive();

				// Are we attempting to operate on a non-integer?
				if (left_type.is_char() || left_type.is_boolean() || right_type.is_char() || right_type.is_boolean()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< plus_operator0.line << " col " << plus_operator0.column
						<< "): refusing to apply addition on a non-integer, for "
						<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Are the expressions of the same type?
				if (left.output_type.tag != right.output_type.tag) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< plus_operator0.line << " col " << plus_operator0.column
						<< "): refusing to add values of different types, for "
						<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Are we attempting to operate on a string?
				if (left_type.is_string() || right_type.is_string()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< plus_operator0.line << " col " << plus_operator0.column
						<< "): cannot apply addition on a string expression, for "
						<< left_type.get_tag_repr() << " with " << right_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Apply addition depending on the integer type.
				expression_semantics.output_type = left.output_type;
				const Index left_index  = expression_semantics.instructions.merge(left.instructions);
				const Index right_index = expression_semantics.instructions.merge(right.instructions);
				const Index add_index   = expression_semantics.instructions.add_instruction({I::AddFrom(B(), left_type.is_word())}, {left.output_index + left_index, right.output_index + right_index});
				expression_semantics.output_index = add_index;
				break;
			} case ::Expression::minus_branch: {
				const ::Expression::Minus &minus           = grammar.expression_minus_storage.at(expression_symbol.data);
				const ::Expression        &expression0     = grammar.expression_storage.at(minus.expression0);
				const LexemeOperator      &minus_operator0 = grammar.lexemes.at(minus.minus_operator0).get_operator();
				const ::Expression        &expression1     = grammar.expression_storage.at(minus.expression1);

				// Get left and right subexpressions.
				const Expression left  = analyze_expression(expression0, constant_scope, type_scope, var_scope, combined_scope);
				const Expression right = analyze_expression(expression1, constant_scope, type_scope, var_scope, combined_scope);
				expression_semantics.lexeme_begin = left.lexeme_begin;
				expression_semantics.lexeme_end   = right.lexeme_end;

				// Make sure left and right are of primitive types.
				if (!left.output_type.resolve_type().is_primitive() || !right.output_type.resolve_type().is_primitive()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< minus_operator0.line << " col " << minus_operator0.column
						<< "): cannot apply subtraction on a non-primitive-typed expression, for "
						<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}
				const Type::Primitive &left_type  = left.output_type.resolve_type().get_primitive();
				const Type::Primitive &right_type = left.output_type.resolve_type().get_primitive();

				// Are we attempting to operate on a non-integer?
				if (left_type.is_char() || left_type.is_boolean() || right_type.is_char() || right_type.is_boolean()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< minus_operator0.line << " col " << minus_operator0.column
						<< "): refusing to apply subtraction a non-integer, for "
						<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Are the expressions of the same type?
				if (left.output_type.tag != right.output_type.tag) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< minus_operator0.line << " col " << minus_operator0.column
						<< "): refusing to subtract values of different types, for "
						<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Are we attempting to operate on a string?
				if (left_type.is_string() || right_type.is_string()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< minus_operator0.line << " col " << minus_operator0.column
						<< "): cannot apply subtraction on a string expression, for "
						<< left_type.get_tag_repr() << " with " << right_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Apply subtraction depending on the integer type.
				expression_semantics.output_type = left.output_type;
				const Index left_index  = expression_semantics.instructions.merge(left.instructions);
				const Index right_index = expression_semantics.instructions.merge(right.instructions);
				const Index sub_index   = expression_semantics.instructions.add_instruction({I::SubFrom(B(), left_type.is_word())}, {left.output_index + left_index, right.output_index + right_index});
				expression_semantics.output_index = sub_index;
				break;
			} case ::Expression::times_branch: {
				const ::Expression::Times &times           = grammar.expression_times_storage.at(expression_symbol.data);
				const ::Expression        &expression0     = grammar.expression_storage.at(times.expression0);
				const LexemeOperator      &times_operator0 = grammar.lexemes.at(times.times_operator0).get_operator();
				const ::Expression        &expression1     = grammar.expression_storage.at(times.expression1);

				// Get left and right subexpressions.
				const Expression left  = analyze_expression(expression0, constant_scope, type_scope, var_scope, combined_scope);
				const Expression right = analyze_expression(expression1, constant_scope, type_scope, var_scope, combined_scope);
				expression_semantics.lexeme_begin = left.lexeme_begin;
				expression_semantics.lexeme_end   = right.lexeme_end;

				// Make sure left and right are of primitive types.
				if (!left.output_type.resolve_type().is_primitive() || !right.output_type.resolve_type().is_primitive()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< times_operator0.line << " col " << times_operator0.column
						<< "): cannot apply multiplication on a non-primitive-typed expression, for "
						<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}
				const Type::Primitive &left_type  = left.output_type.resolve_type().get_primitive();
				const Type::Primitive &right_type = left.output_type.resolve_type().get_primitive();

				// Are we attempting to operate on a non-integer?
				if (left_type.is_char() || left_type.is_boolean() || right_type.is_char() || right_type.is_boolean()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< times_operator0.line << " col " << times_operator0.column
						<< "): refusing to apply multiplication on a non-integer, for "
						<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Are the expressions of the same type?
				if (left.output_type.tag != right.output_type.tag) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< times_operator0.line << " col " << times_operator0.column
						<< "): refusing to multiply values of different types, for "
						<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Are we attempting to operate on a string?
				if (left_type.is_string() || right_type.is_string()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< times_operator0.line << " col " << times_operator0.column
						<< "): cannot apply multiplication on a string expression, for "
						<< left_type.get_tag_repr() << " with " << right_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Apply multiplication depending on the integer type.
				expression_semantics.output_type = left.output_type;
				const Index left_index   = expression_semantics.instructions.merge(left.instructions);
				const Index right_index  = expression_semantics.instructions.merge(right.instructions);
				const Index mult_index   = expression_semantics.instructions.add_instruction({I::MultFrom(B(), left_type.is_word())}, {left.output_index + left_index, right.output_index + right_index});
				const Index ignore_index = expression_semantics.instructions.add_instruction_indexed({I::Ignore(B())}, {{mult_index, 1}}, mult_index);
				expression_semantics.output_index = mult_index;
				break;
			} case ::Expression::slash_branch: {
				const ::Expression::Slash &slash           = grammar.expression_slash_storage.at(expression_symbol.data);
				const ::Expression        &expression0     = grammar.expression_storage.at(slash.expression0);
				const LexemeOperator      &slash_operator0 = grammar.lexemes.at(slash.slash_operator0).get_operator();
				const ::Expression        &expression1     = grammar.expression_storage.at(slash.expression1);

				// Get left and right subexpressions.
				const Expression left  = analyze_expression(expression0, constant_scope, type_scope, var_scope, combined_scope);
				const Expression right = analyze_expression(expression1, constant_scope, type_scope, var_scope, combined_scope);
				expression_semantics.lexeme_begin = left.lexeme_begin;
				expression_semantics.lexeme_end   = right.lexeme_end;

				// Make sure left and right are of primitive types.
				if (!left.output_type.resolve_type().is_primitive() || !right.output_type.resolve_type().is_primitive()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< slash_operator0.line << " col " << slash_operator0.column
						<< "): cannot apply division on a non-primitive-typed expression, for "
						<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}
				const Type::Primitive &left_type  = left.output_type.resolve_type().get_primitive();
				const Type::Primitive &right_type = left.output_type.resolve_type().get_primitive();

				// Are we attempting to operate on a non-integer?
				if (left_type.is_char() || left_type.is_boolean() || right_type.is_char() || right_type.is_boolean()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< slash_operator0.line << " col " << slash_operator0.column
						<< "): refusing to apply division on a non-integer, for "
						<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Are the expressions of the same type?
				if (left.output_type.tag != right.output_type.tag) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< slash_operator0.line << " col " << slash_operator0.column
						<< "): refusing to divide values of different types, for "
						<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Are we attempting to operate on a string?
				if (left_type.is_string() || right_type.is_string()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< slash_operator0.line << " col " << slash_operator0.column
						<< "): cannot apply division on a string expression, for "
						<< left_type.get_tag_repr() << " with " << right_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Apply division depending on the integer type.
				expression_semantics.output_type = left.output_type;
				const Index left_index   = expression_semantics.instructions.merge(left.instructions);
				const Index right_index  = expression_semantics.instructions.merge(right.instructions);
				const Index div_index    = expression_semantics.instructions.add_instruction({I::DivFrom(B(), left_type.is_word())}, {left.output_index + left_index, right.output_index + right_index});
				const Index ignore_index = expression_semantics.instructions.add_instruction_indexed({I::Ignore(B())}, {{div_index, 1}}, div_index);
				expression_semantics.output_index = div_index;
				break;
			} case ::Expression::percent_branch: {
				const ::Expression::Percent &percent           = grammar.expression_percent_storage.at(expression_symbol.data);
				const ::Expression          &expression0       = grammar.expression_storage.at(percent.expression0);
				const LexemeOperator        &percent_operator0 = grammar.lexemes.at(percent.percent_operator0).get_operator();
				const ::Expression          &expression1       = grammar.expression_storage.at(percent.expression1);

				// Get left and right subexpressions.
				const Expression left  = analyze_expression(expression0, constant_scope, type_scope, var_scope, combined_scope);
				const Expression right = analyze_expression(expression1, constant_scope, type_scope, var_scope, combined_scope);
				expression_semantics.lexeme_begin = left.lexeme_begin;
				expression_semantics.lexeme_end   = right.lexeme_end;

				// Make sure left and right are of primitive types.
				if (!left.output_type.resolve_type().is_primitive() || !right.output_type.resolve_type().is_primitive()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< percent_operator0.line << " col " << percent_operator0.column
						<< "): cannot apply mod on a non-primitive-typed expression, for "
						<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}
				const Type::Primitive &left_type  = left.output_type.resolve_type().get_primitive();
				const Type::Primitive &right_type = left.output_type.resolve_type().get_primitive();

				// Are we attempting to operate on a non-integer?
				if (left_type.is_char() || left_type.is_boolean() || right_type.is_char() || right_type.is_boolean()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< percent_operator0.line << " col " << percent_operator0.column
						<< "): refusing to apply mod on a non-integer, for "
						<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Are the expressions of the same type?
				if (left.output_type.tag != right.output_type.tag) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< percent_operator0.line << " col " << percent_operator0.column
						<< "): refusing to mod values of different types, for "
						<< left.output_type.get_tag_repr() << " with " << right.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Are we attempting to operate on a string?
				if (left_type.is_string() || right_type.is_string()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< percent_operator0.line << " col " << percent_operator0.column
						<< "): cannot apply mod on a string expression, for "
						<< left_type.get_tag_repr() << " with " << right_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Apply mod depending on the integer type.
				expression_semantics.output_type = left.output_type;
				const Index left_index      = expression_semantics.instructions.merge(left.instructions);
				const Index right_index     = expression_semantics.instructions.merge(right.instructions);
				const Index div_index       = expression_semantics.instructions.add_instruction({I::DivFrom(B(), left_type.is_word())}, {left.output_index + left_index, right.output_index + right_index});
				const Index ignore_index    = expression_semantics.instructions.add_instruction_indexed({I::Ignore(B())}, {{div_index, 0}}, div_index);
				const Index remainder_index = expression_semantics.instructions.add_instruction_indexed({I::LoadFrom(B(), left_type.is_word())}, {{div_index, 1}}, div_index);
				expression_semantics.output_index = remainder_index;
				break;
			} case ::Expression::tilde_branch: {
				const ::Expression::Tilde &tilde           = grammar.expression_tilde_storage.at(expression_symbol.data);
				const LexemeOperator      &tilde_operator0 = grammar.lexemes.at(tilde.tilde_operator0).get_operator();
				const ::Expression        &expression0     = grammar.expression_storage.at(tilde.expression);

				// Get the subexpression.
				const Expression value = analyze_expression(expression0, constant_scope, type_scope, var_scope, combined_scope);
				expression_semantics.lexeme_begin = tilde.tilde_operator0;
				expression_semantics.lexeme_end   = value.lexeme_end;

				// Make sure value is of primitive type.
				if (!value.output_type.resolve_type().is_primitive()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< tilde_operator0.line << " col " << tilde_operator0.column
						<< "): cannot apply bitwise NOT on a non-primitive-typed expression, for "
						<< value.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}
				const Type::Primitive &value_type  = value.output_type.resolve_type().get_primitive();

				// Are we attempting to operate on a string?
				if (value_type.is_string()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< tilde_operator0.line << " col " << tilde_operator0.column
						<< "): cannot apply bitwise NOT on a string expression, for "
						<< value_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Apply bitwise NOT depending on the integer type.
				expression_semantics.output_type = value.output_type;
				const Index value_index = expression_semantics.instructions.merge(value.instructions);
				const Index not_index   = expression_semantics.instructions.add_instruction({I::NorFrom(B(), value_type.is_word())}, {value.output_index + value_index, value.output_index + value_index});
				expression_semantics.output_index = not_index;
				break;
			} case ::Expression::unary_minus_branch: {
				const ::Expression::UnaryMinus &unary_minus     = grammar.expression_unary_minus_storage.at(expression_symbol.data);
				const LexemeOperator           &minus_operator0 = grammar.lexemes.at(unary_minus.minus_operator0).get_operator();
				const ::Expression             &expression0     = grammar.expression_storage.at(unary_minus.expression);

				// Get the subexpression.
				const Expression value = analyze_expression(expression0, constant_scope, type_scope, var_scope, combined_scope);
				expression_semantics.lexeme_begin = unary_minus.minus_operator0;
				expression_semantics.lexeme_end   = value.lexeme_end;

				// Make sure value is of primitive type.
				if (!value.output_type.resolve_type().is_primitive()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< minus_operator0.line << " col " << minus_operator0.column
						<< "): cannot apply unary minus on a non-primitive-typed expression, for "
						<< value.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}
				const Type::Primitive &value_type = value.output_type.resolve_type().get_primitive();

				// Are we attempting to operate on a non-integer?
				if (value_type.is_char() || value_type.is_boolean()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< minus_operator0.line << " col " << minus_operator0.column
						<< "): refusing to apply unary minus on a non-integer, for "
						<< value.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Are we attempting to operate on a string?
				if (value_type.is_string()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< minus_operator0.line << " col " << minus_operator0.column
						<< "): cannot apply unary minus on a string expression, for "
						<< value_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Apply unary minus depending on the integer type.
				expression_semantics.output_type = value.output_type;
				const Index value_index   = expression_semantics.instructions.merge(value.instructions);
				const Index load_n1_index = expression_semantics.instructions.add_instruction({I::LoadImmediate(B(), value_type.is_word(), ConstantValue(static_cast<int32_t>(-1), 1, 0))});
				const Index mult_index    = expression_semantics.instructions.add_instruction({I::MultFrom(B(), value_type.is_word())}, {load_n1_index, value.output_index + value_index});
				const Index ignore_index  = expression_semantics.instructions.add_instruction_indexed({I::Ignore(B())}, {{mult_index, 1}}, mult_index);
				expression_semantics.output_index = mult_index;
				break;
			} case ::Expression::parentheses_branch: {
				const ::Expression::Parentheses &parentheses                = grammar.expression_parentheses_storage.at(expression_symbol.data);
				const LexemeOperator            &leftparenthesis_operator0  = grammar.lexemes.at(parentheses.leftparenthesis_operator0).get_operator();
				const ::Expression              &expression0                = grammar.expression_storage.at(parentheses.expression); (void) expression0;
				const LexemeOperator            &rightparenthesis_operator0 = grammar.lexemes.at(parentheses.rightparenthesis_operator0).get_operator();

				// Get the subexpression.
				const Expression value = analyze_expression(expression0, constant_scope, type_scope, var_scope, combined_scope);
				expression_semantics.lexeme_begin = parentheses.leftparenthesis_operator0;
				expression_semantics.lexeme_end   = parentheses.rightparenthesis_operator0 + 1;

				// Use the same expression, but update lexeme_begin and lexeme_end.
				uint64_t lexeme_begin = expression_semantics.lexeme_begin;
				uint64_t lexeme_end   = expression_semantics.lexeme_end;
				expression_semantics = value;
				expression_semantics.lexeme_begin = lexeme_begin;
				expression_semantics.lexeme_end   = lexeme_end;
				break;
			}

			// These 5 branches are dynamic.
			case ::Expression::call_branch: {
				const ::Expression::Call      &call                        = grammar.expression_call_storage.at(expression_symbol.data);
				const LexemeIdentifier        &call_identifier             = grammar.lexemes.at(call.identifier).get_identifier();
				const LexemeOperator          &leftparenthesis_operator0   = grammar.lexemes.at(call.leftparenthesis_operator0).get_operator(); (void) leftparenthesis_operator0;
				const ExpressionSequenceOpt   &expression_sequence_opt     = grammar.expression_sequence_opt_storage.at(call.expression_sequence_opt);
				const LexemeOperator          &rightparenthesis_operator0  = grammar.lexemes.at(call.rightparenthesis_operator0).get_operator();

// TODO
#if 0
				expression_constant_value = ConstantValue(ConstantValue::Dynamic::dynamic, call.identifier, call.rightparenthesis_operator0 + 1);

				break;
#endif
			} case ::Expression::chr_branch: {
				const ::Expression::Chr &chr                        = grammar.expression_chr_storage.at(expression_symbol.data);
				const LexemeKeyword     &chr_keyword0               = grammar.lexemes.at(chr.chr_keyword0).get_keyword();
				const LexemeOperator    &leftparenthesis_operator0  = grammar.lexemes.at(chr.leftparenthesis_operator0).get_operator(); (void) leftparenthesis_operator0;
				const ::Expression      &expression0                = grammar.expression_storage.at(chr.expression);
				const LexemeOperator    &rightparenthesis_operator0 = grammar.lexemes.at(chr.rightparenthesis_operator0).get_operator();

				// Convert an integer to a char.

				// Get the subexpression.
				const Expression value = analyze_expression(expression0, constant_scope, type_scope, var_scope, combined_scope);
				expression_semantics.lexeme_begin = chr.chr_keyword0;
				expression_semantics.lexeme_end   = chr.rightparenthesis_operator0 + 1;

				// Make sure value is of primitive type.
				if (!value.output_type.resolve_type().is_primitive()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< chr_keyword0.line << " col " << chr_keyword0.column
						<< "): cannot apply chr() on a non-primitive-typed expression, for "
						<< value.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}
				const Type::Primitive &value_type = value.output_type.resolve_type().get_primitive();

				// Are we attempting to operate on a non-integer?
				if (value_type.is_char() || value_type.is_boolean()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< chr_keyword0.line << " col " << chr_keyword0.column
						<< "): refusing to apply chr() on a non-integer, for "
						<< value.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Are we attempting to operate on a string?
				if (value_type.is_string()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< chr_keyword0.line << " col " << chr_keyword0.column
						<< "): cannot apply chr() on a string expression, for "
						<< value_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Apply chr() depending on the integer type.
				// Truncate all but the lowest bits.  We may be storing e.g. to a byte in an array.
				expression_semantics.output_type = Type::char_type;
				const Index value_index  = expression_semantics.instructions.merge(value.instructions);
				const Index resize_index = expression_semantics.instructions.add_instruction({I::LoadFrom(B(), Type::Primitive::char_type.is_word(), Type::Primitive::integer_type.is_word())}, {value.output_index + value_index});
				expression_semantics.output_index = resize_index;
				break;
			} case ::Expression::ord_branch: {
				const ::Expression::Ord &ord                        = grammar.expression_ord_storage.at(expression_symbol.data);
				const LexemeKeyword     &ord_keyword0               = grammar.lexemes.at(ord.ord_keyword0).get_keyword();
				const LexemeOperator    &leftparenthesis_operator0  = grammar.lexemes.at(ord.leftparenthesis_operator0).get_operator(); (void) leftparenthesis_operator0;
				const ::Expression      &expression0                = grammar.expression_storage.at(ord.expression);
				const LexemeOperator    &rightparenthesis_operator0 = grammar.lexemes.at(ord.rightparenthesis_operator0).get_operator();

				// Convert a char to an integer.

				// Get the subexpression.
				const Expression value = analyze_expression(expression0, constant_scope, type_scope, var_scope, combined_scope);
				expression_semantics.lexeme_begin = ord.ord_keyword0;
				expression_semantics.lexeme_end   = ord.rightparenthesis_operator0 + 1;

				// Make sure value is of primitive type.
				if (!value.output_type.resolve_type().is_primitive()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< ord_keyword0.line << " col " << ord_keyword0.column
						<< "): cannot apply ord() on a non-primitive-typed expression, for "
						<< value.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}
				const Type::Primitive &value_type = value.output_type.resolve_type().get_primitive();

				// Are we attempting to operate on a non-char?
				if (value_type.is_integer() || value_type.is_boolean()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< ord_keyword0.line << " col " << ord_keyword0.column
						<< "): refusing to apply ord() on a non-char, for "
						<< value.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Are we attempting to operate on a string?
				if (value_type.is_string()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< ord_keyword0.line << " col " << ord_keyword0.column
						<< "): cannot apply ord() on a string expression, for "
						<< value_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Apply ord() depending on the integer type.
				// MIPS's "lb" should load a 1-byte value to the lowest bits
				// and fill the rest with 0s if loading from a memory address.
				// (We could always reset it to 0 before loading if not.)
				expression_semantics.output_type = Type::integer_type;
				const Index value_index  = expression_semantics.instructions.merge(value.instructions);
				const Index resize_index = expression_semantics.instructions.add_instruction({I::LoadFrom(B(), Type::Primitive::integer_type.is_word(), Type::Primitive::char_type.is_word())}, {value.output_index + value_index});
				expression_semantics.output_index = resize_index;
				break;
			} case ::Expression::pred_branch: {
				const ::Expression::Pred &pred                       = grammar.expression_pred_storage.at(expression_symbol.data);
				const LexemeKeyword      &pred_keyword0              = grammar.lexemes.at(pred.pred_keyword0).get_keyword();
				const LexemeOperator     &leftparenthesis_operator0  = grammar.lexemes.at(pred.leftparenthesis_operator0).get_operator(); (void) leftparenthesis_operator0;
				const ::Expression       &expression0                = grammar.expression_storage.at(pred.expression);
				const LexemeOperator     &rightparenthesis_operator0 = grammar.lexemes.at(pred.rightparenthesis_operator0).get_operator();

				// Find the predecessor of a value.

				// Get the subexpression.
				const Expression value = analyze_expression(expression0, constant_scope, type_scope, var_scope, combined_scope);
				expression_semantics.lexeme_begin = pred.pred_keyword0;
				expression_semantics.lexeme_end   = pred.rightparenthesis_operator0 + 1;

				// Make sure value is of primitive type.
				if (!value.output_type.resolve_type().is_primitive()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< pred_keyword0.line << " col " << pred_keyword0.column
						<< "): cannot apply pred() on a non-primitive-typed expression, for "
						<< value.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}
				const Type::Primitive &value_type = value.output_type.resolve_type().get_primitive();

				// Are we attempting to operate on a string?
				if (value_type.is_string()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< pred_keyword0.line << " col " << pred_keyword0.column
						<< "): cannot apply pred() on a string expression, for "
						<< value_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Apply pred() depending on the integer type.
				expression_semantics.output_type = value.output_type;
				if (!value_type.is_boolean()) {
					const Index value_index  = expression_semantics.instructions.merge(value.instructions);
					const Index load_1_index = expression_semantics.instructions.add_instruction({I::LoadImmediate(B(), value_type.is_word(), ConstantValue(static_cast<int32_t>(1), 0, 0))});
					const Index sub_index    = expression_semantics.instructions.add_instruction({I::SubFrom(B(), value_type.is_word())}, {value.output_index + value_index, load_1_index});
					expression_semantics.output_index = sub_index;
				} else {
					const Index value_index   = expression_semantics.instructions.merge(value.instructions);
					const Index load_1_index  = expression_semantics.instructions.add_instruction({I::LoadImmediate(B(), value_type.is_word(), ConstantValue(static_cast<int32_t>(1), 0, 0))});
					const Index bnot_index    = expression_semantics.instructions.add_instruction({I::LessThanFrom(B(), value_type.is_word())}, {value.output_index + value_index, load_1_index});
					expression_semantics.output_index = bnot_index;
				}
				break;
			} case ::Expression::succ_branch: {
				const ::Expression::Succ &succ                       = grammar.expression_succ_storage.at(expression_symbol.data);
				const LexemeKeyword      &succ_keyword0              = grammar.lexemes.at(succ.succ_keyword0).get_keyword();
				const LexemeOperator     &leftparenthesis_operator0  = grammar.lexemes.at(succ.leftparenthesis_operator0).get_operator(); (void) leftparenthesis_operator0;
				const ::Expression       &expression0                = grammar.expression_storage.at(succ.expression);
				const LexemeOperator     &rightparenthesis_operator0 = grammar.lexemes.at(succ.rightparenthesis_operator0).get_operator();

				// Find the successor of a value.

				// Get the subexpression.
				const Expression value = analyze_expression(expression0, constant_scope, type_scope, var_scope, combined_scope);
				expression_semantics.lexeme_begin = succ.succ_keyword0;
				expression_semantics.lexeme_end   = succ.rightparenthesis_operator0 + 1;

				// Make sure value is of primitive type.
				if (!value.output_type.resolve_type().is_primitive()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< succ_keyword0.line << " col " << succ_keyword0.column
						<< "): cannot apply succ() on a non-primitive-typed expression, for "
						<< value.output_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}
				const Type::Primitive &value_type = value.output_type.resolve_type().get_primitive();

				// Are we attempting to operate on a string?
				if (value_type.is_string()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze_expression: error (line "
						<< succ_keyword0.line << " col " << succ_keyword0.column
						<< "): cannot apply succ() on a string expression, for "
						<< value_type.get_tag_repr()
						<< "."
						;
					throw SemanticsError(sstr.str());
				}

				// Apply succ() depending on the integer type.
				if (!value_type.is_boolean()) {
					expression_semantics.output_type = value.output_type;
					const Index value_index  = expression_semantics.instructions.merge(value.instructions);
					const Index load_1_index = expression_semantics.instructions.add_instruction({I::LoadImmediate(B(), value_type.is_word(), ConstantValue(static_cast<int32_t>(1), 0, 0))});
					const Index add_index    = expression_semantics.instructions.add_instruction({I::AddFrom(B(), value_type.is_word())}, {value.output_index + value_index, load_1_index});
					expression_semantics.output_index = add_index;
				} else {
					const Index value_index   = expression_semantics.instructions.merge(value.instructions);
					const Index load_1_index  = expression_semantics.instructions.add_instruction({I::LoadImmediate(B(), value_type.is_word(), ConstantValue(static_cast<int32_t>(1), 0, 0))});
					const Index bnot_index    = expression_semantics.instructions.add_instruction({I::LessThanFrom(B(), value_type.is_word())}, {value.output_index + value_index, load_1_index});
					expression_semantics.output_index = bnot_index;
				}
				break;
			}

			// lvalue_branch may or may not be dynamic, depending on whether it is
			// a simple identifier (without .foo or [bar]) that refers to a known
			// constant value in the scope of the expression.
			case ::Expression::lvalue_branch: {
				const ::Expression::Lvalue       &lvalue                      = grammar.expression_lvalue_storage.at(expression_symbol.data);
				const Lvalue                     &lvalue_symbol               = grammar.lvalue_storage.at(lvalue.lvalue);
				const LexemeIdentifier           &lexeme_identifier           = grammar.lexemes.at(lvalue_symbol.identifier).get_identifier();
				const LvalueAccessorClauseList   &lvalue_accessor_clause_list = grammar.lvalue_accessor_clause_list_storage.at(lvalue_symbol.lvalue_accessor_clause_list);

// TODO
#if 0
				// According to the documentation, only lvalues without
				// accessors can be constant (static) expressions.  So check the
				// lvalue type, to see if it's just an identifier.
				if (lvalue_accessor_clause_list.branch != LvalueAccessorClauseList::empty_branch) {
					// It's not just an identifier.  This lvalue expression is not
					// a constant expression.

					// Unpack the lvalue_accessor_clause_list just enough to get the last lexeme.
					uint64_t lexeme_end;
					switch (lvalue_accessor_clause_list.branch) {
						// We already checked that the list is not empty.
						//case LvalueAccessorClauseList::empty_branch: {
						//	// ...
						//	break;
						//}

						case LvalueAccessorClauseList::cons_branch: {
							// Unpack the list.
							const LvalueAccessorClauseList::Cons &last_lvalue_accessor_clause_list_cons = grammar.lvalue_accessor_clause_list_cons_storage.at(lvalue_accessor_clause_list.data);
							const LvalueAccessorClauseList       &last_lvalue_accessor_clause_list      = grammar.lvalue_accessor_clause_list_storage.at(last_lvalue_accessor_clause_list_cons.lvalue_accessor_clause_list);
							const LvalueAccessorClause           &last_lvalue_accessor_clause           = grammar.lvalue_accessor_clause_storage.at(last_lvalue_accessor_clause_list_cons.lvalue_accessor_clause); (void) last_lvalue_accessor_clause;

							// Unpack just the last LvalueAccessorClause.
							switch (last_lvalue_accessor_clause.branch) {
								case LvalueAccessorClause::index_branch: {
									const LvalueAccessorClause::Index &index            = grammar.lvalue_accessor_clause_index_storage.at(last_lvalue_accessor_clause.data);
									const LexemeOperator              &dot_operator0    = grammar.lexemes.at(index.dot_operator0).get_operator(); (void) dot_operator0;
									const LexemeIdentifier            &index_identifier = grammar.lexemes.at(index.identifier).get_identifier(); (void) index_identifier;

									lexeme_end = index.identifier + 1;

									break;
								}

								case LvalueAccessorClause::array_branch: {
									const LvalueAccessorClause::Array &array                  = grammar.lvalue_accessor_clause_array_storage.at(last_lvalue_accessor_clause.data);
									const LexemeOperator              &leftbracket_operator0  = grammar.lexemes.at(array.leftbracket_operator0).get_operator(); (void) leftbracket_operator0;
									const ::Expression                &expression0            = grammar.expression_storage.at(array.expression); (void) expression0;
									const LexemeOperator              &rightbracket_operator0 = grammar.lexemes.at(array.rightbracket_operator0).get_operator(); (void) rightbracket_operator0;

									lexeme_end = array.rightbracket_operator0 + 1;

									break;
								}

								// Unrecognized branch.
								default: {
									std::ostringstream sstr;
									sstr << "Semantics::is_expression_constant: internal error: invalid lvalue_accessor_clause branch at index " << last_lvalue_accessor_clause_list_cons.lvalue_accessor_clause << ": " << last_lvalue_accessor_clause.branch;
									throw SemanticsError(sstr.str());
								}
							}

							break;
						}

						// Unrecognized branch.
						default: {
							std::ostringstream sstr;
							sstr << "Semantics::is_expression_constant: internal error: invalid lvalue_accessor_clause_list branch at index " << lvalue_symbol.lvalue_accessor_clause_list << ": " << lvalue_accessor_clause_list.branch;
							throw SemanticsError(sstr.str());
						}
					};

					expression_constant_value = ConstantValue(ConstantValue::Dynamic::dynamic, lvalue_symbol.identifier, lexeme_end);
					break;
				}

				// Lookup the identifier binding.
				std::optional<IdentifierScope::IdentifierBinding> identifier_binding_search = expression_constant_scope.lookup_copy(lexeme_identifier.text);
				if (!identifier_binding_search) {
					std::ostringstream sstr;
					sstr << "Semantics::is_expression_constant: error (line " << lexeme_identifier.line << " col " << lexeme_identifier.column << "): identifier out of scope when checking for constant lvalue: " << lexeme_identifier.text;
					throw SemanticsError(sstr.str());
				}

				if (!identifier_binding_search->is_static()) {
					// The identifier does not refer to a constant expression.
					expression_constant_value = ConstantValue(ConstantValue::Dynamic::dynamic, lvalue_symbol.identifier, lvalue_symbol.identifier + 1);
					break;
				} else {  // identifier_binding_search->is_static()
					const IdentifierScope::IdentifierBinding::Static &static_ = identifier_binding_search->get_static();
					// Copy the constant value.
					expression_constant_value = ConstantValue(static_.constant_value, lvalue_symbol.identifier, lvalue_symbol.identifier + 1);
					break;
				}
#endif
			}

			// These 3 branches are static.
			case ::Expression::integer_branch:
			case ::Expression::char__branch:
			case ::Expression::string_branch: {
				std::ostringstream sstr;
				sstr << "Semantics::analyze_expression: internal error: an integer, char, or string expression should have been analyzed as a constant expression but wasn't at index " << &expression_symbol - &grammar.expression_storage[0] << " (branch: " << expression_symbol.branch << ").";
				throw SemanticsError(sstr.str());
			}

			// Unrecognized branch.
			default: {
				std::ostringstream sstr;
				sstr << "Semantics::analyze_expression: internal error: invalid expression branch at index " << &expression_symbol - &grammar.expression_storage[0] << ": " << expression_symbol.branch;
				throw SemanticsError(sstr.str());
			}
		}
	}

	// Return the semantics expression.
	return expression_semantics;
}

// | Get the symbol to a string literal, tracking it if this is the first time encountering it.
Semantics::Symbol Semantics::string_literal(const std::string &string) {
	std::map<std::string, Symbol>::const_iterator string_constants_search = string_constants.find(string);
	if (string_constants_search == string_constants.cend()) {
		Symbol string_symbol {labelify(string), "", string_constants.size()};
		string_constants.insert({string, string_symbol});
		return string_symbol;
	} else {
		return string_constants_search->second;
	}
}

const uint64_t Semantics::max_string_requested_label_suffix_length = CPSL_CC_SEMANTICS_MAX_STRING_REQUESTED_LABEL_SUFFIX_LENGTH;

std::string Semantics::labelify(const std::string &string) {
	std::string label_suffix;

	bool last_alnum = false;
	for (const char &c : std::as_const(string)) {
		std::string addition;

		if (isalnum(c)) {
			if (!last_alnum) {
				last_alnum = true;
				addition.push_back('_');
			}
			addition.push_back(tolower(c));
		} else {
			last_alnum = false;
		}

		if (label_suffix.size() + addition.size() > max_string_requested_label_suffix_length) {
			break;
		} else {
			label_suffix += addition;
		}
	}

	return "string_literal" + label_suffix;
}

bool Semantics::would_addition_overflow(int32_t a, int32_t b) {
	int32_t smaller, larger;
	if (a <= b) {
		smaller = a;
		larger  = b;
	} else {
		smaller = b;
		larger  = a;
	}

	if (smaller >= 0) {
		// positive + positive
		// a + b > max <=> b > max - a
		return std::numeric_limits<int32_t>::max() - smaller < larger;
	} else if (larger < 0) {
		// negative + negative
		// a + b < min <=> b < min - a
		return std::numeric_limits<int32_t>::min() - larger > smaller;
	} else {
		// positive + negative or negative + positive
		return false;
	}
}

bool Semantics::would_multiplication_overflow(int32_t a, int32_t b) {
	// Handle cases where the result does not increase in magnitude, while
	// preventing division by zero and enabling the next check's validity.
	if (a == 0 || b == 0) {
		return false;
	}
	if (a == 1 || b == 1) {
		return false;
	}
	if (a == -1 || b == -1) {
		return a != std::numeric_limits<int32_t>::min() && b != std::numeric_limits<int32_t>::min();
	}

	// Since the result is increasing in magnitude, fail at the greatest
	// possible magnitude, while also protecting us from overflows occurring
	// during our checks themselves.
	if (a == std::numeric_limits<int32_t>::min() || b == std::numeric_limits<int32_t>::min()) {
		return true;
	}

	// Get sign of result and absolute values of inputs.
	bool result_nat = (a >= 0) == (b >= 0);
	int32_t a_abs = a >= 0 ? a : -a;
	int32_t b_abs = b >= 0 ? b : -b;

	// Check.
	if (result_nat) {
		// Result is zero or positive (by now, only positive).
		// ab >= max <=> a >= max/b.
		return a_abs >= std::numeric_limits<int32_t>::max()/b_abs;
	} else {
		// Result is zero or negative (by now, only negative).
		// mag(min)-1 == mag(max)
		assert(-(std::numeric_limits<int32_t>::min()+1) == std::numeric_limits<int32_t>::max());
		//return a_abs >= (std::numeric_limits<int32_t>::max()+1)/b_abs;
		// Avoid an overflow.
		if        (a_abs > (std::numeric_limits<int32_t>::max())/b_abs) {
			return true;
		} else if (a_abs == (std::numeric_limits<int32_t>::max())/b_abs) {
			return ((std::numeric_limits<int32_t>::max())%b_abs) >= (b_abs - 1);
		} else {
			return false;
		}
	}
}

bool Semantics::would_division_overflow(int32_t a, int32_t b) {
	if        (a == -1 && b == std::numeric_limits<int32_t>::min()) {
		return true;
	} else if (b == -1 && a == std::numeric_limits<int32_t>::min()) {
		return true;
	} else {
		return false;
	}
}

int32_t Semantics::euclidian_div(int32_t a, int32_t b) {
	int32_t quotient  = a / b;
	int32_t remainder = a % b;
	if (remainder < 0) {
		return quotient + 1;
	} else {
		return quotient;
	}
}

int32_t Semantics::euclidian_mod(int32_t a, int32_t b) {
	int32_t remainder = a % b;
	if (remainder < 0) {
		return remainder + b;
	} else {
		return remainder;
	}
}

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
void Semantics::reset_output() {
	// Clear.

	output                   = Output();
	top_level_scope          = IdentifierScope();
	top_level_type_scope     = IdentifierScope();
	top_level_var_scope      = IdentifierScope();
	top_level_constant_scope = IdentifierScope();
	anonymous_storage        = IdentifierScope();
	top_level_vars.clear();
	string_constants.clear();

	// Reset.

	using S = IdentifierScope;
	using C = ConstantValue;
	using T = Type;

	top_level_constant_scope.scope.insert({"true", S::IdentifierBinding(C::true_constant)});
	top_level_scope.scope.insert({"true", S::IdentifierBinding(C::true_constant)});

	top_level_constant_scope.scope.insert({"false", S::IdentifierBinding(C::false_constant)});
	top_level_scope.scope.insert({"false", S::IdentifierBinding(C::false_constant)});

	top_level_constant_scope.scope.insert({"TRUE", S::IdentifierBinding(C::true_constant)});
	top_level_scope.scope.insert({"TRUE", S::IdentifierBinding(C::true_constant)});

	top_level_constant_scope.scope.insert({"FALSE", S::IdentifierBinding(C::false_constant)});
	top_level_scope.scope.insert({"FALSE", S::IdentifierBinding(C::false_constant)});

	top_level_type_scope.scope.insert({"integer", S::IdentifierBinding(T::integer_type)});
	top_level_scope.scope.insert({"integer", S::IdentifierBinding(T::integer_type)});

	top_level_type_scope.scope.insert({"char", S::IdentifierBinding(T::char_type)});
	top_level_scope.scope.insert({"char", S::IdentifierBinding(T::char_type)});

	top_level_type_scope.scope.insert({"boolean", S::IdentifierBinding(T::boolean_type)});
	top_level_scope.scope.insert({"boolean", S::IdentifierBinding(T::boolean_type)});

	top_level_type_scope.scope.insert({"string", S::IdentifierBinding(T::string_type)});
	top_level_scope.scope.insert({"string", S::IdentifierBinding(T::string_type)});

	top_level_type_scope.scope.insert({"INTEGER", S::IdentifierBinding(T::integer_type)});
	top_level_scope.scope.insert({"INTEGER", S::IdentifierBinding(T::integer_type)});

	top_level_type_scope.scope.insert({"CHAR", S::IdentifierBinding(T::char_type)});
	top_level_scope.scope.insert({"CHAR", S::IdentifierBinding(T::char_type)});

	top_level_type_scope.scope.insert({"BOOLEAN", S::IdentifierBinding(T::boolean_type)});
	top_level_scope.scope.insert({"BOOLEAN", S::IdentifierBinding(T::boolean_type)});

	top_level_type_scope.scope.insert({"STRING", S::IdentifierBinding(T::string_type)});
	top_level_scope.scope.insert({"STRING", S::IdentifierBinding(T::string_type)});

	for (Output::section_t section = Output::null_section; section <= Output::num_sections; section = static_cast<Output::section_t>(static_cast<int>(section) + 1)) {
		output.sections.push_back(std::vector<std::string>());
	}
}

// | Force a re-analysis of the semantics data.
void Semantics::analyze() {
	// It's possible the grammar was reset.  Clear caches and outputs just in
	// case.
	reset_output();

	// If there are no parsed start symbols, then the grammar probably has not
	// yet been set up.  Just return, in this case.  The Semantics value might
	// remain null.
	if (grammar.start_storage.size() <= 0) {
		return;
	}

	// Make sure there is a single start symbol.  Get it.
	if (grammar.start_storage.size() != 1) {
		std::ostringstream sstr;
		sstr << "Semantics::analyze: internal error: the number of parsed start symbols should be exactly 1 but isn't: " << grammar.start_storage.size() << ".";
		throw SemanticsError(sstr.str());
	}
	const Start &start = grammar.start_storage[0];

	// Unpack the program symbol.
	const Program                         &program                              = grammar.program_storage.at(start.program);
	const ConstantDeclOpt                 &constant_decl_opt                    = grammar.constant_decl_opt_storage.at(program.constant_decl_opt);
	const TypeDeclOpt                     &type_decl_opt                        = grammar.type_decl_opt_storage.at(program.type_decl_opt);
	const VarDeclOpt                      &var_decl_opt                         = grammar.var_decl_opt_storage.at(program.var_decl_opt);
	const ProcedureDeclOrFunctionDeclList &procedure_decl_or_function_decl_list = grammar.procedure_decl_or_function_decl_list_storage.at(program.procedure_decl_or_function_decl_list);
	const Block                           &block                                = grammar.block_storage.at(program.block);
	const LexemeOperator                  &dot_operator0                        = grammar.lexemes.at(program.dot_operator0).get_operator(); (void) dot_operator0;

	// First, analyze top-level constants.  These are at the beginning of the
	// parsed program grammar tree.
	switch (constant_decl_opt.branch) {
		case ConstantDeclOpt::empty_branch: {
			// No constant declarations.  Nothing to do here.
			break;
		}

		case ConstantDeclOpt::value_branch: {
			// Unpack the constant_decl.
			const ConstantDeclOpt::Value &constant_decl_opt_value  = grammar.constant_decl_opt_value_storage.at(constant_decl_opt.data);
			const ConstantDecl           &constant_decl            = grammar.constant_decl_storage.at(constant_decl_opt_value.constant_decl);
			const LexemeKeyword          &const_keyword0           = grammar.lexemes.at(constant_decl.const_keyword0).get_keyword(); (void) const_keyword0;
			const ConstantAssignment     &constant_assignment      = grammar.constant_assignment_storage.at(constant_decl.constant_assignment);
			const ConstantAssignmentList &constant_assignment_list = grammar.constant_assignment_list_storage.at(constant_decl.constant_assignment_list);

			// Collect the constant assignments in the list.
			std::vector<const ConstantAssignment *> constant_assignments;
			constant_assignments.push_back(&constant_assignment);
			bool reached_end = false;
			for (const ConstantAssignmentList *last_list = &constant_assignment_list; !reached_end; ) {
				// Unpack the last list encountered.
				switch(last_list->branch) {
					case ConstantAssignmentList::empty_branch: {
						// We're done.
						// (No need to unpack the empty branch.)
						reached_end = true;
						break;
					}

					case ConstantAssignmentList::cons_branch: {
						// Unpack the list.
						const ConstantAssignmentList::Cons &last_constant_assignment_list_cons = grammar.constant_assignment_list_cons_storage.at(last_list->data);
						const ConstantAssignmentList       &last_constant_assignment_list      = grammar.constant_assignment_list_storage.at(last_constant_assignment_list_cons.constant_assignment_list);
						const ConstantAssignment           &last_constant_assignment           = grammar.constant_assignment_storage.at(last_constant_assignment_list_cons.constant_assignment);

						// Add the constant assignment.
						constant_assignments.push_back(&last_constant_assignment);
						last_list = &last_constant_assignment_list;

						// Loop.
						break;
					}

					// Unrecognized branch.
					default: {
						std::ostringstream sstr;
						sstr << "Semantics::analyze: internal error: invalid constant_assignment_list branch at index " << last_list - &grammar.constant_assignment_list_storage[0] << ": " << last_list->branch;
						throw SemanticsError(sstr.str());
					}
				}
			}

			// Correct the order of the list.
			std::reverse(constant_assignments.begin() + 1, constant_assignments.end());

			// Handle the constant assignments.
			for (const ConstantAssignment *next_constant_assignment : constant_assignments) {
				const LexemeIdentifier   &identifier          = grammar.lexemes.at(next_constant_assignment->identifier).get_identifier();
				const LexemeOperator     &equals_operator0    = grammar.lexemes.at(next_constant_assignment->equals_operator0).get_operator(); (void) equals_operator0;
				const ::Expression       &expression          = grammar.expression_storage.at(next_constant_assignment->expression);
				const LexemeOperator     &semicolon_operator0 = grammar.lexemes.at(next_constant_assignment->semicolon_operator0).get_operator(); (void) semicolon_operator0;

				// Calculate the constant value.
				ConstantValue constant_value = is_expression_constant(expression, top_level_constant_scope);

				// Fail if this is not a static value.
				if (!constant_value.is_static()) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze: error (line "
						<< identifier.line << " col " << identifier.column
						<< "): a non-constant expression was found where a constant expression was expected."
						;
					throw SemanticsError(sstr.str());
				}

				// Add this constant to the top-level scope.
				if (top_level_scope.has(identifier.text)) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze: error (line "
						<< identifier.line << " col " << identifier.column
						<< "): redefinition of constant ``" << identifier.text << "\"."
						;
					throw SemanticsError(sstr.str());
				}
				top_level_constant_scope.scope.insert({identifier.text, IdentifierScope::IdentifierBinding(IdentifierScope::IdentifierBinding::Static(constant_value))});
				top_level_scope.scope.insert({identifier.text, IdentifierScope::IdentifierBinding(IdentifierScope::IdentifierBinding::Static(constant_value))});
			}

			// Done handling constant part.
			break;
		}

		// Unrecognized branch.
		default: {
			std::ostringstream sstr;
			sstr << "Semantics::analyze: internal error: invalid constant_decl_opt branch at index " << program.constant_decl_opt << ": " << constant_decl_opt.branch;
			throw SemanticsError(sstr.str());
		}
	}

	// Next, analyze top-level type definitions.
	switch (type_decl_opt.branch) {
		case TypeDeclOpt::empty_branch: {
			// No constant declarations.  Nothing to do here.
			break;
		}

		case TypeDeclOpt::value_branch: {
			// Unpack the type_decl.
			const TypeDeclOpt::Value &type_decl_opt_value  = grammar.type_decl_opt_value_storage.at(type_decl_opt.data);
			const TypeDecl           &type_decl            = grammar.type_decl_storage.at(type_decl_opt_value.type_decl);
			const LexemeKeyword      &type_keyword0        = grammar.lexemes.at(type_decl.type_keyword0).get_keyword(); (void) type_keyword0;
			const TypeAssignment     &type_assignment      = grammar.type_assignment_storage.at(type_decl.type_assignment);
			const TypeAssignmentList &type_assignment_list = grammar.type_assignment_list_storage.at(type_decl.type_assignment_list);

			// Collect the type assignments in the list.
			std::vector<const TypeAssignment *> type_assignments;
			type_assignments.push_back(&type_assignment);
			bool reached_end = false;
			for (const TypeAssignmentList *last_list = &type_assignment_list; !reached_end; ) {
				// Unpack the last list encountered.
				switch(last_list->branch) {
					case TypeAssignmentList::empty_branch: {
						// We're done.
						// (No need to unpack the empty branch.)
						reached_end = true;
						break;
					}

					case TypeAssignmentList::cons_branch: {
						// Unpack the list.
						const TypeAssignmentList::Cons &last_type_assignment_list_cons = grammar.type_assignment_list_cons_storage.at(last_list->data);
						const TypeAssignmentList       &last_type_assignment_list      = grammar.type_assignment_list_storage.at(last_type_assignment_list_cons.type_assignment_list);
						const TypeAssignment           &last_type_assignment           = grammar.type_assignment_storage.at(last_type_assignment_list_cons.type_assignment);

						// Add the type assignment.
						type_assignments.push_back(&last_type_assignment);
						last_list = &last_type_assignment_list;

						// Loop.
						break;
					}

					// Unrecognized branch.
					default: {
						std::ostringstream sstr;
						sstr << "Semantics::analyze: internal error: invalid type_assignment_list branch at index " << last_list - &grammar.type_assignment_list_storage[0] << ": " << last_list->branch;
						throw SemanticsError(sstr.str());
					}
				}
			}

			// Correct the order of the list.
			std::reverse(type_assignments.begin() + 1, type_assignments.end());

			// Handle the type assignments.
			for (const TypeAssignment *next_type_assignment : type_assignments) {
				const LexemeIdentifier &identifier          = grammar.lexemes.at(next_type_assignment->identifier).get_identifier();
				const LexemeOperator   &equals_operator0    = grammar.lexemes.at(next_type_assignment->equals_operator0).get_operator(); (void) equals_operator0;
				const ::Type           &type                = grammar.type_storage.at(next_type_assignment->type);
				const LexemeOperator   &semicolon_operator0 = grammar.lexemes.at(next_type_assignment->semicolon_operator0).get_operator(); (void) semicolon_operator0;

				// Check for redefinition.
				if (top_level_type_scope.has(identifier.text)) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze: error (line "
						<< identifier.line << " col " << identifier.column
						<< "): redefinition of type ``" << identifier.text << "\"."
						;
					throw SemanticsError(sstr.str());
				}
				if (combine_identifier_namespaces && top_level_scope.has(identifier.text)) {
					std::ostringstream sstr;
					sstr
						<< "Semantics::analyze: error (line "
						<< identifier.line << " col " << identifier.column
						<< "): type identifier ``" << identifier.text << "\" has already been assigned."
						<< "  Set combine_identifier_namespaces to 0 to isolate identifier namespaces"
						<< " from each other."
						;
					throw SemanticsError(sstr.str());
				}

				// Calculate the type.
				Type semantics_type = analyze_type(identifier.text, type, top_level_constant_scope, top_level_type_scope, anonymous_storage);

				// Add this type to the top-level scope.
				top_level_type_scope.scope.insert({identifier.text, IdentifierScope::IdentifierBinding(IdentifierScope::IdentifierBinding::Type(semantics_type))});
				top_level_scope.scope.insert({identifier.text, IdentifierScope::IdentifierBinding(IdentifierScope::IdentifierBinding::Type(semantics_type))});
			}

			// Done handling type part.
			break;
		}

		// Unrecognized branch.
		default: {
			std::ostringstream sstr;
			sstr << "Semantics::analyze: internal error: invalid type_decl_opt branch at index " << program.type_decl_opt << ": " << type_decl_opt.branch;
			throw SemanticsError(sstr.str());
		}
	}

	// Next, analyze top-level var definitions.
	switch (var_decl_opt.branch) {
		case VarDeclOpt::empty_branch: {
			// No top-level variable declarations.  Nothing to do here.
			break;
		}

		case VarDeclOpt::value_branch: {
			// Unpack the var_decl.
			const VarDeclOpt::Value           &var_decl_opt_value             = grammar.var_decl_opt_value_storage.at(var_decl_opt.data);
			const VarDecl                     &var_decl                       = grammar.var_decl_storage.at(var_decl_opt_value.var_decl);
			const LexemeKeyword               &var_keyword0                   = grammar.lexemes.at(var_decl.var_keyword0).get_keyword(); (void) var_keyword0;
			const TypedIdentifierSequence     &typed_identifier_sequence      = grammar.typed_identifier_sequence_storage.at(var_decl.typed_identifier_sequence);
			const TypedIdentifierSequenceList &typed_identifier_sequence_list = grammar.typed_identifier_sequence_list_storage.at(var_decl.typed_identifier_sequence_list);

			// Collect the typed identifier sequences in the list.
			std::vector<const TypedIdentifierSequence *> typed_identifier_sequences;
			typed_identifier_sequences.push_back(&typed_identifier_sequence);
			bool reached_end = false;
			for (const TypedIdentifierSequenceList *last_list = &typed_identifier_sequence_list; !reached_end; ) {
				// Unpack the last list encountered.
				switch(last_list->branch) {
					case TypedIdentifierSequenceList::empty_branch: {
						// We're done.
						// (No need to unpack the empty branch.)
						reached_end = true;
						break;
					}

					case TypedIdentifierSequenceList::cons_branch: {
						// Unpack the list.
						const TypedIdentifierSequenceList::Cons &last_typed_identifier_sequence_list_cons = grammar.typed_identifier_sequence_list_cons_storage.at(last_list->data);
						const TypedIdentifierSequenceList       &last_typed_identifier_sequence_list      = grammar.typed_identifier_sequence_list_storage.at(last_typed_identifier_sequence_list_cons.typed_identifier_sequence_list);
						const TypedIdentifierSequence           &last_typed_identifier_sequence           = grammar.typed_identifier_sequence_storage.at(last_typed_identifier_sequence_list_cons.typed_identifier_sequence);

						// Add the constant assignment.
						typed_identifier_sequences.push_back(&last_typed_identifier_sequence);
						last_list = &last_typed_identifier_sequence_list;

						// Loop.
						break;
					}

					// Unrecognized branch.
					default: {
						std::ostringstream sstr;
						sstr << "Semantics::analyze: internal error: invalid typed_identifier_sequence_list branch at index " << last_list - &grammar.typed_identifier_sequence_list_storage[0] << ": " << last_list->branch;
						throw SemanticsError(sstr.str());
					}
				}
			}

			// Correct the order of the list.
			std::reverse(typed_identifier_sequences.begin() + 1, typed_identifier_sequences.end());

			// Handle the typed identifier sequences.
			for (const TypedIdentifierSequence *next_typed_identifier_sequence : typed_identifier_sequences) {
				const IdentList      &ident_list          = grammar.ident_list_storage.at(next_typed_identifier_sequence->ident_list);
				const LexemeOperator &colon_operator0     = grammar.lexemes.at(next_typed_identifier_sequence->colon_operator0).get_operator(); (void) colon_operator0;
				const ::Type         &next_type           = grammar.type_storage.at(next_typed_identifier_sequence->type);
				const LexemeOperator &semicolon_operator0 = grammar.lexemes.at(next_typed_identifier_sequence->semicolon_operator0).get_operator(); (void) semicolon_operator0;

				// Get a copy of the subtype or construct a new anonymous subtype using "anonymous_storage".
				const Type *next_semantics_type;
				// Branch on next_type.  If it's in the "simple" type alias
				// format, it should refer to an existing type, although it's
				// not a type alias.  Otherwise, create an anonymous type.
				if (next_type.branch == ::Type::simple_branch) {
					const ::Type::Simple   &simple            = grammar.type_simple_storage.at(next_type.data);
					const SimpleType       &simple_type       = grammar.simple_type_storage.at(simple.simple_type);
					const LexemeIdentifier &simple_identifier = grammar.lexemes.at(simple_type.identifier).get_identifier();

					// Make sure the reference typed is in scope.
					if (!top_level_type_scope.has(simple_identifier.text)) {
						std::ostringstream sstr;
						sstr
							<< "Semantics::analyze: error (line "
							<< simple_identifier.line << " col " << simple_identifier.column
							<< "): undefined type ``" << simple_identifier.text << "\": not in scope."
							;
						throw SemanticsError(sstr.str());
					}

					// Set next_semantics_type.
					next_semantics_type = &top_level_type_scope.get(simple_identifier.text).get_type();
				} else {
					// Create an anonymous type.
					Type anonymous_type = analyze_type("", next_type, top_level_constant_scope, top_level_type_scope, anonymous_storage);
					anonymous_storage.anonymous_bindings.push_back(IdentifierScope::IdentifierBinding(anonymous_type));
					next_semantics_type = &anonymous_storage.anonymous_bindings[anonymous_storage.anonymous_bindings.size() - 1].get_type();
				}

				// Unpack the ident_list.
				const LexemeIdentifier       &first_identifier         = grammar.lexemes.at(ident_list.identifier).get_identifier();
				const IdentifierPrefixedList &identifier_prefixed_list = grammar.identifier_prefixed_list_storage.at(ident_list.identifier_prefixed_list);

				// Collect the identifiers in the list.
				std::vector<const LexemeIdentifier *> identifiers;
				identifiers.push_back(&first_identifier);
				bool reached_end = false;
				for (const IdentifierPrefixedList *last_list = &identifier_prefixed_list; !reached_end; ) {
					// Unpack the last list encountered.
					switch(last_list->branch) {
						case IdentifierPrefixedList::empty_branch: {
							// We're done.
							// (No need to unpack the empty branch.)
							reached_end = true;
							break;
						}

						case IdentifierPrefixedList::cons_branch: {
							// Unpack the list.
							const IdentifierPrefixedList::Cons &last_identifier_prefixed_list_cons = grammar.identifier_prefixed_list_cons_storage.at(last_list->data);
							const IdentifierPrefixedList       &last_identifier_prefixed_list      = grammar.identifier_prefixed_list_storage.at(last_identifier_prefixed_list_cons.identifier_prefixed_list);
							const LexemeOperator               &last_colon_operator0               = grammar.lexemes.at(last_identifier_prefixed_list_cons.comma_operator0).get_operator(); (void) last_colon_operator0;
							const LexemeIdentifier             &last_identifier                    = grammar.lexemes.at(last_identifier_prefixed_list_cons.identifier).get_identifier();

							// Add the identifier.
							identifiers.push_back(&last_identifier);
							last_list = &last_identifier_prefixed_list;

							// Loop.
							break;
						}

						// Unrecognized branch.
						default: {
							std::ostringstream sstr;
							sstr << "Semantics::analyze_type: internal error: invalid identifier_prefixed_list branch at index " << last_list - &grammar.identifier_prefixed_list_storage[0] << ": " << last_list->branch;
							throw SemanticsError(sstr.str());
						}
					}
				}

				// Correct the order of the list.
				std::reverse(identifiers.begin() + 1, identifiers.end());

				// Handle the identifiers.
				for (const LexemeIdentifier *next_identifier : identifiers) {
					// Duplicate variable definition?
					if (top_level_var_scope.has(next_identifier->text)) {
						std::ostringstream sstr;
						sstr
							<< "Semantics::analyze: error (line "
							<< next_identifier->line << " col " << next_identifier->column
							<< "): redefinition of variable ``" << next_identifier->text << "\"."
							;
						throw SemanticsError(sstr.str());
					}

					// Duplicate identifier binding in another namespace?
					if (combine_identifier_namespaces && top_level_scope.has(next_identifier->text)) {
						std::ostringstream sstr;
						sstr
							<< "Semantics::analyze: error (line "
							<< next_identifier->line << " col " << next_identifier->column
							<< "): variable identifier ``" << next_identifier->text << "\" has already been assigned."
							<< "  Set combine_identifier_namespaces to 0 to isolate identifier namespaces"
							<< " from each other."
							;
						throw SemanticsError(sstr.str());
					}

					// If this is the first line in the global_vars_section,
					// add the initial declarations.
					if (output.is_section_empty(Output::global_vars_section)) {
						std::ostringstream sline;
						sline << ".data";
						output.add_line(Output::global_vars_section, sline.str());
					}

					// Add the variable binding.

					// Use the Var index as its symbol unique identifier.
					const std::string next_identifier_text = std::as_const(next_identifier->text);
					const Symbol var_symbol("global_var_", next_identifier_text, top_level_vars.size());
					const IdentifierScope::IdentifierBinding::Var var(false, *next_semantics_type, true, var_symbol, false, 0, 0);
					top_level_vars.push_back(var);
					top_level_var_scope.scope.insert({next_identifier_text, IdentifierScope::IdentifierBinding(var)});

					// Global variable-width variables are currently unsupported.
					if (!var.type.get_fixed_width()) {
						std::ostringstream sstr;
						sstr
							<< "Semantics::analyze: error (line "
							<< next_identifier->line << " col " << next_identifier->column
							<< "): variable-width globals are currently unsupported; not compiling ``" << next_identifier->text << "\"."
							;
						throw SemanticsError(sstr.str());
					}

					// Compile the variable references.
					output.add_line(Output::global_vars_section, ":", var.symbol);
					if        (var.type.get_size() == 4) {
						std::ostringstream sline;
						sline << "\t.word  " << std::right << std::setw(11) << "0";
						output.add_line(Output::global_vars_section, sline.str());
					} else if (var.type.get_size() == 1) {
						std::ostringstream sline;
						sline << "\t.byte  " << std::right << std::setw(11) << "0";
						output.add_line(Output::global_vars_section, sline.str());
					} else {
						output.add_line(Output::global_vars_section, "\t.align 4");
						std::ostringstream sline;
						sline << "\t.space " << std::right << std::setw(11) << var.type.get_size();
						output.add_line(Output::global_vars_section, sline.str());
					}
				}
			}

			// We're done handling the top-level variable declarations.
			break;
		}

		// Unrecognized branch.
		default: {
			std::ostringstream sstr;
			sstr << "Semantics::analyze: internal error: invalid var_decl_opt branch at index " << program.var_decl_opt << ": " << var_decl_opt.branch;
			throw SemanticsError(sstr.str());
		}
	}

	// Next, analyze the top-level procedures and functions.

	// Add ".text", since unconditionally there will be at least "main:" defined.
	if (!output.is_section_empty(Output::global_vars_section)) {
		// If .data is non-empty, add a newline for readability between .data and .text.
		output.add_line(Output::text_section, "");
	}
	output.add_line(Output::text_section, ".text");

	// Collect the procedure_decl_or_function_decls in the list.
	std::vector<const ProcedureDeclOrFunctionDecl *> procedure_decl_or_function_decls;
	bool reached_end = false;
	for (const ProcedureDeclOrFunctionDeclList *last_list = &procedure_decl_or_function_decl_list; !reached_end; ) {
		// Unpack the last list encountered.
		switch(last_list->branch) {
			case ProcedureDeclOrFunctionDeclList::empty_branch: {
				// We're done.
				// (No need to unpack the empty branch.)
				reached_end = true;
				break;
			}

			case ProcedureDeclOrFunctionDeclList::cons_branch: {
				// Unpack the list.
				const ProcedureDeclOrFunctionDeclList::Cons &last_procedure_decl_or_function_decl_list_cons = grammar.procedure_decl_or_function_decl_list_cons_storage.at(last_list->data);
				const ProcedureDeclOrFunctionDeclList       &last_procedure_decl_or_function_decl_list      = grammar.procedure_decl_or_function_decl_list_storage.at(last_procedure_decl_or_function_decl_list_cons.procedure_decl_or_function_decl_list);
				const ProcedureDeclOrFunctionDecl           &last_procedure_decl_or_function_decl           = grammar.procedure_decl_or_function_decl_storage.at(last_procedure_decl_or_function_decl_list_cons.procedure_decl_or_function_decl);

				// Add the constant assignment.
				procedure_decl_or_function_decls.push_back(&last_procedure_decl_or_function_decl);
				last_list = &last_procedure_decl_or_function_decl_list;

				// Loop.
				break;
			}

			// Unrecognized branch.
			default: {
				std::ostringstream sstr;
				sstr << "Semantics::analyze: internal error: invalid procedure_decl_or_function_decl_list branch at index " << last_list - &grammar.procedure_decl_or_function_decl_list_storage[0] << ": " << last_list->branch;
				throw SemanticsError(sstr.str());
			}
		}
	}

	// Correct the order of the list.
	std::reverse(procedure_decl_or_function_decls.begin(), procedure_decl_or_function_decls.end());

	// Handle the procedure_decl_or_function_decls.
	for (const ProcedureDeclOrFunctionDecl *next_procedure_decl_or_function_decl : procedure_decl_or_function_decls) {
		switch (next_procedure_decl_or_function_decl->branch) {
			case ProcedureDeclOrFunctionDecl::procedure_branch: {
				// Unpack the procedure.
				const ProcedureDeclOrFunctionDecl::Procedure &procedure      = grammar.procedure_decl_or_function_decl_procedure_storage.at(next_procedure_decl_or_function_decl->data);
				const ProcedureDecl                          &procedure_decl = grammar.procedure_decl_storage.at(procedure.procedure_decl);

				// Forward declaration or definition?
				switch (procedure_decl.branch) {
					case ProcedureDecl::forward_branch: {
						// Unpack the forward declaration.
						const ProcedureDecl::Forward &forward                    = grammar.procedure_decl_forward_storage.at(procedure_decl.data);
						const LexemeKeyword          &procedure_keyword0         = grammar.lexemes.at(forward.procedure_keyword0).get_keyword(); (void) procedure_keyword0;
						const LexemeIdentifier       &identifier                 = grammar.lexemes.at(forward.identifier).get_identifier();
						const LexemeOperator         &leftparenthesis_operator0  = grammar.lexemes.at(forward.leftparenthesis_operator0).get_operator(); (void) leftparenthesis_operator0;
						const FormalParameters       &formal_parameters          = grammar.formal_parameters_storage.at(forward.formal_parameters);
						const LexemeOperator         &rightparenthesis_operator0 = grammar.lexemes.at(forward.rightparenthesis_operator0).get_operator(); (void) rightparenthesis_operator0;
						const LexemeOperator         &semicolon_operator0        = grammar.lexemes.at(forward.semicolon_operator0).get_operator(); (void) semicolon_operator0;
						const LexemeKeyword          &forward_keyword0           = grammar.lexemes.at(forward.forward_keyword0).get_keyword(); (void) forward_keyword0;
						const LexemeOperator         &semicolon_operator1        = grammar.lexemes.at(forward.semicolon_operator1).get_operator(); (void) semicolon_operator1;

						// TODO

						// We're done handling the forward declaration.
						break;
					}

					case ProcedureDecl::definition_branch: {
						// Unpack the procedure definition.
						const ProcedureDecl::Definition &definition                 = grammar.procedure_decl_definition_storage.at(procedure_decl.data);
						const LexemeKeyword             &procedure_keyword0         = grammar.lexemes.at(definition.procedure_keyword0).get_keyword(); (void) procedure_keyword0;
						const LexemeIdentifier          &identifier                 = grammar.lexemes.at(definition.identifier).get_identifier();
						const LexemeOperator            &leftparenthesis_operator0  = grammar.lexemes.at(definition.leftparenthesis_operator0).get_operator(); (void) leftparenthesis_operator0;
						const FormalParameters          &formal_parameters          = grammar.formal_parameters_storage.at(definition.formal_parameters);
						const LexemeOperator            &rightparenthesis_operator0 = grammar.lexemes.at(definition.rightparenthesis_operator0).get_operator(); (void) rightparenthesis_operator0;
						const LexemeOperator            &semicolon_operator0        = grammar.lexemes.at(definition.semicolon_operator0).get_operator(); (void) semicolon_operator0;
						const Body                      &body                       = grammar.body_storage.at(definition.body);
						const LexemeOperator            &semicolon_operator1        = grammar.lexemes.at(definition.semicolon_operator1).get_operator(); (void) semicolon_operator1;

						// TODO

						// We're done handling the procedure definition.
						break;
					}

					// Unrecognized branch.
					default: {
						std::ostringstream sstr;
						sstr << "Semantics::analyze: internal error: invalid procedure_decl branch at index " << procedure.procedure_decl << ": " << procedure_decl.branch;
						throw SemanticsError(sstr.str());
					}
				}

				// TODO

				// We're done handling the procedure.
				break;
			}

			case ProcedureDeclOrFunctionDecl::function_branch: {
				// Unpack the function.
				const ProcedureDeclOrFunctionDecl::Function &function      = grammar.procedure_decl_or_function_decl_function_storage.at(next_procedure_decl_or_function_decl->data);
				const FunctionDecl                          &function_decl = grammar.function_decl_storage.at(function.function_decl);

				// Forward declaration or definition?
				switch (function_decl.branch) {
					case FunctionDecl::forward_branch: {
						// Unpack the forward declaration.
						const FunctionDecl::Forward &forward                    = grammar.function_decl_forward_storage.at(function_decl.data);
						const LexemeKeyword         &function_keyword0          = grammar.lexemes.at(forward.function_keyword0).get_keyword(); (void) function_keyword0;
						const LexemeIdentifier      &identifier                 = grammar.lexemes.at(forward.identifier).get_identifier();
						const LexemeOperator        &leftparenthesis_operator0  = grammar.lexemes.at(forward.leftparenthesis_operator0).get_operator(); (void) leftparenthesis_operator0;
						const FormalParameters      &formal_parameters          = grammar.formal_parameters_storage.at(forward.formal_parameters);
						const LexemeOperator        &rightparenthesis_operator0 = grammar.lexemes.at(forward.rightparenthesis_operator0).get_operator(); (void) rightparenthesis_operator0;
						const LexemeOperator        &colon_operator0            = grammar.lexemes.at(forward.colon_operator0).get_operator(); (void) colon_operator0;
						const ::Type                &type                       = grammar.type_storage.at(forward.type);
						const LexemeOperator        &semicolon_operator0        = grammar.lexemes.at(forward.semicolon_operator0).get_operator(); (void) semicolon_operator0;
						const LexemeKeyword         &forward_keyword0           = grammar.lexemes.at(forward.forward_keyword0).get_keyword(); (void) forward_keyword0;
						const LexemeOperator        &semicolon_operator1        = grammar.lexemes.at(forward.semicolon_operator1).get_operator(); (void) semicolon_operator1;

						// TODO

						// We're done handling the forward declaration.
						break;
					}

					case FunctionDecl::definition_branch: {
						// Unpack the function definition.
						const FunctionDecl::Definition &definition                 = grammar.function_decl_definition_storage.at(function_decl.data);
						const LexemeKeyword            &function_keyword0          = grammar.lexemes.at(definition.function_keyword0).get_keyword(); (void) function_keyword0;
						const LexemeIdentifier         &identifier                 = grammar.lexemes.at(definition.identifier).get_identifier();
						const LexemeOperator           &leftparenthesis_operator0  = grammar.lexemes.at(definition.leftparenthesis_operator0).get_operator(); (void) leftparenthesis_operator0;
						const FormalParameters         &formal_parameters          = grammar.formal_parameters_storage.at(definition.formal_parameters);
						const LexemeOperator           &rightparenthesis_operator0 = grammar.lexemes.at(definition.rightparenthesis_operator0).get_operator(); (void) rightparenthesis_operator0;
						const LexemeOperator           &colon_operator0            = grammar.lexemes.at(definition.colon_operator0).get_operator(); (void) colon_operator0;
						const ::Type                   &type                       = grammar.type_storage.at(definition.type);
						const LexemeOperator           &semicolon_operator0        = grammar.lexemes.at(definition.semicolon_operator0).get_operator(); (void) semicolon_operator0;
						const Body                     &body                       = grammar.body_storage.at(definition.body);
						const LexemeOperator           &semicolon_operator1        = grammar.lexemes.at(definition.semicolon_operator1).get_operator(); (void) semicolon_operator1;

						// TODO

						// We're done handling the function definition.
						break;
					}

					// Unrecognized branch.
					default: {
						std::ostringstream sstr;
						sstr << "Semantics::analyze: internal error: invalid function_decl branch at index " << function.function_decl << ": " << function_decl.branch;
						throw SemanticsError(sstr.str());
					}
				}

				// TODO

				// We're done handling the function.
				break;
			}

			// Unrecognized branch.
			default: {
				std::ostringstream sstr;
				sstr << "Semantics::analyze: internal error: invalid procedure_decl_or_function_decl branch at index " << next_procedure_decl_or_function_decl - &grammar.procedure_decl_or_function_decl_storage[0] << ": " << next_procedure_decl_or_function_decl->branch;
				throw SemanticsError(sstr.str());
			}
		}
	}

	// Next, analyze the top-level block (main).

	// Unpack block.
	const LexemeKeyword &begin_keyword0         = grammar.lexemes.at(block.begin_keyword0).get_keyword(); (void) begin_keyword0;
	const StatementSequence &statement_sequence = grammar.statement_sequence_storage.at(block.statement_sequence);
	const LexemeKeyword &end_keyword0           = grammar.lexemes.at(block.end_keyword0).get_keyword(); (void) end_keyword0;

	// TODO

	// The string literals have been analyzed by this point.
	// Add the string literal declarations.
	for (const std::pair<std::string, Symbol> &string_symbol_pair : std::as_const(string_constants)) {
		const std::string &string        = string_symbol_pair.first;
		const Symbol      &symbol        = string_symbol_pair.second;
		std::string        quoted_string = ConstantValue::quote_string(string);

		output.add_line(Output::global_vars_section, ":", symbol);
		output.add_line(Output::global_vars_section, "\t.asciiz " + quoted_string);
	}
}

// Symbol.
inline bool operator< (const Semantics::Symbol &a, const Semantics::Symbol &b) {
	if        (a.prefix           != b.prefix) {
		return a.prefix            < b.prefix;
	} else if (a.requested_suffix != b.requested_suffix) {
		return a.requested_suffix  < b.requested_suffix;
	} else {
		return a.unique_identifier < b.unique_identifier;
	}
}
inline bool operator> (const Semantics::Symbol &a, const Semantics::Symbol &b) { return   b < a;  }
inline bool operator<=(const Semantics::Symbol &a, const Semantics::Symbol &b) { return !(a > b); }
inline bool operator>=(const Semantics::Symbol &a, const Semantics::Symbol &b) { return !(a < b); }

inline bool operator==(const Semantics::Symbol &a, const Semantics::Symbol &b) {
	return a.prefix == b.prefix && a.requested_suffix == b.requested_suffix && a.unique_identifier == b.unique_identifier;
}
inline bool operator!=(const Semantics::Symbol &a, const Semantics::Symbol &b) { return !(a == b); }

// SymbolLocation.
inline bool operator< (const Semantics::Output::SymbolLocation &a, const Semantics::Output::SymbolLocation &b) {
	if        (a.section   != b.section) {
		return a.section   < b.section;
	} else if (a.line      != b.line) {
		return a.line      < b.line;
	} else if (a.start_pos != b.start_pos) {
		return a.start_pos < b.start_pos;
	} else {
		return a.length    < b.length;
	}
}
inline bool operator> (const Semantics::Output::SymbolLocation &a, const Semantics::Output::SymbolLocation &b) { return   b < a;  }
inline bool operator<=(const Semantics::Output::SymbolLocation &a, const Semantics::Output::SymbolLocation &b) { return !(a > b); }
inline bool operator>=(const Semantics::Output::SymbolLocation &a, const Semantics::Output::SymbolLocation &b) { return !(a < b); }

inline bool operator==(const Semantics::Output::SymbolLocation &a, const Semantics::Output::SymbolLocation &b) {
	return a.section == b.section && a.line == b.line && a.start_pos == b.start_pos && a.length == b.length;
}
inline bool operator!=(const Semantics::Output::SymbolLocation &a, const Semantics::Output::SymbolLocation &b) { return !(a == b); }

// Line.
inline bool operator< (const Semantics::Output::Line &a, const Semantics::Output::Line &b) {
	if        (a.line    != b.line) {
		return a.line    < b.line;
	} else {
		return a.symbols < b.symbols;
	}
}
inline bool operator> (const Semantics::Output::Line &a, const Semantics::Output::Line &b) { return   b < a;  }
inline bool operator<=(const Semantics::Output::Line &a, const Semantics::Output::Line &b) { return !(a > b); }
inline bool operator>=(const Semantics::Output::Line &a, const Semantics::Output::Line &b) { return !(a < b); }

inline bool operator==(const Semantics::Output::Line &a, const Semantics::Output::Line &b) {
	return a.line == b.line && a.symbols == b.symbols;
}
inline bool operator!=(const Semantics::Output::Line &a, const Semantics::Output::Line &b) { return !(a == b); }

// Line addition.
inline Semantics::Output::Line operator+(const Semantics::Output::Line &a, const Semantics::Output::Line &b) { return a.plus(b); }
inline Semantics::Output::Line operator+(const Semantics::Output::Line &a, const Semantics::Symbol       &b) { return a.plus(b); }
inline Semantics::Output::Line operator+(const Semantics::Symbol       &a, const Semantics::Output::Line &b) { return Semantics::Output::Line(a).plus(b); }
inline Semantics::Output::Line operator+(const Semantics::Symbol       &a, const Semantics::Symbol       &b) { return Semantics::Output::Line(a).plus(b); }

// Storage.
inline bool operator< (const Semantics::Storage &a, const Semantics::Storage &b) {
	if        (a.max_size       != b.max_size) {
		return a.max_size       < b.max_size;
	} else if (a.is_global      != b.is_global) {
		return a.is_global      < b.is_global;
	} else if (a.global_address != b.global_address) {
		return a.global_address < b.global_address;
	} else if (a.register_      != b.register_) {
		return a.register_      < b.register_;
	} else if (a.dereference    != b.dereference) {
		return a.dereference    < b.dereference;
	} else {
		return a.offset         < b.offset;
	}
}
inline bool operator> (const Semantics::Storage &a, const Semantics::Storage &b) { return   b < a;  }
inline bool operator<=(const Semantics::Storage &a, const Semantics::Storage &b) { return !(a > b); }
inline bool operator>=(const Semantics::Storage &a, const Semantics::Storage &b) { return !(a < b); }

inline bool operator==(const Semantics::Storage &a, const Semantics::Storage &b) {
	return a.max_size == b.max_size && a.is_global == b.is_global && a.global_address == b.global_address & a.register_ == b.register_ && a.dereference == b.dereference && a.offset == b.offset;
}
inline bool operator!=(const Semantics::Storage &a, const Semantics::Storage &b) { return !(a == b); }

#if 0
inline Semantics::Output::Line operator+(const std::string             &a, const Semantics::Output::Line &b) { return Semantics::Output::Line(a) + b; }
inline Semantics::Output::Line operator+(const Semantics::Output::Line &a, const std::string             &b) { return a + Semantics::Output::Line(b); }
#endif /* if 0 */

UnitTests UnitTests::unit_tests;

UnitTests::UnitTests()
{
	assert((run(), true));
}

void UnitTests::run() {
	test_mips_io();
	test_mips_io2();
}

void UnitTests::test_mips_io() {
	// Test MIPS IO with internal construction.

	// Some type aliases to improve readability.
	using M = Semantics::MIPSIO;
	using I = Semantics::Instruction;
	using B = Semantics::Instruction::Base;
	using Index = M::Index;
	using IO    = M::IO;
	using ConstantValue = Semantics::ConstantValue;
	using Output        = Semantics::Output;
	using Storage       = Semantics::Storage;
	using Symbol        = Semantics::Symbol;

	// Storage(uint32_t max_size, bool is_global, Symbol global_address, const std::string &register_, bool dereference, int32_t offset);
	assert(Storage(4, false, Symbol(), "$t2", false, 0) == Storage("$t2"));
	assert(Storage(4, false, Symbol(), "$t0", false, 0) == Storage("$t0"));

	M basic;
	basic.instructions.push_back(I(I::LoadImmediate(B(), true, ConstantValue(static_cast<int32_t>(4), 0, 0), Symbol())));
	basic.instructions.push_back(I(I::LoadFrom(B(), true, 16)));
	basic.connections.insert(         {{1, 0},  {0, 0} });
	basic.reversed_connections.insert({{0, 0}, {{1, 0}}});
	std::vector<Output::Line> lines = basic.emit(
		// Inputs.
		{
		},
		// Working.
		{
			Storage("$t2"),
		},
		// Outputs.
		{
			{{1, 0}, Storage("$t0")},
		}
	);

	std::vector<Output::Line> expected;
	expected.push_back("\tli   $t2, 4");
	expected.push_back("\tla   $t0, 16($t2)");

	assert(lines == expected);

	assert(basic.prepare({{1,0}}) == std::vector<uint32_t>({4}));

	// Now use the utility methods to construct the same MIPSIO.
	M simple;
	Index load_4    = simple.add_instruction({I::LoadImmediate(B(), true, ConstantValue(static_cast<int32_t>(4), 0, 0), Symbol())});
	Index load_from = simple.add_instruction({I::LoadFrom(B(), true, 16)}, {load_4});
	const std::map<IO, Storage> capture_outputs {
		{{load_from, 0}, Storage("$t0")},
	};

	lines = simple.emit(
		// Inputs.
		{},
		// Working.
		{
			// Storage(uint32_t max_size, bool is_global, Symbol global_address, const std::string &register_, bool dereference, int32_t offset);
			Storage("$t2"),
		},
		// Outputs.
		capture_outputs
	);

	assert(lines == expected);

	assert(simple.prepare(capture_outputs) == std::vector<uint32_t>({4}));
}

void UnitTests::test_mips_io2() {
	// Some type aliases to improve readability.
	using M = Semantics::MIPSIO;
	using I = Semantics::Instruction;
	using B = Semantics::Instruction::Base;
	using Index = M::Index;
	using IO    = M::IO;
	using ConstantValue = Semantics::ConstantValue;
	using Output        = Semantics::Output;
	using Storage       = Semantics::Storage;
	using Symbol        = Semantics::Symbol;

	// Storage(uint32_t max_size, bool is_global, Symbol global_address, const std::string &register_, bool dereference, int32_t offset);
	assert(Storage(4, false, Symbol(), "$t2", false, 0) == Storage("$t2"));
	assert(Storage(4, false, Symbol(), "$t3", false, 0) == Storage("$t3"));
	assert(Storage(4, false, Symbol(), "$t0", false, 0) == Storage("$t0"));

	M basic;
	basic.instructions.push_back(I(I::LoadImmediate(B(), true, ConstantValue(static_cast<int32_t>(4), 0, 0), Symbol())));
	basic.instructions.push_back(I(I::LoadFrom(B(), true, 16)));
	basic.instructions.push_back(I(I::LoadImmediate(B(), true, ConstantValue(static_cast<int32_t>(6), 0, 0), Symbol())));
	basic.instructions.push_back(I(I::AddFrom(B(), true)));

	// Input to LoadFrom.
	basic.connections.insert(         {{1, 0},  {0, 0} });
	basic.reversed_connections.insert({{0, 0}, {{1, 0}}});

	// Input to AddFrom.
	basic.connections.insert(         {{3, 0},  {1, 0} });
	basic.reversed_connections.insert({{1, 0}, {{3, 0}}});

	basic.connections.insert(         {{3, 1},  {2, 0} });
	basic.reversed_connections.insert({{2, 0}, {{3, 1}}});

	std::vector<Output::Line> lines = basic.emit(
		// Inputs.
		{
		},
		// Working.
		{
			// Storage(uint32_t max_size, bool is_global, Symbol global_address, const std::string &register_, bool dereference, int32_t offset);
			Storage("$t2"),
			Storage("$t3"),
		},
		// Outputs.
		{
			{{3, 0}, Storage("$t0")},
		}
	);

	std::vector<Output::Line> expected;
	expected.push_back("\tli   $t2, 4");
	expected.push_back("\tla   $t3, 16($t2)");
	expected.push_back("\tli   $t2, 6");
	expected.push_back("\taddu $t0, $t3, $t2");

	assert(lines == expected);

	assert(basic.prepare({{3,0}}) == std::vector<uint32_t>({4, 4}));

	// Now use the utility methods to construct the same MIPSIO.
	M simple;
	Index load_4    = simple.add_instruction({I::LoadImmediate(B(), true, ConstantValue(static_cast<int32_t>(4), 0, 0), Symbol())});
	Index load_from = simple.add_instruction({I::LoadFrom(B(), true, 16)}, {load_4});
	Index load_6    = simple.add_instruction({I::LoadImmediate(B(), true, ConstantValue(static_cast<int32_t>(6), 0, 0), Symbol())}, {});
	Index add       = simple.add_instruction({I::AddFrom(B(), true)}, {load_from, load_6});
	const std::map<IO, Storage> capture_outputs {
		{{add, 0}, Storage("$t0")},
	};

	lines = simple.emit(
		// Inputs.
		{},
		// Working.
		{
			// Storage(uint32_t max_size, bool is_global, Symbol global_address, const std::string &register_, bool dereference, int32_t offset);
			Storage("$t2"),
			Storage("$t3"),
		},
		// Outputs.
		capture_outputs
	);

	assert(lines == expected);

	assert(simple.prepare(capture_outputs) == std::vector<uint32_t>({4, 4}));
}
