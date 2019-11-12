#include "modules.h"
#include <trace/tracer.h>
#include <parser/parser.h>
#include <config/file_config.h>
#include <logger/logger.h>
#include <ui/pinentry_ui.h>

#include <vector>

#include <boost/program_options.hpp>

namespace GravelBox {

int run(const boost::program_options::variables_map &vm) {
	auto config = std::make_unique<GravelBox::FileConfig>(vm.at("config").as<std::string>());
	auto parser = std::make_unique<GravelBox::Parser>(config->syscalldef());
	auto ui = std::make_unique<GravelBox::PinentryUI>(config->pinentry());
	auto logger = std::make_unique<GravelBox::Logger>();
	GravelBox::Tracer tracer(std::move(parser), std::move(config),
							 std::move(ui), std::move(logger));
	// TODO(qzh): pass stdin/stdout/stderr to tracer
	return tracer.run(vm.at("args").as<std::vector<std::string>>());
}

}  // namespace GravelBox
