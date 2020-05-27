#include <algorithm>  // std::move
#include <cassert>    // assert
#include <iostream>   // std::cerr, std::cout, std::endl
#include <map>        // std::map
#include <optional>   // std::optional
#include <set>        // std::set
#include <sstream>    // std::ostringstream
#include <stdexcept>  // std::runtime_error
#include <string>     // std::string
#include <utility>    // std::as_const, std::exit, std::move, std::pair
#include <vector>     // std::vector

#include "cli.hh"

extern "C" {
#include "version.h"
#include "util.h"     // assertm, STR
}

/*
 * Exception types.
 */

cli::CLIError::CLIError()
	: runtime_error("A CLI error occurred.")
	{}

cli::CLIError::CLIError(std::string message)
	: runtime_error(std::move(message))
	{}

/*
 * CLI.
 */

cli::ParsedArgs::ParsedArgs(
	const std::map<std::string, std::string> &specified_string_options,
	const std::set<std::string>              &specified_boolean_options,
	const std::vector<std::string>           &positional_arguments
)
	: specified_string_options(specified_string_options)
	, specified_boolean_options(specified_boolean_options)
	, positional_arguments(positional_arguments)
	{}

cli::ParsedArgs::ParsedArgs(
	std::map<std::string, std::string> &&specified_string_options,
	std::set<std::string>              &&specified_boolean_options,
	std::vector<std::string>           &&positional_arguments
)
	: specified_string_options(std::move(specified_string_options))
	, specified_boolean_options(std::move(specified_boolean_options))
	, positional_arguments(std::move(positional_arguments))
	{}

// | Is the boolean option provided?
bool cli::ParsedArgs::is(const std::string& option) const {
	return specified_boolean_options.find(option) != specified_boolean_options.cend();
}

// | Get the value of an option with a default.
std::string cli::ParsedArgs::get(const std::string& option, const std::string &default_) const {
	std::map<std::string, std::string>::const_iterator string_search = specified_string_options.find(option);
	if (string_search == specified_string_options.cend()) {
		return default_;
	} else {
		return string_search->second;
	}
}

// | Was a string value specified for the option?
bool cli::ParsedArgs::has(const std::string& option) const {
	std::map<std::string, std::string>::const_iterator string_search = specified_string_options.find(option);
	if (string_search == specified_string_options.cend()) {
		return false;
	} else {
		return true;
	}
}

// | Was a string value specified for the option?
std::optional<std::string> cli::ParsedArgs::find(const std::string& option) const {
	std::map<std::string, std::string>::const_iterator string_search = specified_string_options.find(option);
	if (string_search == specified_string_options.cend()) {
		return std::optional<std::string>();
	} else {
		return std::optional<std::string>(string_search->second);
	}
}

// | Get a vector of arguments that would result in this ParsedArgs.
//
// This does not include the first program value.
std::vector<std::string> cli::ParsedArgs::normalized_args() const {
	std::vector<std::string> args;

	for (const std::string &specified_boolean_option : std::as_const(specified_boolean_options)) {
		args.push_back("--" + specified_boolean_option);
	}

	for (const std::map<std::string, std::string>::value_type &specified_string_option : std::as_const(specified_string_options)) {
		args.push_back("--" + specified_string_option.first);
		args.push_back(specified_string_option.second);
	}

	if (positional_arguments.size() >= 1) {
		args.push_back("--");

		for (const std::string &positional_argument : std::as_const(positional_arguments)) {
			args.push_back(positional_argument);
		}
	}

	return args;
}

cli::ArgsSpec::OptionSpec::OptionSpec(bool boolean_option)
	: boolean_option(boolean_option)
	{}

cli::ArgsSpec::ArgsSpec(
	const std::map<std::string, cli::ArgsSpec::OptionSpec> &options,
	const std::map<std::string, std::string>               &option_aliases,
	const std::map<char, std::string>                      &short_aliases
)
	: options(options)
	, option_aliases(option_aliases)
	, short_aliases(short_aliases)
{
	assert(verify());
}
cli::ArgsSpec::ArgsSpec(
	std::map<std::string, cli::ArgsSpec::OptionSpec> &&options,
	std::map<std::string, std::string>               &&option_aliases,
	std::map<char, std::string>                      &&short_aliases
)
	: options(std::move(options))
	, option_aliases(std::move(option_aliases))
	, short_aliases(std::move(short_aliases))
{
	assert(verify());
}

