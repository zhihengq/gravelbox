#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <iostream>
#include <string>

namespace Gravelbox {

/*
 * Logger takes a valid file and writes logs to them.
 *
 *
 */
class Logger {
  public:
    void write(const std::string &syscall) const {
      std::cout << syscall << std::endl;
    }
  private:
    int fd_;
}
#endif // LOGGER_H_
