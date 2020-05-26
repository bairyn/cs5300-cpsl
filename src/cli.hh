#include <string>
#include <vector>

namespace cli {
	// | cli: The command-line interface to the compiler.
	//
	// Wrapper around run.
	int cli(int argc, char *argv[]);

	// | run: Run the command-line interface to the compiler given the provided
	// command-line arguments.
	void run(const std::vector<std::string> &argv);
}