bool cli::ArgsSpec::verify() const {
	for (const std::pair<std::string, std::string> &alias_option : std::as_const(option_aliases)) {
		assertm(options.find(alias_option->second) != options.cend(), "All CLI argument aliases are of existing options.");
	}

	for (const std::pair<char, std::string> &alias_option : std::as_const(short_aliases)) {
		assertm(options.find(alias_option->second) != options.cend(), "All short CLI argument aliases are of existing options.");
	}

	return true;
}

const cli::ArgsSpec cli::ArgsSpec::default_args_spec {
	// std::map<std::string, cli::ArgsSpec::OptionSpec> options
	{
		{"help",    {true}},
		{"version", {true}},
		{"input",   {false}},
		{"output",  {false}},
	},

	// std::map<std::string, std::string> option_aliases
	{
	},

	// std::map<char, std::string> short_aliases
	{
		{'h', "help"},
		{'?', "help"},
		{'V', "version"},
		{'i', "input"},
		{'o', "output"},
	},
};

cli::ParsedArgs cli::ArgsSpec::parse(const std::vector<std::string> &args, const std::optional<std::string> &prog) {
	std::string prog_str;

	if (prog) {
		prog_str = *prog;
	} else {
		prog_str = cli::default_prog;
	}

	std::map<std::string, std::string> specified_string_options;
	std::set<std::string>              specified_boolean_options;
	std::vector<std::string>           positional_arguments;

	// Have we encountered "--"?  When we do, treat the rest of the arguments
	// as positional arguments rather than options.
	bool double_dash = false;
	bool expecting_option_argument = false;
	std::string expecting_option_key;
	std::string expecting_option_trigger;
	for (const std::string& arg : args) {
		if        (expecting_option_argument) {
			specified_string_options.insert({expecting_option_key, arg});
			expecting_option_argument = false;
		} else if (double_dash) {
			positional_arguments.push_back(arg);
		} else if (arg == "--") {
			double_dash = true;
		} else if (arg.size() >= 2 && arg.substr(0, 2) == "--") {
			// Long option.
			std::string arg_noprefix = arg.substr(2);

			// --foo=bar format?
			std::string arg_base;
			std::string::size_type equals_pos = arg_noprefix.find('=');
			if (equals_pos == std::string::npos) {
				arg_base = std::string(arg_noprefix);
			} else {
				arg_base = arg_noprefix.substr(0, equals_pos);
			}

			// Alias?
			std::string option;
			std::map<std::string, std::string>::const_iterator alias_search = option_aliases.find(arg_base);
			if (alias_search != option_aliases.cend()) {
				option = std::string(alias_search->second);
			} else {
				std::map<std::string, cli::ArgsSpec::OptionSpec>::const_iterator option_search = options.find(arg_base);
				if (option_search != options.cend()) {
					option = std::string(arg_base);
				} else {
					std::ostringstream sstr;
					sstr << "cli::ArgsSpec::parse: unrecognized command-line argument `" << arg << "'.";
					throw cli::CLIError(sstr.str());
				}
			}

			// Get the option spec.
			std::map<std::string, cli::ArgsSpec::OptionSpec>::const_iterator option_search = options.find(option);
			if (option_search == options.cend()) {
				std::ostringstream sstr;
				sstr << "cli::ArgsSpec::parse: internal specification error: no option found for `" << option << "' in command-line argument `" << arg << "'.  Was an alias defined for a non-existent option?";
				throw cli::CLIError(sstr.str());
			}
			cli::ArgsSpec::OptionSpec option_spec(option_search->second);

			// Is it a boolean option or a string option?
			if (option_spec.boolean_option) {
				// The option is a boolean option.

				// Fail if '=' was given.
				if (equals_pos != std::string::npos) {
					std::ostringstream sstr;
					sstr << "cli::ArgsSpec::parse: option `" << option << "' does not require an argument, but `" << arg << "' was provided as a command-line argument.";
					throw cli::CLIError(sstr.str());
				} else {
					// Add the option to the set of specified boolean options.
					specified_boolean_options.insert(option);
				}
			} else {
				// The option is a string option.

				// If '=' was given, add the option now.
				if (equals_pos != std::string::npos) {
					std::string arg_value = arg_noprefix.substr(equals_pos);
					specified_string_options.insert({option, arg_value});
				} else {
					// The next argument will the value for this option.
					expecting_option_key      = std::string(option);
					expecting_option_trigger  = std::string(arg);
					expecting_option_argument = true;
				}
			}
		} else if (arg.size() >= 1 && arg[0] == '-' && arg != "-") {
			// Short options.
			std::string arg_noprefix = arg.substr(1);
			for (std::string::size_type pos = 0; pos < arg_noprefix.size(); ++pos) {
				char c = arg_noprefix[pos];

				// Find the short alias.
				std::string option;
				std::map<char, std::string>::const_iterator short_alias_search = short_aliases.find(c);
				if (short_alias_search != short_aliases.cend()) {
					option = std::string(short_alias_search->second);
				} else {
					if (arg_noprefix.size() <= 1) {
						std::ostringstream sstr;
						sstr << "cli::ArgsSpec::parse: unrecognized command-line argument `" << arg << "'.";
						throw cli::CLIError(sstr.str());
					} else {
						std::ostringstream sstr;
						sstr << "cli::ArgsSpec::parse: unrecognized short option `-" << c << "' in command-line argument `" << arg << "'.";
						throw cli::CLIError(sstr.str());
					}
				}

				// Get the option spec.
				std::map<std::string, cli::ArgsSpec::OptionSpec>::const_iterator option_search = options.find(option);
				if (option_search == options.cend()) {
					std::ostringstream sstr;
					sstr << "cli::ArgsSpec::parse: internal specification error: no option found for `" << option << "' (short option: `" << c <<"') in command-line argument `" << arg << "'.  Was an alias defined for a non-existent option?";
					throw cli::CLIError(sstr.str());
				}
				cli::ArgsSpec::OptionSpec option_spec(option_search->second);

				// Is it a boolean option or a string option?
				if (option_spec.boolean_option) {
					// The option is a boolean option.
					specified_boolean_options.insert(option);
				} else {
					// The option is a string option.

					// Is this the last character in the command-line argument?
					if (pos >= arg_noprefix.size() - 1) {
						// This is the last character; expect the string
						// argument as the next option.

						// The next argument will the value for this option.
						expecting_option_key      = std::string(option);
						expecting_option_trigger  = "-" + std::string(1, c);
						expecting_option_argument = true;
					} else {
						// There are more characters in this command-line
						// argument.  Interpret the remaining characters in
						// this command-line argument as the value.
						specified_string_options.insert({option, arg_noprefix.substr(pos + 1)});
						// End the traversal of this argument.
						break;
					}
				}
			}
		} else {
			// Positional argument.
			positional_arguments.push_back(arg);
		}
	}

	// Checks.
	if (expecting_option_argument) {
		std::ostringstream sstr;
		sstr << "cli::ArgsSpec::parse: expecting argument for option `" << expecting_option_trigger << "'.";
		throw cli::CLIError(sstr.str());
	}

	// Return.
	return cli::ParsedArgs(
		std::move(specified_string_options),
		std::move(specified_boolean_options),
		std::move(positional_arguments)
	);
}

