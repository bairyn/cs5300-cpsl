#include <string>
#include <vector>

#define CLI_DEFAULT_PROG cpsl-cc
namespace cli {
	// | cli: The command-line interface to the compiler.
	//
	// Wrapper around run.
	int cli(int argc, char *argv[]);

	// | run: Run the command-line interface to the compiler given the provided
	// command-line arguments.
	void run(const std::vector<std::string> &argv);

	// | The default prog value.
	extern const std::string default_prog;

	// | Print usage and exit.
	void usage(const std::string &prog);

	// | Get the usage string.
	std::string get_usage(const std::string &prog);

	// | Assemble a CPSL file.
	void assemble(const std::string &in_path, const std::string &out_path);
}
