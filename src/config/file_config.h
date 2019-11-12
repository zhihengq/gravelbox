#ifndef FILE_CONFIG_H_
#define FILE_CONFIG_H_

#include <type_traits.h>

#include <cstdint>
#include <regex>
#include <string>

namespace GravelBox {

/**
 * GravelBox configuration loaded from a file.
 */
class FileConfig {
  public:
	enum class Action { ALLOW, DENY, ASK };
	/**
	 * Construct a FileConfig from a configuration file.
	 *
	 * @param config_path the path of the configuration file.
	 */
	explicit FileConfig(const std::string &config_path, std::string &&key);

	/**
	 * Return path of the system call definition file.
	 *
	 * @return std::string path of the syscall definition file.
	 */
	std::string syscalldef() const noexcept { return syscalldef_; }

	/**
	 * Return pinentry program name or path.
	 *
	 * @return std::string pinentry.
	 */
	std::string pinentry() const noexcept { return pinentry_; }

	/**
	 * Return the maximum string length for parsing string arguments.
	 *
	 * @return size_t the maximum string length.
	 */
	size_t max_str_len() const noexcept { return max_str_len_; }

	/**
	 * Get an action for a syscall.
	 *
	 * @param syscall the string representation of the system call with
	 * arguments.
	 * @return Action the action for this syscall.
	 */
	Action get_action(const std::string &syscall) const noexcept;

	/**
	 * Check if the configuration contains a password for user interactions.
	 *
	 * @return true if there is a password. Password should be verified before
	 * accepting user decisions.
	 * @return false if there is no password. Unconditionally accept user
	 * decisions.
	 */
	bool has_password() const noexcept {
		return !password_hash_.empty();
	}

	/**
	 * Verify password for user interactions.
	 *
	 * @param password password entered by the user.
	 * @return true if the password is correct or if the config contains no
	 * password.
	 * @return false if the password is incorrect.
	 */
	bool verify_password(const std::string &password) const noexcept;

	/**
	 * Copy constructor.
	 */
	FileConfig(const FileConfig &) = default;

	/**
	 * Copy assignment.
	 */
	FileConfig &operator=(const FileConfig &) = default;

  private:
	struct ActionGroup {
		Action action;
		std::vector<std::regex> patterns;
		ActionGroup(Action a, std::vector<std::regex> &&p)
			: action(a), patterns(std::move(p)) {}
	};

	const std::string key_;
	std::string password_hash_;
	std::string syscalldef_;
	std::string pinentry_;
	size_t max_str_len_;
	Action action_default_;
	std::vector<ActionGroup> action_groups_;
};

static_assert(IsConfig<FileConfig>::value,
			  "FileConfig does not satisfy Config concept");

};  // namespace GravelBox

#endif  // FILE_CONFIG_H_
