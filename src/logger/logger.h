#ifndef LOGGER_H_
#define LOGGER_H_

#include <iostream>
#include <string>

namespace GravelBox {

/*
 * Logger takes a valid file and writes logs to them.
 */
class Logger {
  public:
	void write(const std::string &syscall) const {
		//std::cout << syscall << std::endl;
	}

  private:
	int fd_ = -1;
};

}  // namespace GravelBox

#endif  // LOGGER_H_
