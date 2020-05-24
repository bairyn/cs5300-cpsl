#include "cli.hh"

#include <algorithm>  // std::move
#include <iostream>   // std::cout, std::endl
#include <string>     // std::string
#include <utility>    // std::exit
#include <vector>     // std::vector

// Wrapper around cli::run.
int cli::cli(int argc, char *argv[]) {
	// Construct argument vector.
	std::vector<std::string> argvec();
	argvec.reserve(argc);
	// Perform a copy: c.f. https://stackoverflow.com/a/261607
	argvec.insert(argvec.end(), &argv[0], &argv[argc]);

	// Run.
	cli::run(std::move(argvec()));

	// Return.
	return 0;
}

// CLI interface.
void cli::run(std::vector<std::string> argv) {
	std::cout << "To be implemented..." << std::endl;
}
