#ifndef CPSL_CC_CLI_HH
#define CPSL_CC_CLI_HH

#include <map>        // std::map
#include <optional>   // std::optional
#include <set>        // std::set
#include <stdexcept>  // std::runtime_error
#include <string>     // std::string
#include <vector>     // std::vector

#define CLI_DEFAULT_PROG cpsl-cc
namespace cli {

	/*
	* Exceptions types.
	*/

	class CLIError : public std::runtime_error {
	public:
		CLIError();
		CLIError(const std::string &message);
	};

	class RunError : public std::runtime_error {
	public:
		RunError();
		RunError(const std::string &message);
	};

	/*
	 * CLI.
	 */

	class LexerError : public std::runtime_error {
	public:
		LexerError();
		LexerError(std::string message);
	};

	// | The output of a parse of command-line arguments.

	// TODO: consider turning the set into a map to track the initial
	// command-line argument that configured the option.
	class ParsedArgs {
	public:
		ParsedArgs(
			const std::map<std::string, std::string> &specified_string_options,
			const std::set<std::string>              &specified_boolean_options,
			const std::vector<std::string>           &positional_arguments
		);
		ParsedArgs(
			std::map<std::string, std::string> &&specified_string_options,
			std::set<std::string>              &&specified_boolean_options,
			std::vector<std::string>           &&positional_arguments
		);

		std::map<std::string, std::string> specified_string_options;
		std::set<std::string>              specified_boolean_options;
		std::vector<std::string>           positional_arguments;

		// Convenience accessors.

		// | Is the boolean option provided?
		bool                       is  (const std::string& option) const;
		// | Get the value of an option with a default.
		std::string                get (const std::string& option, const std::string &default_) const;
		// | Was a string value specified for the option?
		bool                       has (const std::string& option) const;
		// | Was a string value specified for the option?
		std::optional<std::string> find(const std::string& option) const;

		// Other methods.

		// | Get a vector of arguments that would result in this ParsedArgs.
		//
		// This does not include the first program value.
		std::vector<std::string> normalized_args() const;
	};

	// | List of recognized options.
	class ArgsSpec {
	public:
		// | Specification for a single option.
		class OptionSpec {
		public:
			OptionSpec(bool boolean_option);

			// Is this option a boolean option or a string option?
			bool boolean_option;
		};

		// | Construct an argument specification.
		ArgsSpec(
			const std::map<std::string, OptionSpec>  &options,
			const std::map<std::string, std::string> &option_aliases,
			const std::map<char, std::string>        &short_aliases
		);
		ArgsSpec(
			std::map<std::string, OptionSpec>  &&options,
			std::map<std::string, std::string> &&option_aliases,
			std::map<char, std::string>        &&short_aliases
		);

		// | Long option base names.
		// Option name -> OptionSpec
		// Option name: e.g. "help" for "--help".
		std::map<std::string, OptionSpec>  options;

		// | Long option-aliases.
		// Alias -> option name
		// e.g. "usage" -> "help"
		std::map<std::string, std::string> option_aliases;

		// | Short option-aliases.
		// Alias -> option name
		// e.g. 'h' -> "help"
		std::map<char, std::string>        short_aliases;

		// Verify validity of argument specification.
		bool verify() const;

		// Default argument specification.
		static const ArgsSpec default_args_spec;

		// Parse command-line arguments.  "args" should not contain the program
		// name.
		ParsedArgs parse(const std::vector<std::string> &args, const std::optional<std::string> &prog);
	};

	// | cli: The command-line interface to the compiler.
	//
	// Wrapper around run.
	int cli(int argc, char *argv[]);

	// | run: Run the command-line interface to the compiler given the provided
	// command-line arguments.
	void run(const std::vector<std::string> &argv);

