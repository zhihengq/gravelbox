#include <exceptions.h>
#include <trace/tracer.h>
#include <parser/parser.h>
#include <ui/debug_ui.h>

#include <iostream>
#include <string>
#include <system_error>
#include <vector>

int main(int argc, char **argv) {
	try {
		std::vector<std::string> args;
		args.reserve(argc);
		for (char **arg = argv + 1; arg < argv + argc; arg++)
			args.emplace_back(*arg);
		auto parser = std::make_unique<GravelBox::Parser>();
		auto ui = std::make_unique<GravelBox::DebugUI>();
		GravelBox::Tracer tracer{std::move(parser), std::move(ui)};
		tracer.run(args);
		return 0;
	} catch (const std::system_error &se) {
		std::cerr << "Error " << se.code().value() << ": " << se.what()
				  << std::endl;
		return EXIT_FAILURE;
	} catch (const GravelBox::ChildExitException &cee) {
		return cee.exit_code;
	}
}
