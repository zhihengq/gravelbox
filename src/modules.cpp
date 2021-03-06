#include "modules.h"
#include <trace/tracer.h>
#include <parser/parser.h>
#include <config/file_config.h>
#include <logger/logger.h>
#include <ui/pinentry_ui.h>

#include <memory>
#include <vector>

#include <boost/program_options.hpp>

namespace GravelBox {

int run(const boost::program_options::variables_map &vm) {
	auto config = std::make_unique<GravelBox::FileConfig>(
		vm.at("config").as<std::string>());

	std::unique_ptr<GravelBox::PinentryUI> ui;
	if (vm.count("pinentry") > 0)
		ui = std::make_unique<GravelBox::PinentryUI>(
			vm.at("pinentry").as<std::string>());
	if (vm.at("no-signature").as<bool>()) {
		config->dismiss_signature();
		config->remove_password();
	} else {
		constexpr auto message = "Enter the configuration file signing key.";
		constexpr auto prompt = "key: ";
		if (ui == nullptr)
			ui = std::make_unique<GravelBox::PinentryUI>("pinentry");
		PinentryUI::Password key = ui->ask_password(message, prompt, "");
		if (!key)
			return EXIT_FAILURE;
		while (!config->verify_signature(std::move(key.password))) {
			key = ui->ask_password(
				message, prompt,
				"Incorrect key, or the configuration has been changed.");
			if (!key)
				return EXIT_FAILURE;
		}
	}
	if (vm.count("pinentry") == 0)
		ui = std::make_unique<GravelBox::PinentryUI>(config->pinentry());
	auto parser = std::make_unique<GravelBox::Parser>(config->syscalldef());
	auto logger = std::make_unique<GravelBox::Logger>();
	GravelBox::Tracer tracer(std::move(parser), std::move(config),
							 std::move(ui), std::move(logger));
	return tracer.run(
		vm.at("args").as<std::vector<std::string>>(),
		vm.count("stdin") == 0 ? "-" : vm.at("stdin").as<std::string>(),
		vm.count("stdout") == 0 ? "-" : vm.at("stdout").as<std::string>(),
		vm.at("append-stdout").as<bool>(),
		vm.count("stderr") == 0 ? "-" : vm.at("stderr").as<std::string>(),
		vm.at("append-stderr").as<bool>());
}

}  // namespace GravelBox