	// | run_with_parsed: Run the command-line interface to the compiler given
	// the parsed options.
	void run_with_parsed(const ParsedArgs &parsed_args, const ArgsSpec &args_spec, const std::vector<std::string> &args, const std::optional<std::string> &prog);
	void run_with_parsed(const ParsedArgs &parsed_args, const ArgsSpec &args_spec);
	void run_with_parsed(const ParsedArgs &parsed_args);

	// | The default prog value.
	extern const std::string default_prog;

	// | Print usage and exit.
	void usage(const std::optional<std::string> &prog, int exit_code = 0, bool err = false);

	// | Get the usage string.
	std::string get_usage(const std::optional<std::string> &prog = std::optional<std::string>());

	// | Print version information and exit.
	void version(const std::optional<std::string> &prog, int exit_code = 0, bool err = false);

	// | Get the version information.
	std::string get_version_info(const std::optional<std::string> &prog = std::optional<std::string>());

	// | Read the lines of a file.
	std::vector<std::string> readlines (const ParsedArgs &parsed_args, const std::string &input_path);
	// | Write lines to a file.
	void                     writelines(const ParsedArgs &parsed_args, const std::string &output_path, const std::vector<std::string> &lines);

	// | Handle input and output paths after handling information options, e.g.
	// --help and --version.
	void run_with_paths(const ParsedArgs &parsed_args, const std::string &input_path, const std::string &output_path, const ArgsSpec &args_spec, const std::vector<std::string> &args, const std::optional<std::string> &prog);
	void run_with_paths(const ParsedArgs &parsed_args, const std::string &input_path, const std::string &output_path, const ArgsSpec &args_spec);
	void run_with_paths(const ParsedArgs &parsed_args, const std::string &input_path, const std::string &output_path);

	// | Write lexer information after each line and exit.
	void lexer_info(const ParsedArgs &parsed_args, const std::string &input_path, const std::string &output_path, const ArgsSpec &args_spec, const std::vector<std::string> &args, const std::optional<std::string> &prog);
	void lexer_info(const ParsedArgs &parsed_args, const std::string &input_path, const std::string &output_path, const ArgsSpec &args_spec);
	void lexer_info(const ParsedArgs &parsed_args, const std::string &input_path, const std::string &output_path);

	// | Given lines of input, produce lines of output.
	std::vector<std::string> get_lexer_info(const ParsedArgs &parsed_args, const std::vector<std::string> &input_lines, const ArgsSpec &args_spec, const std::vector<std::string> &args, const std::optional<std::string> &prog);
	std::vector<std::string> get_lexer_info(const ParsedArgs &parsed_args, const std::vector<std::string> &input_lines, const ArgsSpec &args_spec);
	std::vector<std::string> get_lexer_info(const ParsedArgs &parsed_args, const std::vector<std::string> &input_lines);

	// | Write parser information after each line and exit.
	void parser_info(const ParsedArgs &parsed_args, const std::string &input_path, const std::string &output_path, const ArgsSpec &args_spec, const std::vector<std::string> &args, const std::optional<std::string> &prog);
	void parser_info(const ParsedArgs &parsed_args, const std::string &input_path, const std::string &output_path, const ArgsSpec &args_spec);
	void parser_info(const ParsedArgs &parsed_args, const std::string &input_path, const std::string &output_path);

	// | Given lines of input, produce lines of output.
	std::vector<std::string> get_parser_info(const ParsedArgs &parsed_args, const std::vector<std::string> &input_lines, const ArgsSpec &args_spec, const std::vector<std::string> &args, const std::optional<std::string> &prog);
	std::vector<std::string> get_parser_info(const ParsedArgs &parsed_args, const std::vector<std::string> &input_lines, const ArgsSpec &args_spec);
	std::vector<std::string> get_parser_info(const ParsedArgs &parsed_args, const std::vector<std::string> &input_lines);

	// | Assemble a CPSL file.
	//void assemble(const std::string &in_path, const std::string &out_path);
}

#endif /* #ifndef CPSL_CC_CLI_HH */
