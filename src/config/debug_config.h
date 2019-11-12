#ifndef DEBUG_CONFIG_H_
#define DEBUG_CONFIG_H_

#include <type_traits.h>

#include <string>

namespace GravelBox {

/**
 * Debug configuration that hard-codes all parameters.
 */
class DebugConfig {
  public:
	/**
	 * Actions to system calls.
	 */
	enum class Action { ALLOW, DENY, ASK };

	/**
	 * Path of syscall definition file.
	 *
	 * @return "syscalldef.json"
	 */
	std::string syscalldef() const noexcept { return "syscalldef.json"; }

	/**
	 * Pinentry selection.
	 * Select the system default.
	 *
	 * @return "pinentry"
	 */
	std::string pinentry() const noexcept { return "pinentry"; }

	/**
	 * Maximum string length for parsing string arguments.
	 *
	 * @return 64
	 */
	size_t max_str_len() const noexcept { return 64; }

	/**
	 * Get an action for a syscall.
	 *
	 * @param syscall syscall string.
	 * @return ASK
	 */
	Action get_action(const std::string &syscall) const noexcept {
		return Action::ASK;
	}

	/**
	 * Check if the configuration contains a password for user interactions.
	 *
	 * @return false.
	 */
	bool has_password() const noexcept { return false; }

	/**
	 * Verify password for user interactions.
	 *
	 * @param password password entered by the user.
	 * @return true.
	 */
	bool verify_password(const std::string &password) const noexcept {
		return true;
	}
};

static_assert(IsConfig<DebugConfig>::value,
			  "DebugConfig does not satisfy the Config concept");

}  // namespace GravelBox

#endif  // DEBUG_CONFIG_H_
