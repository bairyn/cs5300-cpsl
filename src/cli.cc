#include <algorithm>  // std::move
#include <iostream>   // std::cout, std::endl
#include <sstream>    // std::ostringstream
#include <string>     // std::string
#include <utility>    // std::exit
#include <vector>     // std::vector

#include "cli.hh"
#include "util.h"

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
	std::vector<std::string> args(argv.cbegin() + 1, argv.cend());

	if (
		args.size() != 4 ||
		!((args[0] == "-i" && args[2] == "-o") || (args[2] == "-o" && args[0] == "-i"))
	) {
		return cli::usage(argv[0]);
	} else {
		bool i_then_o = args[1] == "-i";

		std::string in_path, out_path;
		if (i_then_o) {
			in_path = args[1];
			out_path = args[3];
		} else {
			in_path = args[3];
			out_path = args[1];
		}

		return cli::assemble(in_path, out_path);
	}
}

const std::string cli::default_prog(STR(CLI_DEFAULT_PROG));

void cli::usage(const std::string &prog) {
	std::cout << cli::get_usage(prog);
	std::exit(0);
}

std::string cli::get_usage(const std::string &prog) {
	std::ostringstream sstr;
	sstr
		<< "Usage: " << prog << " -i PATH.cpsl -o PATH.asm" << std::endl
		;
	return sstr.str();
}

void cli::assemble(const std::string &in_path, const std::string &out_path) {
	std::cout << "To be implemented..." << std::endl;
}
