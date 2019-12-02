#include "file_config.h"
#include <exceptions.h>

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <string>
#include <string_view>

#include <json/json.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>

namespace GravelBox {

constexpr auto kRegexFlags = std::regex_constants::optimize;
constexpr size_t kHashSize = 512 / 8;

[[noreturn]] inline void error(const std::string &path,
							   const std::string &details) {
	throw ConfigException(path, "GravelBox configuration", details);
}

FileConfig::FileConfig(const std::string &config_path) {
	try {
		{
			std::ifstream file(config_path, std::ios::binary);
			if (!file)
				error(config_path, "Cannot open file \"" + config_path + '\"');
			file.exceptions(std::ios::eofbit | std::ios::failbit);
			config_.assign(std::istreambuf_iterator<char>(file),
						   std::istreambuf_iterator<char>());
		}

		Json::Value config;
		{
			std::istringstream iss(config_);
			iss >> config;
		}

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

		auto hex2bytes = [&config_path](const std::string &hex) -> std::string {
			if (hex.size() % 2 != 0)
				error(config_path, "invalid hex string \"" + hex + '\"');
			std::string bytes;
			bytes.reserve(hex.size() / 2);
			for (auto i = hex.begin(); i < hex.end(); i += 2) {
				uint8_t byte;
				try {
					byte = std::stoul(std::string(i, i + 2), nullptr, 16);
				} catch (const std::exception &e) {
					error(config_path, "invalid hex string \"" + hex + '\"');
				}
				bytes.push_back(byte);
			}
			return bytes;
		};

		sanitize(config.isObject(), "config is not an object");
		signature_ = config["signature"].asString();
		password_hash_ = hex2bytes(config["password"].asString());
		if (!password_hash_.empty())
			sanitize(password_hash_.size() == kHashSize,
					 "password hash size incorrect");
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
	} catch (const std::regex_error &re) {
		error(config_path, std::string("Regex error: ") + re.what());
	} catch (const Json::Exception &je) {
		error(config_path, je.what());
	}
}

bool FileConfig::verify_signature(std::string &&key) {
	key_ = std::move(key);
	std::ifstream file(signature_, std::ios::binary | std::ios::ate);
	if (!file)
		throw ConfigException(signature_, "GravelBox configuration signature",
							  "Cannot open file \"" + signature_ + '\"');
	file.exceptions(std::ios::eofbit | std::ios::failbit);
	if (file.tellg() != kHashSize)
		throw ConfigException(signature_, "GravelBox configuration signature",
							  "Incorrect signature length");
	file.seekg(0);
	std::string sig{std::istreambuf_iterator<char>(file),
					std::istreambuf_iterator<char>()};
	file.seekg(0);
	if (verify_hmac(config_, sig)) {
		dismiss_signature();
		return true;
	}
	return false;
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

bool FileConfig::verify_hmac(const std::string &data,
							 const std::string &mac) const noexcept {
	char md[kHashSize];
	auto r = HMAC(EVP_sha512(), key_.data(), key_.size(),
				  reinterpret_cast<const uint8_t *>(data.data()), data.size(),
				  reinterpret_cast<uint8_t *>(md), nullptr);
	if (r == nullptr) {
		// Crypto error for unknown reasons
		ERR_print_errors_fp(stderr);
		return false;
	}
	return std::string_view(md, sizeof(md)) == mac;
}

}  // namespace GravelBox
