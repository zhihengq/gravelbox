#include <trace/tracer.h>

#include <string>
#include <vector>

int main(int argc, char **argv) {
	std::vector<std::string> args;
	args.reserve(argc);
	for (char **arg = argv + 1; arg < argv + argc; arg++)
		args.emplace_back(*arg);
	GravelBox::Tracer tracer;
	tracer.run(args);
	return 0;
}
