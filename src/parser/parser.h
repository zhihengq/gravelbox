#ifndef PARSER_H_
#define PARSER_H_

#include <sys/user.h>

#include <string>

namespace GravelBox {

/**
 * Parser class parses syscall arguments to human readable strings.
 */
class Parser {
 public:
	/**
	 * Parse syscall registers to human readable strings.
	 *
	 * @param regs user registers at syscall entry.
	 * @return a string representation of the syscall with arguments.
	 */
	std::string operator()(const user_regs_struct &regs) const noexcept;
};

}  // namespace GravelBox

#endif  // PARSER_H_
