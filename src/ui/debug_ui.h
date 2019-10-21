#ifndef DEBUG_UI_H_
#define DEBUG_UI_H_

#include <type_traits.h>

#include <iostream>
#include <string>

namespace GravelBox {

/**
 * A simple debug ui. Show syscall on stderr, and always allow the syscall.
 */
class DebugUI {
  public:
	/**
	 * Display the syscall on stderr, and return true.
	 *
	 * @param syscall the human readable string for the syscall.
	 * @return always true.
	 */
	bool ask(const std::string &syscall) const noexcept {
		std::cerr << syscall << std::endl;
		return true;
	}
};

static_assert(IsUI<DebugUI>::value, "DebugUI does not fulfill UI");

}  // namespace GravelBox

#endif  // DEBUG_UI_H_
