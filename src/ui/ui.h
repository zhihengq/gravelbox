#ifndef _UI_H_
#define _UI_H_

#include <type_traits.h>

#include <iostream>
#include <string>

namespace GravelBox {

/**
 * A UI to display args to the user
 */
class UI {
  public:
	/**
	 * Display args of a syscall to user.
	 *
	 * @param human readable args of syscall
	 * @return true if success
	 */
	bool display(const std::string &syscall) const {
		std::cout << syscall << std::endl;
		return true;
	}
};

}  // namespace GravelBox

#endif  // _UI_H_
