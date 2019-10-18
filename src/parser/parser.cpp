#include "parser.h"
#include <utils.h>

#include <cassert>

#include <sys/wait.h>
#include <sys/ptrace.h>

#include <sstream>

namespace GravelBox {

using Utils::check;

std::string Parser::operator()(const user_regs_struct &regs) const noexcept {
	std::ostringstream oss;
	oss << std::dec
		<< static_cast<int64_t>(regs.orig_rax) << '('
		<< std::hex
		<< "0x" << regs.rdi << ", "
		<< "0x" << regs.rsi << ", "
		<< "0x" << regs.rdx << ", "
		<< "0x" << regs.r10 << ", "
		<< "0x" << regs.r10 << ", "
		<< "0x" << regs.r8  << ", "
		<< "0x" << regs.r9  << ')';
	return oss.str();
}

}  // namespace GravelBox
