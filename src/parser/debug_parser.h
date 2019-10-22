#ifndef DEBUG_PARSER_H_
#define DEBUG_PARSER_H_

#include <type_traits.h>

#include <sys/user.h>

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
	std::string operator()(const user_regs_struct &regs) const noexcept {
		std::ostringstream oss;
		oss << "syscall("
			<< std::dec
			<< regs.orig_rax << ", "
			<< std::hex
			<< "0x" << regs.rdi << ", "
			<< "0x" << regs.rsi << ", "
			<< "0x" << regs.rdx << ", "
			<< "0x" << regs.r10 << ", "
			<< "0x" << regs.r8 << ", "
			<< "0x" << regs.r9 << ')';
		return oss.str();
	}
};

static_assert(IsParser<DebugParser>::value,
			  "DebugParser does not fulfill Parser");

}  // namespace GravelBox

#endif  // DEBUG_PARSER_H_
