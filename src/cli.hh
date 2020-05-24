#include <string>
#include <vector>

namespace cli {
	// | cli: The command-line interface to the compiler.
	//
	// Wrapper around run.
	int cli(int argc, char *argv[]);

	// | run: Run the command-line interface to the compiler given the provided
	// command-line arguments.
	void run(std::vector<std::string> argv);
}
