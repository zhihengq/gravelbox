#include <exceptions.h>
#include <trace/tracer.h>
#include <parser/debug_parser.h>
#include <logger/logger.h>
#include <ui/debug_ui.h>

#include <boost/program_options.hpp>

#include <iostream>
#include <string>
#include <system_error>
#include <vector>

int main(int argc, char **argv) {
	namespace po = boost::program_options;
	po::variables_map vm;
	po::options_description visible_desc{
		"GravelBox usage:\n"
		"  " + std::string{argv[0]} + " [options] -- target [args...]\n"
		"Options"
	};
	visible_desc.add_options()
		("help,h", "print help message")
		("stdin,i", po::value<std::string>(), "redirected standard input")
		("stdout,o", po::value<std::string>(), "redirected standard output")
		("stderr,e", po::value<std::string>(), "redirected standard error");
	po::options_description desc = visible_desc;
	desc.add_options()
		("args", po::value<std::vector<std::string>>());
	po::positional_options_description pod;
	pod.add("args", -1);

	try {
		auto parsed = po::command_line_parser{argc, argv}.options(desc).positional(pod).run();
		// "--args" is not actually an option
		for (const auto &opt : parsed.options)
			if ((opt.position_key == -1) && (opt.string_key == "args"))
				throw po::unknown_option("--args");
		po::store(parsed, vm);
		po::notify(vm);
	} catch (const po::error &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << visible_desc;
		return EXIT_FAILURE;
	}

	if (vm.count("help") > 0) {
		std::cerr << visible_desc;
		return EXIT_SUCCESS;
	}

	if (vm.count("args") == 0) {
		std::cerr << "Error: no target provided" << std::endl;
		std::cerr << visible_desc;
		return EXIT_FAILURE;
	}

	try {
		auto parser = std::make_unique<GravelBox::DebugParser>();
		auto ui = std::make_unique<GravelBox::DebugUI>();
		auto logger = std::make_unique<GravelBox::Logger>();
		GravelBox::Tracer tracer{std::move(parser), std::move(ui), std::move(logger)};
		// TODO(qzh): pass stdin/stdout/stderr to tracer
		tracer.run(vm.at("args").as<std::vector<std::string>>());
		return EXIT_SUCCESS;
	} catch (const std::system_error &se) {
		std::cerr << "System error " << se.code().value() << ": " << se.what()
				  << std::endl;
		return EXIT_FAILURE;
	} catch (const GravelBox::ChildExitException &cee) {
		return cee.exit_code;
	}
}
