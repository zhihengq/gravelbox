#include <exceptions.h>
#include <modules.h>

#include <boost/program_options.hpp>

#include <iostream>
#include <string>
#include <system_error>
#include <vector>

constexpr char kDefaultConfig[] = "gravelbox_config.json";

int main(int argc, char **argv) {
	namespace po = boost::program_options;
	po::variables_map vm;
	po::options_description visible_desc{"GravelBox usage:\n"
										 "  "
										 + std::string(argv[0])
										 + " [options] -- target [args...]\n"
										   "Options"};
	visible_desc.add_options()("help,h", "print help message")
		("stdin,i", po::value<std::string>(), "redirected standard input")
		("stdout,o", po::value<std::string>(), "redirected standard output")
		("stderr,e", po::value<std::string>(), "redirected standard error")
		("config,c", po::value<std::string>()->default_value(kDefaultConfig),
					 "configuration file path");
	po::options_description desc = visible_desc;
	desc.add_options()("args", po::value<std::vector<std::string>>());
	po::positional_options_description pod;
	pod.add("args", -1);

	try {
		auto parsed = po::command_line_parser(argc, argv)
						  .options(desc)
						  .positional(pod)
						  .run();
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
		return GravelBox::run(vm);
	} catch (const std::system_error &se) {
		std::cerr << "System error " << se.code().value() << ": " << se.what()
				  << std::endl;
	} catch (const GravelBox::ConfigException &ce) {
		std::cerr << "Configuration error: " << ce.what() << std::endl;
	} catch (const GravelBox::PinentryException &pe) {
		std::cerr << "Pinentry error: " << pe.what() << std::endl;
	} catch (const GravelBox::ChildExitException &cee) { return cee.exit_code; }
	return EXIT_FAILURE;
}
