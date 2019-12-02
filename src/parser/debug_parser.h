#ifndef DEBUG_PARSER_H_
#define DEBUG_PARSER_H_

#include <type_traits.h>
#include <utils.h>

#include <sys/user.h>
#include <sys/types.h>

#include <sstream>
#include <string>

namespace GravelBox {

/**
 * DebugParser class parses syscall numbers to decimal and arguments to
 * hexadecimal.
 */
class DebugParser {
  public:
	/**
	 * Parse syscall registers to debug strings.
	 *
	 * @param regs user registers at syscall entry.
	 * @return a string representation of the syscall with arguments.
	 */
	std::string operator()(const Utils::SyscallArgs &args) const noexcept {
		std::ostringstream oss;
		oss << "syscall("
			<< std::dec
			<< args.number << ", "
			<< std::hex
			<< "0x" << args.args[0] << ", "
			<< "0x" << args.args[1] << ", "
			<< "0x" << args.args[2] << ", "
			<< "0x" << args.args[3] << ", "
			<< "0x" << args.args[4] << ", "
			<< "0x" << args.args[5] << ')';
		return oss.str();
	}

	/**
	 * no-op.
	 */
	void setpid(pid_t) const noexcept {}
};

static_assert(IsParser<DebugParser>::value,
			  "DebugParser does not fulfill Parser");

}  // namespace GravelBox

#endif  // DEBUG_PARSER_H_
