#include "parser.h"
#include <exceptions.h>

#include <sys/user.h>

#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>

#include <json/json.h>

namespace GravelBox {

[[noreturn]] inline void error(const std::string &path,
							   const std::string &details) {
	throw ConfigException(path, "syscall definition", details);
}

Parser::Parser(const std::string &def) : str_(target_) {
	try {
		std::ifstream config(def, std::ios::binary);
		if (!config)
			error(def, "file not found");
		config.exceptions(std::ios::eofbit | std::ios::badbit);
		Json::Value definitions;
		config >> definitions;

		auto sanitize = [&def](bool assertion, const std::string &details) {
			if (!assertion)
				error(def, details);
		};

		sanitize(definitions.isArray(), "definition list is not an array");
		for (const Json::Value &syscall : definitions) {
			sanitize(syscall.isObject(), "syscall definition is not an object");
			uint64_t number = syscall["number"].asUInt64();
			SyscallDef syscalldef(syscall["name"].asString());
			Json::Value params = syscall["params"];
			sanitize(params.isArray(), "parameter definition is not an array");
			for (const Json::Value &param : params) {
				std::string param_str = param.asString();
				try {
					syscalldef.add_param(argtypes_.at(param_str));
				} catch (const std::out_of_range &e) {
					throw ConfigException(def, "syscall definition",
										  "unknown type \"" + param_str + '\"');
				}
			}
			auto [it, insert]
				= syscall_map_.emplace(number, std::move(syscalldef));
			sanitize(insert,
					 "duplicate syscall number " + std::to_string(number));
		}
	} catch (const Json::Exception &je) { error(def, je.what()); }
}

std::string Parser::operator()(const Utils::SyscallArgs &args) const noexcept {
	std::ostringstream oss;
	if (args.int80 || syscall_map_.count(args.number) == 0) {
		oss << (args.int80 ? "syscall32(" : "syscall(") << std::dec
			<< args.number << ", " << std::hex;
		for (size_t i = 0; i < 6; i++)
			oss << "0x" << args.args[i] << (i < 5 ? ", " : ")");
	} else {
		syscall_map_.at(args.number).write(oss, args.args);
	}
	return oss.str();
}

}  // namespace GravelBox
