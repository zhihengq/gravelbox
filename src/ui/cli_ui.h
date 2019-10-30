#ifndef CLI_UI_H_
#define CLI_UI_H_

#include <type_traits.h>

#include <string>

namespace GravelBox {

/**
 * A UI to interact with user
 */
class CliUI {
  public:
	/**
	 * Display the syscall on stderr, and return whether the user wants to
	 * proceed.
	 *
	 * @param syscall the human readable string for the syscall.
	 * @return true if the user allows the syscall.
	 * @return false if the user denys the syscall.
	 */
	bool ask(const std::string &syscall) const;
};

static_assert(IsUI<CliUI>::value, "CliUI does not fulfill UI concept");

}  // namespace GravelBox

#endif  // CLI_UI_H_
