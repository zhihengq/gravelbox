#ifndef _UI_H_
#define _UI_H_

#include <string>

namespace GravelBox {

/**
 * A UI to interact with user
 */
class UI {
  public:
	/**
	 * Display args of a syscall to user.
	 *
	 * @param human readable args of syscall
	 * @return true if success
	 */
	bool display(const std::string &syscall) const;
	
	/**
	 * Ask for user's choice after displaying a system call.
	 * Return integer indicator to represent the answer.
	 * 1 for yes, 0 for no, (-1 for modify -- not implemented yet)
	 *
	 * @return  integer indicator
	 */
	int confirm() const;
};

}  // namespace GravelBox

#endif  // _UI_H_
