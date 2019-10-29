#include "parser.h"
#include <exceptions.h>

#include <sys/user.h>

#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>

#include <json/json.h>

namespace GravelBox {

Parser::Parser(const std::string &def) : str_(target_) {
	try {
		std::ifstream config(def, std::ifstream::binary);
		Json::Value definitions;
		config >> definitions;

		auto sanitize = [&def](bool assertion, const std::string &details) {
			if (!assertion)
				throw ConfigException(def, "syscall definition", details);
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
			syscall_map_.emplace(number, std::move(syscalldef));
		}
	} catch (const Json::Exception &je) {
		throw ConfigException(def, "syscall definition", je.what());
	}
}

std::string Parser::operator()(const user_regs_struct &regs) const noexcept {
	const std::array<uint64_t, 6> args = {
		regs.rdi, regs.rsi, regs.rdx, regs.r10, regs.r8, regs.r9,
	};
	std::ostringstream oss;
	if (syscall_map_.count(regs.orig_rax) == 0) {
		oss << "syscall(" << std::dec << regs.orig_rax << ", " << std::hex;
		for (size_t i = 0; i < 6; i++)
			oss << "0x" << regs.rdi << (i < 5 ? ", " : ")");
	} else {
		syscall_map_.at(regs.orig_rax).write(oss, args);
	}
	return oss.str();
}

}  // namespace GravelBox
