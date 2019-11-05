#include "file_config.h"
#include <exceptions.h>

#include <cstdint>
#include <fstream>
#include <string>

#include <json/json.h>

namespace GravelBox {

constexpr auto kRegexFlags = std::regex_constants::optimize;

[[noreturn]] inline void error(const std::string &path,
							   const std::string &details) {
	throw ConfigException(path, "GravelBox configuration", details);
}

FileConfig::FileConfig(const std::string &config_path) {
	try {
		std::ifstream file(config_path, std::ios::binary);
		if (!file)
			error(config_path, "file not found");
		file.exceptions(std::ios::eofbit | std::ios::badbit);
		Json::Value config;
		file >> config;

		auto sanitize = [&path = config_path](bool assertion,
											  const std::string &details) {
			if (!assertion)
				error(path, details);
		};

		auto to_action = [&config_path](const std::string &s) -> Action {
			if (s == "allow")
				return Action::ALLOW;
			else if (s == "ask")
				return Action::ASK;
			else if (s == "deny")
				return Action::DENY;
			else
				error(config_path, "unknown action: \"" + s + '\"');
		};

		sanitize(config.isObject(), "config is not an object");
		syscalldef_ = config["syscall-definition"].asString();
		pinentry_ = config["pinentry"].asString();
		max_str_len_ = config["max-string-length"].asUInt64();
		action_default_ = to_action(config["default-action"].asString());
		Json::Value action_groups = config["action-groups"];
		sanitize(action_groups.isArray(), "action group is not an array");
		for (const Json::Value &ag : action_groups) {
			Action action = to_action(ag["action"].asString());
			std::vector<std::regex> patterns;
			Json::Value patterns_json = ag["patterns"];
			sanitize(patterns_json.isArray(), "patterns is not an array");
			for (const Json::Value &p : patterns_json)
				patterns.emplace_back(p.asString(), kRegexFlags);
			action_groups_.emplace_back(action, std::move(patterns));
		}
	} catch (const Json::Exception &je) { error(config_path, je.what()); }
}

FileConfig::Action FileConfig::get_action(const std::string &syscall) const
	noexcept {
	for (const ActionGroup &ag : action_groups_)
		for (const std::regex &r : ag.patterns)
			if (std::regex_match(syscall.cbegin(), syscall.cend(), r,
								 std::regex_constants::match_any))
				return ag.action;
	return action_default_;
}

}  // namespace GravelBox
