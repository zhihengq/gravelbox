#ifndef LOGGER_H_
#define LOGGER_H_

#include <iostream>
#include <string>

namespace GravelBox {

/**
 * Logger takes a valid file and writes logs to them.
 */
class Logger {
  public:
	/**
	 * no-op.
	 *
	 * @param syscall the system call string.
	 */
	void write(const std::string &syscall) const {
		// std::cout << syscall << std::endl;
	}
};

}  // namespace GravelBox

#endif  // LOGGER_H_