// Wrapper around cli::run.
int cli::cli(int argc, char *argv[]) {
	// Construct argument vector.
	std::vector<std::string> argvec;
	argvec.reserve(argc);
	// Perform a copy: c.f. https://stackoverflow.com/a/261607
	argvec.insert(argvec.end(), &argv[0], &argv[argc]);

	// Run.
	cli::run(argvec);

	// Return.
	return 0;
}

// CLI interface.
void cli::run(const std::vector<std::string> &argv) {
	// Split args and prog.
	std::vector<std::string> args(argv.cbegin() + 1, argv.cend());
	std::optional<std::string> prog;
	if (argv.size() >= 1) {
		prog.reset();
	} else {
		prog.emplace(std::string(argv[0]));
	}

	// Get the default argument specification.
	cli::ArgsSpec args_spec(cli::ArgsSpec::default_args_spec);

	// Print CLIErrors with friendlier formatting.
	try {
		// Parse the arguments.
		cli::ParsedArgs parsed_args(args_spec.parse(args, prog));

		// Call run with the parsed arguments.
		return cli::run_with_parsed(parsed_args, args_spec, args, prog);
	} catch (const cli::CLIError &ex) {
		const std::string err_msg = ex.what();
		std::string err_msg_noprefix;
		const std::string::size_type separator_pos = err_msg.find(": ");
		if (separator_pos == std::string::npos) {
			err_msg_noprefix = std::string(err_msg);
		} else {
			err_msg_noprefix = err_msg.substr(separator_pos + 2);
		}

		std::cerr << err_msg_noprefix << std::endl;
		return cli::usage(prog, 2, true);
	}
}

const std::string cli::default_prog(STR(CLI_DEFAULT_PROG));

void cli::usage(const std::optional<std::string> &prog, int exit_code, bool err) {
	if (err) {
		std::cerr << cli::get_usage(prog);
	} else {
		std::cout << cli::get_usage(prog);
	}
	std::exit(exit_code);
}

