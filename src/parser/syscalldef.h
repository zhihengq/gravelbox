#ifndef SYSCALLDEF_H_
#define SYSCALLDEF_H_

#include "argtypes.h"

#include <array>
#include <cassert>
#include <cstdint>
#include <ostream>
#include <vector>

namespace GravelBox {

/**
 * A `SyscallDef` contains a signature of an system call.
 * A `SyscallDef` is not aware of its corresponding syscall number, so it will
 * ignore `args.orig_rax` and assumes the arguments are for this syscall.
 */
class SyscallDef {
  public:
	/**
	 * Write the human readable string of the syscall.
	 *
	 * @param os output stream.
	 * @param args syscall argument registers.
	 * @return `os`
	 */
	std::ostream &write(std::ostream &os,
						const std::array<uint64_t, 6> &args) const {
		assert(argtypes_.size() < 6);
		os << fname_ << '(';
		for (size_t i = 0; i < argtypes_.size(); i++) {
			if (i > 0)
				os << ", ";
			auto &type = *argtypes_[i];
			os << type(args[i]);
		}
		os << ')';
	}

  private:
	std::string fname_;
	std::vector<const ArgType *> argtypes_;
};

}  // namespace GravelBox

#endif  // SYSCALLDEF_H_