std::string cli::get_usage(const std::optional<std::string> &prog) {
	std::string prog_str = prog.value_or(cli::default_prog);
	std::ostringstream sstr;
	sstr
		<< "Usage: " << prog_str << " -i PATH.cpsl -o PATH.asm" << std::endl
		;
	return sstr.str();
}

void cli::version(const std::optional<std::string> &prog, int exit_code, bool err) {
	if (err) {
		std::cerr << cli::get_version_info(prog);
	} else {
		std::cout << cli::get_version_info(prog);
	}
	std::exit(exit_code);
}

std::string cli::get_version_info(const std::optional<std::string> &prog) {
	std::string prog_str = prog.value_or(cli::default_prog);
	(void) prog_str;  // Unused.
	std::ostringstream sstr;
	sstr
		<< cpsl_cc_version_string << std::endl
		;
	return sstr.str();
}

void cli::run_with_parsed(const ParsedArgs &parsed_args) {
	return cli::run_with_parsed(parsed_args, cli::ArgsSpec::default_args_spec);
}

void cli::run_with_parsed(const ParsedArgs &parsed_args, const ArgsSpec &args_spec) {
	return cli::run_with_parsed(parsed_args, args_spec, parsed_args.normalized_args(), std::optional<std::string>());
}

void cli::run_with_parsed(const ParsedArgs &parsed_args, const ArgsSpec &args_spec, const std::vector<std::string> &args, const std::optional<std::string> &prog) {
	// Handle information options.
	if       (parsed_args.is("help")) {
		return cli::usage(prog);
	} else if(parsed_args.is("version")) {
		return cli::version(prog);
	}

	// Get output file.
	std::string output_path;
	std::optional<std::string> output_option = parsed_args.find("output");
	if (!output_option) {
		std::ostringstream sstr;
		sstr << "cli::run_with_parsed: missing output path (e.g. -o path/to/output.asm).";
		throw cli::CLIError(sstr.str());
	} else {
		output_path = *output_option;
	}

	// Get input files.
	std::vector<std::string> input_paths;
	std::optional<std::string> input_option = parsed_args.find("input");
	if (!input_option) {
		// It could be specified as a positional argument.
	} else {
		std::string input_path;
		input_path = *input_option;
		input_paths.push_back(input_path);
	}

	// Add positional arguments as input files.
	input_paths.insert(input_paths.end(), parsed_args.positional_arguments.cbegin(), parsed_args.positional_arguments.cend());

	// Ensure there is an input path.
	if (input_paths.size() <= 0) {
		std::ostringstream sstr;
		sstr << "cli::run_with_parsed: missing input path (e.g. -i path/to/output.asm).";
		throw cli::CLIError(sstr.str());
	}

	// Currently, multiple input paths are unsupported.
	if (input_paths.size() >= 2) {
		std::ostringstream sstr;
		sstr << "cli::run_with_parsed: currently, multiple input paths are unsupported, and multiple input paths were provided:";
		for (const std::string &input_path : input_paths) {
			sstr << std::endl << "\t" << input_path;
		}
		throw cli::CLIError(sstr.str());
	}
	std::string input_path = std::string(input_paths[0]);

	return cli::run_with_paths(parsed_args, input_path, output_path, args_spec, args, prog);
}

void cli::run_with_paths(const ParsedArgs &parsed_args, const std::string &input_path, const std::string &output_path) {
	return cli::run_with_paths(parsed_args, input_path, output_path, cli::ArgsSpec::default_args_spec);
}

void cli::run_with_paths(const ParsedArgs &parsed_args, const std::string &input_path, const std::string &output_path, const ArgsSpec &args_spec) {
	return cli::run_with_paths(parsed_args, input_path, output_path, args_spec, parsed_args.normalized_args(), std::optional<std::string>());
}

// | Handle input and output paths after handling information options, e.g.
// --help and --version.
void cli::run_with_paths(const ParsedArgs &parsed_args, const std::string &input_path, const std::string &output_path, const ArgsSpec &args_spec, const std::vector<std::string> &args, const std::optional<std::string> &prog) {
	std::cout << "To be implemented..." << std::endl;
	std::cout << "Input path: " << input_path << std::endl;
	std::cout << "Output path: " << output_path << std::endl;
	return;
}

/*
	// | Assemble a CPSL file.
void cli::assemble(const std::string &in_path, const std::string &out_path) {
	std::cout << "To be implemented..." << std::endl;
}
*/
